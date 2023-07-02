#pragma once

#include <memory>
#include <mutex>
#include <condition_variable>

#include "aabb.h"

struct Divider
{
	AaBb sandbox_;

	struct Result
	{
		std::mutex mutex_;
		std::condition_variable notFull_;
		uint32_t finishCount_ = 0;
		float miniCosts_[3];
	};

	struct
	{
		const float kTriangle_ = 1.f;
		const float kAabb_ = 1.f * 2;
	} sah_;

	std::vector<uint32_t> sortedAabbIndices_;

	void run(std::shared_ptr<std::vector<AaBb> > sharedAabbs, std::shared_ptr<Result> sharedResult, uint32_t axisIndex);
};
