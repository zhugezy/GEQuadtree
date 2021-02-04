#include <vector>
#include <cstddef>
#include <iostream>
#include <cstdlib>
#include <ctime>
#include <cassert>
#include <unordered_set>
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
	std::cout << "root address: " << tree.build(8, 0, 0, 80, 80) << std::endl;
	for (int i = 0; i < 5; ++i) {
		int r = rand() % 8, c = rand() % 8;
		std::cout << "r = " << r << " c = " << c << " add = " << tree.getNodePtr(r, c) << std::endl;
	}
	
}

void test2() {
	srand(time(NULL));
	GEQuadTree tree;
	std::cout << "root address: " << tree.build(8, 0, 0, 80, 80) << std::endl;
	tree.root->split();
}
GEQuadTree tree;
void basictest() {
	
	tree.build(8, 0, 0, 80, 80);
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
	tree.getNodePtrFromString("02")->split();
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("022"), NORTH) == tree.getNodePtrFromString("3"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("022"), SOUTH) == tree.getNodePtrFromString("021"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("022"), WEST) == tree.getNodePtrFromString("023"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("022"), EAST) == tree.getNodePtrFromString("1"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("022"), NORTHEAST) == tree.getNodePtrFromString("20"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("022"), NORTHWEST) == tree.getNodePtrFromString("3"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("022"), SOUTHEAST) == tree.getNodePtrFromString("1"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("022"), SOUTHWEST) == tree.getNodePtrFromString("020"));

	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("20"), NORTH) == tree.getNodePtrFromString("230"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("20"), SOUTH) == tree.getNodePtrFromString("1"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("20"), WEST) == tree.getNodePtrFromString("3"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("20"), EAST) == tree.getNodePtrFromString("21"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("20"), NORTHEAST) == tree.getNodePtrFromString("22"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("20"), NORTHWEST) == tree.getNodePtrFromString("3"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("20"), SOUTHEAST) == tree.getNodePtrFromString("1"));
	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("20"), SOUTHWEST) == tree.getNodePtrFromString("022"));

	assert(tree.getNeighbourNodePtr(tree.getNodePtrFromString("22"), WEST) == tree.getNodePtrFromString("231"));
	std::cout << "Getneighbour test passed" << std::endl;
	//=======================================

	unordered_set<QuadTreeNode*> st;
	unordered_set<string> st2;
	int cnt;
	
	st = tree.queryRange(23, 62, 35, 70);
	st2.clear();
	cnt = 0;
	for (QuadTreeNode* ptr: st) {
		cnt++;
		st2.insert(ptr->name);
	}
	assert(st2.find("023") != st2.end());
	assert(st2.find("022") != st2.end());
	assert(st2.find("1") != st2.end());
	assert(st2.find("3") != st2.end());
	assert(st2.find("20") != st2.end());
	assert(st2.find("21") != st2.end());
	assert(st2.find("230") != st2.end());
	assert(st2.find("231") != st2.end());
	assert(st2.find("22") != st2.end());
	assert(st2.find("232") != st2.end());
	assert(st2.find("233") != st2.end());
	assert(cnt == 11);
	

	st = tree.queryRange(16, 53, 16, 23);
	st2.clear();
	cnt = 0;
	for (QuadTreeNode* ptr: st) {
		cnt++;
		st2.insert(ptr->name);
	}
	assert(st2.find("00") != st2.end());
	assert(st2.find("03") != st2.end());
	assert(st2.find("020") != st2.end());
	assert(st2.find("013") != st2.end());
	assert(st2.find("021") != st2.end());
	assert(st2.find("012") != st2.end());
	assert(st2.find("1") != st2.end());
	assert(cnt == 7);

	st = tree.queryRange(13, 21, 67, 68);
	st2.clear();
	cnt = 0;
	for (QuadTreeNode* ptr: st) {
		cnt++;
		st2.insert(ptr->name);
	}
	assert(st2.find("3") != st2.end());
	assert(cnt == 1);

	st = tree.queryRange(22, 79, 34, 34);
	st2.clear();
	cnt = 0;
	for (QuadTreeNode* ptr: st) {
		cnt++;
		st2.insert(ptr->name);
	}
	assert(st2.find("023") != st2.end());
	assert(st2.find("022") != st2.end());
	assert(st2.find("1") != st2.end());
	assert(cnt == 3);

	st = tree.queryRange(1, 3, 19, 64);
	st2.clear();
	cnt = 0;
	for (QuadTreeNode* ptr: st) {
		cnt++;
		st2.insert(ptr->name);
	}
	assert(st2.find("00") != st2.end());
	assert(st2.find("03") != st2.end());
	assert(st2.find("3") != st2.end());
	assert(cnt == 3);

	st = tree.queryRange(31, 49, 12, 64);
	st2.clear();
	cnt = 0;
	for (QuadTreeNode* ptr: st) {
		cnt++;
		st2.insert(ptr->name);
	}
	assert(st2.find("012") != st2.end());
	assert(st2.find("021") != st2.end());
	assert(st2.find("022") != st2.end());
	assert(st2.find("3") != st2.end());
	assert(st2.find("1") != st2.end());
	assert(st2.find("20") != st2.end());
	assert(st2.find("230") != st2.end());
	assert(cnt == 7);

	std::cout << "Range Query (1) test passed" << std::endl;
	//=======================================

	//y=y*2+10, x=x+5 and testing again.
	GEQuadTree tree2;
	tree2.build(8, 5, 10, 80, 160);
	tree2.root->split();
	tree2.getNodePtrFromString("2")->split();
	tree2.getNodePtrFromString("23")->split();
	tree2.getNodePtrFromString("0")->split();
	tree2.getNodePtrFromString("01")->split();
	tree2.getNodePtrFromString("02")->split();

	st = tree2.queryRange(28, 67, 80, 150);
	st2.clear();
	cnt = 0;
	for (QuadTreeNode* ptr: st) {
		cnt++;
		st2.insert(ptr->name);
	}
	assert(st2.find("023") != st2.end());
	assert(st2.find("022") != st2.end());
	assert(st2.find("1") != st2.end());
	assert(st2.find("3") != st2.end());
	assert(st2.find("20") != st2.end());
	assert(st2.find("21") != st2.end());
	assert(st2.find("230") != st2.end());
	assert(st2.find("231") != st2.end());
	assert(st2.find("22") != st2.end());
	assert(st2.find("232") != st2.end());
	assert(st2.find("233") != st2.end());
	assert(cnt == 11);
	

	st = tree2.queryRange(21, 58, 42, 56);
	st2.clear();
	cnt = 0;
	for (QuadTreeNode* ptr: st) {
		cnt++;
		st2.insert(ptr->name);
	}
	assert(st2.find("00") != st2.end());
	assert(st2.find("03") != st2.end());
	assert(st2.find("020") != st2.end());
	assert(st2.find("013") != st2.end());
	assert(st2.find("021") != st2.end());
	assert(st2.find("012") != st2.end());
	assert(st2.find("1") != st2.end());
	assert(cnt == 7);

	st = tree2.queryRange(18, 26, 144, 146);
	st2.clear();
	cnt = 0;
	for (QuadTreeNode* ptr: st) {
		cnt++;
		st2.insert(ptr->name);
	}
	assert(st2.find("3") != st2.end());
	assert(cnt == 1);

	std::cout << "Range Query (2) test passed" << std::endl;
	//=======================================
}


int main() {
	basictest();
	advancedtest();
}