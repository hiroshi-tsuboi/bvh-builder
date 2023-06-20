#include "tcp-server.h"

struct BvhBuilder : TcpServer::Reader
{
	void main(Fifo* fifo) override;
};

void BvhBuilder::main(Fifo* fifo)
{
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
