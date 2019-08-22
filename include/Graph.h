#ifndef GRAPH_H
#define GRAPH_H

#include "Edge.h"
#include "utils.h"

class Graph {
public:
	Edge *edges;
	int *head;
	int N, M;

	int *deg;
	int maxDeg;

	Graph (int, int); // init with the number of vertices and the number of edges
	void addEdge (int ,int, int num = 1);// add an edge from u to v

	~Graph ();

private:
	int cnt;
};

#endif