#include "triangular.h"
#include "matrix3x3.h"

#include <map>
#include <tuple>
#include <cassert>

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

				auto& edgeIndices = linkages_[key];

				edgeIndices.push_back(std::make_pair(uint32_t(triangles_.size()), edgeIndex));
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


std::vector<uint32_t> Triangular::pack(std::vector<uint32_t>& triangleIndices)
{
	std::vector<uint32_t> r;
	std::map<uint32_t, uint32_t> localVertexIndices;
	std::map<uint32_t, uint32_t> localVertexCounts;

	for (auto triangleIndex: triangleIndices)
	{
		auto& triangle = triangles_.at(triangleIndex);

		for (uint32_t i = 0; i < 3; ++i)
		{
			auto vertexIndex = triangle.indices_[i];
			if (localVertexIndices.count(vertexIndex) == 0)
			{
				localVertexIndices.emplace(vertexIndex, localVertexIndices.size());
				localVertexCounts.emplace(vertexIndex, 1);
			}
			else
			{
				localVertexCounts.at(vertexIndex) += 1;
			}
		}
	}

	if (5 < localVertexIndices.size())
	{
		return r; // failed to pack triangles
	}

	int pivotIndex = -1;

	for (const auto& [vertex, count]: localVertexCounts)
	{
		if (count == triangleIndices.size())
		{
			pivotIndex = localVertexIndices.at(vertex);
			break;
		}
	}

	if (pivotIndex < 0)
	{
		return r; // failed to pack triangles
	}

	auto pivotOffset = localVertexIndices.size() - pivotIndex;

	// pack triangle index

	uint32_t triangleBitData = triangleIndices.size() - 1;
	uint32_t triangleBitShift = 2;

	for (auto triangleIndex: triangleIndices)
	{
		auto& triangle = triangles_.at(triangleIndex);

		int startIndex = -1;
		for (uint32_t i = 0; i < 3; ++i)
		{
			if (localVertexIndices.at(triangle.indices_[i]) == pivotIndex)
			{
				startIndex = i;
				break;
			}
		}
		assert(0 <= startIndex);

		for (uint32_t i = 1; i < 3; ++i)
		{
			auto localVertexIndex = uint32_t((localVertexIndices.at(triangle.indices_[(i + startIndex) % 3]) + pivotOffset) % localVertexIndices.size());
			assert(0 < localVertexIndex);
			triangleBitData |= (localVertexIndex - 1) << triangleBitShift;
			triangleBitShift += 2;
		}
	}

	r.push_back(triangleBitData);

	for (auto triangleIndex: triangleIndices)
	{
		r.push_back(triangleIndex);
	}

	{
		auto offset = r.size();

		r.resize(r.size() + localVertexIndices.size());

		for (const auto& [vertex, index]: localVertexIndices)
		{
			r.at(offset + (index + pivotOffset) % localVertexIndices.size()) = vertex;
		}
	}

	return r;
}

