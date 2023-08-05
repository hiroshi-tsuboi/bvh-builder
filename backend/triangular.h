#pragma once

#include <cstdint>
#include <vector>
#include <map>

#include "matrix3x3.h"

struct Triangular
{
	std::vector<float> vertices_; // vertex = (x,y,z)

	struct Triangle
	{
		uint32_t indices_[4]; // = (vertex-index0,vertex-index1,vertex-index2,polygon-index)
		//Matrix3x3 matrix_; // for barycentric coordinates
	};

	std::vector<Triangle> triangles_;

	std::map<std::pair<uint32_t, uint32_t>, std::vector<std::pair<uint32_t, uint32_t> > > linkages_; // linkages[(vertexIndex0, vertexIndex1)] = (triangleIndex, edgeIndex)

	bool create(std::vector<float>& vertices, uint32_t vertexOffset, uint32_t vertexStride, std::vector<std::vector<uint32_t> >& polygons);
};
