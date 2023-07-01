#include "bvh.h"
#include "aabb.h"

bool Bvh::build(const Triangular& triangular)
{
	std::vector<AaBb> aabbs;

	aabbs.resize(triangular.indices_.size() / 4);

	for (size_t triangleIndex = 0; triangleIndex < aabbs.size(); ++triangleIndex)
	{
		aabbs.at(triangleIndex).create(triangular.vertices_, triangular.indices_, triangleIndex);
	}


	// TODO

	return true;
}

