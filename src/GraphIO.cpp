#include "GraphIO.h"

GraphIO::GraphIO() {

}

void GraphIO::input(Graph *&G, std::string inputFile, bool edgeTag) {
	edgeTag = true;
	std::ifstream fin(inputFile.c_str());

	int N, M, u, v;

	fin >> N >> M;

	G = new Graph(N, M);

	if (edgeTag) {
		for (int i = 1; i <= M; ++ i) {
			fin >> u >> v;
			G -> addEdge(u, v, i);
			G -> addEdge(v, u, i);
		}
	}
	else {
		for (int i = 1; i <= M; ++ i) {
			fin >> u >> v;
			G -> addEdge(u, v);
			G -> addEdge(v, u);
		}
	}

	//printf("%d\n", G -> N);

	fin.close();
}

void GraphIO::output(Partition *Pa, std::string outputFile, std::string msg, std::string costFile) {
	std::cout << "----" << msg << "----" << std::endl;
	std::cout << "Part Num: " << Pa -> PartNum << std::endl;
	double cost = Pa -> cntCost();
	std::cout << "Cost: " << cost << std::endl;

	std::ofstream costOut(costFile.c_str());
	if (Pa -> check()) {
		std::cout << "Correct Partition" << std::endl;
		costOut << cost << std::endl;
	}
	else {
		std::cout << "Wrong Partition" << std::endl;
		costOut << -1 << std::endl;
	}

	for (int i = 1; i <= Pa -> PartNum; ++ i) {
		std::cout << "Part " << i << ": " << Pa -> size(i);
		if (i % 4 == 0 || i == Pa -> PartNum) std::cout << std::endl;
		else std::cout << "\t";
	}

	std::ofstream fout(outputFile.c_str());

	fout << Pa -> PartNum << std::endl;
	for (int i = 1; i <= Pa -> PartNum; ++ i) {
		fout << Pa -> size(i) << std::endl;
		for (auto p : Pa -> EP[i]) 
			fout << p.first << " " << p.second << std::endl;
	}

	fout.close();
	costOut.close();
}
