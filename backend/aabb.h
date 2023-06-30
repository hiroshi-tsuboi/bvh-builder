#pragma once

#include <cfloat>

struct AaBb
{
	float mini_[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
	float maxi_[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

	float area_ = 0;

	void grow(float point[3]);
	float area();
};
