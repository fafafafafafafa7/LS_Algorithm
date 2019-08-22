#include "MoveBlockG.h"

MoveBlockG::MoveBlockG(Graph *g, Partition *pa, int lowSize, int upSize) : BLS(g, pa, lowSize, upSize) {
	blockCount = 0;
	blocks = new std::vector<int> [g -> N + 1];
	vis = new bool [g -> N + 1];
	blockInd = new int [g -> N + 1];
	blockEdgeSize = new int [g -> N + 1];
	partInd = new int [g -> N + 1];
	blockSizeCount = new int [g -> N + 1];
}

void MoveBlockG::localSearch() {

	blockTot();

	clock_t startTime = clock();

	std::set<int> partTmp;

	bool update = true;
	while (update) {
		update = false;

		if (stopCheck(startTime)) break;

		setPartInd();
		for (int i = 1; i <= Pa -> PartNum; ++ i) {
			//if (partTmp.find(partInd[i]) != partTmp.end()) continue;
			//partTmp.insert(partInd[i]);

			blockSearch(partInd[i]);
			// std::cout << "Part: " << partInd[i] << std::endl;
			setBlockInd();

			for (int b = 1; b <= blockCount; ++ b) {
				bool flag = move(blockInd[b], partInd[i]);
				if (flag) update = true;
				if (blocks[blockInd[b]].size() >= 20) break;
			}
			if (update) break;
		}
	}

	std::ofstream timeOut("temp/Time.txt");
	double Time = clock() - startTime;
	Time = Time / CLOCKS_PER_SEC;

	timeOut << Time << std::endl;

	timeOut.close();

	blockTot();

	restore();
}

bool MoveBlockG::stopCheck(clock_t startTime) {
	double Time = clock() - startTime;
	Time = Time / CLOCKS_PER_SEC;
	return Time > timeLim;
	// for (auto block : IndepenBlocks)
	// 	if (block -> ver.size() <= 10) return false;
	// return true;
}

int MoveBlockG::findMaxPart() {
	int maxPos = 0;
	int maxSize = 0;

	for (int i = 1; i <= Pa -> PartNum; ++ i) {
		if (maxSize < (int) (EPS[i].size())) 
			maxSize = (int) (EPS[i].size()), maxPos = i;
	}

	return maxPos;
}

void MoveBlockG::blockSearch(int &i) {
	blockCount = 0;
	for(auto v : Ver[i]) 
		vis[v] = 0;

	for (auto v : Ver[i]) 
		if (vis[v] == 0) {
			++ blockCount;
			blocks[blockCount].clear();
			blockEdgeSize[blockCount] = 0;
			bfs(i, v);
		}
}

void MoveBlockG::bfs(int &i, int &v) {
	static std::queue<int> que;
	while (que.size()) que.pop();

	vis[v] = 1;
	que.push(v);

	while (que.size()) {
		int u = que.front();
		que.pop();
		blocks[blockCount].push_back(u);

		for (auto w : NB[i][u]) {
			if (adjustable(u, w)) continue;

			++ blockEdgeSize[blockCount];
			if (vis[w]) continue;

			vis[w] = 1;
			que.push(w);
		}
	}
	blockEdgeSize[blockCount] >>= 1;
}

void MoveBlockG::setBlockInd() {
	for (int b = 1; b <= blockCount; ++ b)
		blockInd[b] = b;
	std::sort(blockInd + 1, blockInd + blockCount + 1, [this](int l, int r) {return cmp(l, r);});
}

bool MoveBlockG::cmp(int a, int b) {
	return blocks[a].size() < blocks[b].size();
}

