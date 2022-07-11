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
#include <cstdio>
#include <array>
#include <tuple>
#include <unordered_map>
#include <unordered_set>

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

struct Neighbor {
	Poly* poly;
	float distance;
	Vec2 toPoint;
};

struct Neighborhood {
	Node* node;
	float distance;
};

class RTree {
public:
	RTree();
	~RTree();
	void insert(Poly const& poly);
	void print();
	void erase(Vec2 const& min, Vec2 const& max);
	void forEachPoly(std::function<void(Poly const&)> const& fun);
	void forEachMBB(std::function<void(MBB const&, int, int)> const& fun);
	void forEachNearest(int k, Vec2 const& vec2, std::function<void(Poly const&, Vec2, Vec2, float)> const& fun);
	size_t size() const;
	float testOverlapping(std::vector<Vec2> const& testPoints);

private:
	void freeMemory(Node* current);
	MBB buildMBB(Poly const& poly);
	void insertHelper(MBB mbb);
	float computeDensity(MBB const& a, MBB const& b);
	float computeArea(float xmin, float ymin, float xmax, float ymax);
	void expandMBB(MBB& expand, MBB const& include);
	void split(Node& node, MBB& mbb, MBB& newMBB);
	MBB& findChild(Node& parent, void* child);
	void forEachPolyHelper(std::function<void(Poly const&)> const& fun, Node& current);
	void forEachMBBHelper(std::function<void(MBB const&, int, int)> const& fun, Node& current, int lvl);
	std::tuple<Vec2, float> computeMinDist(Vec2 const& fromPoint, MBB const& toMBB);
	bool isInside(Vec2 const& vec2, MBB const& mbb);
	bool eraseHelper(Vec2 const& min, Vec2 const& max);
	bool isIntersecting(Vec2 const& min, Vec2 const& max, MBB const& mbb);
	void reinsertExcept(void* except);
	void updateParentsAfterRemoval(std::stack<Node*>& stack);
	void getFirstIntersection(Node const& node, MBB const& mbb);
	

	
	size_t _size;
	Node* _root;
	int _height;
	inline static const float log2ORDER = log2(ORDER);
};