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

struct QuadTreeNode {
	QuadTreeNode* nxt[BRANCH_COUNT];
	QuadTreeNode* pre;
	int rowLow, rowHigh, columnLow, columnHigh;// the rows' and columns' range in this grid.
	int elementCount;//	Elements contained in this node  % with its subtree excluded. % 
	bool isLeaf, isMerged;

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
	int NUM; //Total row and column count. Must be pow of 2.  eg. NUM=8 means 8*8 Grid.

	GEQuadTree();

	GEQuadTree(int totNum);

	/* 	Build a new tree. 
				totnum: number of grid rows and columns;
			Return: root.
		every element in gridPtr will point to root initially.
		NUM would be set here.
	*/
	QuadTreeNode* build(int totNum);


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
				row: row number, starting from 0 to ROW-1.
				column: column number, starting from 0 to COLUMN-1.
				direction: Must be SOUTH/SOUTHEAST/EAST/NORTHEAST/NORTH/NORTHWEST/WEST/SOUTHWEST, marking the neighbour's direction.
			Return: the pointer of the neighbour node.
	*/
	QuadTreeNode* getNeighbourNodePtr(QuadTreeNode* node, int direction);

	~GEQuadTree();
};