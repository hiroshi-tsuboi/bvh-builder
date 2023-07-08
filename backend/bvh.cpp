#include <thread>
#include <memory>
#include <iostream>
#include <chrono>

#include "bvh.h"
#include "aabb.h"

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
		aabbs.at(triangleIndex).create(triangular.vertices_, triangular.indices_, triangleIndex);
	}

	for (auto& aabb: aabbs)
	{
		root_.aabb_.grow(aabb);
	}

	leftAmount_.count_ = 1;

	fork(&root_, 0, sharedAabbs, 0);

	{
		auto thread = std::thread(&Bvh::Log::main, &log_);
		thread.detach();
	}
	
	return true;
}

void Bvh::join()
{
	std::unique_lock<std::mutex> lk(mutex_);

	leftAmount_.signal_.wait(lk, [&]{ return leftAmount_.count_ == 0; });

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
