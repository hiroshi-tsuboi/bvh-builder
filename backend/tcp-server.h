#pragma once

#include <cstdlib>
#include <thread>
#include <string>
#include <vector>

#include "fifo.h"

struct TcpServer
{
	std::mutex mutex_;
	std::vector<std::string> logs_;
	std::vector<std::string> errors_;

	struct Reader
	{
		virtual void main(Fifo* fifo) = 0;
	};

	void main(int sfd, Fifo* fifo);

	void run(std::string ipv6Address, int port, Reader* reader);
};
