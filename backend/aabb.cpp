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

float AaBb::area() const
{
	auto ex = maxi_[0] - mini_[0];
	auto ey = maxi_[1] - mini_[1];
	auto ez = maxi_[2] - mini_[2];

	auto halfArea = ex * ey + ey * ez + ez * ex;

	return halfArea + halfArea;
}

void AaBb::create(const std::vector<float>& vertices, const std::vector<uint32_t>& triangles, uint32_t triangleIndex)
{
	auto triangleBaseIndex = triangleIndex * 4;
	
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

void AaBb::merge(const AaBb& aabb0, const AaBb& aabb1)
{
	for (int i = 0; i < 3; ++i)
	{
		mini_[i] = fmin(aabb0.mini_[i], aabb1.mini_[i]);
		maxi_[i] = fmax(aabb0.maxi_[i], aabb1.maxi_[i]);
	}
}

