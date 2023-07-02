#include <thread>
#include <memory>

#include "bvh.h"
#include "aabb.h"
#include "divider.h"

bool Bvh::build(const Triangular& triangular)
{
	auto sharedAabbs = std::make_shared<std::vector<AaBb> >();

	auto& aabbs = *sharedAabbs.get();

	aabbs.resize(triangular.indices_.size() / 4);

	for (size_t triangleIndex = 0; triangleIndex < aabbs.size(); ++triangleIndex)
	{
		aabbs.at(triangleIndex).create(triangular.vertices_, triangular.indices_, triangleIndex);
	}

	std::vector<Divider> dividers(3);
	std::vector<std::thread*> threads;
	threads.reserve(dividers.size());
	uint32_t axisIndex = 0;

	for (auto& divider: dividers)
	{
		divider.axisIndex_ = axisIndex++;
		auto thread = new std::thread(&Divider::run, &divider, sharedAabbs);
		threads.push_back(thread);
	}

	for (auto thread: threads)
	{
		if (thread)
		{
			thread->join();
			delete thread;
		}
	}

	for (auto& divider: dividers)
	{
		if (divider.leftCount_ == 0)
		{
			// TODO
			return true;
		}
	}

	float miniCost = FLT_MAX;
	uint32_t miniAxisIndex = 0;
	for (auto& divider: dividers)
	{
		if (divider.miniCost_ < miniCost)
		{
			miniCost = divider.miniCost_;
			miniAxisIndex = divider.axisIndex_;
		}
	}

	// TODO

	return true;
}

