#pragma once
#include "routing.hpp"
class UI{
public:
    UI() = delete;
    void createBoard(int x, int y, int z);
    void addPin(int x, int y);
    void addFanout(int x, int y, int signal = -999);
    void setMode(int mode);
    void getResult();
private:
    PCB *pcb;
    int mode, pinNum;
    vector<pair<int, int>> pin;
    vector<pair<pair<int, int>, int>> fanout;
    int boundX, boundY, boundZ;
};