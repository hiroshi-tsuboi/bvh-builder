#pragma once

#include "triangular.h"

#include <map>
#include <array>

struct Linkage : std::map<std::pair<uint32_t, uint32_t>, std::vector<std::pair<uint32_t, uint32_t> > > 
{
	std::vector<std::array<uint32_t, 3> > triangles_; 

	void create(const Triangular& triangular);
};


