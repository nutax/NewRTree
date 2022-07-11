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
#include <exception>

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
	void print();
	void erase(Vec2 const& vec2);
	void forEachPoly(std::function<void(Poly const&)> const& fun);
	void forEachMBB(std::function<void(MBB const&, int, int)> const& fun);
	void forEachNearest(int k, Vec2 const& vec2, std::function<void(Poly const&, Vec2, Vec2, float)> const& fun);
	size_t size() const;

private:
	void freeMemory(Node* current);
	MBB buildMBB(Poly const& poly);
	void insertHelper(MBB mbb);
	float computeDensity(MBB const& a, MBB const& b);
	float computeArea(float xmin, float ymin, float xmax, float ymax);
	void expandMBB(MBB& expand, MBB const& include);
	void split(Node& node, MBB& mbb, MBB& newMBB);
	MBB& findChild(Node& parent, void* child);
	void reinsertExcept(std::stack<Node*>& stack, void* except);
	void updateParentsAfterRemoval(std::stack<Node*>& stack);
	void getFirstIntersection(Node const& node, MBB const& mbb);
	

	
	size_t _size;
	Node* _root;
	inline static const float log2ORDER = log2(ORDER);
};