#include "tcp-server.h"
#include <vector>
#include <iostream>
#include <memory.h>

#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <error.h>

#include <cassert>

void TcpServer::main(int sfd, Fifo* fifo)
{
	uint8_t buffer[0x1000];

	while (1)
	{
		auto r = recv(sfd, buffer, sizeof(buffer), 0);
		if (r < 0)
		{
			std::lock_guard<std::mutex> lk(mutex_);
			errors_.push_back("failed recv(), error = " + std::to_string(errno));
			break;
		}

		if (r == 0)
		{
			break; // shutdown
		}

		fifo->write(buffer, r);
	}

	fifo->destroy();
}

void TcpServer::run(std::string ipv6Address, int port, Reader* reader)
{
	assert(reader != nullptr);

	struct sockaddr_in6 sa6;

	memset(&sa6, 0, sizeof(sa6));

	sa6.sin6_family = AF_INET6;
	sa6.sin6_port = htons(port);

	if (ipv6Address.empty())
	{
		sa6.sin6_addr = in6addr_any;
	}
	else
	{	
		auto r = inet_pton(AF_INET6, ipv6Address.c_str(), &sa6.sin6_addr);
		if (r == 0)
		{
			errors_.push_back("failed inet_pton() due to not in presentation format.");
			return;
		}
		else if (r < 0)
		{
			errors_.push_back("failed inet_pton(), error = " + std::to_string(errno));
			return;
		}
	}

	std::vector<int> sfds;
	std::vector<Fifo*> fifos;
	std::vector<std::thread*> threads;

	do
	{
		{
			auto sfd = socket(AF_INET6, SOCK_STREAM, 0);
			if (sfd < 0)
			{
				errors_.push_back("failed to create socket, error = " + std::to_string(errno));
				break;
			}	
			sfds.push_back(sfd);
		}

		{
			int opt = 1;
			auto r = setsockopt(sfds.at(0), SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT, &opt, sizeof(opt));
			if (r < 0)
			{
				errors_.push_back("failed setsockopt(), error = " + std::to_string(errno));
				break;
			}
		}

		{
			auto r = bind(sfds.at(0), (sockaddr*)&sa6, sizeof(sa6));
			if (r < 0)
			{
				errors_.push_back("failed to bind socket, error = " + std::to_string(errno));
				break;
			}
		}

		{
			auto r = listen(sfds.at(0), 5);
			if (r < 0)
			{
				errors_.push_back("failed listen(), error = " + std::to_string(errno));
				break;
			}
		}

		while (1)
		{
			sockaddr_storage ss;
			socklen_t sz = sizeof(ss); 

			auto sfd = accept(sfds.at(0), (sockaddr*)&ss, &sz);
			if (sfd < 0)
			{
				std::lock_guard<std::mutex> lk(mutex_);
				errors_.push_back("failed accept(), error = " + std::to_string(errno));
				break;
			}

			sfds.push_back(sfd);

			auto fifo = new Fifo();
			if (fifo == nullptr)
			{
				std::lock_guard<std::mutex> lk(mutex_);
				errors_.push_back("failed to create fifo.");
				break;
			}

			fifos.push_back(fifo);

			if (!fifo->initialize(0x10000))
			{
				std::lock_guard<std::mutex> lk(mutex_);
				errors_.push_back("failed to create fifo->buffer_.");
				break;
			}

			{
				auto thread = new std::thread(&TcpServer::main, this, sfd, fifo);
				if (thread == nullptr)
				{
					std::lock_guard<std::mutex> lk(mutex_);
					errors_.push_back("failed to create thread");
					break;
				}
				threads.push_back(thread);
			}

			std::string header;

			fifo->read(header);

			std::cout << header << std::endl;

			if (header == "shutdown")
			{
				break;
			}

			if (fifo->broken())
			{
				continue;
			}

			{
				auto thread = new std::thread(&TcpServer::Reader::main, reader, fifo);
				if (thread == nullptr)
				{
					std::lock_guard<std::mutex> lk(mutex_);
					errors_.push_back("failed to create thread");
					break;
				}
				threads.push_back(thread);
			}
		}
	} while (0);

	for (auto thread: threads)
	{
		thread->join();
		delete thread;
	}

	shutdown(sfds.at(0), SHUT_RDWR);

	for (auto sfd: sfds)
	{
		close(sfd);
	}

	for (auto fifo : fifos)
	{
		delete fifo;
	}
}
