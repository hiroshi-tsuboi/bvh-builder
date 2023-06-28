#include "fifo.h"

void Fifo::read(std::string& string)
{
	string.clear();

	while (1)
	{
		char c;

		if (!read(&c, sizeof(c)))
		{
			break;
		}

		if (c == '\0')
		{
			break;
		}

		string.push_back(c);
	}
}

void Fifo::read(std::vector<float>& values)
{
	uint32_t sz;

	read(&sz, sizeof(sz));

	values.resize(sz);

	read(values.data(), sizeof(values[0]) * sz);
}

void Fifo::read(std::vector<std::vector<uint32_t> >& valuesv)
{
	uint32_t sz;

	read(&sz, sizeof(sz));

	valuesv.resize(sz);

	for (auto& values: valuesv)
	{
		read(&sz, sizeof(sz));

		values.resize(sz);

		read(values.data(), sizeof(values[0]) * sz);
	}
}

bool Fifo::read(void* ptr, size_t sz)
{
	auto dst = static_cast<uint8_t*>(ptr);

	while (0 < sz)
	{
		std::unique_lock<std::mutex> lk(mutex_);

		not_.empty_.wait(lk, [&]{ return index_.amount() != 0; });

		auto n = index_.amount();

		if (buffer_.size() < n)
		{
			return false;
		}

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

	return true;
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

void Fifo::destroy()
{
	std::unique_lock<std::mutex> lk(mutex_);

	not_.full_.wait(lk, [&]{ return index_.amount() == 0; });

	index_.writer_ = index_.reader_ - 1;

	not_.empty_.notify_one();
}
