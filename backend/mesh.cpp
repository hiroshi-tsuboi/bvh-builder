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

	// TODO

	return true;
}
