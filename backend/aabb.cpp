#include "aabb.h"

#include <cmath>

void AaBb::grow(const AaBb& aabb)
{
	for (int i = 0; i < 3; ++i)
	{
		mini_[i] = fmin(mini_[i], aabb.mini_[i]);
		maxi_[i] = fmax(maxi_[i], aabb.maxi_[i]);
	}
}

void AaBb::grow(float vertex[3])
{
	for (int i = 0; i < 3; ++i)
	{
		mini_[i] = fmin(mini_[i], vertex[i]);
		maxi_[i] = fmax(maxi_[i], vertex[i]);
	}
}

void AaBb::minimize(const AaBb& sandbox)
{
	for (int i = 0; i < 3; ++i)
	{
		mini_[i] = fmax(mini_[i], sandbox.mini_[i]);
		maxi_[i] = fmin(maxi_[i], sandbox.maxi_[i]);
	}
}

float AaBb::halfArea() const
{
	auto ex = maxi_[0] - mini_[0];
	auto ey = maxi_[1] - mini_[1];
	auto ez = maxi_[2] - mini_[2];

	return ex * ey + ey * ez + ez * ex;
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
