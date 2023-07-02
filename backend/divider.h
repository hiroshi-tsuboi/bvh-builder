#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>

#include "aabb.h"

struct Divider
{
	AaBb sandbox_;

	std::mutex mutex_;
	std::condition_variable notFull_;

	uint32_t finishCount_ = 0;

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
