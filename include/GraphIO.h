#ifndef GRAPHIO_H
#define GRAPHIO_H

#include "Graph.h"
#include "Partition.h"
#include "utils.h"

class GraphIO {
public:
	GraphIO();
	void input(Graph *&, std::string, bool edgeTag = false);
	void output(Partition *, std::string, std::string msg = "", std::string costFile = "");
};

#endif
