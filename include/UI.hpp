//#pragma once
#include "routing.hpp"
class UI{
public:
    UI() = default;
    void createBoard(int x, int y, int z);
    void addPin(int x, int y, int signal);
    void addFanout(int x, int y, int signal = -999);
    void setMode(int mode);
    void getResult();
    void outputResult();
private:
    PCB *pcb;
    int mode, pinNum;
    vector<pair<pair<int, int>, int>> pin, fanout;
    int boundX, boundY, boundZ;
    vector<vector<vector<int>>> result;
};