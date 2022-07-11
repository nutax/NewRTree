#include "rtree.h"

RTree::RTree()
{
	_root = nullptr;
	_size = 0;
}

RTree::~RTree()
{
	if(_root != nullptr) freeMemory(_root);
}

void RTree::freeMemory(Node* current)
{
	if (current->leaf) {
		for (auto& mbb : current->regions) {
			delete mbb.child;
		}
	}
	else {
		for (auto& mbb : current->regions) {
			freeMemory((Node*)(mbb.child));
		}
	}
	delete current;
}

void RTree::insert(Poly const& poly)
{
	insertHelper(buildMBB(poly));
}

void RTree::print()
{
	std::queue<Node*> bfs;
	bfs.push(_root);
	while (!(bfs.empty())) {
		auto& front = bfs.front();
		for (auto& mbb : front->regions) {
			std::printf(" R[ X1:%f, Y1:%f, X2:%f, Y2:%f ] ", mbb.min.x, mbb.min.y, mbb.max.x, mbb.max.y);
		}
		std::printf("\n\n");
		if (!(front->leaf)) {
			for (auto& mbb : front->regions) {
				bfs.push((Node*)(mbb.child));
			}
		}
		bfs.pop();
	}
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

void RTree::insertHelper(MBB newMBB)
{
	_size++;
	if (_root == nullptr) {
		_root = new Node;
		_root->regions.push_back(newMBB);
		return;
	}

	Node* current = _root;
	while (!(current->leaf)) {
		auto bestMBB = std::max_element(std::begin(current->regions), std::end(current->regions),
			[this, &newMBB](MBB const& a, MBB const& b) {return computeDensity(a, newMBB) < computeDensity(b, newMBB); });
		expandMBB(*bestMBB, newMBB);
		current = (Node*) (bestMBB->child);
	}

	MBB updatedMBB;
	while (true) {
		if (current->regions.size() < ORDER) {
			current->regions.push_back(newMBB);
			return;
		}

		split(*current, updatedMBB, newMBB);

		if (current->parent == nullptr) break;
		
		findChild(*(current->parent), current) = updatedMBB;
	}

	Node* newRoot = new Node;
	newRoot->regions.push_back(updatedMBB);
	newRoot->regions.push_back(newMBB);
	Node* a = (Node*) (updatedMBB.child);
	Node* b = (Node*) (newMBB.child);
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

void RTree::split(Node& updatedNode, MBB& updatedMBB, MBB& newMBB)
{
	std::vector<MBB> regions(updatedNode.regions);
	regions.push_back(newMBB);

	int seed1, seed2;
	float minDensity = std::numeric_limits<float>::max();
	for (int i = 0; i < (ORDER - 1); ++i) {
		for (int j = i + 1; j < ORDER; ++j) {
			float density = computeDensity(regions[i], regions[j]);
			if (density < minDensity) minDensity = density, seed1 = i, seed2 = j;
		}
	}

	updatedNode.regions.clear();
	updatedNode.regions.push_back(regions[seed1]);
	updatedMBB.min = updatedNode.regions.back().min;
	updatedMBB.max = updatedNode.regions.back().max;


	newMBB.child = new Node;
	Node& newNode = *((Node*)(newMBB.child));
	newNode.parent = updatedNode.parent;
	newNode.regions.push_back(regions[seed2]);
	newMBB.min = newNode.regions.back().min;
	newMBB.max = newNode.regions.back().max;


	regions[seed2] = regions.back(); regions.pop_back();
	regions[seed1] = regions.back(); regions.pop_back();
	

	while (!(regions.empty())) {
		float const densityU = computeDensity(updatedMBB, regions.back());
		float const densityN = computeDensity(newMBB, regions.back());
		if (densityU > densityN) {
			expandMBB(updatedMBB, regions.back());
			updatedNode.regions.push_back(regions.back());
		}
		else if (densityU < densityN) {
			expandMBB(newMBB, regions.back());
			newNode.regions.push_back(regions.back());

		}
		else {
			if (newNode.regions.size() > updatedNode.regions.size()) {
				expandMBB(updatedMBB, regions.back());
				updatedNode.regions.push_back(regions.back());
			}
			else { 
				expandMBB(newMBB, regions.back());
				newNode.regions.push_back(regions.back());
			}
		}
		regions.pop_back();
	}

}

MBB& RTree::findChild(Node& parent, void* child)
{
	for (auto& mbb : parent.regions) {
		if (mbb.child == child) return mbb;
	}
	std::runtime_error("COULD NOT FIND CHILD!");
}
