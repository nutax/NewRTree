#include "hrtree.h"

HRTree::HRTree(int weight, int height) : RTree()
{
	_hilbertDimSize = std::max(weight, height);
    auto n = ceil(log(_hilbertDimSize) / log(2));
	_hilbertDimSize = pow(2, ceil(log((double)_hilbertDimSize) / log(2)));
	_hilbertMatrix = std::vector<unsigned>( _hilbertDimSize*_hilbertDimSize, 0 );
    buildHilbertMatrix((int) n, 0, 0, _hilbertDimSize, 0, 0, _hilbertDimSize);
}

HRTree::~HRTree()
{
}

unsigned& HRTree::atHilbertMatrix(int i, int j)
{
	return _hilbertMatrix[i * _hilbertDimSize + j];
}

void HRTree::buildHilbertMatrix(int n, int x, int y, int xi, int xj, int yi, int yj)
{
    static unsigned hilbertCounter = 0;
    if (n <= 0) {
        atHilbertMatrix(x + (xi + yi) / 2, y + (xj + yj) / 2) = hilbertCounter++;
    }
    else {
        buildHilbertMatrix(n - 1, x, y, yi / 2, yj / 2, xi / 2, xj / 2);
        buildHilbertMatrix(n - 1, x + xi / 2, y + xj / 2, xi / 2, xj / 2, yi / 2, yj / 2);
        buildHilbertMatrix(n - 1, x + xi / 2 + yi / 2, y + xj / 2 + yj / 2, xi / 2, xj / 2, yi / 2, yj / 2);
        buildHilbertMatrix(n - 1, x + xi / 2 + yi, y + xj / 2 + yj, -yi / 2, -yj / 2, -xi / 2, -xj / 2);
    }
}
