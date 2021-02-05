#define BRANCH_COUNT 4
#define SOUTH 0x0100
#define NORTH 0x1000
#define EAST 0x0001
#define WEST 0x0010
#define SOUTHEAST 0x0101
#define NORTHEAST 0x1001
#define SOUTHWEST 0x0110
#define NORTHWEST 0x1010
#ifdef DEBUG
using namespace std;
#endif

struct Element {
	double x, y;
	Element();
	Element(double _x, double _y);
};

struct QuadTreeNode {
	QuadTreeNode* nxt[BRANCH_COUNT];
	QuadTreeNode* pre;
	int rowLow, rowHigh, columnLow, columnHigh;// the rows' and columns' range in this grid.
	int elementCount;//	Elements contained in this node  % with its subtree excluded. % 
	bool isLeaf, isMerged;
#ifdef DEBUG
	std::string name;
#endif
	QuadTreeNode();


	/* 	Split this node into 4 sons. <==> create its nxt[4]. */
	// 	CAUTION: Only leaf node can split!
	void split();
	

	/* 	Merge the 4 sons into 1 node. <==> delete its nxt[4]. */
	// 	CAUTION: Nodes with nonleaf sons cannot be merged!
	void merge();
};


struct GEQuadTree {
	QuadTreeNode* root;
	std::vector<QuadTreeNode*> gridPtr;// The bottom-left grid is labeled as grid[0][0].
	std::vector<std::vector<Element> > gridElements;
	int NUM; //Total row and column count. Must be pow of 2.  eg. NUM=8 means 8*8 Grid.
	double xMin, yMin, xTot, yTot;

	GEQuadTree();

	GEQuadTree(int totNum, double xMin, double yMin, double xTot, double yTot);

	/* 	Build a new tree. 
				totNum: number of grid rows and columns;
				xMin, yMin: the coordinate of the bottom-left corner.
				xTot, yTot: the side length of the area.
			Return: root.
		every element in gridPtr will point to root initially.
		NUM would be set here.
	*/
	QuadTreeNode* build(int totNum, double xMin, double yMin, double xTot, double yTot);


	/* 	Add an element.
				ele: the element.
			Return: the cell index (r, c) where ele is added into.

	*/
	std::pair<int, int> addElement(Element ele);


	/*	Get the pointer of the tree node which is grid[row][column] pointing to. 
				row: row number, starting from 0 to ROW-1.
				column: column number, starting from 0 to COLUMN-1.
			Return: pointer described above.
	*/
	QuadTreeNode* getNodePtr(int row, int column);


	/* 	Return the location (row, column) of the cell in the most SW/SE/NW/NE corner of the grid(a node).
				node: node's pointer.
				direction: Must be SOUTHEAST/NORTHEAST/NORTHWEST/SOUTHWEST, marking the corner's direction.
			Return: the pair(row, column) representing the corner cell's index.
	*/
	std::pair<int, int> calcCornerCell(QuadTreeNode* node, int direction);


	/*	Get grid[row][column]'s neighbour node.
		CAUTION: Would get exception when out of bound!
				row: row number, starting from 0 to ROW-1.
				column: column number, starting from 0 to COLUMN-1.
				direction: Must be SOUTH/SOUTHEAST/EAST/NORTHEAST/NORTH/NORTHWEST/WEST/SOUTHWEST, marking the neighbour's direction.
			Return: the pointer of the neighbour node.
	*/
	QuadTreeNode* getNeighbourNodePtr(QuadTreeNode* node, int direction);


	/*	Get node's all neighbour nodes.
				queryNode: query node.
			Return: an unordered_set containing all neighbours' pointers.
	*/
	std::unordered_set<QuadTreeNode*> getAllNeighbourNodePtr(QuadTreeNode* queryNode);


	/*	Get the pointer of the tree node by traversal. 
		Shouldn't be used in functions other than testing functions.
		The node must exist.
				str: the path. e.g. "202"= root->nxt[2]->nxt[0]->nxt[2].
			Return: the pointer of the node.
	*/
	QuadTreeNode* getNodePtrFromString(std::string str);

	/*	Get an unordered_set of nodes in the query range.
				xLow, xHigh, yLow, yHigh: the query range.
			Return: an unordered_set of nodes in the query range. 

	*/
	std::unordered_set<QuadTreeNode*> queryRange(double xLow, double xHigh, double yLow, double yHigh);

	/* Get a vector of k-nearest neighbours of (xQuery, yQuery).
				xQuery, yQuery: the query location.
				K: number of nearest neighbours.
			Return: a vector of of k-nearest neighbours of (xQuery, yQuery).
	*/
	std::vector<Element> querykNearestNeighbour(double xQuery, double yQuery, int K);

	~GEQuadTree();
};