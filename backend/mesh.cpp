#include "mesh.h"

#include <iostream>

bool Mesh::load(Fifo* fifo)
{
	{
		uint32_t materialCount;

		fifo->read(&materialCount, sizeof(materialCount));

		materials_.resize(materialCount);
	}

	for (auto& material: materials_)
	{
		fifo->read(material.name_);
		std::cout << "name = " << material.name_ << std::endl;

		fifo->read(material.vertexFormat_);
		std::cout << "vertex format = " << material.vertexFormat_ << std::endl;

		fifo->read(material.vertices_);
		std::cout << "vertex count = " << material.vertices_.size() << std::endl;

		// TODO
	}

	fifo->read(polygons_);
	std::cout << "polygon count = " << polygons_.size() << std::endl;

#if 0
	if (polygons_.size())
	{
		uint32_t miniIndex = polygons_.at(0).at(0);
		uint32_t maxiIndex = polygons_.at(0).at(0);
		for (auto& polygon: polygons_)
		{
			for (auto index: polygon)
			{
				if (index < miniIndex) miniIndex = index;
				if (index > maxiIndex) maxiIndex = index;
			}
		}
		std::cout << "mini-index = " << miniIndex << " maxi-index = " << maxiIndex << std::endl;
	}
#endif

	// TODO

	return true;
}
