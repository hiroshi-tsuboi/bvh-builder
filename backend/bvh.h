#pragma once

#include "aabb.h"
#include "triangular.h"

#include <thread>
#include <mutex>
#include <condition_variable>
#include <queue>
#include <atomic>

struct Bvh
{
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
		virtual void dump(Bvh* bvh) = 0;
		virtual void create(const std::vector<AaBb>& aabbs) = 0;
		virtual void destroy() = 0;
	};

	struct Node : Obj
	{
		Obj* childs_[2] = {nullptr, nullptr};
		int type() override { return kNode; }
		void dump(Bvh* bvh) override;
		void create(const std::vector<AaBb>& aabbs) override { /* nop */ };
		void link(Obj* child, int index);
		void destroy() override;
	};

	struct Leaf : Obj
	{
		std::vector<uint32_t> triangleIndices_;
		int type() override { return kLeaf; } 
		void dump(Bvh* bvh) override;
		void create(const std::vector<AaBb>& aabbs) override;
		void destroy() override { /* nop */ };
	};

	Node root_;

	int maxTreeLevel_;

	struct Result
	{
		std::mutex mutex_;
		std::condition_variable notFull_;

		double miniCosts_[3];

		uint32_t finishCount_ = 0;

		void write(uint32_t index, double value);
		void join(bool waitForAll = false);
	};

	struct LeftAmount
	{
		std::mutex mutex_;
		std::condition_variable signal_;
		uint32_t count_ = 0;

		void push(uint32_t count);
		void pop();
		void join();
	} leftAmount_;

	struct
	{
		double kTriangle_ = 1.f;
		double kAabb_ = 1.f * 2;
		int kTriangleCountThreshold_ = 2;
	} sah_;

	struct Log
	{
		std::mutex mutex_;
		std::queue<std::string> lines_;
		void push(const std::string& line);
		void main();
		void join();
	} log_;

	void divide(Bvh::Node* parent, const int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs, std::shared_ptr<Result> sharedResult, const uint32_t axisIndex, int treeLevel);

	void fork(Bvh::Node* parent, const int childIndex, std::shared_ptr<std::vector<AaBb> > sharedAabbs, int treeLevel);

	bool build(const Triangular& triangular, int extraTreeLevel = 1);

	void join(bool dumpNodes = false);
};
