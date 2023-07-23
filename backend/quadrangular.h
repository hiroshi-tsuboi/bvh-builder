#pragma once

#include "triangular.h"

struct Quadrangular
{
	struct Quadrangle
	{
		uint32_t triangleIndices_[2];
		uint32_t vertexIndices_[4];
	};

	std::vector<Quadrangle> quadrangles_;

	bool create(const Triangular & triangular);
};
