#include "Flow.h"

Flow::Flow(const int &maxVer) {
	suf = new int [maxVer + 10];
	G = new std::vector<E> [maxVer + 10];
	dis = new int [maxVer + 10];
	current = new int [maxVer + 10];
}

void Flow::init(const int &verSize) {
	//printf("verSize: %d\n", verSize);
	for (int i = 0; i <= verSize + 5; ++ i) {
		suf[i] = 0;
		G[i].clear();
		dis[i] = 0;
		current[i] = 0;
	}

	S = verSize + 1;
	T = verSize + 2;
}

void Flow::addArc(const int &u, const int &v, const int &flow) {
	//if (flow < 0) printf("fuck\n");

	//printf("cntEdge: %d\n", cntEdge);
	G[u].push_back(E(u, v, flow, true, G[v].size()));
	G[v].push_back(E(v, u, 0, false, G[u].size() - 1));
}

bool Flow::bfs() {
    static std::queue<int> q;
    while (q.size()) q.pop();
    q.push(S);
    for (int i = 0; i <= T + 1; ++ i)
    	dis[i] = -1;
    dis[S] = 0;
    while (!q.empty()) {
        int index = q.front();
        q.pop();
        int sz = int(G[index].size());
        for (int i = 0; i < sz; ++ i) {
            E &e = G[index][i];
            if (e.flow > 0) {
                if (dis[e.v] < 0) {
                    dis[e.v] = dis[index] + 1;
                    q.push(e.v);
                }
            }
        }
    }
    return bool(~dis[T]); // 返回是否能够到达汇点
}

int Flow::dfs(int index, int maxflow) {
    if (index == T)
        return maxflow;
    // i = current[index] 当前弧优化
    int sz = int(G[index].size());
    for (int i = current[index]; i < sz; ++ i) {
        E &e = G[index][i];
        current[index] = i;
        if (dis[e.v] == dis[index] + 1 && e.flow > 0) {
            int flow = dfs(e.v, std::min(maxflow, e.flow));
            if (flow != 0) {
                e.flow -= flow; // 正向边流量降低
                G[e.v][G[index][i].rev].flow += flow; // 反向边流量增加
                return flow;
            }
        }
    }
    return 0; // 找不到增广路 退出
}

int Flow::maxFlow() {
	int ans = 0;
    while (bfs()) {// 建立分层图
        int flow;
        for (int i = 0; i <= T + 1; ++ i)
        	current[i] = 0; // BFS后应当清空当前弧数组
        while (bool(flow = dfs(S, INF))) // 一次BFS可以进行多次增广
            ans += flow;
    }
    return ans;
}

void Flow::refine() {
	for (int v = 1; v <= T; ++ v) {
		for (auto &e : G[v]) {
			if (e.dir && e.flow == 0) {
				//printf("%d %d\n", v, e.v);
                // printf("suf[%d]=%d\n", v, e.v);
				suf[v] = e.v;
			}
		}
	}
}

Flow::~Flow() {
	delete [] suf;
	delete [] G;
	delete [] dis;
	delete [] current;
}