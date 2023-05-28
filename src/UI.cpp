#include "../include/UI.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
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
    string outputfname = "sper";
    ofstream fout(outputfname + ".gdt");
    time_t rawtime;
	struct tm * timeinfo = nullptr;
	time (&rawtime);
	timeinfo = localtime (&rawtime);
	char timeseq[25];
	// tm_year is the years counted from 1900 A.D.
	sprintf(timeseq, "%d-%02d-%02d %02d:%02d:%02d", timeinfo->tm_year + 1900, timeinfo->tm_mon+1, timeinfo->tm_mday, timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec);
    string last_token = "test";
    fout << "gds2{5\n";
    fout << "m=" << timeseq << " a=" << timeseq << "\n";
    fout << "lib '" << last_token << "' 0.001 1e-09\n";
    fout << "cell{c=" << timeseq << " m=" << timeseq << " '" << last_token << "'\n";
    double unitL = 0.1;

    for(int col = 0; col < boundX*2-1; col++){
        for(int track = 0; track < boundY*2-1; track++){
            if(col%2 == 0 && track%2 == 0){
                fout << "b{" << 255 << " xy(";
                    fout << col << " " << track << " ";
                    fout << col+unitL << " " << track << " ";
                    fout << col+unitL << " " << track+unitL << " ";
                    fout << col << " " << track+unitL;
                    fout << ")}\n";
            }

        }
    }
    for(int col = 0; col < boundX*2-1; col++){
        for(int track = 0; track < boundY*2-1; track++){
            for(int layer = 0; layer < boundZ; layer++){
                if(result[col][track][layer] == -1) continue;
                if(col%2 == 1 && track%2 == 0 && result[col][track][layer] < pin.size()){
                    fout << "b{" << layer << " xy(";
                    fout << col-1 << " " << track << " ";
                    fout << col+1 << " " << track << " ";
                    fout << col+1 << " " << track+unitL << " ";
                    fout << col-1 << " " << track+unitL;
                    fout << ")}\n";
                }
                else if(col%2 == 0 && track%2 == 1 && result[col][track][layer] < pin.size()){
                    fout << "b{" << layer << " xy(";
                    fout << col << " " << track-1 << " ";
                    fout << col+unitL << " " << track-1 << " ";
                    fout << col+unitL << " " << track+1 << " ";
                    fout << col << " " << track+1;
                    fout << ")}\n";
                }
            }
        }
    }
    /*for(auto &it : fanout){
            int x = it.first.first, y = it.first.second, signal = it.second;
            if(x == 0) fout << "t{" << 220 << " tt1 mc m2 xy(" << -0.5 << " " << y << ") '" << signal << "'}\n";
            else if(x == boundX-1) fout << "t{" << 220 << " tt1 mc m2 xy(" << boundX*2-1.5 << " " << y << ") '" << signal << "'}\n";
            else if(y == 0) fout << "t{" << 220 << " tt1 mc m2 xy(" << x << " " << -0.5 << ") '" << signal << "'}\n";
            else if(y == boundY-1) fout << "t{" << 220 << " tt1 mc m2 xy(" << x << " " << boundY*2-1.5 << ") '" << signal << "'}\n";
        }*/

    auto isFanout = [](const vector<pair<pair<int, int>, int>>& v, int x, int y){
        for(auto &it : v){
            if(it.first.first*2 == x && it.first.second*2 == y) return true;
        }
        return false;
    };
    for(int i = 0; i < boundX*2-1; i++){
        for(int layer = 0; layer < boundZ; layer++){
            if(result[i][0][layer] != -1 && result[i][0][layer] < pin.size() && isFanout(fanout, i, 0)){
                fout << "t{" << 220 << " tt1 mc m2 xy(" << i << " " << -0.5 << ") '" << result[i][0][layer] << "'}\n";
            }
            if(result[i][boundY*2-2][layer] != -1 && result[i][boundY*2-2][layer] < pin.size() && isFanout(fanout, i, boundY*2-2)){
                fout << "t{" << 220 << " tt1 mc m2 xy(" << i << " " << boundY*2-1.5 << ") '" << result[i][boundY*2-2][layer] << "'}\n";
            }
        }
    }
    for(int i = 0; i < boundY*2-1; i++){
        for(int layer = 0; layer < boundZ; layer++){
            if(result[0][i][layer] != -1 && result[0][i][layer] < pin.size() && isFanout(fanout, 0, i)){
                fout << "t{" << 220 << " tt1 mc m2 xy(" << -0.5 << " " << i << ") '" << result[0][i][layer] << "'}\n";
            }
            if(result[boundY*2-2][i][layer] != -1 && result[boundY*2-2][i][layer] < pin.size() && isFanout(fanout, boundY*2-2, i)){
                fout << "t{" << 220 << " tt1 mc m2 xy(" << boundX*2-1.5 << " " << i << ") '" << result[boundX*2-2][i][layer] << "'}\n";
            }
        }
    }

    fout << "}\n";
    fout << "}\n";
    fout.close();
    std::string command = "./gdt2gds " + outputfname + ".gdt " + outputfname + ".gds";
    std::cout << command << "\n";
    system(command.c_str());
    
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