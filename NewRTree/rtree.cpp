#include "rtree.h"

RTree::RTree()
{
	_root = nullptr;
	_size = 0;
	_height = 0;
}

RTree::~RTree()
{
	//if(_root != nullptr) freeMemory(_root);
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
	try { delete current; }
	catch (std::exception e) {

	}

}

void RTree::insert(Poly const& poly)
{
	insertHelper(buildMBB(poly));
}

void RTree::print()
{
	std::printf("\n\n\nRTREE PRINT  | LINE == NODE |  BFS ORDER | SIZE = %u\n\n", _size);
	if (_root == nullptr) return;
	std::queue<Node*> bfs;
	bfs.push(_root);
	while (!(bfs.empty())) {
		auto& front = bfs.front();

		if (front == _root) printf("ADR: %p | PAR: %p | ROOT \n", front, front->parent);
		else if (front->leaf) printf("ADR: %p | PAR: %p | LEAF \n", front, front->parent);
		else printf("ADR: %p | PAR: %p | INTERNAL \n", front, front->parent);
		for (auto& mbb : front->regions) {
			std::printf("R[(%.1f,%.1f), (%.1f,%.1f), z: %u]  ", mbb.min.x, mbb.min.y, mbb.max.x, mbb.max.y, mbb.z);
		}
		if (front->regions.size() == 0) std::printf("EMPTY");
		std::printf("\n\n");
		if (!(front->leaf)) {
			for (auto& mbb : front->regions) {
				bfs.push((Node*)(mbb.child));
			}
		}
		bfs.pop();
	}
}

void RTree::erase(Vec2 const& min, Vec2 const& max)
{
	while (eraseHelper(min, max));
}

void RTree::forEachPoly(std::function<void(Poly const&)> const& fun)
{
	if (_root != nullptr) forEachPolyHelper(fun, *_root);
}

void RTree::forEachMBB(std::function<void(MBB const&, int, int)> const& fun)
{
	if (_root != nullptr) forEachMBBHelper(fun, *_root, 0);
}

void RTree::forEachNearest(int k, Vec2 const& fromPoint, std::function<void(Poly const&, Vec2, Vec2, float)> const& fun)
{
	if (_root == nullptr) return;

	auto compareNeighbor = [](Neighbor const& a, Neighbor const& b) {
		return a.distance < b.distance;
	};
	auto compareNeighborhood = [](Neighborhood const& a, Neighborhood const& b) {
		return a.distance > b.distance;
	};

	std::priority_queue<Neighbor, std::vector<Neighbor>,
		decltype(compareNeighbor)> neighbors{ compareNeighbor };
	std::priority_queue<Neighborhood, std::vector<Neighborhood>,
		decltype(compareNeighborhood)> neighborhoods{ compareNeighborhood };

	for (int i = 0; i < k; ++i) neighbors.push({ nullptr, std::numeric_limits<float>::max() });
	neighborhoods.push({ _root, 0 });

	while (!(neighborhoods.empty())) {
		auto closestNeighborhood = neighborhoods.top(); neighborhoods.pop();
		auto const& farestNeighbor = neighbors.top();
		if (closestNeighborhood.distance < farestNeighbor.distance) {
			if (closestNeighborhood.node->leaf) {
				for (auto& mbb : closestNeighborhood.node->regions) {
					auto const& farestNeighbor = neighbors.top();
					auto [toPoint, distance] = computeMinDist(fromPoint, mbb);
					if (distance < farestNeighbor.distance) {
						neighbors.pop();
						neighbors.push({ ((Poly*)(mbb.child)), distance, toPoint });
					}
				}
			}
			else {
				for (auto& mbb : closestNeighborhood.node->regions) {
					auto [toPoint, distance] = computeMinDist(fromPoint, mbb);
					if (distance < farestNeighbor.distance) {
						neighborhoods.push({ ((Node*)(mbb.child)), distance });
					}
				}
			}
		}
	}

	while (!(neighbors.empty())) {
		auto [poly, distance, toPoint] = neighbors.top();
		neighbors.pop();
		if (poly == nullptr) continue;
		fun(*poly, fromPoint, toPoint, distance);
	}
}

size_t RTree::size() const
{
	return _size;
}

