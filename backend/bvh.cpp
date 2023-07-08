#include <thread>
#include <memory>
#include <iostream>
#include <chrono>

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
		triangleIndices_.push_back(aabb.ownerIndex_);
	}
}

void Bvh::fork(Bvh::Node* parent, int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs, int treeLevel)
{
	auto sharedResult = std::make_shared<Result>();

	for (uint32_t axisIndex = 0; axisIndex < 3; ++axisIndex)
	{
		auto thread = std::thread(&Bvh::divide, this, parent, childIndex, sharedAabbs, sharedResult, axisIndex, treeLevel);
		thread.detach();
	}
}

void Bvh::Result::write(uint32_t index, float value)
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

bool Bvh::build(const Triangular& triangular, int maxTreeLevel)
{
	if (triangular.indices_.empty())
	{
		return true;
	}

	maxTreeLevel_ = maxTreeLevel;

	auto sharedAabbs = std::make_shared<std::vector<AaBb> >();

	auto& aabbs = *sharedAabbs.get();

	aabbs.resize(triangular.indices_.size() / 4);

	for (size_t triangleIndex = 0; triangleIndex < aabbs.size(); ++triangleIndex)
	{
		auto triangle = triangular.at(triangleIndex);

		auto& aabb = aabbs.at(triangleIndex);

		for (int i = 0; i < 3; ++i)
		{
			aabb.grow(triangle.vertices_[i]);
		}

		aabb.ownerIndex_ = triangle.index_;
	}

	for (auto& aabb: aabbs)
	{
		root_.aabb_.grow(aabb);
	}

	leftAmount_.push(1);

	fork(&root_, 0, sharedAabbs, 0);

	{
		auto thread = std::thread(&Bvh::Log::main, &log_);
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
