#pragma once
#include "solver.hpp"
using namespace std;

class PCB{
public:
    PCB();
    void setGridMap(int r, int c, int l, vector<pair<int, int>> &pin, vector<pair<pair<int, int>, int>> &fanout, int mode);
    void setPinConstraint(int x, int y);
    void setEdgeConstraint(int x, int y, int z);
    void setGridConstraint(int x, int y, int z);
    void setOutConstraint(int x, int y, int net = -999);
    void setLength();
    void setConstraint();

private:
    SOLVER *solver;
    int row, col, layer, pinNum;
    vector<vector<vector<expr>>> gridMap, netMap;
    vector<vector<int>> outGrid, pinGrid;
    expr totalLen, maxNetLen, minNetLen;
    func_decl bool2int;
    int mode;

};

