#include "Graph.h"

Graph::Graph(int NN, int MM) {
	N = NN;
	M = MM;
	head = new int[NN + 10];
	edges = new Edge[2 * MM + 10];
	deg = new int[N + 10];
	cnt = maxDeg = 0;
	for (int i = 0; i < N + 10; ++ i) head[i] = -1;
}

Graph::~Graph() {
	delete [] head;
	delete [] edges;
	delete [] deg;
}

void Graph::addEdge(int u, int v, int num) {
	edges[cnt].to = v;
	edges[cnt].next = head[u];
	edges[cnt].num = num;
	head[u] = cnt++;
	++ deg[u];
	if (deg[u] > maxDeg) maxDeg = deg[u];
}