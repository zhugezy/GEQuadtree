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
	tree.root->nxt[2]->split();
	tree.root->nxt[2]->nxt[3]->split();
	tree.root->nxt[0]->split();
	assert(tree.root->nxt[1]->nxt[1] == NULL);
	assert(tree.root->nxt[1]->isLeaf == true);
	assert(tree.root->nxt[1]->isMerged == false);

	assert(tree.root->nxt[2]->nxt[2] != NULL);
	assert(tree.root->nxt[2]->isLeaf == false);
	assert(tree.root->nxt[2]->isMerged == false);

	assert(tree.root->nxt[2]->nxt[2]->nxt[3] == NULL);
	assert(tree.root->nxt[2]->nxt[2]->isLeaf == true);
	assert(tree.root->nxt[2]->nxt[2]->isMerged == false);

	assert(tree.root->nxt[2]->nxt[3]->nxt[3] != NULL);
	assert(tree.root->nxt[2]->nxt[3]->isLeaf == false);
	assert(tree.root->nxt[2]->nxt[3]->isMerged == false);

	assert(tree.root->nxt[2]->nxt[3]->nxt[3]->nxt[0] == NULL);
	assert(tree.root->nxt[2]->nxt[3]->nxt[3]->isLeaf == true);
	assert(tree.root->nxt[2]->nxt[3]->nxt[3]->isMerged == false);
	std::cout << "Split test passed" << std::endl;
	//=======================================
	assert(tree.root == tree.gridPtr[6*8+5]);
	assert(tree.root == tree.gridPtr[4*8+7]);
	assert(tree.root == tree.gridPtr[0*8+2]);
	assert(tree.getNodePtr(6, 4) == tree.root->nxt[2]->nxt[3]->nxt[0]);
	assert(tree.gridPtr[6*8+4] == tree.root->nxt[2]->nxt[3]->nxt[0]);

	assert(tree.getNodePtr(4, 1) == tree.root->nxt[3]);
	assert(tree.gridPtr[4*8+1] == tree.root->nxt[3]);

	assert(tree.getNodePtr(0, 2) == tree.root->nxt[0]->nxt[1]);
	assert(tree.gridPtr[0*8+2] == tree.root->nxt[0]->nxt[1]);

	assert(tree.root == tree.gridPtr[6*8+5]);

	tree.root->nxt[0]->nxt[1]->split();
	assert(tree.gridPtr[0*8+2] == tree.root->nxt[0]->nxt[1]);
	assert(tree.getNodePtr(0, 2) == tree.root->nxt[0]->nxt[1]->nxt[0]);
	std::cout << "Lazy update (split) test passed" << std::endl;
	//=======================================
	assert(tree.root->nxt[0]->nxt[1]->nxt[1]->isLeaf == true);
	assert(tree.root->nxt[0]->nxt[1]->nxt[1]->isMerged == false);

	tree.root->nxt[0]->nxt[1]->merge();
	assert(tree.root->nxt[0]->nxt[1]->nxt[1]->isLeaf == false);
	assert(tree.root->nxt[0]->nxt[1]->nxt[1]->isMerged == true);
	assert(tree.root->nxt[0]->nxt[1]->isLeaf == true);
	assert(tree.root->nxt[0]->nxt[1]->isMerged == false);

	tree.root->nxt[0]->merge();
	assert(tree.root->nxt[0]->nxt[1]->nxt[1]->isLeaf == false);
	assert(tree.root->nxt[0]->nxt[1]->nxt[1]->isMerged == true);
	assert(tree.root->nxt[0]->nxt[1]->isLeaf == false);
	assert(tree.root->nxt[0]->nxt[1]->isMerged == true);
	assert(tree.root->nxt[0]->nxt[3]->isLeaf == false);
	assert(tree.root->nxt[0]->nxt[3]->isMerged == true);
	assert(tree.root->nxt[0]->isLeaf == true);
	assert(tree.root->nxt[0]->isMerged == false);
	std::cout << "Merge test passed" << std::endl;
	//=======================================
	assert(tree.gridPtr[0*8+2] == tree.root->nxt[0]->nxt[1]->nxt[0]);
	assert(tree.getNodePtr(0, 2) == tree.root->nxt[0]);

	tree.root->nxt[0]->split();	
	tree.root->nxt[0]->nxt[1]->split();
	assert(tree.getNodePtr(0, 2) == tree.root->nxt[0]->nxt[1]->nxt[0]);

	tree.root->nxt[0]->nxt[1]->merge();
	assert(tree.getNodePtr(0, 2) == tree.root->nxt[0]->nxt[1]);

	tree.root->nxt[0]->merge();
	assert(tree.getNodePtr(0, 2) == tree.root->nxt[0]);
	
	tree.root->nxt[0]->split();
	tree.root->nxt[0]->nxt[1]->split();
	assert(tree.getNodePtr(0, 2) == tree.root->nxt[0]->nxt[1]->nxt[0]);
	std::cout << "Lazy update (split & merge) test passed" << std::endl;
	//=======================================
	assert(tree.calcCornerCell(tree.root->nxt[0]->nxt[1]->nxt[2], NORTHEAST) == std::make_pair(1, 3));
	assert(tree.calcCornerCell(tree.root->nxt[1], SOUTHWEST) == std::make_pair(0, 4));
	assert(tree.calcCornerCell(tree.root->nxt[2]->nxt[0], NORTHWEST) == std::make_pair(5, 4));
	assert(tree.calcCornerCell(tree.root->nxt[2]->nxt[2], SOUTHEAST) == std::make_pair(6, 7));
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