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
	_size++;
	MBB mbb = buildMBB(poly);
	Poly* child = new Poly(poly);

	insertHelper(mbb, child);
}
