#include "divider.h"

#include <tuple>
#include <algorithm>

#include <cassert>

void Divider::run(std::shared_ptr<std::vector<AaBb> > sharedAabbs)
{
	const auto& aabbs = *sharedAabbs.get();
	assert(1 < aabbs.size());

	{ // sort aabbs
		std::vector<std::tuple<float, float, uint32_t> > items;

		items.reserve(aabbs.size());

		for (uint32_t i = 0; i < aabbs.size(); ++i)
		{
			auto& aabb = aabbs.at(i);
			items.push_back(std::make_tuple(aabb.center(axisIndex_), 1.f / aabb.halfArea(), i));
		}

		std::sort(items.begin(), items.end());

		sortedAabbIndices_.reserve(aabbs.size());

		for (auto& item: items)
		{
			sortedAabbIndices_.push_back(std::get<2>(item));
		}
	}

	std::vector<float> leftHalfAreas, rightHalfAreas;

	leftHalfAreas.reserve(aabbs.size() - 1);
	rightHalfAreas.reserve(aabbs.size() - 1);

	AaBb left, right;

	for (uint32_t i = 1; i < aabbs.size(); ++i)
	{
		auto leftIndex = sortedAabbIndices_.at(i - 1);
		auto rightIndex = sortedAabbIndices_.at(aabbs.size() - i);

		left.grow(aabbs.at(leftIndex));
		right.grow(aabbs.at(rightIndex));
		leftHalfAreas.push_back(left.halfArea());
		rightHalfAreas.push_back(right.halfArea());
	}

	left.grow(aabbs.at(aabbs.size() - 1));
	const auto baseHalfArea = left.halfArea();

	miniCost_ = sah_.kTriangle_ * aabbs.size();

	for (uint32_t i = 1; i < aabbs.size(); ++i)
	{
		auto leftTriangleCount = i;
		auto rightTriangleCount = aabbs.size() - leftTriangleCount;

		auto cost = sah_.kAabb_ + (leftHalfAreas.at(i - 1) * leftTriangleCount + rightHalfAreas.at(aabbs.size() - i) * rightTriangleCount) * sah_.kTriangle_ / baseHalfArea;
		if (cost < miniCost_)
		{
			miniCost_ = cost;
			leftCount_ = i;
		}
	}

	// TODO
}
