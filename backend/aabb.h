#pragma once

#include <vector>

#include <cstdint>
#include <cfloat>

struct AaBb
{
	float mini_[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
	float maxi_[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

	void grow(float vertex[3]);
	float area() const;
	float center(uint32_t axisIndex) const
	{
		return (mini_[axisIndex] + maxi_[axisIndex]) * 0.5;
	}

	void create(const std::vector<float>& vertices, const std::vector<uint32_t>& triangles, uint32_t triangleIndex);

	void merge(const AaBb& aabb0, const AaBb& aabb1);
};
