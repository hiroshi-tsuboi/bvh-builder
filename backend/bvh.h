#pragma once

#include "aabb.h"
#include "triangular.h"

#include <thread>
#include <mutex>
#include <condition_variable>

struct Bvh
{
	std::mutex mutex_;

	struct
	{
		std::condition_variable signal_;
		uint32_t count_ = 0;
	} leftAmount_;

	int maxTreeLevel_;

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
		virtual void create(const std::vector<AaBb>& aabbs) = 0;
		virtual void destroy() = 0;
	};

	struct Node : Obj
	{
		Obj* childs_[2] = {nullptr, nullptr};
		int type() override { return kNode; }
		void create(const std::vector<AaBb>& aabbs) override { /* nop */ };
		void link(Obj* child, int index);
		void destroy() override;
	};

	struct Leaf : Obj
	{
		std::vector<uint32_t> triangleIndices_;
		int type() override { return kLeaf; } 
		void create(const std::vector<AaBb>& aabbs) override;
		void destroy() override { /* nop */ };
	};

	Node root_;

	struct Result
	{
		std::mutex mutex_;
		std::condition_variable notFull_;
		uint32_t finishCount_ = 0;
		float miniCosts_[3];
	};

	struct
	{
		float kTriangle_ = 1.f;
		float kAabb_ = 1.f * 2;
		int kTriangleCountThreshold_ = 2;
	} sah_;

	struct
	{
		std::vector<std::string> lines_;
	} log_;

	void divide(Bvh::Node* parent, int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs, std::shared_ptr<Result> sharedResult, uint32_t axisIndex, int treeLevel);

	void fork(Bvh::Node* parent, int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs, int treeLevel);

	bool build(const Triangular& triangular, int maxTreeLevel = 1);

	void join();
};
