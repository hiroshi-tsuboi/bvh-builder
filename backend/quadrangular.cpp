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

	for (const auto& [edge, triangles]: edgeLinks)
	{
		if (triangles.size() < 2)
		{
			continue;
		}


		std::vector<AaBb> aabbs;
		std::vector<std::tuple<double, double, uint32_t, uint32_t> > halfAreas;

		for (uint32_t i = 0; i < triangles.size(); ++i)
		{
			if (triangleIndices_.count(triangles.at(i).first))
			{
				continue;
			}

			for (uint32_t j = i + 1; j < triangles.size(); ++j)
			{
				if (triangleIndices_.count(triangles.at(j).first))
				{
					continue;
				}

				// TODO

			}

		}

	}

	// TODO
}
