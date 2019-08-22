#include "Solver.h"

Solver::Solver(Graph *g, int partNum, int lowSize, int upSize) {
	G = g;
	PartNum = partNum;

	Pa = new Partition(G, PartNum);

	UpSize = upSize;
	LowSize = lowSize;

	//printf("%d\n", G -> N);
}

void Solver::storePartition(std::string path) {
	std::ofstream ofs(path.c_str());

	for (int i = 1; i <= PartNum; ++ i) {
		ofs << Pa->EP[i].size() << std::endl;
		for (int j = 0; j < Pa->EP[i].size(); ++ j)
			ofs << Pa->EP[i][j].first << " " << Pa->EP[i][j].second << std::endl;
	}

	ofs.close();
}

Solver::~Solver() {
	delete Pa;
}