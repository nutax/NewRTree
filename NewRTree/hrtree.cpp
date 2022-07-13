#include "hrtree.h"

HRTree::HRTree(size_t weight, size_t height) : RTree()
{
	_hilbertDimSize = std::max(weight, height);
	
}
