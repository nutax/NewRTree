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

static constexpr size_t ORDER = 3;
static constexpr size_t MINIMUM = (ORDER/2) + (ORDER%2);

struct Vec2 {
	float x, y;
};

struct MBB {
	unsigned z;
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
	double testOverlapping(std::vector<Vec2> const& testPoints);
	void eraseRandom();

protected:
	
	virtual MBB buildMBB(Poly const& poly);
	virtual MBB& queryBestMBB(Node& current, MBB const& newMBB);
	virtual void split(Node& node, MBB& mbb, MBB& newMBB);
	virtual void eraseSelected(Node* current, MBB* toErase);
	virtual bool lendOverflow(Node& current, MBB& newMBB);
	

	void insertHelper(MBB mbb);
	void freeMemory(Node* current);
	float computeDensity(MBB const& a, MBB const& b);
	float computeArea(float xmin, float ymin, float xmax, float ymax);
	void expandMBB(MBB& expand, MBB const& include);
	MBB& findChild(Node& parent, void* child);
	void forEachPolyHelper(std::function<void(Poly const&)> const& fun, Node& current);
	void forEachMBBHelper(std::function<void(MBB const&, int, int)> const& fun, Node& current, int lvl);
	std::tuple<Vec2, float> computeMinDist(Vec2 const& fromPoint, MBB const& toMBB);
	bool isInside(Vec2 const& vec2, MBB const& mbb);
	bool eraseHelper(Vec2 const& min, Vec2 const& max);
	bool isIntersecting(Vec2 const& min, Vec2 const& max, MBB const& mbb);
	void* removeSubTree(Node& current, MBB& toReinsert);
	void updateParents(Node& current);
	void updateSizeAfterRemoval(Node& subCurrent);
	void reinsertExcept(Node& subCurrent, void* except);
	void testOverlappingHelper(Vec2 const& testPoint, Node& current, double& counter, double& total);
	std::tuple<Node*, MBB*> pickRandom(Node& current);
	void updateFamilyRelations(Node& current);

	
	size_t _size;
	Node* _root;
	int _height;
	inline static const float log2ORDER = log2(ORDER);
};