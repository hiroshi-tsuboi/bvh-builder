#pragma once

#include <memory>
#include "aabb.h"

struct Divider
{
	AaBb sandbox_;

	uint32_t axisIndex_;

	struct
	{
		float kTriangle_ = 1.f;
		float kAabb_ = 1.f * 2;
	} sah_;

	float miniCost_;
	uint32_t leftCount_ = 0;
	std::vector<uint32_t> sortedAabbIndices_;

	void run(std::shared_ptr<std::vector<AaBb> > sharedAabbs);
};
