#pragma once
#include "rtree.h"

class HRTree : public RTree {
public:
	HRTree(int weight, int height);
	~HRTree();


protected:
	unsigned& atHilbertMatrix(int i, int j);
	void buildHilbertMatrix(int n, int x, int y, int xi, int xj, int yi, int yj);

private:
	std::vector<unsigned> _hilbertMatrix;
	int _hilbertDimSize;
};