double RTree::testOverlapping(std::vector<Vec2> const& testPoints)
{
	if (_root == nullptr) return 0;

	double overlaps = 0;
	double total = 0;
	for (auto const& testPoint : testPoints) {
		testOverlappingHelper(testPoint, *_root, overlaps, total);
	}

	return ((overlaps) / (overlaps + testPoints.size()));
}

void RTree::eraseRandom()
{
	if (_root == nullptr) return;
	if (_size == 1) {
		_size = 0;
		_height = 0;
		delete _root;
		_root = nullptr;
	}
	auto [current, toErase] = pickRandom(*_root);
	eraseSelected(current, toErase);
}

MBB RTree::buildMBB(Poly const& poly)
{
	MBB mbb;
	auto xmin = std::min_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.x < b.x; });
	auto ymin = std::min_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.y < b.y; });
	auto xmax = std::max_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.x < b.x; });
	auto ymax = std::max_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.y < b.y; });
	mbb.min = { xmin->x, ymin->y };
	mbb.max = { xmax->x, ymax->y };
	mbb.child = new Poly(poly);
	return mbb;
}

MBB& RTree::queryBestMBB(Node& current, MBB const& newMBB)
{
	int j = 0;
	auto maxDensity = computeEnlargement(current.regions[j], newMBB);
	for (int i = 1; i < current.regions.size(); ++i) {
		auto localDensity = computeEnlargement(current.regions[i], newMBB);
		if (localDensity > maxDensity) {
			j = i;
			maxDensity = localDensity;
		}
	}
	return current.regions[j];
}

void RTree::insertHelper(MBB newMBB)
{
	_size++;
	_height = ceil(log2(_size + 1) / log2ORDER) + 1;
	if (_root == nullptr) {
		_root = new Node;
		_root->regions.push_back(newMBB);
		return;
	}

	Node* current = _root;
	while (!(current->leaf)) {
		MBB& bestMBB = queryBestMBB(*current, newMBB);
		current = (Node*)(bestMBB.child);
	}

	MBB updatedMBB;
	while (true) {
		const unsigned regionSize = current->regions.size();
		if (regionSize < ORDER) {
			current->regions.push_back(newMBB);
			//updateFamilyRelations(*current);
			sortNode(*current);
			updateChilds(*current);
			updateParents(*current);
			return;
		}

		if (lendOverflow(*current, newMBB)) return;

		split(*current, updatedMBB, newMBB);

		if (current->parent == nullptr) break;

		findChild(*(current->parent), current) = updatedMBB;
		current = current->parent;
	}

	Node* newRoot = new Node;
	newRoot->parent = nullptr;
	newRoot->leaf = 0;
	newRoot->regions.push_back(updatedMBB);
	newRoot->regions.push_back(newMBB);
	updateChilds(*newRoot);
	//Node* a = (Node*) (updatedMBB.child);
	//Node* b = (Node*) (newMBB.child);
	///*a->left = nullptr;
	//a->right = b;
	//b->right = nullptr;
	//b->left = a;*/
	//a->parent = newRoot;
	//b->parent = newRoot;
	_root = newRoot;
	//updateFamilyRelations(*_root);
	//updateFamilyRelations(*a);
	//updateFamilyRelations(*b);
	//updateParents(*a);
	//updateParents(*b);
}

float RTree::computeEnlargement(MBB const& a, MBB const& b)
{
	float const xenlargment = (a.max.x > b.min.x && b.max.x > a.min.x) * (abs(a.max.x - b.max.x) + abs(a.min.x - b.min.x));
	float const yenlargment = (a.max.y > b.min.y && b.max.y > a.min.y) * (abs(a.max.y - b.max.y) + abs(a.min.y - b.min.y));

	return xenlargment + yenlargment;
}

