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

void AaBb::grow(const Vertex& vertex)
{
	vertices_.push_back(vertex);
	for (int i = 0; i < 3; ++i)
	{
		auto& value = vertex.values_[i];
		mini_[i] = fmin(mini_[i], value);
		maxi_[i] = fmax(maxi_[i], value);
	}
}

bool AaBb::shrinkIntoLeft(uint32_t axisIndex, float value)
{
	auto& maxi = maxi_[axisIndex];
	if (value < maxi)
	{
		maxi = value;
		return true;
	}
	return false;
}

bool AaBb::shrinkIntoRight(uint32_t axisIndex, float value)
{
	auto& mini = mini_[axisIndex];
	if (mini < value)
	{
		mini = value;
		return true;
	}
	return false;
}

AaBb AaBb::halfLeft(uint32_t axisIndex) const
{
	AaBb aabb;
       
	aabb.grow(*this);

	aabb.maxi_[axisIndex] = center(axisIndex);

	return aabb;
}

AaBb AaBb::halfRight(uint32_t axisIndex) const
{
	AaBb aabb;
       
	aabb.grow(*this);

	aabb.mini_[axisIndex] = center(axisIndex);

	return aabb;
}

float AaBb::halfArea() const
{
	auto ex = maxi_[0] - mini_[0];
	auto ey = maxi_[1] - mini_[1];
	auto ez = maxi_[2] - mini_[2];

	return ex * ey + ey * ez + ez * ex;
}

bool AaBb::inside(const Vertex& vertex) const
{
	for (int i = 0; i < 3; ++i)
	{
		auto value = vertex.values_[i];
		if (value < mini_[i] || maxi_[i] < value)
		{
			return false; // outside
		}
	}

	return true;
}

AaBb AaBb::optimize() const
{
	assert(!vertices_.empty());

	AaBb aabb;
	double origin[3][2];

	for (uint32_t axisIndex = 0; axisIndex < 3; ++axisIndex)
	{
		origin[axisIndex][0] = mini_[axisIndex];
		origin[axisIndex][1] = maxi_[axisIndex];
		if (maxi_[axisIndex] < mini_[axisIndex])
		{
			return aabb;
		}
	}

	AaBb::Vertex vertex_j = vertices_.back();
	auto inside_j = inside(vertex_j);

	for (const auto& vertex_i: vertices_)
	{
		const auto inside_i = inside(vertex_i);
		
		if (inside_i ^ inside_j)
		{
			for (uint32_t axisIndex = 0; axisIndex < 3; ++axisIndex)
			{
				// Plane : dot(N, P - O) = 0, N = (1,0,0) or (0,1,0) or (0,0,1)
				// Line : P(t) = vertex_i * t + vertex_j * (1 - t), 0 < t < 1
				//
				// dot(N, P(t) - O) = 0
				// P(t) = vertex_i * t + vertex_j * (1 - t)
				// dot(N, vertex_i - vertex_j) * t = dot(N, O - vertex_j)
				// t = dot(N, O - vertex_j) / dot(N, vertex_i - vertex_j)

				auto d = vertex_i.values_[axisIndex] - vertex_j.values_[axisIndex];
				if (d == 0)
				{
					continue;
				}

				for (uint32_t side = 0; side < 2; ++side)
				{
					auto t = (origin[axisIndex][side] - vertex_j.values_[axisIndex]) / d;
					if (t <= 0 || 1 <= t)
					{
						continue;
					}

					AaBb::Vertex vertex;

					for (uint32_t index = 0; index < 3; ++index)
					{
						vertex.values_[index] = vertex_i.values_[index] * t + vertex_j.values_[index] * (1 - t);
					}

					if (inside(vertex))
					{
						aabb.grow(vertex);
						axisIndex = 3;
						break;
					}
				}
			}
		}

		if (inside_i)
		{
			aabb.grow(vertex_i);
		}

		inside_j = inside_i;
		vertex_j = vertex_i;
	}

	assert(!aabb.vertices_.empty());

	return aabb;
}
