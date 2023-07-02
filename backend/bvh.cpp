#include <thread>
#include <memory>

#include "bvh.h"
#include "aabb.h"
#include "divider.h"

bool Bvh::build(const Triangular& triangular)
{
	auto sharedAabbs = std::make_shared<std::vector<AaBb> >();
	auto sharedResult = std::make_shared<Divider::Result>();

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
		auto thread = new std::thread(&Divider::run, &divider, sharedAabbs, sharedResult, axisIndex++);
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
#if 0
	float miniCost = FLT_MAX;
	int64_t miniAxisIndex = -1;
	for (auto& divider: dividers)
	{
		if (divider.miniCost_ < miniCost)
		{
			miniAxisIndex = divider.axisIndex_;
			miniCost = divider.miniCost_;
		}
	}

	if (miniAxisIndex < 0 || dividers.at(miniAxisIndex).leftCount_ == 0)
	{
		// TODO create leaf
	}
	else
	{
		// TODO divied aabb to create node
	}
#endif
	return true;
}

