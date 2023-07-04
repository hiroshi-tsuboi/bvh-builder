#include <thread>
#include <memory>

#include "bvh.h"
#include "aabb.h"

void Bvh::Node::link(Obj* child, int index)
{
	std::lock_guard<std::mutex> lk(mutex_);

	childs_[index] = child;
}

void Bvh::Leaf::create(std::shared_ptr<std::vector<AaBb> > sharedAabbs)
{
	auto& aabbs = *sharedAabbs.get();

	triangleIndices_.reserve(aabbs.size());

	for (auto& aabb: aabbs)
	{
		triangleIndices_.push_back(aabb.ownerIndex_);
	}
}

void Bvh::fork(Bvh::Node* parent, int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs)
{
	std::lock_guard<std::mutex> lk(mutex_);

	++nodeParentCount_;

	auto sharedResult = std::make_shared<Result>();

	for (uint32_t axisIndex = 0; axisIndex < 3; ++axisIndex)
	{
		queue_.push(std::thread(&Bvh::divide, this, parent, childIndex, sharedAabbs, sharedResult, axisIndex));
	}
}

bool Bvh::build(const Triangular& triangular)
{
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

	fork(&root_, 0, sharedAabbs);

	return true;
}

void Bvh::join()
{
	while (1)
	{
		std::unique_lock<std::mutex> lk(mutex_);


		
		

	}
}
