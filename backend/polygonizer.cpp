#include "polygonizer.h"

#include <map>
#include <cmath>

bool Polygonizer::create(const Triangular & triangular)
{
	std::map<std::pair<uint32_t, uint32_t>, std::vector<std::pair<uint32_t, uint32_t> > > edgeLinks;
	std::vector<std::tuple<double, double, double> > triangleNormals;

	for (uint32_t triangleIndex = 0; triangleIndex < triangular.triangles_.size(); ++triangleIndex)
	{
		auto& triangle = triangular.triangles_.at(triangleIndex);
		double edges[2][3];

		// link edge

		for (uint32_t i = 0; i < 3; ++i)
		{
			auto vi = triangle.indices_[i];
			auto vj = triangle.indices_[(i + 1) % 3];

			if (i < 3)
			{
				auto& edge = edges[i];
				for (uint32_t j = 0; j <3; ++j)
				{
					edge[j] = double(triangular.vertices_.at(vj * 3 + j)) - double(triangular.vertices_.at(vi * 3 + j));
				}
			}

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

		{ // triangle normal
			double normal[3];
			double d = 0;

			for (uint32_t i = 0; i < 3; ++i)
			{
				auto j = (i + 1) % 3;
				auto k = (i + 2) % 3;

				normal[i] = edges[0][j] * edges[1][k] - edges[0][k] * edges[1][j];

				d += normal[i] * normal[i];
			}

			if (d == 0)
			{
				triangleNormals.push_back(std::make_tuple(d, d, d));
			}
			else
			{
				auto r = sqrt(d);

				triangleNormals.push_back(std::make_tuple(normal[0] / r, normal[1] / r, normal[2] / r));
			}
		}
	}

	// TODO

	return !polygons_.empty();
}
