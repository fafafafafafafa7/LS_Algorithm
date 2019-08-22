#include "FL.h"

FL::FL(Graph *g, int partNum, int lowSize, int upSize) : Solver(g, partNum, lowSize, upSize) {
}

void FL::setPartitionFile(std::string partitionFile) {
    PartitionFile = partitionFile;
}

void FL::solve() {
    std::ifstream fin(PartitionFile.c_str());
    for (int i = 1; i <= PartNum; ++i) {
        int num, u, v;
        fin >> num;
        for (int j = 0; j < num; ++j) {
            fin >> u >> v;
            Pa->addEdge(i, u, v);
        }
    }
    fin.close();
}