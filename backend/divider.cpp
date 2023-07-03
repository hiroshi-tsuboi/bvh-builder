#include "bvh.h"

#include <tuple>
#include <algorithm>

#include <cassert>

void Bvh::divide(Bvh::Node* parent, int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs, std::shared_ptr<Result> sharedResult, uint32_t axisIndex)
{
	const auto& aabbs = *sharedAabbs.get();
	assert(1 < aabbs.size());

	std::vector<uint32_t> sortedAabbIndices;

	{ // sort aabbs
		std::vector<std::tuple<float, float, uint32_t> > items;

		items.reserve(aabbs.size());

		for (uint32_t i = 0; i < aabbs.size(); ++i)
		{
			auto& aabb = aabbs.at(i);
			items.push_back(std::make_tuple(aabb.center(axisIndex), 1.f / aabb.halfArea(), i));
		}

		std::sort(items.begin(), items.end());

		sortedAabbIndices.reserve(aabbs.size());

		for (auto& item: items)
		{
			sortedAabbIndices.push_back(std::get<2>(item));
		}
	}

	std::vector<float> leftHalfAreas, rightHalfAreas;

	leftHalfAreas.reserve(aabbs.size() - 1);
	rightHalfAreas.reserve(aabbs.size() - 1);

	AaBb left, right;

	for (uint32_t i = 1; i < aabbs.size(); ++i)
	{
		auto leftIndex = sortedAabbIndices.at(i - 1);
		auto rightIndex = sortedAabbIndices.at(aabbs.size() - i);

		left.grow(aabbs.at(leftIndex));
		right.grow(aabbs.at(rightIndex));
		leftHalfAreas.push_back(left.halfArea());
		rightHalfAreas.push_back(right.halfArea());
	}

	left.grow(aabbs.at(aabbs.size() - 1));
	const auto baseHalfArea = left.halfArea();

	uint32_t leftCount = 0;
	auto miniCost = sah_.kTriangle_ * aabbs.size();

	for (uint32_t i = 1; i < aabbs.size(); ++i)
	{
		auto leftTriangleCount = i;
		auto rightTriangleCount = aabbs.size() - leftTriangleCount;

		auto cost = sah_.kAabb_ + (leftHalfAreas.at(i - 1) * leftTriangleCount + rightHalfAreas.at(aabbs.size() - i) * rightTriangleCount) * sah_.kTriangle_ / baseHalfArea;
		if (cost < miniCost)
		{
			miniCost = cost;
			leftCount = i;
		}
	}

	// wait for threads

	auto& result = *sharedResult.get();
	{
		std::unique_lock<std::mutex> lk(result.mutex_);

		result.miniCosts_[axisIndex] = miniCost;

		++result.finishCount_;

		if (3 == result.finishCount_)
		{
			result.notFull_.notify_all();
		}
		else
		{
			result.notFull_.wait(lk, [&]{ return result.finishCount_ == 3; });
		}
	}

	for (auto cost: result.miniCosts_)
	{
		if (cost < miniCost)
		{
			return;
		}
	}

	auto parentNode = dynamic_cast<Bvh::Node*>(parent);

	assert(parentNode != nullptr);

	if (leftCount == 0)
	{
		auto leaf = new Bvh::Leaf();
		if (leaf == nullptr)
		{
			// fatal error
			return;
		}

		leaf->create(sharedAabbs);
		parentNode->link(leaf, childIndex);
	}
	else
	{
		auto childNode = new Bvh::Node();
		if (childNode == nullptr)
		{
			// fatal error
			return;
		}

		// TODO childNode->create()
		parentNode->link(childNode, childIndex);

		//const uint32_t orderIndices[3] = {0, leftCount, aabbs.size()};
		for (int childIndex = 0; childIndex < 2; ++childIndex)
		{
			auto sharedChildAabbs = std::make_shared<std::vector<AaBb> >();

			// TODO copy left(childIndex == 0) or right(childIndex == 1) aabbs
				
			fork(childNode, childIndex, sharedChildAabbs);
		}
	}
}
