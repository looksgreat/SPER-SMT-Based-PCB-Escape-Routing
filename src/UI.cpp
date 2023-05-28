#include "../include/UI.hpp"
#include <iostream>
#include <pybind11/pybind11.h>
#include <pybind11/stl.h>
using namespace std;

void UI::createBoard(int x, int y, int z){
    pcb = new PCB();
    pinNum = 0;
    boundX = x; boundY = y; boundZ = z;
}

void UI::addPin(int x, int y, int signal){
    if(x >= boundX || y >= boundY) throw runtime_error("Pin out of bound");
    pinNum++;
    pin.push_back(make_pair(make_pair(x, y), signal));
}

void UI::addFanout(int x, int y, int signal){
    if(x >= boundX || y >= boundY) throw runtime_error("Fanout out of bound");
    if(signal != -999) fanout.push_back(make_pair(make_pair(x, y), signal));
    else fanout.push_back(make_pair(make_pair(x, y), -999));
}

void UI::setMode(int mode){
    this->mode = mode;
}

void UI::getResult(){
    pcb->setGridMap(boundX, boundY, boundZ, pin, fanout, mode);
    pcb->setConstraint();
    pcb->getSolution(result);
    if(mode == 3) pcb->BFS(result);
    
}

void UI::outputResult(){
    for(int l = 0; l < boundZ; l++){
        cout << "layer: " << l << endl;
        for(int y = boundY*2-2; y >= 0; y--){
            for(int x = 0; x < boundX*2-1; x++){
                if(x%2 == 0 && y%2 == 0){
                    if(result[x][y][l] == -1) cout << ". ";
                    else if(result[x][y][l] < pin.size()) cout << result[x][y][l] << " ";
                    //else if(result[x][y][l] == 0) cout << "x ";
                    else cout << ". ";
                } 
                else if(x%2 == 0 && y%2 == 1){
                    if(result[x][y][l] == -1) cout << ". ";
                    else if(result[x][y][l] < pin.size()) cout << "| ";
                    else cout << ". ";
                }
                else if(x%2 == 1 && y%2 == 0){
                    if(result[x][y][l] == -1) cout << ". ";
                    else if(result[x][y][l] < pin.size()) cout << "--";
                    else cout << ". ";
                } 
                else cout << "  ";
            }
            cout << endl;
        }
        cout << endl;
    }
}

namespace py = pybind11;

PYBIND11_MODULE(sper, m) {
    py::class_<UI>(m, "UI")
        .def(py::init<>())
        .def("createBoard", &UI::createBoard)
        .def("addPin", &UI::addPin)
        .def("addFanout", &UI::addFanout)
        .def("setMode", &UI::setMode)
        .def("getResult", &UI::getResult)
        .def("outputResult", &UI::outputResult);
}