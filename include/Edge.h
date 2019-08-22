#ifndef EDGE_H
#define EDGE_H

class Edge {
public:
	int to;
	int next;
	bool del;
	int num;
	Edge() {
		to = next = del = 0;
	}
};

#endif