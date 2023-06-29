#pragma once

#include <vector>
#include <string>
#include "fifo.h"

struct Mesh
{
	std::string name_;

	struct Material
	{
		std::string name_;
		std::string vertexFormat_;
		float diffuse_[3];
		float specular_[3];
		//
		std::vector<float> vertices_;
	};

	std::vector<Material> materials_;

	std::vector<std::vector<uint32_t> > polygons_;

	bool load(Fifo* fifo);
};
