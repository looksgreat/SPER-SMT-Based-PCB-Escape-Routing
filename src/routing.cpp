#include "../include/routing.hpp"
using namespace std;

PCB::PCB():
    solver(new SOLVER), bool2int(z3::function("bool2int", solver->get_bool_sort(), solver->get_int_sort())){
        solver->add(bool2int(solver->bool_val(1)) == 1);
        solver->add(bool2int(solver->bool_val(0)) == 0);
    }

void PCB::setGridMap(int r, int c, int l, vector<pair<int, int>> &pin, vector<pair<pair<int, int>, int>> &fanout, int mode){
    row = r*2-1; col = c*2-1; layer = l;
    this->mode = mode;
    gridMap.resize(col, vector<vector<expr>>(row, vector<expr>(layer)));
    netMap.resize(col, vector<vector<expr>>(row, vector<expr>(layer)));
    for(int i = 0; i < col; i++){
        for(int j = 0; j < row; j++){
            for(int k = 0; k < layer; k++){
                if(!(i % 2 == 1 && j % 2 == 1)) gridMap[i][j][k] = solver->bool_const("grid_"+to_string(i)+"_"+to_string(j)+"_"+to_string(k));
                if(!(i % 2 == 1 && j % 2 == 1)) netMap[i][j][k] = solver->int_const("net_"+to_string(i)+"_"+to_string(j)+"_"+to_string(k));
            }
        }
    }
    pinGrid.resize(col, vector<int>(row, 0));
    outGrid.resize(col, vector<int>(row, 0));

    for(auto &it : pin){
        pinGrid[it.first*2][it.second*2] = 1;
    }
    for(auto &it : fanout){
        outGrid[it.first.first*2][it.first.second*2] = 1;
    }
}

void PCB::setPinConstraint(int x, int y){
    expr_vector pinGroup = solver->getVector();
    for(int i = 0; i < layer; i++){
        pinGroup.push_back(gridMap[x][y][i]);
        expr_vector neighbor = solver->getVector();
        if(x > 0) neighbor.push_back(gridMap[x-1][y][i]);
        if(x < col-1) neighbor.push_back(gridMap[x+1][y][i]);
        if(y > 0) neighbor.push_back(gridMap[x][y-1][i]);
        if(y < row-1) neighbor.push_back(gridMap[x][y+1][i]);
        solver->add(!gridMap[x][y][i] || (atleast(neighbor, 1) && atmost(neighbor, 1)));
    }
    solver->add(atleast(pinGroup, 1) && atmost(pinGroup, 1));
}

void PCB::setEdgeConstraint(int x, int y, int z){
    if(x % 2 == 1 && y % 2 == 0){//horizontal
        solver->add((!gridMap[x][y][z] || gridMap[x-1][y][z]) && (!gridMap[x][y][z] || gridMap[x+1][y][z]));
        solver->add(!gridMap[x][y][z] || netMap[x-1][y][z] == netMap[x+1][y][z]);
    }
    else if(y % 2 == 1 && x % 2 == 0){//vertical
        solver->add((!gridMap[x][y][z] || gridMap[x][y-1][z]) && (!gridMap[x][y][z] || gridMap[x][y+1][z]));
        solver->add(!gridMap[x][y][z] || netMap[x][y-1][z] == netMap[x][y+1][z]);
    }
}

void PCB::setGridConstraint(int x, int y, int z){
    expr top = (y == 0 ? solver->bool_val(0) : gridMap[x][y-1][z]), topNet = (y == 0 ? solver->int_val(0) : netMap[x][y-1][z]);
    expr bottom = (y == row-1 ? solver->bool_val(0) : gridMap[x][y+1][z]), bottomNet = (y == row-1 ? solver->int_val(0) : netMap[x][y+1][z]);
    expr left = (x == 0 ? solver->bool_val(0) : gridMap[x-1][y][z]), leftNet = (x == 0 ? solver->int_val(0) : netMap[x-1][y][z]);
    expr right = (x == col-1 ? solver->bool_val(0) : gridMap[x+1][y][z]), rightNet = (x == col-1 ? solver->int_val(0) : netMap[x+1][y][z]);
    expr sit1 = (top && bottom && !left && !right && topNet == bottomNet), sit2 = (!top && !bottom && left && right && leftNet == rightNet), 
     sit3 = (top && !bottom && left && !right && topNet == leftNet), sit4 = (!top && bottom && left && !right && bottomNet == leftNet), 
     sit5 = (top && !bottom && !left && right && rightNet == topNet), sit6 = (!top && bottom && !left && right && rightNet == bottomNet);
    solver->add((!top && !bottom && !left && !right) || (sit1 || sit2 || sit3 || sit4 || sit5 || sit6));
}

void PCB::setOutConstraint(int x, int y, int net = -999){
    expr_vector outGroup = solver->getVector();
    for(int i = 0; i < layer; i++){
        outGroup.push_back(gridMap[x][y][i]);
        if(net != -999) solver->add(!gridMap[x][y][i] || netMap[x][y][i] == net);
    }
    solver->add(atleast(outGroup, 1) && atmost(outGroup, 1));
}

void PCB::setLength(){
    expr_vector totalEdge = solver->getVector();
    vector<expr_vector> eachEdge(pinNum, solver->getVector());
    for(int x = 0; x < col; x++){
        for(int y = 0; y < row; y++){
            for(int z = 0; z < layer; z++){
                if((x % 2 == 1 && y % 2 == 0) || (y % 2 == 1 && x % 2 == 0)){
                    totalEdge.push_back(bool2int(gridMap[x][y][z]));
                    for(int i = 0; i < pinNum; i++){
                        eachEdge[i].push_back(bool2int(gridMap[x][y][z] && (netMap[x][y][z] == i)));
                    }
                }
            }
        }
    }
    solver->add(totalLen == sum(totalEdge));
    vector<expr> tmplen(pinNum, solver->int_val(0));
    expr BOOL1 = solver->bool_val(0), BOOL2 = solver->bool_val(0);
    for(int i = 0; i < pinNum; i++){
        solver->add(tmplen[i] == sum(eachEdge[i]));
        solver->add(maxNetLen >= tmplen[i]);
        solver->add(minNetLen <= tmplen[i]);
        BOOL1 = BOOL1 || (maxNetLen == tmplen[i]);
        BOOL2 = BOOL2 || (minNetLen == tmplen[i]);
    }
    solver->add(BOOL1 && BOOL2);
    
}

void PCB::setConstraint(){
    for(int x = 0; x < col; x++){
        for(int y = 0; y < row; y++){
            if(pinGrid[x][y] == 1){
                setPinConstraint(x, y);
            }
            else if(outGrid[x][y] == 1){
                setOutConstraint(x, y);
            }
            else{
                for(int z = 0; z < layer; z++){
                    if((x % 2 == 1 && y % 2 == 0) || (y % 2 == 1 && x % 2 == 0)) setEdgeConstraint(x, y, z);
                    else if(x % 2 == 0 && y % 2 == 0) setGridConstraint(x, y, z);
                }
            }
        }
    }
    setLength();
}

    