float RTree::computeArea(float xmin, float ymin, float xmax, float ymax)
{
	return (xmax - xmin) * (ymax - ymin);
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
	float minEnlargment = std::numeric_limits<float>::max();
	for (int i = 0; i < (ORDER - 1); ++i) {
		for (int j = i + 1; j < ORDER; ++j) {
			float enlargment = computeEnlargement(regions[i], regions[j]);
			if (enlargment < minEnlargment) minEnlargment = enlargment, seed1 = i, seed2 = j;
		}
	}


	updatedMBB.child = &updatedNode;
	updatedNode.regions.clear();
	updatedNode.regions.push_back(regions[seed1]);
	updatedMBB.min = updatedNode.regions.back().min;
	updatedMBB.max = updatedNode.regions.back().max;


	newMBB.child = new Node;
	Node& newNode = *((Node*)(newMBB.child));
	newNode.leaf = updatedNode.leaf;
	newNode.parent = updatedNode.parent;
	newNode.regions.push_back(regions[seed2]);
	newMBB.min = newNode.regions.back().min;
	newMBB.max = newNode.regions.back().max;


	regions[seed2] = regions.back(); regions.pop_back();
	regions[seed1] = regions.back(); regions.pop_back();


	while (!(regions.empty()) && updatedNode.regions.size() < MINIMUM && newNode.regions.size() < MINIMUM) {
		int selectedMBB = 0;
		int selectedNode = 0;
		float maxDiffEnlargment = std::numeric_limits<float>::min();
		for (int i = 0; i < regions.size(); ++i) {
			float const updatedNodeEnlargment = computeEnlargement(updatedMBB, regions[i]);
			float const newNodeEnlargment = computeEnlargement(newMBB, regions[i]);
			int const node = updatedNodeEnlargment < newNodeEnlargment;
			float const diffEnlargment =
				node * (newNodeEnlargment - updatedNodeEnlargment) +
				!node * (updatedNodeEnlargment - newNodeEnlargment);
			if (maxDiffEnlargment < diffEnlargment) {
				selectedMBB = i;
				selectedNode = node;
				maxDiffEnlargment = diffEnlargment;
			}
		}
		if (selectedNode) {
			expandMBB(newMBB, regions[selectedMBB]);
			newNode.regions.push_back(regions[selectedMBB]);
		}
		else {
			expandMBB(updatedMBB, regions[selectedMBB]);
			updatedNode.regions.push_back(regions[selectedMBB]);
		}
		regions[selectedMBB] = regions.back();
		regions.pop_back();
	}

	if (updatedNode.regions.size() == MINIMUM) {
		for (auto& mbb : regions) {
			expandMBB(newMBB, mbb);
			newNode.regions.push_back(mbb);
		}
	}
	else {
		for (auto& mbb : regions) {
			expandMBB(updatedMBB, mbb);
			updatedNode.regions.push_back(mbb);
		}
	}
	updateChilds(updatedNode);
	updateChilds(newNode);
}

MBB& RTree::findChild(Node& parent, void* child)
{
	for (auto& mbb : parent.regions) {
		if (mbb.child == child) return mbb;
	}
	std::runtime_error("COULD NOT FIND CHILD!");
}

void RTree::forEachPolyHelper(std::function<void(Poly const&)> const& fun, Node& current)
{
	if (current.leaf) {
		for (auto& mbb : current.regions) {
			fun(*((Poly*)(mbb.child)));
		}
	}
	else {
		for (auto& mbb : current.regions) {
			forEachPolyHelper(fun, *((Node*)(mbb.child)));
		}
	}
}

void RTree::forEachMBBHelper(std::function<void(MBB const&, int, int)> const& fun, Node& current, int lvl)
{
	for (auto& mbb : current.regions) {
		fun(mbb, lvl, _height);
	}
	if (!(current.leaf)) {
		for (auto& mbb : current.regions) {
			forEachMBBHelper(fun, *((Node*)(mbb.child)), lvl + 1);
		}
	}
}

std::tuple<Vec2, float> RTree::computeMinDist(Vec2 const& fromPoint, MBB const& toMBB)
{
	Vec2 toPoint;

	toPoint.x = fromPoint.x +
		(fromPoint.x < toMBB.min.x) * (toMBB.min.x - fromPoint.x) +
		(fromPoint.x > toMBB.max.x) * (toMBB.max.x - fromPoint.x);
	toPoint.y = fromPoint.y +
		(fromPoint.y < toMBB.min.y) * (toMBB.min.y - fromPoint.y) +
		(fromPoint.y > toMBB.max.y) * (toMBB.max.y - fromPoint.y);

	float const xdiff = toPoint.x - fromPoint.x;
	float const ydiff = toPoint.y - fromPoint.y;
	float const distance = xdiff * xdiff + ydiff * ydiff;

	return { toPoint, distance };
}

