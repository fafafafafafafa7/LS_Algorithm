#ifndef MoveBlockG_H
#define MoveBlockG_H

#include "BLS.h"

class MoveBlockG : public BLS {
public:
	MoveBlockG(Graph *, Partition *, int, int);

	void localSearch();

	~MoveBlockG();

private:
	std::vector<int> *blocks; // blocks[i] stores the ith block
	int *blockEdgeSize; // blockEdgeSize[i] is the number of edges of blocks[i]
	int blockCount; // the number of blocks in part i
	int *blockInd; // the index of each sorted block
	bool *vis; // used in bfs
	int *partInd;
	int *blockSizeCount;

	bool stopCheck(clock_t);

	int findMaxPart();
	void blockSearch(int &); // searching all blocks in part i
	void bfs(int &, int &); // start bfs from vertex v in part i
	void setBlockInd(); // initialize blockInd
	bool cmp(int, int); // compare function of array blockInd
	bool move(int &, int &); // move the block from part i, reture true if success
	int mostSuit(int &, int &); // return the index of the part which is most suitable for the block, return -1 if there is no suitable part
	int cap(std::vector<int> &, int &); // return the number of vertices both in block and part i
	int edgeMostSuit(int *, int &, int &, int &); // return the most suitable part for an edge
	void setPartInd();
	bool cmpPart(int, int);

	void blockTot();
};

#endif