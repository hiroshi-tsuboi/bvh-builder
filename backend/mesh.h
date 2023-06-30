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

		int vertexCount_;
		int vertexStride()
		{
			return vertices_.size() / vertexCount_;
		}

		struct
		{
			int uv_ = -1;
			int normal_ = -1;
			int vertex_ = -1;
		} offset_;
	};

	std::vector<Material> materials_;

	std::vector<std::vector<uint32_t> > polygons_;

	bool load(Fifo* fifo);
};
