#pragma once

#include <vector>

#include <cstdint>
#include <cfloat>

struct AaBb
{
	float mini_[3] = {FLT_MAX, FLT_MAX, FLT_MAX};
	float maxi_[3] = {-FLT_MAX, -FLT_MAX, -FLT_MAX};

	int64_t ownerIndex_ = -1;

	void grow(const AaBb& aabb);
	void grow(float vertex[3]);

	void shrinkMin(int axisIndex, float value)
	{
		auto& mini = mini_[axisIndex];
		if (mini < value)
		{
			mini = value;
		}
	}
	void shrinkMax(int axisIndex, float value)
	{
		auto& maxi = maxi_[axisIndex];
		if (value < maxi)
		{
			maxi = value;
		}
	}

	void minimize(const AaBb& sandbox);

	float halfArea() const;
	float center(uint32_t axisIndex) const
	{
		return (mini_[axisIndex] + maxi_[axisIndex]) * 0.5;
	}

	void create(const std::vector<float>& vertices, const std::vector<uint32_t>& triangles, uint32_t triangleIndex);
};
