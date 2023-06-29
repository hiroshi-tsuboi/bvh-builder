#include "tcp-server.h"
#include "mesh.h"

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

	for (auto& mesh: meshes)
	{
		if (!mesh.load(fifo))
		{
			return; // error
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