bool MoveBlockG::move(int &b, int &i) {
	std::vector<int> &block = blocks[b];

	int j = mostSuit(b, i);
	if (j == -1) return false;

	static std::map<std::pair<int, int>, int> movePlan;
	int *tmpSize = new int [Pa -> PartNum + 1];

	movePlan.clear();

	for (int k = 1; k <= Pa -> PartNum; ++ k) {
		if (k == i) tmpSize[k] = (int) EPS[k].size() - blockEdgeSize[b];
		else if (k == j) tmpSize[k] = (int) EPS[k].size() + blockEdgeSize[b];
		else tmpSize[k] = (int) EPS[k].size();
	}

	if (tmpSize[i] < LowSize) {
		delete [] tmpSize;
		return false;
	}

	if (tmpSize[j] > UpSize) {
		delete [] tmpSize;
		return false;
	}

	int mi, ma;

	for (auto v : block) 
		for (auto u : NB[i][v]) {
			mi = std::min(v, u);
			ma = std::max(v, u);

			if (!adjustable(v, u)) {
				movePlan[std::make_pair(mi, ma)] = j;
				continue;
			}

			if (movePlan[std::make_pair(mi, ma)] != 0) continue;

			int k = edgeMostSuit(tmpSize, u, v, i);
			// if (k > 16 || k < 1) std::cout << "fuck" << std::endl;
			if (tmpSize[k] + 1 > UpSize) {
				delete [] tmpSize;
				return false;
			}

			movePlan[std::make_pair(mi, ma)] = k;

			tmpSize[k] += 1;
			tmpSize[i] -= 1;
		}
	for (auto p : movePlan) {
		int u = p.first.first;
		int v = p.first.second;
		int t = p.second;
		//std::cout << "fuck: " << u << " " << v << " " << t << std::endl;
		//int c;
		//std::cin >> c;
		moveEdge(u, v, i, t);
	}

	delete [] tmpSize;
	return true;
}

int MoveBlockG::mostSuit(int &b, int &i) {
	std::vector<int> &block = blocks[b];

	int minSize = G -> N + 1;
	int minPos = -1;

	for (int j = 1; j <= Pa -> PartNum; ++ j) {
		if (j == i) continue;
		if ((int) (EPS[j].size()) + blockEdgeSize[b] <= UpSize && minSize > (int) (EPS[j].size()) && cap(block, j))
			minSize = (int) (EPS[j].size()), minPos = j;
	}
	return minPos;
}

int MoveBlockG::cap(std::vector<int> &block, int &i) {
	int count = 0;
	for (auto v : block) 
		if (Ver[i].find(v) != Ver[i].end()) ++ count;

	return count;
}

int MoveBlockG::edgeMostSuit(int *tmpSize, int &u, int &v, int &i) {
	static std::bitset<BITSET_SIZE> tmp;
	tmp = partCover[u] & partCover[v];

	int minSize = G -> M + 1;
	int minPos;

	for (int j = 1; j <= Pa -> PartNum; ++ j) {
		if (i == j || tmp[j] == 0) continue;
		if (minSize > tmpSize[j])
			minSize = tmpSize[j], minPos = j;
	}
	return minPos;
}

bool MoveBlockG::cmpPart(int i, int j) {
	return EPS[i].size() > EPS[j].size();
}

void MoveBlockG::setPartInd() {
	for (int i = 1; i <= Pa -> PartNum; ++ i)
		partInd[i] = i;
	std::sort(partInd + 1, partInd + 1 + Pa -> PartNum, [this](int l, int r){return cmpPart(l, r);});
}

void MoveBlockG::blockTot() {
	for (int s = 1; s <= G -> N; ++ s)
		blockSizeCount[s] = 0;
	int blockTotCount = 0;
	for (int i = 1; i <= Pa -> PartNum; ++ i) {
		//std::cout << "haha" << std::endl;
		blockCount = 0;
		for(auto v : Ver[i]) 
			vis[v] = 0;

		for (auto v : Ver[i]) 
			if (vis[v] == 0) {
				++ blockCount;
				//std::cout << blockCount << std::endl;
				blocks[blockCount].clear();
				blockEdgeSize[blockCount] = 0;
				bfs(i, v);
			}
		blockTotCount += blockCount;
		for (int b = 1; b <= blockCount; ++ b)
			++ blockSizeCount[blocks[b].size()];
	}

	std::cout << blockTotCount << std::endl;
	for (int s = 1; s <= G -> N; ++ s)
		if (blockSizeCount[s])
			std::cout << s << ": " << blockSizeCount[s] << "\t";
	std::cout << std::endl;
}

MoveBlockG::~MoveBlockG() {
	delete [] blocks;
	delete [] vis;
	delete [] blockInd;
	delete [] blockEdgeSize;
	delete [] partInd;
	delete [] blockSizeCount;
}
