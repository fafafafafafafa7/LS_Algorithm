#include "MoveBlockF.h"

MoveBlockF::MoveBlockF(Graph *g, Partition *pa, int lowSize, int upSize) : BLS(g, pa, lowSize, upSize) {

    blockCount = new int[g->N + 1];

    verToBlockAdd = new Block **[pa->PartNum + 1];
    for (int i = 0; i <= Pa->PartNum; ++i)
        verToBlockAdd[i] = new Block *[g->N + 1];

    vis = new bool[g->N + 1];

    splitVis = new int[g->N + 1];

    verColor = new bool[g->N + 1];

    tmpSize = new int[pa->PartNum + 1];

    tmpAdjEdge = new std::set<std::pair<int, int>>[pa->PartNum + 1];

    adjEdgeTable = new std::pair<int, int>[g->M + 1];

    flow = new Flow(Pa->PartNum + G->M + 10);
}

void MoveBlockF::localSearch() {

    //tmpCost = nowCost;

    std::ifstream preCost("temp/afterCost.txt");
    double pc;
    preCost >> pc;
    preCost.close();

    blockTot("temp/blockBefore.txt");
    moveTot("temp/totBefore.txt");

    for (int i = 1; i <= Pa->PartNum; ++i)
        for (int v = 1; v <= G->N; ++v)
            verToBlockAdd[i][v] = NULL;

    setBlockBar();
    /*
	std::cout << "finish set Block Bar" << std::endl;

	for (int s = 0; s <= G -> N; ++ s)
		if (BlockBar[1][s].size())
			std::cout << s << ": " << BlockBar[1][s].size() << "\t";
	std::cout << std::endl;
	*/
    setVerColor();

    clock_t startTime = clock();
    clock_t endTime = clock();

    int cnt = 0;

    while (true) {
        getIndeBlo();
        // for (auto block : IndepenBlocks) {
        //     printf("(%d %d) ", block->part, block->seed);
        // }
        // printf("\n");
        ++cnt;
        // if (IndepenBlocks.size() == 1 && IndepenBlocks[0] -> ver[0] == 9119) {
        // 	int &i = IndepenBlocks[0] -> part;
        // 	int v = 9119;
        // 	std::cout << cap(IndepenBlocks[0], 16) << std::endl;
        // }

        // for (auto block : IndepenBlocks)
        // 	std::cout << block << "\t";
        // std::cout << std::endl;
        // std::cout << "------" << std::endl;
        if (stopCheck(startTime))
            break;
        // if (cnt > 10) break;
        // for (auto block : IndepenBlocks)
        // 	block -> output();

        // if (IndepenBlocks.size() == 0) break;
        if (!planMove()) {
            punish();
            continue;
        }
        if (!flowMove()) {
            punish();
            continue;
        }
        applyMove();
        if (nowCost <= pc * G->N)
            endTime = clock();
        //std::cout << nowCost << std::endl;
        //std::cout << IndepenBlocks.size() << std::endl;

        // std::cout << "nowCost: " << nowCost << std::endl;
        //std::cout << "tmpCost: " << tmpCost << std::endl;
        cnt = 0;
    }

    std::ofstream timeOut("temp/Time.txt");
    double Time = endTime - startTime;
    Time = Time / CLOCKS_PER_SEC;

    timeOut << Time << std::endl;

    timeOut.close();

    moveTot("temp/totAfter.txt");
    blockTot("temp/blockAfter.txt");

    restore();
}

MoveBlockF::Block::Block(const int &i) {
    edgeSize = 0;
    verSize = 0;
    part = i;
    deg = 0;
    seed = 0;
}

MoveBlockF::BlockNode::BlockNode(Block *blo) {
    block = blo;
}

MoveBlockF::BlockNode::BlockNode() {
}

bool MoveBlockF::BlockNode::operator<(const BlockNode &A) const {
    if (block->deg == A.block->deg) {
        if (block->seed == A.block->seed) return block < A.block;
        return block->seed < A.block->seed;
    }
    return block->deg < A.block->deg;
}

