#pragma once

#include "aabb.h"

struct Divider
{
	struct
	{
		float kTriangle_ = 1.f;
		float kAabb_ = 1.f * 2;
	} sah_;

	uint32_t leftCount_ = 0;
	std::vector<uint32_t> sortedAabbIndices_;

	void process(const std::vector<AaBb>& aabbs, uint32_t axisIndex);
};
