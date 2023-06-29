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

	struct Polygon
	{
		std::vector<int> indices_; // (vertex, [uv, normal])
		int materialIndex_ = -1;
	};

	std::vector<Polygon> polygons_;

	bool load(Fifo* fifo);
};
