#include "linkage.h"

void Linkage::create(const Triangular& triangular)
{
	for (uint32_t triangleIndex = 0; triangleIndex < triangular.triangles_.size(); ++triangleIndex)
	{
		auto& triangle = triangular.triangles_.at(triangleIndex);

		// link edge

		for (uint32_t edgeIndex = 0; edgeIndex < 3; ++edgeIndex)
		{
			auto vi = triangle.indices_[edgeIndex];
			auto vj = triangle.indices_[(edgeIndex + 1) % 3];

			if (vj < vi)
			{
				auto tmp = vi;
				vi = vj;
				vj = tmp;
			}

			auto key = std::make_pair(vi, vj);

			if (count(key) == 0)
			{
				emplace(key, std::vector<std::pair<uint32_t, uint32_t> >());
			}

			auto& edgeIndices = at(key);

			edgeIndices.push_back(std::make_pair(triangleIndex, edgeIndex));
		}
	}
}
