#pragma once

#include "triangular.h"

#include <map>

struct Linkage : std::map<std::pair<uint32_t, uint32_t>, std::vector<uint32_t> > 
{
	void create(const Triangular& triangular);
};


