#pragma once

#include <cstdint>
#include <vector>

struct Triangular
{
	std::vector<float> vertices_; // vertex = (x,y,z)

	struct Triangle
	{
		uint32_t indices_[4]; // = (vertex-index0,vertex-index1,vertex-index2,polygon-index)
		float matrix_[3][3]; // for barycentric coordinates
	};

	std::vector<Triangle> triangles_;

	bool create(std::vector<float>& vertices, uint32_t vertexOffset, uint32_t vertexStride, std::vector<std::vector<uint32_t> >& polygons);
};
