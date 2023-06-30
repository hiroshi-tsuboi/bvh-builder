#include "aabb.h"

#include <cmath>

void AaBb::grow(float point[3])
{
	for (int i = 0; i < 3; ++i)
	{
		mini_[i] = fmin(mini_[i], point[i]);
		maxi_[i] = fmax(maxi_[i], point[i]);
	}
}

float AaBb::area()
{
	if (0.f < area_)
	{
		return area_;
	}
	
	float len_i_1 = maxi_[2] - mini_[2];
	for (int i = 0; i < 3; ++i)
	{
		float len_i = maxi_[i] - mini_[i];

		area_ += len_i * len_i_1;

		len_i_1 = len_i;
	}

	area_ *= 2.f;

	return area_;
}
