// NewRTree.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "rtree.h"


int main()
{
    RTree r;
    r.insert({ {5,5}, {10,10}, {15,5} });
    r.insert({ {15,15}, {20,10}, {20,15} });
    r.insert({ {35,10}, {35,15}, {40,10} });
    r.insert({ {35,20}, {40,20}, {40,15} });
    r.insert({ {30,0}, {30,5}, {35,5}, {40, 0}, {35, 0} });
    r.print();
    r.insert({ {40,5}, {45,10}, {45,0} });
    r.print();
}