bool RTree::isInside(Vec2 const& vec2, MBB const& mbb)
{
	return
		vec2.x >= mbb.min.x && vec2.x <= mbb.max.x &&
		vec2.y >= mbb.min.y && vec2.y <= mbb.max.y;
}

bool RTree::eraseHelper(Vec2 const& min, Vec2 const& max)
{
	if (_root == nullptr) return false;

	std::queue<Node*> bfs;
	bfs.push(_root);
	Node* current = nullptr;
	MBB* toErase = nullptr;

	while (!(bfs.empty())) {
		auto& front = bfs.front();
		for (auto& mbb : front->regions) {
			if (isIntersecting(min, max, mbb)) {
				if (front->leaf) {
					current = front;
					toErase = &mbb;
					goto BFS_END;
				}
				else {
					bfs.push((Node*)(mbb.child));
				}
			}
		}
		bfs.pop();
	}

BFS_END:
	if (toErase == nullptr) return false;
	if (_size == 1) {
		_size = 0;
		_height = 0;
		delete _root;
		_root = nullptr;
		return false;
	}
	eraseSelected(current, toErase);
	return true;
}

bool RTree::isIntersecting(Vec2 const& min, Vec2 const& max, MBB const& mbb)
{
	return
		(mbb.min.x <= max.x && mbb.max.x >= min.x) &&
		(mbb.min.y <= max.y && mbb.max.y >= min.y);
}

void* RTree::removeSubTree(Node& current, MBB& toReinsert)
{
	void* subTree = toReinsert.child;

	for (int i = 0; i < current.regions.size(); ++i) {
		if (&(current.regions[i]) == &toReinsert) {
			current.regions.erase(current.regions.begin() + i);
			break;
		}
	}

	updateParents(current);

	if (current.leaf) {
		_size -= 1;
		_height = ceil(log2(_size + 1) / log2ORDER) + 1;
		return subTree;
	}
	updateSizeAfterRemoval(*((Node*)subTree));
	return subTree;
}

void RTree::updateParents(Node& current)
{
	if (current.parent == nullptr) return;

	auto xmin = std::min_element(std::begin(current.regions), std::end(current.regions),
		[](MBB const& a, MBB const& b) {return a.min.x < b.min.x; });
	auto ymin = std::min_element(std::begin(current.regions), std::end(current.regions),
		[](MBB const& a, MBB const& b) {return a.min.y < b.min.y; });
	auto xmax = std::max_element(std::begin(current.regions), std::end(current.regions),
		[](MBB const& a, MBB const& b) {return a.max.x < b.max.x; });
	auto ymax = std::max_element(std::begin(current.regions), std::end(current.regions),
		[](MBB const& a, MBB const& b) {return a.max.y < b.max.y; });
	auto zmax = std::max_element(std::begin(current.regions), std::end(current.regions),
		[](MBB const& a, MBB const& b) {return a.z < b.z; });

	MBB& mbb = findChild(*(current.parent), &current);
	mbb.min = { xmin->min.x, ymin->min.y };
	mbb.max = { xmax->max.x, ymax->max.y };
	mbb.z = zmax->z;
	updateParents(*(current.parent));
}

void RTree::updateSizeAfterRemoval(Node& subCurrent)
{
	if (subCurrent.leaf) {
		_size -= subCurrent.regions.size();
		_height = ceil(log2(_size + 1) / log2ORDER) + 1;
	}
	else {
		for (auto& mbb : subCurrent.regions) {
			updateSizeAfterRemoval(*((Node*)(mbb.child)));
		}
	}
}

void RTree::reinsertExcept(Node& subCurrent, void* except)
{
	if (subCurrent.leaf) {
		for (auto& mbb : subCurrent.regions) {
			if (mbb.child != except) insertHelper(mbb);
		}
	}
	else {
		for (auto& mbb : subCurrent.regions) {
			reinsertExcept(*((Node*)(mbb.child)), except);
			delete mbb.child;
		}
	}
}

