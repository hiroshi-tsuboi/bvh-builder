#pragma once

#include "aabb.h"

struct Divider
{
	float sah_;
	uint32_t centerIndex_;
	std::vector<uint32_t> aabbIndices_;

	void process(std::vector<AaBb>& aabbs, uint32_t axisIndex);
};
