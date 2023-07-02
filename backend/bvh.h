#pragma once

#include "triangular.h"

#include <queue>
#include <thread>
#include <mutex>

struct Bvh
{
	std::mutex mutex_;
	std::queue<std::thread*> queue_; 

	bool build(const Triangular& triangular);
};
