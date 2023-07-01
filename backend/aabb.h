#pragma once

#include <vector>

#include <cstdint>
#include <cfloat>

struct AaBb
{
	float mini_[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
	float maxi_[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

	float area_ = 0;

	void grow(float vertex[3]);
	float area();

	void create(std::vector<float>& vertices, std::vector<uint32_t>& triangles, uint32_t triangleIndex);

	void merge(AaBb& aabb0, AaBb& aabb1);
};
