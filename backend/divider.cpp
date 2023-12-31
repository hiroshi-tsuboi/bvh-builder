#include "bvh.h"

#include <tuple>
#include <algorithm>

#include <cassert>
#include <cmath>

void Bvh::divide(Bvh::Node* parent, const int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs, std::shared_ptr<Result> sharedResult, const uint32_t axisIndex, int treeLevel)
{
	const auto& aabbs = *sharedAabbs.get();
	assert(1 < aabbs.size());

	std::vector<uint32_t> sortedAabbIndices;

	{ // sort aabbs
		std::vector<std::tuple<double, double, double, uint32_t> > items;

		items.reserve(aabbs.size());

		for (uint32_t i = 0; i < aabbs.size(); ++i)
		{
			auto& aabb = aabbs.at(i);
			items.push_back(std::make_tuple(aabb.center_[axisIndex], aabb.center_[(axisIndex + 1) % 3], aabb.center_[(axisIndex + 2) % 3], i));
		}

		std::sort(items.begin(), items.end());

		sortedAabbIndices.reserve(aabbs.size());

		for (auto& item: items)
		{
			sortedAabbIndices.push_back(std::get<3>(item));
		}
	}

	// left: increase from minus
	// right: decrease from plus

	std::vector<double> leftHalfAreas, rightHalfAreas;

	leftHalfAreas.reserve(aabbs.size());
	rightHalfAreas.reserve(aabbs.size());

	AaBb leftAabb, rightAabb;

	for (uint32_t i = 0; i < aabbs.size(); ++i)
	{
		auto leftIndex = sortedAabbIndices.at(i);
		auto rightIndex = sortedAabbIndices.at(aabbs.size() - 1 - i);

		leftAabb.grow(aabbs.at(leftIndex).left(axisIndex));
		rightAabb.grow(aabbs.at(rightIndex).right(axisIndex));
		leftHalfAreas.push_back(leftAabb.halfArea());
		rightHalfAreas.push_back(rightAabb.halfArea());
	}

	leftAabb.grow(aabbs.at(sortedAabbIndices.at(aabbs.size() - 1)));
	const auto baseHalfArea = leftAabb.halfArea();

	uint32_t leftCount = 0;
	auto miniCost = sah_.kTriangle_ * aabbs.size();

	for (uint32_t i = 0; i < aabbs.size(); ++i)
	{
		auto leftTriangleCount = i + 1;
		auto rightTriangleCount = aabbs.size() - i;

		auto cost = sah_.kAabb_ + (leftHalfAreas.at(i) * leftTriangleCount + rightHalfAreas.at(aabbs.size() - 1 - i) * rightTriangleCount) * sah_.kTriangle_ / baseHalfArea;
		if (cost < miniCost)
		{
			miniCost = cost;
			leftCount = leftTriangleCount;
		}
	}

	// wait for sibling threads

	auto& result = *sharedResult.get();

	result.write(axisIndex, miniCost);

	// select axisIndex to divide

	bool waitForAll = true;

	for (uint32_t i = 1; i < 3; ++i)
	{
		auto j = (axisIndex + i) % 3;
		const auto cost = result.miniCosts_[j];
		if (cost < miniCost)
		{
			waitForAll = false;
			break;
		}
		if (cost == miniCost)
		{
			if (j < axisIndex)
			{ // may make leaf on any axisIndex
				waitForAll = false;
				break;
			}
		}
	}

	result.join(waitForAll);

	if (!waitForAll)
	{
		return;
	}

	// create node or leaf

	if (leftCount == 0)
	{
		auto leaf = new Bvh::Leaf();
		if (leaf == nullptr)
		{
			// fatal error
			return;
		}
		leaf->aabb_.grow(leftAabb);
		leaf->create(aabbs);
		parent->link(leaf, childIndex);
	}
	else if (++treeLevel < maxTreeLevel_)
	{
		auto node = new Bvh::Node();
		if (node == nullptr)
		{
			// fatal error
			return;
		}

		node->aabb_.grow(leftAabb);
		parent->link(node, childIndex);

		leftAmount_.push(2);

		const auto threshold = aabbs.at(sortedAabbIndices.at(leftCount - 1)).center_[axisIndex];

		for (int sideIndex = 0; sideIndex < 2; ++sideIndex)
		{
			size_t beginIndex = 0;
			size_t endIndex = leftCount;
			if (0 < sideIndex)
			{
				beginIndex = leftCount - 1;
				endIndex = sortedAabbIndices.size();
			}	

			auto sharedChildAabbs = std::make_shared<std::vector<AaBb> >();
			auto& childAabbs = *sharedChildAabbs.get();
			childAabbs.reserve(endIndex - beginIndex);

			for (size_t j = beginIndex; j < endIndex; ++j)
			{
				AaBb aabb = aabbs.at(sortedAabbIndices.at(j));
				if (sideIndex == 0)
				{
					if (!aabb.shrinkIntoLeft(axisIndex, threshold))
					{
						childAabbs.push_back(aabb);
						continue;
					}
				}
				else
				{
					if (!aabb.shrinkIntoRight(axisIndex, threshold))
					{
						childAabbs.push_back(aabb);
						continue;
					}
				}

				auto optimizedAabb = aabb.optimize(axisIndex, sideIndex ^ 1);

				if (optimizedAabb.empty())
				{
					continue;
				}

				childAabbs.push_back(optimizedAabb);
			}	
	
			if (childAabbs.size() <= sah_.kTriangleCountThreshold_)
			{
				auto leaf = new Bvh::Leaf();
				if (leaf == nullptr)
				{
					// fatal error
					return;
				}
				leaf->create(childAabbs);
				for (auto& aabb: childAabbs)
				{
					leaf->aabb_.grow(aabb);
				}
				node->link(leaf, sideIndex);
				leftAmount_.pop();
				continue;
			}

			fork(node, sideIndex, sharedChildAabbs, treeLevel);

			if (sideIndex == 0)
			{
				std::this_thread::yield();
			}
		}
	}
	else
	{
		// debug code
		log_.push(std::string("left triangles = ") + std::to_string(aabbs.size()));
	}

	leftAmount_.pop();
}