void MoveBlockF::blockTot(const char *msg) {

    std::ofstream blockOut(msg);

    BlockBar.clear();
    setBlockBar();
    for (int s = 0; s <= G->N; ++s)
        blockCount[s] = 0;
    for (auto blo : BlockBar) {
        ++blockCount[blo.block->ver.size()];
    }

    blockOut << "{";

    for (int s = 0; s <= G->N; ++s)
        if (blockCount[s]) {
            blockOut << s << ": " << blockCount[s] << ",";
        }

    blockOut << "}" << std::endl;

    blockOut.close();

    BlockBar.clear();
}

void MoveBlockF::moveTot(const char *msg) {
    std::ofstream moveOut(msg);

    int cnt = 0;
    for (int i = 1; i <= Pa->PartNum; ++i)
        for (auto e : EPS[i])
            cnt += adjustable(e.first, e.second);

    moveOut << cnt << std::endl;
    moveOut.close();
}

void MoveBlockF::setDeg(Block *block) {
    for (auto v : block->ver)
        block->deg += G->deg[v];
}

void MoveBlockF::setVerSize(Block *block) {
    block->verSize = block->ver.size();
}

bool MoveBlockF::stopCheck(clock_t startTime) {
    double Time = clock() - startTime;
    Time = Time / CLOCKS_PER_SEC;
    return Time > timeLim;
    // for (auto block : IndepenBlocks)
    // 	if (block -> ver.size() <= 10) return false;
    // return true;
}

void MoveBlockF::setBlockBar() {
    shufflePartOrder();
    for (int j = 1; j <= Pa->PartNum; ++j) {
        int i = partOrder[j];
        // clear vis
        for (auto v : Ver[i])
            vis[v] = false;

        // bfs start
        for (auto v : Ver[i]) {
            if (vis[v])
                continue;
            auto newBlockAdd = bfs(i, v);
            setBlock(newBlockAdd);
        }
    }
}

MoveBlockF::Block *MoveBlockF::bfs(const int &i, const int &v) {
    // init a new block
    Block *newBlock = new Block(i);

    static std::queue<int> que;
    // clear que
    while (que.size())
        que.pop();

    que.push(v);
    vis[v] = 1;

    while (que.size()) {
        int u = que.front();
        que.pop();

        newBlock->ver.push_back(u);

        for (auto w : NB[i][u]) {
            if (adjustable(u, w))
                continue;
            ++newBlock->edgeSize;
            if (vis[w])
                continue;

            vis[w] = 1;
            que.push(w);
        }
    }

    newBlock->edgeSize >>= 1;
    return newBlock;
}

void MoveBlockF::getIndeBlo() {

    //std::cout << "start get inde block" << std::endl;

    while (true) {
        IndepenBlocks.clear();
        coloredVer.clear();

        shufflePartOrder();

        int i = 0;
        static std::set<int> tmpPartSet;
        tmpPartSet.clear();
        for (auto bloNode : BlockBar) {
            ++i;
            if (i == 60)
                break;

            Block *indeBlo = bloNode.block;
            if (tmpPartSet.find(indeBlo->part) != tmpPartSet.end())
                continue;
            tmpPartSet.insert(indeBlo->part);
            // indeBlo = greedyPickBlock(BlockBar[i]);

            //indeBlo -> output();

            if (maintainBlock(indeBlo)) {
                // std::cout << "haha" << std::endl;
                break;
            }
            if (checkColor(indeBlo)) {
                //-- i;
                continue;
            }
            color(indeBlo);
            IndepenBlocks.push_back(indeBlo);
        }
        if (IndepenBlocks.size() == 0)
            continue;
        // IndepenBlocks[0] -> output();
        resetColor();
        break;
    }
    //std::cout << BlockBar[1][1].size() << std::endl;
    //std::cout << IndepenBlocks.size() << std::endl;
    setPunishBlocks();
    //std::cout << "end get inde block" << std::endl;
}

