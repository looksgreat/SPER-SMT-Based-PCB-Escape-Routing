#include "../include/UI.hpp"
UI::UI():
    pcb(new PCB()), pinNum(0){}

void UI::createBoard(int x, int y, int z){
    boundX = x; boundY = y; boundZ = z;
}

void UI::addPin(int x, int y){
    if(x >= boundX || y >= boundY) throw runtime_error("Pin out of bound");
    pinNum++;
    pin.push_back(make_pair(x, y));
}

void UI::addFanout(int x, int y, int signal = -999){
    if(x >= boundX || y >= boundY) throw runtime_error("Fanout out of bound");
    if(signal != -999) fanout.push_back(make_pair(make_pair(x, y), signal));
    else fanout.push_back(make_pair(make_pair(x, y), 0));
}

void UI::setMode(int mode){
    this->mode = mode;
}

void UI::getResult(){
    pcb->setGridMap(boundX, boundY, boundZ, pin, fanout, mode);
}