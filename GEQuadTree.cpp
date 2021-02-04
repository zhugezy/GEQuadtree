#include <vector>
#include <cstddef>
#include <functional>
#include <iostream>
#include <cmath>
#include <cstring>
#include <string>
#include <utility>
#include <unordered_set>
#include <queue>
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
			if ((i & 1) ^ (i >> 1)) { //i == 1/2
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
			#ifdef DEBUG
			nxt[i]->name = this->name + (char)(i + '0');
			#endif
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

GEQuadTree::GEQuadTree(int totNum, double xMin, double yMin, double xTot, double yTot) {
	build(totNum, xMin, yMin, xTot, yTot);
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

QuadTreeNode* GEQuadTree::build(int totNum, double xMin, double yMin, double xTot, double yTot) {
	root = new QuadTreeNode();
	root->elementCount = 0;
	root->rowLow = root->columnLow = 0;
	root->rowHigh = root->columnHigh = totNum - 1;
	root->pre = NULL;
	root->isLeaf = true;
	root->isMerged = false;
#ifdef DEBUG
	root->name = "";
#endif
	gridPtr.resize(totNum * totNum, root);
	NUM = totNum;
	this->xMin = xMin;
	this->yMin = yMin;
	this->xTot = xTot;
	this->yTot = yTot;
	return root;
}

QuadTreeNode* GEQuadTree::getNodePtrFromString(std::string str) {
	QuadTreeNode* ret = root;
	for (int i = 0; i < str.size(); ++i) {
		ret = ret->nxt[str[i] - '0'];
	}
	return ret;
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
				int c1 = row > ((ret->rowLow + ret->rowHigh) >> 1);
				int c2 = column > ((ret->columnLow + ret->columnHigh) >> 1);
				int id = ((c1<<1)|c2)^c1;//bitwise-trick: (c1,c2)->id: (0,0)->0, (0,1)->1, (1,1)->2, (1,0)->3
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
	//if (corner.first + dir_row < NUM && corner.second + dir_col < NUM && corner.second + dir_col >= 0 && corner.first + dir_row > 0)
		return getNodePtr(corner.first + dir_row, corner.second + dir_col);
	//else
		//return NULL;
}

bool segIntersect(double l1, double r1, double l2, double r2) {
	return !((r1 < l2) || (r2 < l1));
}

bool segContain(double l1, double r1, double l2, double r2) {
	if (l1 > l2) {
		swap(l1, l2);
		swap(r1, r2);
	}
	return l1 <= l2 && r1 <= r2;
}
std::unordered_set<QuadTreeNode*> GEQuadTree::queryRange(double xLow, double xHigh, double yLow, double yHigh) {
	int columnLow = (xLow - this->xMin) / (xTot / NUM);
	int rowLow = (yLow - this->yMin) / (yTot / NUM);
	std::unordered_set<QuadTreeNode*> ret;
	std::queue<std::pair<int, int> > que;
	ret.insert(getNodePtr(rowLow, columnLow));
	que.push(std::make_pair(rowLow, columnLow));
	while (!que.empty()) {
		pair<int, int> curCellID = que.front();
		que.pop();
		QuadTreeNode* curNode = getNodePtr(curCellID.first, curCellID.second);
		//cout << curNode->name << endl;

		pair<int, int> rightCellID;
		rightCellID.first = curCellID.first;
		rightCellID.second = curNode->columnHigh + 1;
		if (rightCellID.second < NUM) {
			QuadTreeNode* rightNeighbour = getNodePtr(rightCellID.first, rightCellID.second);
			if (rightNeighbour != NULL) {
				//cout << "check: " << rightNeighbour->name << endl;
				//cout << rightNeighbour->rowLow << " " << rightNeighbour->rowHigh << " " << rightNeighbour->columnLow << " " << rightNeighbour->columnHigh << endl;
				if (rightCellID.second * (xTot / NUM) <= xHigh && ret.find(rightNeighbour) == ret.end()) {
					ret.insert(rightNeighbour);
					if (!segIntersect(curNode->rowLow * (yTot / NUM), curNode->rowHigh * (yTot / NUM), yLow, yLow)) {
						rightCellID = calcCornerCell(rightNeighbour, SOUTHWEST);
					}
					que.push(rightCellID);
				}
			}
		}
		

		pair<int, int> upperCellID;
		upperCellID.first = curNode->rowHigh + 1;
		upperCellID.second = curCellID.second;
		if (upperCellID.first < NUM) {
			QuadTreeNode* upperNeighbour = getNodePtr(upperCellID.first, upperCellID.second);
			if (upperNeighbour != NULL) {
				//cout << "check: " << upperNeighbour->name << endl;
				//cout << upperNeighbour->rowLow << " " << upperNeighbour->rowHigh << " " << upperNeighbour->columnLow << " " << upperNeighbour->columnHigh << endl;
				if (upperCellID.first * (yTot / NUM) <= yHigh && ret.find(upperNeighbour) == ret.end()) {
					ret.insert(upperNeighbour);
					if (!segIntersect(curNode->columnLow * (xTot / NUM), curNode->columnHigh * (xTot / NUM), xLow, xLow)) {
						upperCellID = calcCornerCell(upperNeighbour, SOUTHWEST);
					}
					que.push(upperCellID);
				}
			}
		}
		
		
	}
	return ret;
}