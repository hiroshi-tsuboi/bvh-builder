#include <thread>
#include <memory>
#include <iostream>
#include <chrono>
#include <cassert>

#include "bvh.h"
#include "aabb.h"

void Bvh::Node::dump(Bvh* bvh)
{
	bvh->log_.push(std::string("node aabb ="));
	bvh->log_.push(std::string("\tmin = ") + std::to_string(aabb_.mini_[0]) + ","  + std::to_string(aabb_.mini_[1])  + ","  + std::to_string(aabb_.mini_[2]));
	bvh->log_.push(std::string("\tmax = ") + std::to_string(aabb_.maxi_[0]) + ","  + std::to_string(aabb_.maxi_[1])  + ","  + std::to_string(aabb_.maxi_[2]));

	for (auto child: childs_)
	{
		if (child)
		{
			child->dump(bvh);
		}
	}
}

void Bvh::Leaf::dump(Bvh* bvh)
{
	bvh->log_.push(std::string("leaf triangles ="));
	for (auto index: triangleIndices_)
	{
		bvh->log_.push(std::string("\t") + std::to_string(index));
	}
}

void Bvh::Node::link(Obj* child, int index)
{
	std::lock_guard<std::mutex> lk(mutex_);
	assert(childs_[index] == nullptr);
	childs_[index] = child;
}

void Bvh::Node::destroy()
{
	for (auto child: childs_)
	{
		if (child == nullptr)
		{
			continue;
		}

		child->destroy();
		delete child;
	}
}

void Bvh::Leaf::create(const std::vector<AaBb>& aabbs)
{
	triangleIndices_.reserve(aabbs.size());

	for (auto& aabb: aabbs)
	{
		for (auto index: aabb.ownerIndices_)
		{
			if (0 <= index)
			{
				triangleIndices_.push_back(index);
			}
		}
	}
}

void Bvh::fork(Bvh::Node* parent, const int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs, int treeLevel)
{
	auto sharedResult = std::make_shared<Result>();

	for (uint32_t axisIndex = 0; axisIndex < 3; ++axisIndex)
	{
		std::thread thread(&Bvh::divide, this, parent, childIndex, sharedAabbs, sharedResult, axisIndex, treeLevel);
		thread.detach();
	}
}

void Bvh::Result::write(uint32_t index, double value)
{
	std::unique_lock<std::mutex> lk(mutex_);

	miniCosts_[index] = value;

	++finishCount_;
       
	if (finishCount_ == 3)
	{
		notFull_.notify_all();
	}
	else
	{
		notFull_.wait(lk, [&]{ return 3 <= finishCount_; });
	}
}

void Bvh::Result::join(bool waitForAll)
{
	std::unique_lock<std::mutex> lk(mutex_);

	++finishCount_;

	if (waitForAll)
	{
		if (finishCount_ != 6)
		{
			notFull_.wait(lk, [&]{ return finishCount_ == 6; });
		}
		return;
	}

	if (finishCount_ == 6)
	{
		notFull_.notify_one();
	}
}

void Bvh::LeftAmount::push(uint32_t count)
{
	std::lock_guard<std::mutex> lk(mutex_);
	count_ += count;
}

void Bvh::LeftAmount::pop()
{
	std::lock_guard<std::mutex> lk(mutex_);
	--count_;
	if (count_ == 0)
	{
		signal_.notify_one();
	}
}

void Bvh::LeftAmount::join()
{
	std::unique_lock<std::mutex> lk(mutex_);

	signal_.wait(lk, [&]{ return count_ == 0; });
}

bool Bvh::build(const Triangular& triangular, int extraTreeLevel)
{
	if (triangular.triangles_.empty())
	{
		return true;
	}

	int maxTreeLevel = 1;
	{
		const auto triangleCount = triangular.triangles_.size();
		while (maxTreeLevel < 64)
		{
			if (triangleCount <= (1ull << maxTreeLevel))
			{
				break;
			}
			++maxTreeLevel;
		}
	}
	maxTreeLevel_ = maxTreeLevel + extraTreeLevel;

	auto sharedAabbs = std::make_shared<std::vector<AaBb> >();

	auto& aabbs = *sharedAabbs.get();

	aabbs.resize(triangular.triangles_.size());

	for (size_t triangleIndex = 0; triangleIndex < aabbs.size(); ++triangleIndex)
	{
		const auto& triangle = triangular.triangles_.at(triangleIndex);

		auto& aabb = aabbs.at(triangleIndex);

		for (uint32_t i = 0; i < 3; ++i)
		{
			AaBb::Vertex vertex;
			auto baseVertexIndex = triangle.indices_[i] * 3;

			for (uint32_t j = 0; j < 3; ++j)
			{
				vertex.values_[j] = triangular.vertices_.at(baseVertexIndex + j);
			}

			aabb.grow(vertex);
		}

		aabb.ownerIndices_[0] = triangleIndex;
		aabb.fix();
	}

	for (auto& aabb: aabbs)
	{
		root_.aabb_.grow(aabb);
	}

	leftAmount_.push(1);

	fork(&root_, 0, sharedAabbs, 0);

	{
		std::thread thread(&Bvh::Log::main, &log_);
		thread.detach();
	}
	
	return true;
}

void Bvh::join(bool dumpNodes)
{
	leftAmount_.join();

	if (dumpNodes)
	{
		root_.dump(this);
	}

	log_.join();
}

void Bvh::Log::push(const std::string& line)
{
	std::lock_guard<std::mutex> lk(mutex_);
	lines_.push(line);
}

void Bvh::Log::main()
{
	while (1)
	{
		{
			std::unique_lock<std::mutex> lk(mutex_);

			while (!lines_.empty())
			{
				auto& line = lines_.front();
				std::cout << line << std::endl;
				lines_.pop();

				if (line == "finish")
				{
					return;
				}
			}
		}
	
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}

void Bvh::Log::join()
{
	push("finish");

	while (1)
	{
		{
			std::unique_lock<std::mutex> lk(mutex_);

			if (lines_.empty())
			{
				break;
			}	
		}

		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
}
