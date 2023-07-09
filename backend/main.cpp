#include "tcp-server.h"
#include "mesh.h"
#include "bvh.h"

#include <iostream>

struct BvhBuilder : TcpServer::Reader
{
	bool dumpNodes_ = false;
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

	std::vector<Bvh*> bvhs;
	bvhs.reserve(triangulars.size());
	for (uint32_t i = 0; i < triangulars.size(); ++i)
	{
		auto bvh = new Bvh();

		bvhs.push_back(bvh);

		bvh->build(triangulars.at(i));
	}

	for (auto bvh: bvhs)
	{
		bvh->join(dumpNodes_);
	}

	// TODO

	for (auto bvh: bvhs)
	{
		bvh->root_.destroy();

		delete bvh;
	}
}

int main(int argc, char** argv)
{
	std::string ipv6Address;
	int port = 8080;
	BvhBuilder bvhBuilder;

	for (int i = 1; i < argc; ++i)
	{
		auto arg = std::string(argv[i]);
		if (arg == "--debug")
		{
			bvhBuilder.dumpNodes_ = true;
		}
		// TODO
	}

	TcpServer tcpServer;

	tcpServer.run(ipv6Address, port, &bvhBuilder);

	return 0;
}
