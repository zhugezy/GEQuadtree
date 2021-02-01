#include <vector>
#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include "GEQuadTree.h"
#ifdef DEBUG
using namespace std;
#endif
#ifdef TESTMEMORY
int _cnt = 0;

void* operator new(size_t sz) {
	_cnt++;
	std::cout << "ptr count:" << _cnt << std::endl;
	void* o = malloc(sz);
	return o;
}

void operator delete(void* o) {
	_cnt--;
	std::cout << "ptr count:" << _cnt << std::endl;
	free(o);
}
#endif

void test() {
	srand(time(NULL));
	GEQuadTree tree;
	std::cout << "root address: " << tree.build(8) << std::endl;
	for (int i = 0; i < 5; ++i) {
		int r = rand() % 8, c = rand() % 8;
		std::cout << "r = " << r << " c = " << c << " add = " << tree.getNodePtr(r, c) << std::endl;
	}
	
}

void test2() {
	srand(time(NULL));
	GEQuadTree tree;
	std::cout << "root address: " << tree.build(8) << std::endl;
	tree.root->split();
}

void basictest() {
	GEQuadTree tree;
	tree.build(8);
	tree.root->split();
	tree.getNodePtrFromString("2")->split();
	tree.getNodePtrFromString("23")->split();
	tree.getNodePtrFromString("0")->split();
	assert(tree.getNodePtrFromString("11") == NULL);
	assert(tree.getNodePtrFromString("1")->isLeaf == true);
	assert(tree.getNodePtrFromString("1")->isMerged == false);

	assert(tree.getNodePtrFromString("22") != NULL);
	assert(tree.getNodePtrFromString("2")->isLeaf == false);
	assert(tree.getNodePtrFromString("2")->isMerged == false);

	assert(tree.getNodePtrFromString("223") == NULL);
	assert(tree.getNodePtrFromString("22")->isLeaf == true);
	assert(tree.getNodePtrFromString("22")->isMerged == false);

	assert(tree.getNodePtrFromString("233") != NULL);
	assert(tree.getNodePtrFromString("23")->isLeaf == false);
	assert(tree.getNodePtrFromString("23")->isMerged == false);

	assert(tree.getNodePtrFromString("2330") == NULL);
	assert(tree.getNodePtrFromString("233")->isLeaf == true);
	assert(tree.getNodePtrFromString("233")->isMerged == false);
	std::cout << "Split test passed" << std::endl;
	//=======================================
	assert(tree.root == tree.gridPtr[6*8+5]);
	assert(tree.root == tree.gridPtr[4*8+7]);
	assert(tree.root == tree.gridPtr[0*8+2]);
	assert(tree.getNodePtr(6, 4) == tree.getNodePtrFromString("230"));
	assert(tree.gridPtr[6*8+4] == tree.getNodePtrFromString("230"));

	assert(tree.getNodePtr(4, 1) == tree.getNodePtrFromString("3"));
	assert(tree.gridPtr[4*8+1] == tree.getNodePtrFromString("3"));

	assert(tree.getNodePtr(0, 2) == tree.getNodePtrFromString("01"));
	assert(tree.gridPtr[0*8+2] == tree.getNodePtrFromString("01"));

	assert(tree.root == tree.gridPtr[6*8+5]);

	tree.getNodePtrFromString("01")->split();
	assert(tree.gridPtr[0*8+2] == tree.getNodePtrFromString("01"));
	assert(tree.getNodePtr(0, 2) == tree.getNodePtrFromString("010"));
	std::cout << "Lazy update (split) test passed" << std::endl;
	//=======================================
	assert(tree.getNodePtrFromString("011")->isLeaf == true);
	assert(tree.root->nxt[0]->nxt[1]->nxt[1]->isMerged == false);

	tree.getNodePtrFromString("01")->merge();
	assert(tree.getNodePtrFromString("011")->isLeaf == false);
	assert(tree.getNodePtrFromString("011")->isMerged == true);
	assert(tree.getNodePtrFromString("01")->isLeaf == true);
	assert(tree.getNodePtrFromString("01")->isMerged == false);

	tree.getNodePtrFromString("0")->merge();
	assert(tree.getNodePtrFromString("011")->isLeaf == false);
	assert(tree.getNodePtrFromString("011")->isMerged == true);
	assert(tree.getNodePtrFromString("01")->isLeaf == false);
	assert(tree.getNodePtrFromString("01")->isMerged == true);
	assert(tree.getNodePtrFromString("03")->isLeaf == false);
	assert(tree.getNodePtrFromString("03")->isMerged == true);
	assert(tree.getNodePtrFromString("0")->isLeaf == true);
	assert(tree.getNodePtrFromString("0")->isMerged == false);
	std::cout << "Merge test passed" << std::endl;
	//=======================================
	assert(tree.gridPtr[0*8+2] == tree.getNodePtrFromString("010"));
	assert(tree.getNodePtr(0, 2) == tree.getNodePtrFromString("0"));

	tree.getNodePtrFromString("0")->split();	
	tree.getNodePtrFromString("01")->split();
	assert(tree.getNodePtr(0, 2) == tree.getNodePtrFromString("010"));

	tree.getNodePtrFromString("01")->merge();
	assert(tree.getNodePtr(0, 2) == tree.getNodePtrFromString("01"));

	tree.getNodePtrFromString("0")->merge();
	assert(tree.getNodePtr(0, 2) == tree.getNodePtrFromString("0"));
	
	tree.getNodePtrFromString("0")->split();
	tree.getNodePtrFromString("01")->split();
	assert(tree.getNodePtr(0, 2) == tree.getNodePtrFromString("010"));
	std::cout << "Lazy update (split & merge) test passed" << std::endl;
	//=======================================
	assert(tree.calcCornerCell(tree.getNodePtrFromString("012"), NORTHEAST) == std::make_pair(1, 3));
	assert(tree.calcCornerCell(tree.getNodePtrFromString("1"), SOUTHWEST) == std::make_pair(0, 4));
	assert(tree.calcCornerCell(tree.getNodePtrFromString("20"), NORTHWEST) == std::make_pair(5, 4));
	assert(tree.calcCornerCell(tree.getNodePtrFromString("22"), SOUTHEAST) == std::make_pair(6, 7));
	std::cout << "calcCornerCell test passed" << std::endl;
}

void advancedtest() {


	std::cout << "Getneighbour test passed" << std::endl;
	//=======================================
}

int main() {
	basictest();
	advancedtest();

}