void RTree::testOverlappingHelper(Vec2 const& testPoint, Node& current, double& counter, double& total)
{
	total += 1;
	int options = 0;
	for (auto& mbb : current.regions) {
		if (isInside(testPoint, mbb)) {
			options = 1;
			if (current.leaf) counter += 1;
			else testOverlappingHelper(testPoint, *((Node*)(mbb.child)), counter, total);
		}
	}
	//counter += !options;
}

std::tuple<Node*, MBB*> RTree::pickRandom(Node& current)
{
	static unsigned long x = 123456789;
	static unsigned long y = 362436069;
	static unsigned long z = 521288629;

	unsigned long t;
	x ^= x << 16;
	x ^= x >> 5;
	x ^= x << 1;

	t = x;
	x = y;
	y = z;
	z = t ^ x ^ y;

	size_t const randomI = z % current.regions.size();
	if (current.leaf) return { &current, &(current.regions[randomI]) };
	return pickRandom(*((Node*)(current.regions[randomI].child)));
}

//void RTree::updateFamilyRelations(Node& current)
//{
//	std::sort(std::begin(current.regions), std::end(current.regions),
//		[](MBB const& a, MBB const& b) { return a.z < b.z; });
//
//	if (current.leaf) return;
//
//	if (current.left != nullptr) {
//		((Node*)(current.left->regions.back().child))->right = ((Node*)(current.regions.front().child));
//		((Node*)(current.regions.front().child))->left = ((Node*)(current.left->regions.back().child));
//	}
//	else {
//		((Node*)(current.regions.front().child))->left = nullptr;
//	}
//	((Node*)(current.regions[1].child))->left = ((Node*)(current.regions.front().child));
//	((Node*)(current.regions.front().child))->right = ((Node*)(current.regions[1].child));
//	((Node*)(current.regions.front().child))->parent = &current;
//	int i;
//	for (i = 1; i < (current.regions.size() - 1); ++i) {
//		((Node*)(current.regions[i - 1].child))->right = ((Node*)(current.regions[i].child));
//		((Node*)(current.regions[i].child))->left = ((Node*)(current.regions[i - 1].child));
//		((Node*)(current.regions[i + 1].child))->left = ((Node*)(current.regions[i].child));
//		((Node*)(current.regions[i].child))->right = ((Node*)(current.regions[i + 1].child));
//		((Node*)(current.regions[i].child))->parent = &current;
//	}
//	((Node*)(current.regions[i - 1].child))->right = ((Node*)(current.regions[i].child));
//	((Node*)(current.regions[i].child))->left = ((Node*)(current.regions[i - 1].child));
//	((Node*)(current.regions[i].child))->parent = &current;
//	if (current.right != nullptr) {
//		((Node*)(current.right->regions.front().child))->left = ((Node*)(current.regions[i].child));
//		((Node*)(current.regions[i].child))->right = ((Node*)(current.right->regions.front().child));
//	}
//	else {
//		((Node*)(current.regions[i].child))->right = nullptr;
//	}
//}

void RTree::sortNode(Node& current)
{
	std::sort(std::begin(current.regions), std::end(current.regions),
		[](MBB const& a, MBB const& b) { return a.z < b.z; });
}

void RTree::updateChilds(Node& current)
{
	if (current.leaf) return;
	for (auto& mbb : current.regions) {
		((Node*)(mbb.child))->parent = &current;
	}
}

void RTree::eraseSelected(Node* current, MBB* toErase)
{
	auto& toReinsert = toErase;
	void* toDelete = toReinsert->child;
	while (current->parent != nullptr && current->regions.size() == MINIMUM) {
		toReinsert = &(findChild(*(current->parent), current));
		current = current->parent;
	}
	if (current->parent == nullptr) {
		void* subTree = _root;
		_root = nullptr;
		_size = 0;
		_height = 0;
		reinsertExcept(*((Node*)subTree), toDelete);
		delete subTree;
		delete toDelete;
		return;
	}
	void* subTree = removeSubTree(*current, *toReinsert);
	if (subTree == toDelete) {
		delete toDelete;
		return;
	}
	reinsertExcept(*((Node*)subTree), toDelete);
	delete subTree;
	delete toDelete;
	return;
}

bool RTree::lendOverflow(Node& current, MBB& newMBB)
{
	return false;
}