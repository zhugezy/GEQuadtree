#include <vector>
#include <cstddef>
#include <functional>
#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <utility>
#include "GEQuadTree.h"
#ifdef DEBUG
using namespace std;
#endif
QuadTreeNode::QuadTreeNode() {
	nxt[0] = nxt[1] = nxt[2] = nxt[3] = NULL;
}

void QuadTreeNode::split() {
	isLeaf = false;
	isMerged = false; //delete it?
	int rowMid = (rowLow + rowHigh) >> 1, columnMid = (columnLow + columnHigh) >> 1;
	for (int i = 0; i < BRANCH_COUNT; ++i) {
		//nodes' nxt[] are all set NULL initially.
		if (nxt[i] == NULL) {
			nxt[i] = new QuadTreeNode();
			nxt[i]->pre = this;
			nxt[i]->depth = depth + 1;
			if (i ^ (i >> 1)) { //i == 1/2
				nxt[i]->columnLow = columnMid + 1;
				nxt[i]->columnHigh = columnHigh;
			} else {//i == 0/3
				nxt[i]->columnLow = columnLow;
				nxt[i]->columnHigh = columnMid;
			}
			if (i & 2) {// i == 2/3
				nxt[i]->rowLow = rowMid + 1;
				nxt[i]->rowHigh = rowHigh;
			} else { //i == 0/1
				nxt[i]->rowLow = rowLow;
				nxt[i]->rowHigh = rowMid;
			}
		}
		nxt[i]->elementCount = 0;
		nxt[i]->isLeaf = true;
		nxt[i]->isMerged = false;
	}
}

void QuadTreeNode::merge() {
	isLeaf = true;
	isMerged = false; //delete it?
	for (int i = 0; i < BRANCH_COUNT; ++i) {
		nxt[i]->isLeaf = false;
		nxt[i]->isMerged = true;
		//delete nxt[i];
	}
}

GEQuadTree::GEQuadTree() {}

GEQuadTree::GEQuadTree(int totNum) {
	build(totNum);
}

//Test: OK
GEQuadTree::~GEQuadTree() {
	std::function<void(QuadTreeNode*)> func;
	func = [&](QuadTreeNode* currentRoot) {
		if (!(currentRoot->isLeaf)) {
			for (int i = 0; i < BRANCH_COUNT; ++i) {
				func(currentRoot->nxt[i]);
			}
		}
		delete currentRoot;
	};
	func(root);
}

QuadTreeNode* GEQuadTree::build(int totNum) {
	root = new QuadTreeNode();
	root->elementCount = 0;
	root->depth = 0;
	root->rowLow = root->columnLow = 0;
	root->rowHigh = root->columnHigh = totNum - 1;
	root->pre = NULL;
	root->isLeaf = true;
	root->isMerged = false;
	gridPtr.resize(totNum * totNum, root);
	NUM = totNum;
	DEPTH = round(log2(totNum));
	return root;
}

QuadTreeNode* GEQuadTree::getNodePtr(int row, int column) {
	QuadTreeNode* ret = gridPtr[row * NUM + column];
	//Lazy update handled here:
	if (ret->isLeaf == false) {
		if (ret->isMerged == true) {
			//get up
			while (ret->isLeaf == false) {
				ret = ret->pre;
			}
		} else {
			//get down
			while (ret->isLeaf == false) {
				int c1 = (row & (1 << (DEPTH - ret->depth - 1))) > 0;
				int c2 = (column & (1 << (DEPTH - ret->depth - 1))) > 0;
				int id = ((c1<<1)|c2)^c1;//trick: (0,0)->0, (0,1)->1, (1,1)->2, (1,0)->3
				ret = ret->nxt[id];
			}
		}
		gridPtr[row * NUM + column] = ret;
	}
	return ret;
}

std::pair<int, int> GEQuadTree::calcCornerCell(QuadTreeNode* node, int direction) {
	if (direction == NORTHWEST) {
		return std::make_pair(node->rowHigh, node->columnLow);
	} else if (direction == NORTHEAST) {
		return std::make_pair(node->rowHigh, node->columnHigh);
	} else if (direction == SOUTHWEST) {
		return std::make_pair(node->rowLow, node->columnLow);
	} else {
		return std::make_pair(node->rowLow, node->columnHigh);
	}
}
/* findCorner:  direction --> which corner? 
	S/W/SW --> SW
	N/NW   --> NW
	E/SE   --> SE
	NE     --> NE
*/
QuadTreeNode* GEQuadTree::getNeighbourNodePtr(QuadTreeNode* node, int direction) {
	int dir_row = 0, dir_col = 0;
	std::pair<int, int> corner;
	if (direction & NORTH) {
		dir_row = 1;
		if (direction & EAST) {
			corner = calcCornerCell(node, NORTHEAST);
		} else {
			corner = calcCornerCell(node, NORTHWEST);
		}
	} else {
		if (direction & SOUTH) {
			dir_row = -1;
		}
		if (direction & EAST) {
			corner = calcCornerCell(node, SOUTHEAST);
		} else {
			corner = calcCornerCell(node, SOUTHWEST);
		}
	} 
	if (direction & EAST) {
		dir_col = 1;
	} else if (direction & WEST) {
		dir_col = -1;
	}
	
	return getNodePtr(corner.first + dir_row, corner.second + dir_col);
}