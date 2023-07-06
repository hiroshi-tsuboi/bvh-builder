#include "bvh.h"

#include <tuple>
#include <algorithm>

#include <cassert>
#include <cmath>

void Bvh::divide(Bvh::Node* parent, int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs, std::shared_ptr<Result> sharedResult, uint32_t axisIndex)
{
	const auto& aabbs = *sharedAabbs.get();
	assert(1 < aabbs.size());

	std::vector<uint32_t> sortedAabbIndices;

	{ // sort aabbs
		std::vector<std::tuple<float, float, uint32_t> > items;

		items.reserve(aabbs.size());

		for (uint32_t i = 0; i < aabbs.size(); ++i)
		{
			auto& aabb = aabbs.at(i);
			items.push_back(std::make_tuple(aabb.center(axisIndex), 1.f / aabb.halfArea(), i));
		}

		std::sort(items.begin(), items.end());

		sortedAabbIndices.reserve(aabbs.size());

		for (auto& item: items)
		{
			sortedAabbIndices.push_back(std::get<2>(item));
		}
	}

	// left: increase from minus
	// right: decrease from plus

	std::vector<float> leftHalfAreas, rightHalfAreas;

	leftHalfAreas.reserve(aabbs.size());
	rightHalfAreas.reserve(aabbs.size());

	AaBb leftAabb, rightAabb;

	for (uint32_t i = 0; i < aabbs.size(); ++i)
	{
		auto leftIndex = sortedAabbIndices.at(i);
		auto rightIndex = sortedAabbIndices.at(aabbs.size() - 1 - i);

		leftAabb.grow(aabbs.at(leftIndex).cutRight(axisIndex));
		rightAabb.grow(aabbs.at(rightIndex).cutLeft(axisIndex));
		leftHalfAreas.push_back(leftAabb.halfArea());
		rightHalfAreas.push_back(rightAabb.halfArea());
	}

	leftAabb.grow(aabbs.at(sortedAabbIndices.at(aabbs.size() - 1)));
	const auto baseHalfArea = leftAabb.halfArea();

	uint32_t leftCount = 0;
	auto miniCost = sah_.kTriangle_ * aabbs.size();

	for (uint32_t i = 0; i < aabbs.size(); ++i)
	{
		auto leftTriangleCount = i + 1;
		auto rightTriangleCount = aabbs.size() - i;

		auto cost = sah_.kAabb_ + (leftHalfAreas.at(i) * leftTriangleCount + rightHalfAreas.at(aabbs.size() - 1 - i) * rightTriangleCount) * sah_.kTriangle_ / baseHalfArea;
		if (cost < miniCost)
		{
			miniCost = cost;
			leftCount = leftTriangleCount;
		}
	}

	// wait for threads

	auto& result = *sharedResult.get();
	{
		std::unique_lock<std::mutex> lk(result.mutex_);

		result.miniCosts_[axisIndex] = miniCost;

		++result.finishCount_;

		if (3 == result.finishCount_)
		{
			result.notFull_.notify_all();
		}
		else
		{
			result.notFull_.wait(lk, [&]{ return result.finishCount_ == 3; });
		}
	}

	// select axisIndex to divide

	for (uint32_t i = 0; i < 3; ++i)
	{
		const auto cost = result.miniCosts_[i];
		if (cost < miniCost)
		{
			return;
		}
		if (cost == miniCost)
		{
			if (i < axisIndex)
			{ // may make leaf on any axisIndex
				return;
			}
		}
	}

	// create node or leaf

	if (leftCount == 0)
	{
		auto leaf = new Bvh::Leaf();
		if (leaf == nullptr)
		{
			// fatal error
			return;
		}
		leaf->aabb_ = leftAabb;
		leaf->create(aabbs);
		parent->link(leaf, childIndex);
	}
	else
	{
		auto node = new Bvh::Node();
		if (node == nullptr)
		{
			// fatal error
			return;
		}

		node->aabb_ = leftAabb;
		parent->link(node, childIndex);

		{
			std::lock_guard<std::mutex> lk(mutex_);

			leftAmount_.count_ += 2;
		}

		const float threshold = aabbs.at(sortedAabbIndices.at(leftCount - 1)).center(axisIndex);

		for (int index = 0; index < 2; ++index)
		{
			size_t beginIndex = 0;
			size_t endIndex = leftCount;
			if (0 < index)
			{
				beginIndex = leftCount - 1;
				endIndex = sortedAabbIndices.size();
			}	

			auto sharedChildAabbs = std::make_shared<std::vector<AaBb> >();
			auto& childAabbs = *sharedChildAabbs.get();
			childAabbs.reserve(endIndex - beginIndex);

			for (size_t j = beginIndex; j < endIndex; ++j)
			{
				AaBb aabb = aabbs.at(sortedAabbIndices.at(j));

				if (index == 0)
				{
					aabb.shrinkIntoLeft(axisIndex, threshold);
				}
				else
				{
					aabb.shrinkIntoRight(axisIndex, threshold);
				}

				childAabbs.push_back(aabb);
			}	
	
			if (childAabbs.size() <= sah_.triangleCountThreshold_)
			{
				auto leaf = new Bvh::Leaf();
				if (leaf == nullptr)
				{
					// fatal error
					return;
				}
				leaf->create(childAabbs);
				for (auto& aabb: childAabbs)
				{
					leaf->aabb_.grow(aabb);
				}
				node->link(leaf, index);
				continue;
			}

			fork(node, index, sharedChildAabbs);
		}
	}

	{
		std::lock_guard<std::mutex> lk(mutex_);

		--leftAmount_.count_;

		leftAmount_.signal_.notify_one();
	}
}