void MoveBlockF::punish() {
    for (auto block : punishBlocks) {
        Block *newBlock = new Block(block->part);
        // newBlock -> part = block -> part;
        newBlock->deg = block->deg + 10;
        newBlock->verSize = block->verSize;
        newBlock->edgeSize = block->edgeSize;

        for (auto v : block->ver)
            newBlock->ver.push_back(v);

        resetBlock(block);
        setBlock(newBlock);
    }
}

void MoveBlockF::setPunishBlocks() {
    punishBlocks.clear();
    for (auto block : IndepenBlocks)
        punishBlocks.push_back(block);
}

// MoveBlockF::Block * MoveBlockF::randPickBlock(std::set<BlockNode> &blockBar) {
// 	auto it = blockBar.begin();
// 	auto startBlock = *it;
// 	it = blockBar.end();
// 	-- it;
// 	auto endBlock = *it;

// 	long long range = endBlock - startBlock + 1;
// 	long long bias = rand() % range;

// 	it = blockBar.lower_bound(startBlock + bias);
// 	return *it;
// }

MoveBlockF::Block *MoveBlockF::greedyPickBlock(std::set<BlockNode> &blockBar) {
    return blockBar.begin()->block;
}

bool MoveBlockF::maintainBlock(Block *block) {
    if (split(block))
        return true;
    if (merge(block))
        return true;
    return false;
}

bool MoveBlockF::split(Block *block) {
    if ((block->ver).size() == 1)
        return false;
    ++splitTime;

    for (auto v : block->ver)
        splitVis[v] = splitTime;

    static std::vector<Block *> splitedBlock;
    splitedBlock.clear();

    for (auto v : block->ver) {
        int i = block->part;
        if (splitVis[v] == splitTime) {
            Block *newBlock = splitBfs(i, v);
            splitedBlock.push_back(newBlock);
        }
    }

    //std::cout << "splitedBlock size: " << splitedBlock.size() << std::endl;

    if (splitedBlock.size() == 1) {
        block->edgeSize = splitedBlock[0]->edgeSize;
        delete splitedBlock[0];
        return false;
    }

    resetBlock(block);

    for (auto sBlock : splitedBlock)
        setBlock(sBlock);

    return true;
}

bool MoveBlockF::merge(Block *block) {
    int &i = block->part;
    for (auto v : block->ver) {
        for (auto u : NB[i][v]) {
            if (verToBlockAdd[i][u] != block && !adjustable(u, v)) {
                Block *newBlock = new Block(i);
                newBlock->ver.clear();
                newBlock->ver.assign(block->ver.begin(), block->ver.end());

                Block *uBlock = verToBlockAdd[i][u];
                if (uBlock != NULL) {
                    for (auto w : uBlock->ver)
                        newBlock->ver.push_back(w);
                    resetBlock(uBlock);
                } else
                    newBlock->ver.push_back(u);

                resetBlock(block);
                setBlock(newBlock);
                return true;
            }
        }
    }
    return false;
}

MoveBlockF::Block *MoveBlockF::splitBfs(const int &i, const int &v) {
    static std::queue<int> que;
    while (que.size())
        que.pop();

    Block *newBlock = new Block(i);

    que.push(v);
    splitVis[v] = -splitTime;

    while (que.size()) {
        int u = que.front();
        que.pop();

        newBlock->ver.push_back(v);

        for (auto w : NB[i][u]) {
            if (adjustable(u, w))
                continue;
            ++newBlock->edgeSize;
            if (splitVis[w] != splitTime)
                continue;

            splitVis[w] = -splitTime;
            que.push(w);
        }
    }

    newBlock->edgeSize >>= 1;
    return newBlock;
}

void MoveBlockF::setSplitVis() {
    splitTime = 0;
    for (int v = 1; v <= G->N; ++v)
        splitVis[v] = 0;
}

void MoveBlockF::setVerColor() {
    for (int i = 1; i <= G->N; ++i)
        verColor[i] = false;
    coloredVer.clear();
}

bool MoveBlockF::checkColor(Block *block) {
    for (auto v : block->ver)
        if (verColor[v])
            return true;
    return false;
}

