#ifndef FL_H
#define FL_H

#include "Solver.h"

class FL : public Solver {
public:
    FL(Graph *, int, int, int);
    void setPartitionFile(std::string);
    void solve();
    ~FL();
private:
    std::string PartitionFile;
};

#endif