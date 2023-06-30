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

		int offsets[3] = {-1, -1, -1};
		int targetIndex = -1;
		int targetCount = 0;
		int targetOffset = 0;
		for (auto c: material.vertexFormat_)
		{
			if (c == '_')
			{
				// delimiter
			}
			else if (targetIndex < 0)
			{
				if (c == 'T')
				{
					targetIndex = 0;
				}
				else if (c == 'N')
				{
					targetIndex = 1;
				}
				else if (c == 'V')
				{
					targetIndex = 2;
				}
				else
				{
					// error
				}
			}
			else if ('0' <= c && c <= '9')
			{
				targetCount *= 10;
				targetCount += c - '0';
			}
			else if (c == 'F' && 0 < targetCount)
			{
				offsets[targetIndex] = targetOffset;
				targetOffset += targetCount;
				// reset
				targetIndex = -1;
				targetCount = 0;
			}
		}

		if (0 <= offsets[0]) material.offset_.uv_ = offsets[0];
		if (0 <= offsets[1]) material.offset_.normal_ = offsets[1];
		if (0 <= offsets[2]) material.offset_.vertex_ = offsets[2];

		material.vertexCount_ = material.vertices_.size() / targetOffset;

		std::cout << "vertex count = " << material.vertexCount_ << std::endl;
		std::cout << "vertex offset = " << material.offset_.vertex_ << std::endl;

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
