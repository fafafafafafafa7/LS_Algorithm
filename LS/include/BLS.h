#ifndef BLS_H
#define BLS_H

#include "Graph.h"
#include "Partition.h"

#define BITSET_SIZE 128

class BLS {
public:
	BLS(Graph *, Partition *, int, int);

	virtual void localSearch() {

	}

	~BLS();

protected:
	Partition *Pa;
	Graph *G;
	int UpSize;
	int LowSize; // no use now

	int nowCost;

	std::set<int> **NB; // NB[i][v] is the set of neighbor of vertex v in part i
	int **degree; // degree[i][v] is the degree of vertex v in part i, this array is set to accelerate the speed of asking degrees
	std::bitset<BITSET_SIZE> *partCover; // partCover[v] is the set of reachable part for vertex v
	std::set<int> *Ver; // Ver[i] stores the vertices in part i

	std::set<std::pair<int, int> > *EPS; // Edge Partition stored in sets;

	int *partOrder;

	void shufflePartOrder();
	
	void moveEdge(int &, int &, int &, int &); // move edge (u, v) from part i to part j
	bool adjustable(int &, int &);

	void restore();

private:
	void setEPS();
	void setNB();
	void setDegree();
	void setVer();
	void setPartCover();
	void setPartOrder();
};

#endif