#ifndef FLOW_H
#define FLOW_H

#include "utils.h"

class Flow {
public:
	int S, T;
	int *suf;

	Flow(const int &);

	void init(const int &);
	void addArc(const int &, const int &, const int &);
	int maxFlow();

	void refine();

	~Flow();

private:
	const int INF = 0x3f3f3f3f;

	struct E {
		int u, v, flow;
		bool dir;
		int rev; 
		E(const int &uu, const int &vv, const int &ff, const bool &dd, const int &rr) : u(uu), v(vv), flow(ff), dir(dd), rev(rr) {}
		E() {}
	};

	std::vector<E> *G;
	int *dis;
	int *current;

	int dfs(int, int);
	bool bfs();
};

#endif