#include "divider.h"

#include <tuple>
#include <algorithm>

void Divider::process(std::vector<AaBb>& aabbs, uint32_t axisIndex)
{
	std::vector<std::tuple<float, float, uint32_t> > items;

	items.reserve(aabbs.size());

	for (uint32_t i = 0; i < aabbs.size(); ++i)
	{
		auto& aabb = aabbs.at(i);
		items.push_back(std::make_tuple(aabb.center(axisIndex), 1.f / aabb.halfArea(), i));
	}

	std::sort(items.begin(), items.end());

	std::vector<float> leftHalfAreas, rightHalfAreas;

	leftHalfAreas.reserve(aabbs.size() + 1);
	rightHalfAreas.reserve(aabbs.size() + 1);

	leftHalfAreas.push_back(0.f);
	rightHalfAreas.push_back(0.f);

	AaBb left, right;

	for (uint32_t i = 0; i < aabbs.size(); ++i)
	{
		left.grow(aabbs.at(i));
		right.grow(aabbs.at(aabbs.size() - 1 - i));

		leftHalfAreas.push_back(left.halfArea());
		rightHalfAreas.push_back(right.halfArea());
	}

	// TODO
}
