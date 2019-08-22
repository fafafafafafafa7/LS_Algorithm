#ifndef MoveBlockF_H
#define MoveBlockF_H

#include "BLS.h"
#include "Flow.h"

class MoveBlockF : public BLS{
public:
	MoveBlockF(Graph *, Partition *, int, int);
	void localSearch();
	~MoveBlockF();


	struct Block {
		std::vector<int> ver;
		int edgeSize;
		int verSize;
		int part;
		int deg;
		int seed;

		Block(const int &); // set the part
		void output() {
			std::cout << "------" << std::endl;
			std::cout << "Address: " << this << std::endl;
			std::cout << "part: " << part << std::endl;
			std::cout << "ver size: " << ver.size() << std::endl;
			std::cout << "edge size: " << edgeSize << std::endl;
			std::cout << "deg: " << deg << std::endl;
			std::cout << "ver: ";
			for (auto v : ver) std::cout << v << " ";
			std::cout << std::endl;
		}
	};

	struct BlockNode {
		Block * block;

		BlockNode(Block *);
		BlockNode();

		bool operator < (const BlockNode &) const;
	};
private:

	int *blockCount; // blockCount[s] is the number of blocks of size s

	Block ***verToBlockAdd; // verToBlockAdd[i][v] is the address of the block containing v in part i

	std::set<BlockNode> BlockBar; // BlockBar[i][s] is the set of addresses of blocks
	bool *vis; // used in bfs

	std::vector<Block *> IndepenBlocks; // store the independent blocks set
	std::vector<Block *> punishBlocks;

	bool *verColor;
	std::vector<int> coloredVer;

	std::map<Block *, int> movePlanBlock; // block plans to move to movePlanBlock[block]
	std::map<std::pair<int, int>, std::pair<int, int> > movePlanEdge; // edge plans to move to movePlanEdge[edge]
	int *tmpSize; // the size of each part after setting movePlan

	std::set<std::pair<int, int> > *tmpAdjEdge; // tmpAdgEdge[i] is the set of adjustable edges of the block in part i
	std::pair<int, int> *adjEdgeTable;
	int adjEdgeCount;

	int *splitVis;
	int splitTime;

	Flow *flow;

	int tmpCost;

	void blockTot(const char *);
	void moveTot(const char *);

	void setDeg(Block *);
	void setVerSize(Block *);

	bool stopCheck(clock_t);

	void setBlockBar(); // initialize BlockBar
	Block * bfs(const int &, const int &); // bfs(i, v) bfs starts from v in part i

	void getIndeBlo(); // get an independent blocks set
	void punish();
	void setPunishBlocks();
	//Block * randPickBlock(std::set<Block *> &); // random pick a block
	Block * greedyPickBlock(std::set<BlockNode> &);

	bool maintainBlock(Block *); // split and merge, return true if some change take place
	bool split(Block *); // return true if suc
	bool merge(Block *); // return true if suc
	Block * splitBfs(const int &, const int &);
	void setSplitVis();
	
	void setVerColor();
	bool checkColor(Block *);
	void color(Block *);
	void resetColor();

	bool planMove(); // return true if suc
	void setTmpSize();
	int mostSuit(Block *);
	int cap(Block *, const int &);

	bool flowMove();
	int setTmpAdjEdge(); // get adj edges for each block

	void applyMove();
	
	void resetBlock(Block *);
	void newBlock(const int &, const int &);
	void setBlock(Block *);
};

#endif