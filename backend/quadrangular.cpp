#include "quadrangular.h"

#include <iostream>
#include <map>
#include <cmath>

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

				std::vector<uint32_t> vertexIndices;

				vertexIndices.push_back(triangle_i.indices_[(triangles.at(i).second + 2) % 3]);
				vertexIndices.push_back(triangle_i.indices_[triangles.at(i).second]);
				vertexIndices.push_back(triangle_j.indices_[(triangles.at(j).second + 2) % 3]);
				vertexIndices.push_back(triangle_j.indices_[triangles.at(j).second]);

				AaBb aabb;

				aabb.ownerIndices_[0] = triangles.at(i).first;
				aabb.ownerIndices_[1] = triangles.at(j).first;

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

				auto aabbIndex = uint32_t(aabbs.size());

				aabbs.push_back(aabb);

				items.push_back(std::make_tuple(aabb.volume(), aabb.halfArea(), aabbIndex));
			}
		}
	}

	// TODO
}
