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

	std::vector<float> leftHalfAreas, rightHalfAreas;

	leftHalfAreas.reserve(aabbs.size() - 1);
	rightHalfAreas.reserve(aabbs.size() - 1);

	AaBb leftAabb, rightAabb;

	for (uint32_t i = 1; i < aabbs.size(); ++i)
	{
		auto leftIndex = sortedAabbIndices.at(i - 1);
		auto rightIndex = sortedAabbIndices.at(aabbs.size() - i);

		leftAabb.grow(aabbs.at(leftIndex));
		rightAabb.grow(aabbs.at(rightIndex));
		leftHalfAreas.push_back(leftAabb.halfArea());
		rightHalfAreas.push_back(rightAabb.halfArea());
	}

	leftAabb.grow(aabbs.at(sortedAabbIndices.at(aabbs.size() - 1)));
	const auto baseHalfArea = leftAabb.halfArea();

	uint32_t leftCount = 0;
	auto miniCost = sah_.kTriangle_ * aabbs.size();

	for (uint32_t i = 1; i < aabbs.size(); ++i)
	{
		auto leftTriangleCount = i;
		auto rightTriangleCount = aabbs.size() - leftTriangleCount;

		auto cost = sah_.kAabb_ + (leftHalfAreas.at(i - 1) * leftTriangleCount + rightHalfAreas.at(aabbs.size() - i) * rightTriangleCount) * sah_.kTriangle_ / baseHalfArea;
		if (cost < miniCost)
		{
			miniCost = cost;
			leftCount = i;
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

	for (auto cost: result.miniCosts_)
	{
		if (cost < miniCost)
		{
			return;
		}
	}

	if (leftCount == 0)
	{
		auto leaf = new Bvh::Leaf();
		if (leaf == nullptr)
		{
			// fatal error
			return;
		}

		leaf->create(sharedAabbs);
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

		const float threshold = aabbs.at(sortedAabbIndices.at(leftCount - 1)).center(axisIndex);

		for (int index = 0; index < 2; ++index)
		{
			size_t beginIndex = 0;
			size_t endIndex = leftCount;
			if (0 < index)
			{
				beginIndex = leftCount;
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
					aabb.maxi_[axisIndex] = fmin(aabb.maxi_[axisIndex], threshold);
				}
				else
				{
					aabb.mini_[axisIndex] = fmax(aabb.mini_[axisIndex], threshold);
				}

				childAabbs.push_back(aabb);
			}	
				
			fork(node, index, sharedChildAabbs);
		}
	}
}
