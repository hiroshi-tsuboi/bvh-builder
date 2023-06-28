#include "tcp-server.h"

#include <iostream>

struct BvhBuilder : TcpServer::Reader
{
	void main(Fifo* fifo) override;
};

void BvhBuilder::main(Fifo* fifo)
{
	uint32_t meshCount;

	fifo->read(&meshCount, sizeof(meshCount));

	std::cout << "mesh count = " << meshCount << std::endl;

	for (uint32_t meshIndex = 0; meshIndex < meshCount; ++meshIndex)
	{
		uint32_t materialCount;

		fifo->read(&materialCount, sizeof(materialCount));

		for (uint32_t materialIndex = 0; materialIndex < materialCount; ++materialIndex)
		{
			std::string name;
			fifo->read(name);
			std::cout << "name = " << name << std::endl;

			std::string vertexFormat;
			fifo->read(vertexFormat);
			std::cout << "vertex format = " << vertexFormat << std::endl;

			std::vector<float> vertices;
			fifo->read(vertices);
			std::cout << "vertex count = " << vertices.size() << std::endl;
		}
	}
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
