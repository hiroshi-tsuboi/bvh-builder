#include "fifo.h"

void Fifo::read(std::string& string)
{
	string.clear();

	while (1)
	{
		char c;

		read(&c, sizeof(c));

		if (c == '\0')
		{
			break;
		}

		string.push_back(c);
	}
}

void Fifo::read(void* ptr, size_t sz)
{
	auto dst = static_cast<uint8_t*>(ptr);

	while (0 < sz)
	{
		std::unique_lock<std::mutex> lk(mutex_);

		not_.empty_.wait(lk, [&]{ return index_.amount() != 0; });

		auto n = index_.amount();

		if (sz < n)
		{
			n = sz;
		}

		sz -= n;

		for (uint32_t i = 0; i < n; ++i)
		{
			*dst++ = buffer_.at(index_.reader_++ % buffer_.size());
		}

		not_.full_.notify_one();
	}
}

void Fifo::write(void* ptr, size_t sz)
{
	auto src = static_cast<uint8_t*>(ptr);

	while (0 < sz)
	{
		std::unique_lock<std::mutex> lk(mutex_);
		
		not_.full_.wait(lk, [&]{ return index_.amount() != buffer_.size(); });

		auto n = buffer_.size() - index_.amount();

		if (sz < n)
		{
			n = sz;
		}

		sz -= n;

		for (uint32_t i = 0; i < n; ++i)
		{
			buffer_.at(index_.writer_++ % buffer_.size()) = *src++;
		}

		not_.empty_.notify_one();
	}
}

bool Fifo::initialize(size_t sz)
{
	buffer_.resize(sz);

	return !buffer_.empty();
}

void Fifo::finalize()
{
	buffer_.clear(); 
}
