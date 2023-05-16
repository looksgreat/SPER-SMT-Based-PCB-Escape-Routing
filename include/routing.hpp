#pragma once
#include "solver.hpp"
using namespace std;

class PCB{
public:
    PCB(){}
    void setGridMap(int r, int c, int l);
    void setPinConstraint(int x, int y, int z);
    void setEdgeConstraint(int x, int y, int z);
    void setGridConstraint(int x, int y, int z);
    void setOutConstraint(int x, int y, int z);
    void setConstraint();

private:
    SOLVER *solver;
    int row, col, layer;
    vector<vector<vector<expr>>> gridMap, netMap;
    vector<vector<vector<int>>> gridType;

};

class UI{
public:
    UI(){}
    void createBoard(int x, int y, int z);
    void addPin(int x, int y);
    void addFanout(int x, int y, int signal = -999);
    void setMode(int mode);
private:
    PCB *pcb;
    int mode;
}