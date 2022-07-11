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
	}

	MBB splittedMBB;
	while (true) {
		if (current->regions.size() < ORDER) {
			current->regions.push_back(mbb);
			return;
		}

		splittedMBB = split(current, mbb);

		if (current->parent == nullptr) break;
	}

}