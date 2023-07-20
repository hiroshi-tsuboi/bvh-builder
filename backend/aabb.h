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

	double center_[3] = {0, 0, 0};

	void grow(const AaBb& aabb);
	void grow(const Vertex& vertex);
	void fix(); // finalize center_

	bool shrinkIntoLeft(uint32_t axisIndex, double value);
	bool shrinkIntoRight(uint32_t axisIndex, double value);

	// not copy vertices
	AaBb left(uint32_t axisIndex) const;
	AaBb right(uint32_t axisIndex) const;

	double halfArea() const;
#if 0
	double origin(uint32_t axisIndex) const
	{
		return (mini_[axisIndex] + maxi_[axisIndex]) * 0.5;
	}
#endif
	bool inside(const Vertex& vertex) const;

	AaBb optimize(const uint32_t axisIndex, const uint32_t sideIndex) const;

	bool empty() const
	{
		return vertices_.size() < 3;
	}

	bool validate() const;
};
