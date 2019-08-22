#ifndef SOLVER_H
#define SOLVER_H

#include "Graph.h"
#include "Partition.h"
#include "utils.h"

class Solver {
public:
	Solver(Graph *, int, int, int);

	Graph *G;
	int PartNum;

	Partition *Pa;
	int UpSize;
	int LowSize;

	virtual void solve(){
		//std::cout << "fuck" << std::endl;
	}

	virtual void setPartitionFile(std::string){

	}

	void storePartition(std::string);

	~Solver();
};

#endif