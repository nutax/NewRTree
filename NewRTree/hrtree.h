#pragma once
#include "rtree.h"

class HRTree : public RTree {
public:
	HRTree(int weight, int height);
	~HRTree();

	
protected:
	MBB buildMBB(Poly const& poly) override;
	MBB& queryBestMBB(Node& current, MBB const& newMBB) override;
	bool lendOverflow(Node& current, MBB& newMBB) override;
	void split(Node& node, MBB& mbb, MBB& newMBB) override;
	//void eraseSelected(Node* current, MBB* toErase) override;
	Node* queryLeftSibling(Node& current);

private:
	void buildHilbertMatrix(float n, float x, float y, float xi, float xj, float yi, float yj);
	void lendOverflowR(Node& current, MBB& newMBB);


	std::vector<std::vector<int>> _hilbertMatrix;
	int _hilbertDimSize;
	int _hilbertCounter = 0;
};