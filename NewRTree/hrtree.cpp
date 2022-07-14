#include "hrtree.h"

HRTree::HRTree(int weight, int height) : RTree()
{
	_hilbertDimSize = std::max(weight, height);
    float n = (float) ceil(log(_hilbertDimSize) / log(2));
	_hilbertDimSize = pow(2, ceil(log((double)_hilbertDimSize) / log(2)));
	_hilbertMatrix = std::vector<std::vector<int>>((int)_hilbertDimSize, std::vector<int>((int)_hilbertDimSize));
    buildHilbertMatrix(n, 0, 0, _hilbertDimSize, 0, 0, _hilbertDimSize);
}

HRTree::~HRTree()
{
}

MBB HRTree::buildMBB(Poly const& poly)
{
    MBB mbb;
    float xavg = 0, yavg = 0;
    for (auto const& vertex : poly) {
        xavg += vertex.x;
        yavg += vertex.y;
    }
    xavg /= poly.size();
    yavg /= poly.size();

    auto xmin = std::min_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.x < b.x; });
    auto ymin = std::min_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.y < b.y; });
    auto xmax = std::max_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.x < b.x; });
    auto ymax = std::max_element(std::begin(poly), std::end(poly), [](Vec2 const& a, Vec2 const& b) {return a.y < b.y; });
    
    mbb.z = _hilbertMatrix[xavg][yavg];
    mbb.min = { xmin->x, ymin->y };
    mbb.max = { xmax->x, ymax->y };
    mbb.child = new Poly(poly);
    printf("\n\nTO INSERT : [(%.1f, %.1f), (%.1f, %.1f), z: %u]\n", mbb.min.x, mbb.min.y, mbb.max.x, mbb.max.y, mbb.z);


    return mbb;
}

MBB& HRTree::queryBestMBB(Node& current, MBB const& newMBB)
{
    for (auto& mbb : current.regions) {
        if (mbb.z > newMBB.z) return mbb;
    }
    current.regions.back().z = newMBB.z;
    return current.regions.back();
}


bool HRTree::lendOverflow(Node& current, MBB& newMBB)
{
    Node* sibling = queryLeftSibling(current);
    int i = 0;
    do {
        if (sibling == nullptr) return false;
        if (sibling->regions.size() < ORDER) break;
        sibling = queryLeftSibling(*sibling);
        ++i;
    } while (true);
    lendOverflowR(current, newMBB);
    return true;
}


