#include <thread>
#include <memory>

#include "bvh.h"
#include "aabb.h"

void Bvh::Node::create(std::shared_ptr<std::vector<AaBb> > sharedAabbs, Obj* parent, int childIndex, Bvh& bvh)
{
	auto& aabbs = *sharedAabbs.get();

	for (auto& aabb: aabbs)
	{
		aabb_.grow(aabb);
	}

	// TODO
}

void Bvh::Leaf::create(std::shared_ptr<std::vector<AaBb> > sharedAabbs, Obj* parent, int childIndex, Bvh& bvh)
{
	auto& aabbs = *sharedAabbs.get();

	triangleIndices_.reserve(aabbs.size());

	for (auto& aabb: aabbs)
	{
		aabb_.grow(aabb);
		triangleIndices_.push_back(aabb.ownerIndex_);
	}

	// TODO
}

bool Bvh::build(const Triangular& triangular)
{
	auto sharedAabbs = std::make_shared<std::vector<AaBb> >();
	auto sharedResult = std::make_shared<Result>();

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

	for (uint32_t axisIndex = 0; axisIndex < 3; ++axisIndex)
	{
		std::lock_guard<std::mutex> lk(mutex_);

		auto thread = new std::thread(&Bvh::divide, this, &root_, sharedAabbs, sharedResult, axisIndex++);
		queue_.push(thread);
	}

	return true;
}

void Bvh::join()
{
	// TODO
}
