#include "divider.h"

#include <tuple>
#include <algorithm>

#include <cassert>

void Divider::process(std::vector<AaBb>& aabbs, uint32_t axisIndex)
{
	assert(1 < aabbs.size());

	std::vector<std::tuple<float, float, uint32_t> > items;

	items.reserve(aabbs.size());

	for (uint32_t i = 0; i < aabbs.size(); ++i)
	{
		auto& aabb = aabbs.at(i);
		items.push_back(std::make_tuple(aabb.center(axisIndex), 1.f / aabb.halfArea(), i));
	}

	std::sort(items.begin(), items.end());

	std::vector<float> leftHalfAreas, rightHalfAreas;

	leftHalfAreas.reserve(aabbs.size() - 1);
	rightHalfAreas.reserve(aabbs.size() - 1);

	AaBb left, right;

	for (uint32_t i = 1; i < aabbs.size(); ++i)
	{
		left.grow(aabbs.at(i - 1));
		right.grow(aabbs.at(aabbs.size() - i));
		leftHalfAreas.push_back(left.halfArea());
		rightHalfAreas.push_back(right.halfArea());
	}

	left.grow(aabbs.at(aabbs.size() - 1));
	const auto baseHalfArea = left.halfArea();

	float miniCost = sah_.kTriangle_ * aabbs.size();

	for (uint32_t i = 1; i < aabbs.size(); ++i)
	{
		auto leftTriangleCount = i;
		auto rightTriangleCount = aabbs.size() - leftTriangleCount;

		auto cost = sah_.kAabb_ + (leftHalfAreas.at(i - 1) * leftTriangleCount + rightHalfAreas.at(aabbs.size() - i) * rightTriangleCount) * sah_.kTriangle_ / baseHalfArea;
		if (cost < miniCost)
		{
			miniCost = cost;
			leftCount_ = i;
		}
	}

	// TODO
}
