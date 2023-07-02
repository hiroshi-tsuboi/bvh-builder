#pragma once

#include "triangular.h"

#include <queue>
#include <thread>
#include <mutex>

struct Bvh
{
	std::mutex mutex_;
	std::queue<std::thread*> queue_; 

	enum
	{
		kNode,
		kLeaf,
	};

	struct Obj
	{
		virtual int type() = 0;
	};

	struct Node : Obj
	{
		Obj* childs_[2];

		int type() override { return kNode; } 
	};

	struct Leaf : Obj
	{
		std::vector<uint32_t> triangleIndices_;

		int type() override { return kLeaf; } 
	};

	std::vector<Obj*> objs_;

	bool build(const Triangular& triangular);
};
