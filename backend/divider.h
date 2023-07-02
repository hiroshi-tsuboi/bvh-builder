#pragma once

#include <memory>
#include <mutex>

#include "aabb.h"

struct Divider
{
	AaBb sandbox_;

	std::mutex mutex_;

	struct
	{
		float kTriangle_ = 1.f;
		float kAabb_ = 1.f * 2;
	} sah_;

	float miniCost_;
	uint32_t leftCount_ = 0;
	std::vector<uint32_t> sortedAabbIndices_;

	void run(std::shared_ptr<std::vector<AaBb> > sharedAabbs, uint32_t axisIndex);
};
