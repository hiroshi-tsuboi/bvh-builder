#include "aabb.h"

#include <iostream>
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
		auto value = vertex.values_[i];
		mini_[i] = fmin(mini_[i], value);
		maxi_[i] = fmax(maxi_[i], value);
		center_[i] += value;
	}
}

void AaBb::fix()
{
	auto base = double(vertices_.size());
	for (int i = 0; i < 3; ++i)
	{
		center_[i] /= base;
	}
}

bool AaBb::shrinkIntoLeft(uint32_t axisIndex, double value)
{
	auto& maxi = maxi_[axisIndex];
	if (value < maxi)
	{
		maxi = value;
		assert(mini_[axisIndex] <= maxi);
		return true;
	}
	return false;
}

bool AaBb::shrinkIntoRight(uint32_t axisIndex, double value)
{
	auto& mini = mini_[axisIndex];
	if (mini < value)
	{
		mini = value;
		assert(mini <= maxi_[axisIndex]);
		return true;
	}
	return false;
}

AaBb AaBb::halfLeft(uint32_t axisIndex) const
{
	AaBb aabb;
       
	aabb.grow(*this);

	aabb.maxi_[axisIndex] = origin(axisIndex);

	return aabb;
}

AaBb AaBb::halfRight(uint32_t axisIndex) const
{
	AaBb aabb;
       
	aabb.grow(*this);

	aabb.mini_[axisIndex] = origin(axisIndex);

	return aabb;
}

double AaBb::halfArea() const
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

AaBb AaBb::optimize(const uint32_t axisIndex, const uint32_t sideIndex) const
{
	assert(!vertices_.empty());

	AaBb aabb;

	AaBb::Vertex vertex_j = vertices_.back();
	auto inside_j = inside(vertex_j);

	for (const auto& vertex_i: vertices_)
	{
		const auto inside_i = inside(vertex_i);

		if (inside_i ^ inside_j)
		{
			const auto d = vertex_i.values_[axisIndex] - vertex_j.values_[axisIndex];
			if (d == 0)
			{
				// line on plane
			}
			else
			{
				// Plane : dot(N, P - O) = 0, N = (1,0,0) or (0,1,0) or (0,0,1)
				// Line : P(t) = vertex_i * t + vertex_j * (1 - t), 0 < t < 1
				//
				// dot(N, P(t) - O) = 0
				// P(t) = vertex_i * t + vertex_j * (1 - t)
				// dot(N, vertex_i - vertex_j) * t = dot(N, O - vertex_j)
				// t = dot(N, O - vertex_j) / dot(N, vertex_i - vertex_j)

				auto o = 0 == sideIndex ? mini_[axisIndex] : maxi_[axisIndex];

				auto t = (o - vertex_j.values_[axisIndex]) / d;

				if (0 < t && t < 1)
				{
					AaBb::Vertex vertex;

					vertex.values_[axisIndex] = o;

					for (uint32_t j = 1; j < 3; ++j)
					{
						auto index = (axisIndex + j) % 3;
						// got bad result:  vertex.values_[index] = vertex_i.values_[index] * t +  vertex_j.values_[index] * (1 - t)
						vertex.values_[index] = vertex_j.values_[index];
						vertex.values_[index] += (vertex_i.values_[index] - vertex_j.values_[index]) * t;
					}

					assert(inside(vertex));

					aabb.grow(vertex);
				}
				else
				{
					// vertex[i or j] on plane
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

	return aabb;
}

bool AaBb::validate() const
{
	for (auto& vertex: vertices_)
	{
		if (!inside(vertex))
		{
			return false;
		}
	}

	return !vertices_.empty();
}
