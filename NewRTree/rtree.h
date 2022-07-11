#pragma once
#include <queue>
#include <vector>
#include <functional>
#include <iostream>
#include <stack>
#include <algorithm>
#include <math.h>
#include <stdint.h>
#include <limits.h>

#define ORDER 3

struct Vec2 {
	float x, y;
};

struct MBB {
	int z;
	Vec2 min, max;
	void* child;
};

struct Node {
	int leaf = 1;
	std::vector<MBB> regions;
	Node *parent = nullptr, *left = nullptr, *right = nullptr;
};

using Poly = std::vector<Vec2>;

class RTree {
public:
	RTree();
	~RTree();
	void insert(Poly const& poly);
	void erase(Vec2 const& vec2);
	void forEachPoly(std::function<void(Poly const&)> const& fun);
	void forEachMBB(std::function<void(MBB const&, int, int)> const& fun);
	void forEachNearest(int k, Vec2 const& vec2, std::function<void(Poly const&, Vec2, Vec2, float)> const& fun);
	size_t size() const;
	void print();

private:
	MBB buildMBB(Poly const& poly);
	void insertHelper(MBB mbb);
	float computeDensity(MBB const& a, MBB const& b);
	MBB& getBestMBB(Node const& node, MBB const& mbb);
	void expandMBB(MBB& expand, MBB const& include);
	MBB split(Node* node, MBB& mbb);
	void reinsertExcept(std::stack<Node*>& stack, void* except);
	void updateParentsAfterRemoval(std::stack<Node*>& stack);
	void getFirstIntersection(Node const& node, MBB const& mbb);
	

	
	int _size;
	Node* _root;
	inline static const float log2ORDER = log2(ORDER);
};