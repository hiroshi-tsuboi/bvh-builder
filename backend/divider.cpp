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

	// TODO
}
