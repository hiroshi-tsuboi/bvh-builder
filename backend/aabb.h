#pragma once

#include <vector>

#include <cstdint>
#include <cfloat>

struct AaBb
{
	float mini_[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
	float maxi_[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

	int64_t ownerIndex_ = -1;

	struct Vertex
	{
		double values_[3]; // (x,y,z)
	};

	std::vector<Vertex> vertices_;

	void grow(const AaBb& aabb);
	void grow(const Vertex& vertex);

	bool shrinkIntoLeft(uint32_t axisIndex, float value);
	bool shrinkIntoRight(uint32_t axisIndex, float value);

	AaBb halfLeft(uint32_t axisIndex) const;
	AaBb halfRight(uint32_t axisIndex) const;

	float halfArea() const;
	float center(uint32_t axisIndex) const
	{
		return (mini_[axisIndex] + maxi_[axisIndex]) * 0.5;
	}

	bool inside(const Vertex& vertex) const;

	AaBb optimize() const;

	bool empty()
	{
		return vertices_.size() < 3;
	}
};
