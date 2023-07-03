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
		Obj* childs_[2] = {nullptr, nullptr};
		int type() override { return kNode; }
		void create(std::shared_ptr<std::vector<AaBb> > sharedAabbs, Obj* parent, int childIndex, Bvh& bvh) override;
		void link(Obj* child, int index);
	};

	struct Leaf : Obj
	{
		std::vector<uint32_t> triangleIndices_;
		int type() override { return kLeaf; } 
		void create(std::shared_ptr<std::vector<AaBb> > sharedAabbs, Obj* parent, int childIndex, Bvh& bvh) override;
	};

	Node root_;

	std::vector<Obj*> objs_;

	struct Result
	{
		std::mutex mutex_;
		std::condition_variable notFull_;
		uint32_t finishCount_ = 0;
		float miniCosts_[3];
	};

	const struct
	{
		float kTriangle_ = 1.f;
		float kAabb_ = 1.f * 2;
	} sah_;

	void divide(Bvh::Node* node, int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs, std::shared_ptr<Result> sharedResult, uint32_t axisIndex);

	void fork(Bvh::Node* parent, int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs);

	bool build(const Triangular& triangular);

	void join();
};
