#include "aabb.h"

#include <cmath>

void AaBb::grow(float vertex[3])
{
	for (int i = 0; i < 3; ++i)
	{
		mini_[i] = fmin(mini_[i], vertex[i]);
		maxi_[i] = fmax(maxi_[i], vertex[i]);
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

bool AaBb::create(std::vector<float>& vertices, std::vector<uint32_t>& triangles)
{
	for (size_t triangleBaseIndex = 0; triangleBaseIndex < triangles.size(); triangleBaseIndex += 4)
	{
		for (uint32_t i = 0; i < 3; ++i)
		{
			float vertex[3];

			auto vertexBaseIndex = triangles.at(triangleBaseIndex + i) * 3;

			for (uint32_t j = 0; j < 3; ++j)
			{
				vertex[j] = vertices.at(vertexBaseIndex + j);
			}

			grow(vertex);
		}
	}

	return true;
}

