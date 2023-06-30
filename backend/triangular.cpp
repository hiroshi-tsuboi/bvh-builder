#include "triangular.h"

#include <map>
#include <tuple>

bool Triangular::create(std::vector<float>& vertices, uint32_t vertexOffset, uint32_t vertexStride, std::vector<std::vector<uint32_t> >& polygons)
{
	std::map<std::tuple<float, float, float>, uint32_t> vertexMap;

	for (auto& indices: polygons)
	{
		auto indexSize = indices.size();
		for (size_t baseIndex = 0; (baseIndex + 2) < indexSize; ++baseIndex)
		{ // [0,1,2], [0,2,3], [0,3,4], [0,4,5], ...
			for (uint32_t i = 0; i < 3; ++i)
			{
				auto index = indices.at(0 == i ? 0 : (baseIndex + i));
				auto baseIndex = index * vertexStride + vertexOffset;

				auto x = vertices.at(baseIndex);
				auto y = vertices.at(baseIndex + 1);
				auto z = vertices.at(baseIndex + 2);

				auto vertex = std::make_tuple(x, y, z);

				if (vertexMap.count(vertex) == 0)
				{
					vertexMap.emplace(vertex, uint32_t(vertexMap.size()));
				}

				indices_.push_back(vertexMap.at(vertex));
			}
		}
	}

	vertices_.resize(vertexMap.size() * 3);
	for (const auto& [vertex, index]: vertexMap)
	{
		auto offset = index * 3;
		vertices_.at(offset) = std::get<0>(vertex);
		vertices_.at(offset + 1) = std::get<1>(vertex);
		vertices_.at(offset + 2) = std::get<2>(vertex);
	}

	return true;
}
