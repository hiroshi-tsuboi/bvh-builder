#include "divider.h"

#include <utility>
#include <algorithm>

void Divider::process(std::vector<AaBb>& aabbs, uint32_t axisIndex)
{
	std::vector<std::pair<float, uint32_t> > centers;

	centers.reserve(aabbs.size());

	for (uint32_t i = 0; i < aabbs.size(); ++i)
	{
		centers.push_back(std::make_pair(aabbs.at(i).center(axisIndex), i));
	}

	std::sort(centers.begin(), centers.end());

	// TODO
}
