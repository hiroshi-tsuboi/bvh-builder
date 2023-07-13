#include "triangular.h"
#include "matrix3x3.h"

#include <map>
#include <tuple>

bool Triangular::create(std::vector<float>& vertices, uint32_t vertexOffset, uint32_t vertexStride, std::vector<std::vector<uint32_t> >& polygons)
{
	std::map<std::tuple<float, float, float>, uint32_t> vertexMap;

	{
		size_t triangleCount = 0;
		for (auto& indices: polygons)
		{
			triangleCount += indices.size() - 2;
		}
		triangles_.reserve(triangleCount);
	}

	uint32_t polygonIndex = 0;
	for (auto& indices: polygons)
	{
		auto indexSize = indices.size();
		for (size_t cornerIndex = 0; (cornerIndex + 2) < indexSize; ++cornerIndex)
		{ // (0,1,2), (0,2,3), (0,3,4), (0,4,5) ...
			Triangle triangle;
			Matrix3x3 matrix;

			triangle.indices_[3] = polygonIndex;
			for (uint32_t i = 0; i < 3; ++i)
			{
				auto index = indices.at(0 == i ? 0 : (cornerIndex + i));
				auto baseIndex = index * vertexStride + vertexOffset;

				auto x = vertices.at(baseIndex);
				auto y = vertices.at(baseIndex + 1);
				auto z = vertices.at(baseIndex + 2);

				auto vertex = std::make_tuple(x, y, z);

				if (vertexMap.count(vertex) == 0)
				{
					vertexMap.emplace(vertex, uint32_t(vertexMap.size()));
				}

				triangle.indices_[i] = vertexMap.at(vertex);

				matrix.values_[0][i] = x;
				matrix.values_[1][i] = y;
				matrix.values_[2][i] = z;
			}

			Matrix3x3 dummy;
			if (!matrix.inverse(dummy))
			{
				continue;		
			}

			triangles_.push_back(triangle);
		}
		++polygonIndex;
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

