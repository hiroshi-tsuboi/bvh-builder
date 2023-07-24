#include "quadrangular.h"

#include <iostream>
#include <map>
#include <algorithm>
#include <cmath>
#include <cassert>

void Quadrangular::create(const Triangular & triangular)
{
	std::map<std::pair<uint32_t, uint32_t>, std::vector<std::pair<uint32_t, uint32_t> > > edgeLinks;

	for (uint32_t triangleIndex = 0; triangleIndex < triangular.triangles_.size(); ++triangleIndex)
	{
		auto& triangle = triangular.triangles_.at(triangleIndex);

		// link edge

		for (uint32_t i = 0; i < 3; ++i)
		{
			auto vi = triangle.indices_[i];
			auto vj = triangle.indices_[(i + 1) % 3];

			if (vj < vi)
			{
				auto tmp = vi;
				vi = vj;
				vj = tmp;
			}

			auto key = std::make_pair(vi, vj);

			auto& triangles = edgeLinks[key];

			triangles.push_back(std::make_pair(triangleIndex, i));
		}
	}

	std::vector<AaBb> aabbs;
	std::vector<std::tuple<double, double, uint32_t> > items;

	for (const auto& [edge, triangles]: edgeLinks)
	{
		if (triangles.size() < 2)
		{
			continue;
		}

		for (uint32_t i = 0; i < triangles.size(); ++i)
		{
			if (triangleIndices_.count(triangles.at(i).first))
			{
				continue;
			}

			auto& triangle_i = triangular.triangles_.at(triangles.at(i).first);

			for (uint32_t j = i + 1; j < triangles.size(); ++j)
			{
				if (triangleIndices_.count(triangles.at(j).first))
				{
					continue;
				}

				auto& triangle_j = triangular.triangles_.at(triangles.at(j).first);

				uint32_t vertexIndices[4];

				vertexIndices[0] = triangle_i.indices_[(triangles.at(i).second + 2) % 3];
				vertexIndices[1] = triangle_i.indices_[triangles.at(i).second];
				vertexIndices[2] = triangle_j.indices_[(triangles.at(j).second + 2) % 3];
				vertexIndices[3] = triangle_j.indices_[triangles.at(j).second];

				assert(triangle_j.indices_[triangles.at(j).second] == triangle_i.indices_[(triangles.at(i).second + 1) % 3]);

				AaBb aabb;

				for (auto index: vertexIndices)
				{
					AaBb::Vertex vertex;
					index *= 3;
					for (auto& value: vertex.values_)
					{
						value = triangular.vertices_.at(index++);
					}
					aabb.grow(vertex);
				}

				{ // check for convex
					AaBb::Vertex vertex_j;
					double edges[4][3];

					vertex_j = aabb.vertices_.front();
					for (uint32_t i = 0; i < 4; ++i)
					{
						auto& vertex_i = aabb.vertices_.at((i + 1) % 4);
						for (uint32_t k = 0; k < 3; ++k)
						{
							edges[i][k] = vertex_i.values_[k] - vertex_j.values_[k];
						}
						vertex_j = vertex_i;
					}

					double normals[2][3];

					for (uint32_t normalIndex = 0; normalIndex < 2; ++normalIndex)
					{
						auto& normal = normals[normalIndex];
						auto& e0 = edges[normalIndex * 2];
						auto& e1 = edges[normalIndex * 2 + 1];
						for (uint32_t i = 0; i < 3; ++i)
						{
							uint32_t j = (i + 1) % 3;
							uint32_t k = (i + 2) % 3;
							normal[i] = e0[j] * e1[k] - e0[k] * e1[j];
						}
					}

					if ((normals[0][0] * normals[1][0] + normals[0][1] * normals[1][1] + normals[0][2] * normals[1][2]) <= 0)
					{
						continue;
					}
				}

				aabb.ownerIndices_[0] = triangles.at(i).first;
				aabb.ownerIndices_[1] = triangles.at(j).first;

				auto aabbIndex = uint32_t(aabbs.size());

				aabbs.push_back(aabb);

				items.push_back(std::make_tuple(aabb.volume(), aabb.halfArea(), aabbIndex));
			}
		}
	}

	std::sort(items.begin(), items.end());

	for (auto& item: items)
	{
		auto& aabb = aabbs.at(std::get<2>(item));

		if (triangleIndices_.count(aabb.ownerIndices_[0])) continue;
		if (triangleIndices_.count(aabb.ownerIndices_[1])) continue;

		aabbs_.push_back(aabb);

		triangleIndices_.insert(aabb.ownerIndices_[0]);
		triangleIndices_.insert(aabb.ownerIndices_[1]);
	}
}