void MoveBlockF::color(Block *block) {
    for (auto v : block->ver) {
        // color v
        verColor[v] = true;
        coloredVer.push_back(v);

        // color nei of v
        for (int i = G->head[v]; ~i; i = G->edges[i].next) {
            int &u = G->edges[i].to;
            coloredVer.push_back(u);
            verColor[u] = true;
        }
    }
}

void MoveBlockF::resetColor() {
    for (auto v : coloredVer)
        verColor[v] = false;
}

bool MoveBlockF::planMove() {
    movePlanBlock.clear();
    movePlanEdge.clear();
    setTmpSize();

    std::random_shuffle(IndepenBlocks.begin(), IndepenBlocks.end());

    static std::vector<Block *> tmpIndepenBlocks;
    tmpIndepenBlocks.clear();

    for (auto block : IndepenBlocks) {

        int i = -1;

        if (block->verSize == 0)
            continue;

        if (block->verSize == 1) {
            int &v = block->ver[0];
			i = partCover[v]._Find_first();
			if (i == partCover[v].size())
				i = -1;
            // printf("block->edgeSize = %d\n", block->edgeSize);
        }
		else i = mostSuit(block);
        if (i == -1)
            continue;

        tmpIndepenBlocks.push_back(block);

        tmpSize[block->part] -= block->edgeSize;
        tmpSize[i] += block->edgeSize;

        movePlanBlock[block] = i;
    }

    if (tmpIndepenBlocks.size() == 0)
        return false;

    IndepenBlocks.clear();
    IndepenBlocks.assign(tmpIndepenBlocks.begin(), tmpIndepenBlocks.end());

    for (int i = 1; i <= Pa->PartNum; ++i)
        tmpSize[i] = std::max(tmpSize[i], (int)EPS[i].size());

    return true;
}

void MoveBlockF::setTmpSize() {
    for (int i = 1; i <= Pa->PartNum; ++i)
        tmpSize[i] = EPS[i].size();
}

int MoveBlockF::mostSuit(Block *block) {
    shufflePartOrder();
    int minPos = -1, minSize = G->M + 1;

    for (int i = 1; i <= Pa->PartNum; ++i) {
        int j = partOrder[i];
        if (j == block->part)
            continue;
        if (tmpSize[j] + block->edgeSize <= UpSize && cap(block, j) && tmpSize[j] < minSize)
            minPos = j, minSize = tmpSize[j];
    }
    return minPos;
}

int MoveBlockF::cap(Block *block, const int &i) {
    int cnt = 0;
    for (auto v : block->ver)
        if (Ver[i].find(v) != Ver[i].end())
            ++cnt; // can be polished
    return cnt;
}

bool MoveBlockF::flowMove() {
    int sum = setTmpAdjEdge();
    adjEdgeCount = 0;

    flow->init(sum + Pa->PartNum + 2);

    // flow to T
    shufflePartOrder();
    for (int j = 1; j <= Pa->PartNum; ++j) {
        int i = partOrder[j];
        if (UpSize >= tmpSize[i])
            flow->addArc(i, flow->T, UpSize - tmpSize[i]);
    }

    // flow from S and inner

    static std::bitset<BITSET_SIZE> tmp;

    for (auto block : IndepenBlocks) {
        int &i = block->part;

        // printf("%d: ", i);

        for (auto p : tmpAdjEdge[i]) {
            adjEdgeTable[++adjEdgeCount] = p;

            // if (p.first == 5 && p.second == 417) {
            // 	std::cout << (NB[i][5].find(417) != NB[i][5].end()) << std::endl;
            // }

            // flow from S
            flow->addArc(flow->S, adjEdgeCount + Pa->PartNum, 1);

            // flow inner
            tmp = partCover[p.first] & partCover[p.second];
            shufflePartOrder();
            for (int k = 1; k <= Pa->PartNum; ++k) {
                int j = partOrder[k];
                if (tmp[j] == 0 || i == j)
                    continue;
                // printf("%d ", j);
                flow->addArc(adjEdgeCount + Pa->PartNum, j, 1);
            }
        }
        // printf("\n");
    }

    // std::cout << flow -> S << " " << flow -> T << std::endl;

    int f = flow->maxFlow();
    flow->refine();

    int tmpCount = 0;

    static std::vector<Block *> tmpIndepenBlocks;
    tmpIndepenBlocks.clear();

    for (auto block : IndepenBlocks) {
        int &i = block->part;
        bool flag = true;

        int preCount = tmpCount;

        for (auto p : tmpAdjEdge[i]) {
            ++tmpCount;
            if (flow->suf[tmpCount + Pa->PartNum] == 0)
                flag = false;
        }

        // if exist an edge with no suf
        if (!flag)
            continue;

        for (auto p : tmpAdjEdge[i]) {
            ++preCount;
            movePlanEdge[p] = std::make_pair(i, flow->suf[preCount + Pa->PartNum]);
        }

        tmpIndepenBlocks.push_back(block);
    }

    IndepenBlocks.clear();
    IndepenBlocks.assign(tmpIndepenBlocks.begin(), tmpIndepenBlocks.end());

    if (IndepenBlocks.size() == 0)
        return false;

    return true;
}

