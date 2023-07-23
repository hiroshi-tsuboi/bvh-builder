#pragma once

#include <set>

#include "triangular.h"
#include "aabb.h"

struct Quadrangular
{
	std::vector<AaBb> aabbs_;
	std::set<uint32_t> triangleIndices_;

	void create(const Triangular & triangular);
};
