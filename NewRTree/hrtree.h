#pragma once
#include "rtree.h"

class HRTree : public RTree {
public:
	HRTree(size_t weight, size_t height);
	~HRTree();


protected:
	unsigned& atHilbertMatrix(int i, int j);


private:
	std::vector<unsigned> _hilbertMatrix;
	size_t _hilbertDimSize;
};