void HRTree::split(Node& updatedNode, MBB& updatedMBB, MBB& newMBB)
{
    std::vector<MBB> regions(updatedNode.regions);
    regions.push_back(newMBB);
    std::sort(std::begin(regions), std::end(regions),
        [](MBB const& a, MBB const& b) { return a.z < b.z; });
    
    updatedMBB.child = &updatedNode;
    updatedNode.regions.clear();

    newMBB.child = new Node;
    Node& newNode = *((Node*)(newMBB.child));
    newNode.leaf = updatedNode.leaf;
    newNode.parent = updatedNode.parent;

    int i = 0;
    for (; i <= ORDER / 2; ++i) updatedNode.regions.push_back(regions[i]);
    for (; i <= ORDER; ++i) newNode.regions.push_back(regions[i]);

    updateChilds(updatedNode);
    updateChilds(newNode);


    //newNode.right = updatedNode.right;
    //if(newNode.right != nullptr )newNode.right->left = &newNode;
    //newNode.left = &updatedNode;
    //updatedNode.right = &newNode;
    //updateFamilyRelations(updatedNode);
    //updateFamilyRelations(newNode);

    auto xmin = std::min_element(std::begin(updatedNode.regions), std::end(updatedNode.regions),
        [](MBB const& a, MBB const& b) {return a.min.x < b.min.x; });
    auto ymin = std::min_element(std::begin(updatedNode.regions), std::end(updatedNode.regions),
        [](MBB const& a, MBB const& b) {return a.min.y < b.min.y; });
    auto xmax = std::max_element(std::begin(updatedNode.regions), std::end(updatedNode.regions),
        [](MBB const& a, MBB const& b) {return a.max.x < b.max.x; });
    auto ymax = std::max_element(std::begin(updatedNode.regions), std::end(updatedNode.regions),
        [](MBB const& a, MBB const& b) {return a.max.y < b.max.y; });
    auto zmax = std::max_element(std::begin(updatedNode.regions), std::end(updatedNode.regions),
        [](MBB const& a, MBB const& b) {return a.z < b.z; });
    updatedMBB.min = { xmin->min.x, ymin->min.y };
    updatedMBB.max = { xmax->max.x, ymax->max.y };
    updatedMBB.z = zmax->z;
    updateParents(updatedNode);

    xmin = std::min_element(std::begin(newNode.regions), std::end(newNode.regions),
        [](MBB const& a, MBB const& b) {return a.min.x < b.min.x; });
    ymin = std::min_element(std::begin(newNode.regions), std::end(newNode.regions),
        [](MBB const& a, MBB const& b) {return a.min.y < b.min.y; });
    xmax = std::max_element(std::begin(newNode.regions), std::end(newNode.regions),
        [](MBB const& a, MBB const& b) {return a.max.x < b.max.x; });
    ymax = std::max_element(std::begin(newNode.regions), std::end(newNode.regions),
        [](MBB const& a, MBB const& b) {return a.max.y < b.max.y; });
    zmax = std::max_element(std::begin(newNode.regions), std::end(newNode.regions),
        [](MBB const& a, MBB const& b) {return a.z < b.z; });

    newMBB.min = { xmin->min.x, ymin->min.y };
    newMBB.max = { xmax->max.x, ymax->max.y };
    newMBB.z = zmax->z;


    //updateParents(updatedNode);
    //updateParents(newNode);
}

Node* HRTree::queryLeftSibling(Node& current)
{
    int i = 0;
    int j = 0;
    Node* ancestor = current.parent;
    while (true) {
        if (ancestor == nullptr) return nullptr;

        ++j;
        i = 0;
        for (auto& mbb : current.parent->regions) {
            if (mbb.child == &current) break;
            ++i;
        }

        if (i > 0) break;
        ancestor = ancestor->parent;
    }
    --j;
    Node* sibling = ((Node*)(ancestor->regions[i - 1].child));
    for (; j > 0; --j) {
        sibling = ((Node*)(sibling->regions.back().child));
    }
    return sibling;
    
}



void HRTree::buildHilbertMatrix(float n, float x, float y, float xi, float xj, float yi, float yj)
{
    if (n <= 0) {
        _hilbertMatrix[x + ((xi + yi) / 2)][y + ((xj + yj) / 2)] = _hilbertCounter++;
    }
    else {
        buildHilbertMatrix(n - 1, x, y, yi / 2, yj / 2, xi / 2, xj / 2);
        buildHilbertMatrix(n - 1, x + xi / 2, y + xj / 2, xi / 2, xj / 2, yi / 2, yj / 2);
        buildHilbertMatrix(n - 1, x + xi / 2 + yi / 2, y + xj / 2 + yj / 2, xi / 2, xj / 2, yi / 2, yj / 2);
        buildHilbertMatrix(n - 1, x + xi / 2 + yi, y + xj / 2 + yj, -yi / 2, -yj / 2, -xi / 2, -xj / 2);
    }
}

void HRTree::lendOverflowR(Node& current, MBB& newMBB)
{
    if (current.regions.size() < ORDER) {
        current.regions.push_back(newMBB);
        //updateFamilyRelations(current);
        updateParents(current);
    }
    else if(current.regions.front().z < newMBB.z) {
        MBB aux = current.regions.front();
        current.regions[0] = newMBB;
        //updateFamilyRelations(current);
        updateParents(current);
        lendOverflowR(*(queryLeftSibling(current)), aux);
    }
    else {
        lendOverflowR(*(queryLeftSibling(current)), newMBB);
    }
}
