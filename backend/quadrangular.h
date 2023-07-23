#pragma once

#include "triangular.h"

struct Polygonizer
{
	struct Polygon
	{
		std::vector<uint32_t> triangleIndices_;
		std::vector<uint32_t> vertexIndies_;
	};

	std::vector<Polygon> polygons_;

	bool create(const Triangular & triangular);
};
