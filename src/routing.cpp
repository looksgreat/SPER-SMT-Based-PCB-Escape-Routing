#include "../include/routing.hpp"
using namespace std;

PCB::PCB():
    solver(new SOLVER){}

void PCB::setGridMap(int r, int c, int l){
    row = r*2-1; col = c*2-1; layer = l*2-1;
    gridMap.resize(row, vector<vector<expr>>(col, vector<expr>(layer)));
    for(int i = 0; i < row; i++){
        for(int j = 0; j < col; j++){
            for(int k = 0; k < layer; k++){
                gridMap[i][j][k] = solver->bool_const("grid_"+to_string(i)+"_"+to_string(j)+"_"+to_string(k));
                netMap[i][j][k] = solver->int_const("net_"+to_string(i)+"_"+to_string(j)+"_"+to_string(k));
            }
        }
    }
    gridType.resize(row, vector<vector<int>>(col, vector<int>(layer, 0)));
}