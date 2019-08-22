#include "Partition.h"

Partition::Partition(Graph *g, int partNum) {
	G = g;
	PartNum = partNum;

	EP = new std::vector<std::pair<int, int> > [partNum + 10];
}

void Partition::addEdge(int k, int u, int v) {
	//printf("%d\n", k);
	EP[k].push_back(std::make_pair(u, v));
}

void Partition::addEdge(int k, std::pair<int, int> p) {
	//printf("%d\n", k);
	EP[k].push_back(p);
}

bool Partition::check() {
	GraphEdges.clear();
	PartitionEdges.clear();

	int v, u, i;
	for (v = 1; v <= G -> N; ++ v) {
		for (i = G -> head[v]; ~i; i = G -> edges[i].next) {
			u = G -> edges[i].to;
			if (v < u) GraphEdges.push_back(std::make_pair(v, u));
		}
	}

	for (i = 1; i <= PartNum; ++ i) 
		for (auto p : EP[i]) {
			u = p.first;
			v = p.second;
			if (v > u) std::swap(v, u);
			PartitionEdges.push_back(std::make_pair(v, u));
		}

	std::sort(GraphEdges.begin(), GraphEdges.end());
	std::sort(PartitionEdges.begin(), PartitionEdges.end());

	bool flag = true;

	for (int i = 0; i < (int)GraphEdges.size(); ++ i) {
		if (GraphEdges[i] != PartitionEdges[i]) {
			std::cout << "wrong on i=" << i << std::endl;
			std::cout << "GraphEdges[i]=" << GraphEdges[i].first << "," << GraphEdges[i].second << std::endl;
			std::cout << "PartitionEdges[i]=" << PartitionEdges[i].first << "," << PartitionEdges[i].second << std::endl;
			flag = false;
			break;
		}
	}

	return flag;	
}

int Partition::size(int k) {
	return EP[k].size();
}

int Partition::cntVer() {
	int cost = 0;
	for (int i = 1; i <= PartNum; ++ i) {
		vs.clear();
		for (auto p : EP[i]) {
			vs.insert(p.first);
			vs.insert(p.second);
		}
		cost += (int)vs.size();
	}
	return cost;
}

double Partition::cntCost() {
	return (double)cntVer() / (G -> N);
}

Partition::~Partition() {
	delete [] EP;
}