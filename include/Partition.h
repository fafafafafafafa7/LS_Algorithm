#ifndef PARTITION_H
#define PARTITION_H

#include "utils.h"
#include "Graph.h"

class Partition {
public:
	int PartNum;
	std::vector<std::pair<int, int> > *EP;
	
	Partition(Graph *, int);

	// addEdge part u v
	void addEdge(int, int, int);
	void addEdge(int, std::pair<int, int>);
	int size(int);
	bool check();
	double cntCost();
	int cntVer();

	~Partition();

private:
	Graph *G;

	std::vector<std::pair<int, int> > GraphEdges;
	std::vector<std::pair<int, int> > PartitionEdges;

	std::set<int> vs;
};

#endif