int MoveBlockF::setTmpAdjEdge() {
    int sum = 0;
    for (auto block : IndepenBlocks) {
        int &i = block->part;
        tmpAdjEdge[i].clear();

        for (auto v : block->ver) {
            for (auto u : NB[i][v]) {
                if (!adjustable(v, u))
                    continue;
                int mi = std::min(v, u);
                int ma = std::max(v, u);
                tmpAdjEdge[i].insert(std::make_pair(mi, ma));
            }
        }

        sum += tmpAdjEdge[i].size();
    }
    return sum;
}

void MoveBlockF::applyMove() {
    // set move plan for !adj edges

    for (auto block : IndepenBlocks) {
        int i = block->part;

        //tmpCost -= cap(block, movePlanBlock[block]);

        for (auto v : block->ver) {
            for (auto u : NB[i][v]) {
                if (adjustable(v, u))
                    continue;
                int mi = std::min(v, u);
                int ma = std::max(v, u);
                // if (movePlanBlock[block] > 64)
                // 	std::cout << "fuck" << std::endl;
                movePlanEdge[std::make_pair(mi, ma)] = std::make_pair(i, movePlanBlock[block]);
            }
        }

        resetBlock(block);
    }

    for (auto p : movePlanEdge) {
        int u = p.first.first, v = p.first.second;
        int i = p.second.first, j = p.second.second;

        newBlock(j, u);
        newBlock(j, v);

        moveEdge(u, v, i, j);

        // if (u == 5 && v == 417) {
        // 	std::cout << (NB[i][5].find(417) != NB[i][5].end()) << std::endl;
        // }
    }

    //std::cout << "Finish deal with a set of blocks" << std::endl;
}

void MoveBlockF::resetBlock(Block *block) {
    int i = block->part;
    for (auto v : block->ver)
        verToBlockAdd[i][v] = NULL;
    BlockBar.erase(BlockNode(block));

    delete block;
}

void MoveBlockF::newBlock(const int &i, const int &v) {
    if (verToBlockAdd[i][v] != NULL)
        return;

    Block *block = new Block(i);
    block->ver.push_back(v);
    setBlock(block);
}

void MoveBlockF::setBlock(Block *block) {
    int &i = block->part;
    for (auto v : block->ver)
        verToBlockAdd[i][v] = block;
    if (!block->deg)
        setDeg(block);
    if (!block->verSize)
        setVerSize(block);
    if (!block->seed)
        block->seed = rand();
    BlockBar.insert(block);
}

MoveBlockF::~MoveBlockF() {
    for (int i = 0; i <= Pa->PartNum; ++i)
        delete[] verToBlockAdd[i];
    delete[] verToBlockAdd;

    delete[] vis;

    delete[] verColor;

    delete[] tmpSize;

    delete[] tmpAdjEdge;

    delete[] adjEdgeTable;

    delete flow;
}
// EVU95GMSRF
