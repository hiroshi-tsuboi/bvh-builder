#pragma once

#include "aabb.h"
#include "triangular.h"

#include <queue>
#include <thread>
#include <mutex>
#include <condition_variable>

struct Bvh
{
	std::mutex mutex_;
	std::condition_variable notFull_;
	std::queue<std::thread*> queue_; 

	enum
	{
		kNode,
		kLeaf,
	};

	struct Obj
	{
		std::mutex mutex_;
		AaBb aabb_;
		virtual int type() = 0;
		virtual void create(std::shared_ptr<std::vector<AaBb> > sharedAabbs, Obj* parent, int childIndex, Bvh& bvh) = 0;
	};

	struct Node : Obj
	{
		Obj* childs_[2];
		int type() override { return kNode; }
		void create(std::shared_ptr<std::vector<AaBb> > sharedAabbs, Obj* parent, int childIndex, Bvh& bvh) override;
	};

	struct Leaf : Obj
	{
		std::vector<uint32_t> triangleIndices_;
		int type() override { return kLeaf; } 
		void create(std::shared_ptr<std::vector<AaBb> > sharedAabbs, Obj* parent, int childIndex, Bvh& bvh) override;
	};

	std::vector<Obj*> objs_;

	bool build(const Triangular& triangular);

	void join();
};
