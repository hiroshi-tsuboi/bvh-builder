#pragma once

#include <cstdint>
#include <vector>

struct Triangular
{
	std::vector<float> vertices_; // vertex = (x,y,z)
	std::vector<uint32_t> indices_; // triangle = (vertex-index0,vertex-index1,vertex-index2,polygon-index)

	bool create(std::vector<float>& vertices, uint32_t vertexOffset, uint32_t vertexStride, std::vector<std::vector<uint32_t> >& polygons);

	struct Triangle
	{
		float vertices_[3][3];
		uint32_t index_;
	};

	Triangle at(uint32_t triangleIndex) const;
};
