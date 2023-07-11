#include "aabb.h"

#include <cmath>
#include <cassert>

void AaBb::grow(const AaBb& aabb)
{
	for (int i = 0; i < 3; ++i)
	{
		mini_[i] = fmin(mini_[i], aabb.mini_[i]);
		maxi_[i] = fmax(maxi_[i], aabb.maxi_[i]);
	}
}

void AaBb::grow(const float vertex[3])
{
	for (int i = 0; i < 3; ++i)
	{
		auto& value = vertex[i];
		mini_[i] = fmin(mini_[i], value);
		maxi_[i] = fmax(maxi_[i], value);
		vertices_.push_back(double(value));
	}
}

bool AaBb::shrinkIntoLeft(uint32_t axisIndex, float value)
{
	auto& maxi = maxi_[axisIndex];
	if (value < maxi)
	{
		maxi = value;
		if (halfArea() <= 0.f)
		{
			return false;
		}
	}
	return mini_[axisIndex] <= maxi;
}

bool AaBb::shrinkIntoRight(uint32_t axisIndex, float value)
{
	auto& mini = mini_[axisIndex];
	if (mini < value)
	{
		mini = value;
		if (halfArea() <= 0.f)
		{
			return false;
		}
	}
	return mini <= maxi_[axisIndex];
}

AaBb AaBb::cutHalfLeft(uint32_t axisIndex) const
{
	AaBb aabb;
       
	aabb = *this;

	aabb.mini_[axisIndex] = center(axisIndex);

	return aabb;
}

AaBb AaBb::cutHalfRight(uint32_t axisIndex) const
{
	AaBb aabb;
       
	aabb = *this;

	aabb.maxi_[axisIndex] = center(axisIndex);

	return aabb;
}

float AaBb::halfArea() const
{
	auto ex = maxi_[0] - mini_[0];
	auto ey = maxi_[1] - mini_[1];
	auto ez = maxi_[2] - mini_[2];

	return ex * ey + ey * ez + ez * ex;
}
