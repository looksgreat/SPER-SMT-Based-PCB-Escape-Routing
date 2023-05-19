//#pragma once
#include "solver.hpp"
using namespace std;

class PCB{
public:
    PCB(): solver(new SOLVER), tmplen(solver->getVector()){}
    void setGridMap(int r, int c, int l, vector<pair<pair<int, int>, int>> &pin, vector<pair<pair<int, int>, int>> &fanout, int mode);
    void setPinConstraint(int x, int y, int net);
    void setEdgeConstraint(int x, int y, int z);
    void setGridConstraint(int x, int y, int z);
    void setOutConstraint(int x, int y, int net = -999);
    void setLength();
    void setConstraint();
    void getSolution(vector<vector<vector<int>>> &result);

private:
    SOLVER *solver;
    int row, col, layer, pinNum;
    vector<vector<vector<expr>>> gridMap, netMap;
    vector<vector<int>> outGrid, pinGrid;
    //expr_vector tmplen = solver->getVector();
    expr_vector tmplen;
    expr totalLen = solver->get_expr(), maxNetLen = solver->get_expr(), minNetLen = solver->get_expr();
    int mode;

};

