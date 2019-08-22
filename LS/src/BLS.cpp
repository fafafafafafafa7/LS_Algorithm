#include "BLS.h"

BLS::BLS(Graph *g, Partition *pa, int lowSize, int upSize) {
    G = g;
    Pa = pa;
    UpSize = upSize;
    LowSize = lowSize;

    //printf("%d %d\n", lowSize, upSize);

    EPS = new std::set<std::pair<int, int>>[pa->PartNum + 1];

    NB = new std::set<int> *[pa->PartNum + 1];
    for (int i = 0; i <= pa->PartNum; ++i)
        NB[i] = new std::set<int>[g->N + 1];

    degree = new int *[pa->PartNum + 1];
    for (int i = 0; i <= pa->PartNum; ++i)
        degree[i] = new int[g->N + 1];

    partCover = new std::bitset<BITSET_SIZE>[g->N + 1];

    Ver = new std::set<int>[pa->PartNum + 1];

    partOrder = new int[pa->PartNum + 1];

    setEPS();
    setNB();
    setVer();
    setDegree();
    setPartCover();
    setPartOrder();

    nowCost = Pa->cntVer();
}

void BLS::setEPS() {
    for (int i = 1; i <= Pa->PartNum; ++i)
        for (auto e : Pa->EP[i]) {
            EPS[i].insert(std::make_pair(std::min(e.first, e.second),
                                         std::max(e.first, e.second)));
        }
}

void BLS::setNB() {
    for (int i = 1; i <= Pa->PartNum; ++i)
        for (auto p : Pa->EP[i]) {
            NB[i][p.first].insert(p.second);
            NB[i][p.second].insert(p.first);
        }
}

void BLS::setDegree() {
    for (int i = 1; i <= Pa->PartNum; ++i)
        for (auto v : Ver[i])
            degree[i][v] = (int)NB[i][v].size();
}

void BLS::setPartCover() {
    for (int i = 1; i <= Pa->PartNum; ++i)
        for (auto p : Pa->EP[i]) {
            partCover[p.first].set(i);
            partCover[p.second].set(i);
        }
}

void BLS::setVer() {
    for (int i = 1; i <= Pa->PartNum; ++i)
        for (auto p : Pa->EP[i]) {
            Ver[i].insert(p.first);
            Ver[i].insert(p.second);
        }
}

void BLS::setPartOrder() {
    for (int i = 1; i <= Pa->PartNum; ++i)
        partOrder[i] = i;
}

void BLS::shufflePartOrder() {
    std::random_shuffle(partOrder + 1, partOrder + 1 + Pa->PartNum);
}

void BLS::restore() {
    for (int i = 1; i <= Pa->PartNum; ++i) {
        Pa->EP[i].clear();
        for (auto e : EPS[i])
            Pa->EP[i].push_back(e);
    }
}

void BLS::moveEdge(int &u, int &v, int &i, int &j) {
    int mi = std::min(u, v);
    int ma = std::max(u, v);

    /*-----------deal with i-----------*/
    NB[i][u].erase(v);
    NB[i][v].erase(u);

    --degree[i][u];
    --degree[i][v];

    if (degree[i][u] == 0) {
        partCover[u].reset(i);
        Ver[i].erase(u);
        --nowCost;
    }
    if (degree[i][v] == 0) {
        partCover[v].reset(i);
        Ver[i].erase(v);
        --nowCost;
    }

    EPS[i].erase(std::make_pair(mi, ma));

    /*-----------deal with j-----------*/
    NB[j][u].insert(v);
    NB[j][v].insert(u);

    ++degree[j][u];
    ++degree[j][v];

    if (degree[j][u] == 1) {
        partCover[u].set(j);
        Ver[j].insert(u);
        ++nowCost;
    }
    if (degree[j][v] == 1) {
        partCover[v].set(j);
        Ver[j].insert(v);
        ++nowCost;
    }

    EPS[j].insert(std::make_pair(mi, ma));
}

bool BLS::adjustable(int &u, int &v) {
    static std::bitset<BITSET_SIZE> tmp;
    tmp = partCover[u] & partCover[v];

    return (tmp.count() > 1);
}

BLS::~BLS() {
    delete[] EPS;
    for (int i = 0; i <= Pa->PartNum; ++i)
        delete[] NB[i];
    delete[] NB;

    for (int i = 0; i <= Pa->PartNum; ++i)
        delete[] degree[i];
    delete[] degree;

    delete[] partCover;

    delete[] Ver;
}