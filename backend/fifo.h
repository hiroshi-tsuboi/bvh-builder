#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <mutex>
#include <condition_variable>

struct Fifo
{
	std::vector<uint8_t> buffer_;

	std::mutex mutex_;

	struct
	{
		std::condition_variable empty_, full_;
	} not_;

	struct
	{
		uint32_t writer_ = 0;
		uint32_t reader_ = 0;

		auto amount()
		{
			return writer_ - reader_;
		}
	} index_;

	void read(std::string& string);
	void read(std::vector<float>& values);
	void read(std::vector<std::vector<uint32_t> >& valuesv);

	bool read(void* ptr, size_t sz);
	void write(void* ptr, size_t sz);

	bool initialize(size_t sz);
	void finalize();

	void destroy();

	bool broken()
	{
		return buffer_.size() < index_.amount();
	}
};
