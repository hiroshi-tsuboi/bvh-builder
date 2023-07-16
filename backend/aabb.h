#pragma once

#include <vector>

#include <cstdint>
#include <cfloat>

struct AaBb
{
	double mini_[3] = {DBL_MAX, DBL_MAX, DBL_MAX};
	double maxi_[3] = {-DBL_MAX, -DBL_MAX, -DBL_MAX};

	int64_t ownerIndex_ = -1;

	struct Vertex
	{
		double values_[3]; // (x,y,z)
	};

	std::vector<Vertex> vertices_;

	void grow(const AaBb& aabb);
	void grow(const Vertex& vertex);

	bool shrinkIntoLeft(uint32_t axisIndex, double value);
	bool shrinkIntoRight(uint32_t axisIndex, double value);

	// not copy vertices
	AaBb halfLeft(uint32_t axisIndex) const;
	AaBb halfRight(uint32_t axisIndex) const;

	double halfArea() const;
	double center(uint32_t axisIndex) const
	{
		return (mini_[axisIndex] + maxi_[axisIndex]) * 0.5;
	}

	bool inside(const Vertex& vertex) const;

	AaBb optimize(const uint32_t axisIndex, const uint32_t sideIndex) const;

	bool empty() const
	{
		return vertices_.size() < 3;
	}

	bool validate() const;
};
