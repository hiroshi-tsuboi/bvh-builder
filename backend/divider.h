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

	const struct
	{
		float kTriangle_ = 1.f;
		float kAabb_ = 1.f * 2;
	} sah_;

	void run(std::shared_ptr<std::vector<AaBb> > sharedAabbs, std::shared_ptr<Result> sharedResult, uint32_t axisIndex);
};
