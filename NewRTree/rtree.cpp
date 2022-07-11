#include "rtree.h"

RTree::RTree()
{
	_root = nullptr;
	_size = 0;
}

RTree::~RTree()
{
}

void RTree::insert(Poly const& poly)
{
	insertHelper(buildMBB(poly));
}

MBB RTree::buildMBB(Poly const& poly)
{
	MBB mbb;
	auto xmin = std::min_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.y < b.y; });
	auto ymin = std::min_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.y < b.y; });
	auto xmax = std::max_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.x < b.x; });
	auto ymax = std::max_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.y < b.y; });
	mbb.min = { xmin->x, ymin->y };
	mbb.max = { xmax->x, ymax->y };
	mbb.child = new Poly(poly);
	return mbb;
}

void RTree::insertHelper(MBB mbb)
{
	_size++;
	if (_root == nullptr) {
		_root = new Node;
		_root->regions.push_back(mbb);
		return;
	}

	Node* current = _root;
	while (!(current->leaf)) {
		auto bestMBB = std::max_element(std::begin(current->regions), std::end(current->regions),
			[this, &mbb](MBB const& a, MBB const& b) {return computeDensity(a, mbb) < computeDensity(b, mbb); });
		expandMBB(*bestMBB, mbb);
		current = (Node*) (bestMBB->child);
	}

	MBB updatedMBB;
	while (true) {
		if (current->regions.size() < ORDER) {
			current->regions.push_back(mbb);
			return;
		}

		split(*current, mbb, updatedMBB);

		if (current->parent == nullptr) break;
		
		findChild(*(current->parent), current) = updatedMBB;
	}

	Node* newRoot = new Node;
	newRoot->regions.push_back(updatedMBB);
	newRoot->regions.push_back(mbb);
	Node* a = (Node*) (updatedMBB.child);
	Node* b = (Node*) (mbb.child);
	a->left = b;
	b->right = a;
	_root = newRoot;
}

float RTree::computeDensity(MBB const& a, MBB const& b)
{
	float const xmin = std::min(a.min.x, b.min.x);
	float const ymin = std::min(a.min.y, b.min.y);
	float const xmax = std::max(a.max.x, b.max.x);
	float const ymax = std::max(a.max.y, b.max.y);


	float const mass = computeArea(a.min.x, a.min.y, a.max.x, a.max.y) + 
		computeArea(b.min.x, b.min.y, b.max.x, b.max.y);
	float const volume = computeArea(xmin, ymin, xmax, ymax);

	return mass / volume;
}

float RTree::computeArea(float xmin, float ymin, float xmax, float ymax)
{
	return (xmax - xmin)*(ymax - ymin);
}

void RTree::expandMBB(MBB& expand, MBB const& include)
{
	expand.min.x = std::min(expand.min.x, include.min.x);
	expand.min.y = std::min(expand.min.y, include.min.y);
	expand.max.x = std::max(expand.max.x, include.max.x);
	expand.max.y = std::max(expand.max.y, include.max.y);
}
