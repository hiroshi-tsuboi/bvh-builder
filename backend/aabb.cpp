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

void AaBb::grow(const double vertex[3])
{
	for (int i = 0; i < 3; ++i)
	{
		auto& value = vertex[i];
		mini_[i] = fmin(mini_[i], value);
		maxi_[i] = fmax(maxi_[i], value);
		vertices_.push_back(value);
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

bool AaBb::inside(const double vertex[3]) const
{
	for (int i = 0; i < 3; ++i)
	{
		auto value = vertex[i];
		if (value < mini_[i] || maxi_[i] < value)
		{
			return false; // outside
		}
	}

	return true;
}

AaBb AaBb::optimize() const
{
	AaBb aabb;
	double origin[3][2];

	for (uint32_t axisIndex = 0; axisIndex < 3; ++axisIndex)
	{
		origin[axisIndex][0] = mini_[axisIndex];
		origin[axisIndex][1] = maxi_[axisIndex];
	}

	const auto vertexCount = vertices_.size() / 3;

	auto vertex_j = vertices_.data() + (vertexCount - 1) * 3;
	auto inside_j = inside(vertex_j);

	for (size_t i = 0; i < vertices_.size(); i += 3)
	{
		auto vertex_i = vertices_.data() + i * 3;
		auto inside_i = inside(vertex_i);
		
		if (inside_i ^ inside_j)
		{
			double miniT = 1;
			double miniVertex[3];

			for (uint32_t axisIndex = 0; axisIndex < 3; ++axisIndex)
			{
				// Plane : dot(N, P - O) = 0, N = (1,0,0) or (0,1,0) or (0,0,1)
				// Line : P(t) = vertex_i * t + vertex_j * (1 - t), 0 < t < 0
				//
				// dot(N, P(t) - O) = 0
				// P(t) = vertex_i * t + vertex_j * (1 - t)
				// dot(N, vertex_i - vertex_j) * t = dot(N, O) - vertex_j
				// t = (dot(N, O) - vertex_j) / dot(N, vertex_i - vertex_j)

				auto d = vertex_i[axisIndex] - vertex_j[axisIndex];
				if (d == 0)
				{
					continue;
				}

				for (uint32_t side = 0; side < 2; ++side)
				{
					auto t = (origin[axisIndex][side] - vertex_j[axisIndex]) / d;
					if (t <= 0 || 1 <= t)
					{
						continue;
					}
					if (miniT <= t)
					{
						continue;
					}

					miniT = t;

					for (uint32_t index = 0; index < 3; ++index)
					{
						miniVertex[index] = vertex_i[index] * t + vertex_j[index] * (1 - t);
					}
				}
			}

			assert(0 < miniT && miniT < 1);

			aabb.grow(miniVertex);
		}

		if (inside_i)
		{
			aabb.grow(vertex_i);
		}

		inside_j = inside_i;
		vertex_j = vertex_i;
	}

	return aabb;
}
