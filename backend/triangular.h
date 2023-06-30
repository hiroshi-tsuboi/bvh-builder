#pragma once

#include <cstdint>
#include <vector>

struct Triangular
{
	std::vector<float> vertices_;
	std::vector<uint32_t> indices_;

	bool create(std::vector<float>& vertices, uint32_t vertexOffset, uint32_t vertexStride, std::vector<std::vector<uint32_t> >& polygons);
};
