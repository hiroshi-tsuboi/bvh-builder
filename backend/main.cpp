#include "tcp-server.h"
#include "mesh.h"
#include "triangular.h"

#include <iostream>

struct BvhBuilder : TcpServer::Reader
{
	void main(Fifo* fifo) override;
};

void BvhBuilder::main(Fifo* fifo)
{
	std::vector<Mesh> meshes;

	{
		uint32_t meshCount;

		fifo->read(&meshCount, sizeof(meshCount));

		std::cout << "mesh count = " << meshCount << std::endl;

		meshes.resize(meshCount);
	}

	std::vector<Triangular> triangulars;
	triangulars.reserve(meshes.size());

	for (auto& mesh: meshes)
	{
		if (!mesh.load(fifo))
		{
			return; // error
		}

		if (mesh.materials_.size())
		{
			triangulars.resize(triangulars.size() + 1);
			auto& triangular = triangulars.back();

			auto& material = mesh.materials_.at(0);
			if (triangular.create(material.vertices_, material.offset_.vertex_, material.vertexStride(), mesh.polygons_))
			{
				std::cout << "triangular:  vertex-count: " << material.vertexCount_ << " -> " << triangular.vertices_.size() / 3 << std::endl;
			}
			else
			{
				// error		
			}
		}
	}

	// TODO
}

int main(int argc, char** argv)
{
	std::string ipv6Address;
	int port = 8080;

	for (int i = 1; i < argc; ++i)
	{
		// TODO
	}

	TcpServer tcpServer;

	BvhBuilder bvhBuilder;

	tcpServer.run(ipv6Address, port, &bvhBuilder);

	return 0;
}
