#include "../include/routing.hpp"
#include <iostream>
#include <algorithm>
#include <queue>
#include "z3++.h"
using namespace std;



void PCB::setGridMap(int r, int c, int l,vector<pair<pair<int, int>, int>> &pin, vector<pair<pair<int, int>, int>> &fanout, int mode){
    //solver = new SOLVER();
    row = r*2-1; col = c*2-1; layer = l;
    this->mode = mode;
    gridMap.resize(col, vector<vector<expr>>(row, vector<expr>(layer, solver->get_expr())));
    netMap.resize(col, vector<vector<expr>>(row, vector<expr>(layer, solver->get_expr())));
    for(int i = 0; i < col; i++){
        for(int j = 0; j < row; j++){
            for(int k = 0; k < layer; k++){
                if(!(i % 2 == 1 && j % 2 == 1)) gridMap[i][j][k] = solver->bool_const("grid_"+to_string(i)+"_"+to_string(j)+"_"+to_string(k));
                if(!(i % 2 == 1 && j % 2 == 1)) netMap[i][j][k] = solver->int_const("net_"+to_string(i)+"_"+to_string(j)+"_"+to_string(k));
            }
        }
    }
    pinGrid.resize(col, vector<int>(row, -1));
    outGrid.resize(col, vector<int>(row, -1));

    for(auto &it : pin){
        pinGrid[it.first.first*2][it.first.second*2] = it.second;
    }
    for(auto &it : fanout){
        outGrid[it.first.first*2][it.first.second*2] = it.second;
    }
    this->pinNum = pin.size();
    pinVect = &pin;
    fanoutVect = &fanout;
}

void PCB::setPinConstraint(int x, int y, int net){
    expr_vector pinGroup = solver->getVector();
    for(int i = 0; i < layer; i++){
        pinGroup.push_back(gridMap[x][y][i]);
        expr_vector neighbor = solver->getVector();
        if(x > 0) neighbor.push_back(gridMap[x-1][y][i]);
        if(x < col-1) neighbor.push_back(gridMap[x+1][y][i]);
        if(y > 0) neighbor.push_back(gridMap[x][y-1][i]);
        if(y < row-1) neighbor.push_back(gridMap[x][y+1][i]);
        solver->add(!gridMap[x][y][i] || (atleast(neighbor, 1) && atmost(neighbor, 1) && netMap[x][y][i] == net));
    }
    solver->add(atleast(pinGroup, 1) && atmost(pinGroup, 1));
}

void PCB::setEdgeConstraint(int x, int y, int z){
    if(x % 2 == 1 && y % 2 == 0){//horizontal
        solver->add((!gridMap[x][y][z] || gridMap[x-1][y][z]) && (!gridMap[x][y][z] || gridMap[x+1][y][z]));
        solver->add(!gridMap[x][y][z] || (netMap[x-1][y][z] == netMap[x+1][y][z] && netMap[x][y][z] == netMap[x-1][y][z]));
    }
    else if(y % 2 == 1 && x % 2 == 0){//vertical
        solver->add((!gridMap[x][y][z] || gridMap[x][y-1][z]) && (!gridMap[x][y][z] || gridMap[x][y+1][z]));
        solver->add(!gridMap[x][y][z] || (netMap[x][y-1][z] == netMap[x][y+1][z] && netMap[x][y][z] == netMap[x][y-1][z]));
    }
}

void PCB::setGridConstraint(int x, int y, int z){
    expr bottom = ((y == 0 || x == 0 || x == col-1) ? solver->bool_val(0) : gridMap[x][y-1][z]), topNet = (y == 0 ? solver->int_val(0) : netMap[x][y-1][z]);
    expr top = ((y == row-1 || x == 0 || x == col-1) ? solver->bool_val(0) : gridMap[x][y+1][z]), bottomNet = (y == row-1 ? solver->int_val(0) : netMap[x][y+1][z]);
    expr left = ((x == 0 || y == 0 || y == row-1) ? solver->bool_val(0) : gridMap[x-1][y][z]), leftNet = (x == 0 ? solver->int_val(0) : netMap[x-1][y][z]);
    expr right = ((x == col-1 || y == 0 || y == row-1) ? solver->bool_val(0) : gridMap[x+1][y][z]), rightNet = (x == col-1 ? solver->int_val(0) : netMap[x+1][y][z]);
    expr sit1 = (top && bottom && !left && !right && topNet == bottomNet), sit2 = (!top && !bottom && left && right && leftNet == rightNet), 
     sit3 = (top && !bottom && left && !right && topNet == leftNet), sit4 = (!top && bottom && left && !right && bottomNet == leftNet), 
     sit5 = (top && !bottom && !left && right && rightNet == topNet), sit6 = (!top && bottom && !left && right && rightNet == bottomNet);
    solver->add((!top && !bottom && !left && !right && !gridMap[x][y][z]) || (gridMap[x][y][z] && (sit1 || sit2 || sit3 || sit4 || sit5 || sit6)));
}

void PCB::setOutConstraint(int x, int y, int net){
    expr_vector outGroup = solver->getVector();
    for(int i = 0; i < layer; i++){
        outGroup.push_back(gridMap[x][y][i]);
        if(net != -999) solver->add(!gridMap[x][y][i] || netMap[x][y][i] == net);
    }
    solver->add(atleast(outGroup, 1) && atmost(outGroup, 1));
}

void PCB::setBoundingBox(){
    //pin idx, radius
    vector<pair<int, int>> boundPin;
    for(int i = 0; i < pinVect->size(); i++){
        int x = (*pinVect)[i].first.first*2, y = (*pinVect)[i].first.second*2;
        int arr[4];
        arr[0] = x; arr[1] = col-1-x; arr[2] = y; arr[3] = row-1-y;
        std::sort(arr, arr+4);
        for(int r = arr[0]; r < arr[3]; r++){
            int radius = r, fanoutNum = fanoutVect->size(), inBox = 0;
            for(int fanout = 0; fanout < fanoutNum; fanout++){
                //test if fanout is in the bounding box
                int fx = (*fanoutVect)[fanout].first.first*2, fy = (*fanoutVect)[fanout].first.second*2;
                if(fx >= x-radius && fx <= x+radius && fy >= y-radius && fy <= y+radius) inBox++;
            }
            if(inBox >= fanoutNum/2){
                boundPin.push_back(make_pair(i, radius));
                break;
            }
        }
    }
    //set bounding box
    for(int i = 0; i < boundPin.size(); i++){
        int pinX = (*pinVect)[boundPin[i].first].first.first*2, pinY = (*pinVect)[boundPin[i].first].first.second*2, pinNet = (*pinVect)[boundPin[i].first].second;
        int radius = boundPin[i].second;
        for(int x = 0; x < col; x++){
            for(int y = 0; y < row; y++){
                for(int z = 0; z < layer; z++){
                    if(!(x % 2 == 1 && y % 2 == 1)){
                        if(x > pinX + radius+1 || x < pinX - radius-1 || y > pinY + radius+1 || y < pinY - radius-1) solver->add(!(netMap[x][y][z] == pinNet));
                        
                    }
                }
            }
        }
    }

}

void PCB::setLength(){
    func_decl bool2int = z3::function("bool2int", solver->get_bool_sort(), solver->get_int_sort());
    solver->add(bool2int(solver->bool_val(0)) == 0);
    solver->add(bool2int(solver->bool_val(1)) == 1);

    expr_vector totalEdge = solver->getVector();
    vector<expr_vector> eachEdge;
    for(int i = 0; i < pinNum; i++) eachEdge.push_back(solver->getVector());
    for(int x = 0; x < col; x++){
        for(int y = 0; y < row; y++){
            for(int z = 0; z < layer; z++){
                if(((x % 2 == 1 && y % 2 == 0) || (y % 2 == 1 && x % 2 == 0)) && x != 0 && y != 0 && x != col-1 && y != row-1){
                    totalEdge.push_back(bool2int(gridMap[x][y][z]));
                    for(int i = 0; i < pinNum; i++){
                        eachEdge[i].push_back(bool2int(gridMap[x][y][z] && (netMap[x][y][z] == i)));
                    }
                }
            }
        }
    }
    totalLen = solver->int_const("tL");
    maxNetLen = solver->int_const("maxLen");
    minNetLen = solver->int_const("minLen");
    solver->add(totalLen == sum(totalEdge));
    tmplen = solver->getVector();
    expr BOOL1 = solver->bool_val(0), BOOL2 = solver->bool_val(0);
    for(int i = 0; i < pinNum; i++){
        expr tmp = solver->int_const(("len" + to_string(i)));
        solver->add(tmp == sum(eachEdge[i]));
        solver->add(maxNetLen >= tmp);
        solver->add(minNetLen <= tmp);
        BOOL1 = BOOL1 || (maxNetLen == tmp);
        BOOL2 = BOOL2 || (minNetLen == tmp);
        tmplen.push_back(tmp);
    }
    solver->add(BOOL1 && BOOL2);
    
}

void PCB::setConstraint(){
    for(int x = 0; x < col; x++){
        for(int y = 0; y < row; y++){
            if(pinGrid[x][y] != -1){
                setPinConstraint(x, y, pinGrid[x][y]);
            }
            else if(outGrid[x][y] != -1){
                if(outGrid[x][y] == -999) setOutConstraint(x, y);
                else setOutConstraint(x, y, outGrid[x][y]);
            }
            else{
                for(int z = 0; z < layer; z++){
                    if(((x % 2 == 1 && y % 2 == 0) || (y % 2 == 1 && x % 2 == 0)) && x != 0 && y != 0 && x != col-1 && y != row-1) setEdgeConstraint(x, y, z);
                    else if(x % 2 == 0 && y % 2 == 0) setGridConstraint(x, y, z);
                }
            }
        }
    }
    //setBoundingBox();
    setLength();
}

void PCB::getSolution(vector<vector<vector<int>>> &result){
    solver->push();
    setBoundingBox();
    bool pass;
    if(mode == 0){
        pass = solver->minimize(totalLen);
        pass = solver->minimize(maxNetLen - minNetLen);
    }
    else if(mode == 1){
        pass = solver->minimize(maxNetLen - minNetLen);
        pass = solver->minimize(totalLen);
    }
    //cout << solver->check() << endl;
    int satisfy = solver->check();
    if(satisfy == 0){
        cout << "switch to no bounding box mode" << endl;
        solver->pop();
        satisfy = solver->check();
    }
    else{
        cout << "bounding box mode" << endl;
    }
    if(satisfy == 0) throw runtime_error("unsat");
    model& m = solver->get_model();
    result.resize(col, vector<vector<int>>(row, vector<int>(layer, -1)));
    vector<vector<vector<int>>> test;
    test.resize(col, vector<vector<int>>(row, vector<int>(layer, -1)));
    for(int x = 0; x < col; x++){
        for(int y = 0; y < row; y++){
            for(int z = 0; z < layer; z++){
                if(!(x % 2 == 1 && y % 2 == 1)){
                    if(solver->eval_bool(gridMap[x][y][z])/* && x != 0 && y != 0 && x != col-1 && y != row-1*/) result[x][y][z] = solver->eval_num(netMap[x][y][z]);
                    test[x][y][z] = result[x][y][z];
                    //else result[x][y][z] = 0;
                }
            }
        }
    }
    cout << "total length: " << solver->eval_num(totalLen) << endl;
    cout << "length diff: " << solver->eval_num(maxNetLen) << " - " <<  solver->eval_num(minNetLen) << " = " << solver->eval_num(maxNetLen) - solver->eval_num(minNetLen) << endl;
    BFS(test);
}

void PCB::BFS(vector<vector<vector<int>>> &result){
    auto result_backup = result;
    for(int time = 0; time < 3; time++){
        vector<vector<vector<int>>> rt(col, vector<vector<int>>(row, vector<int>(layer, -1)));
        for(auto &it : (*pinVect)){
            int x = it.first.first*2, y = it.first.second*2, net = it.second, targetx, targety, z;
            //cout << "net:" << net << endl;
            for(int l = 0; l < layer; l++){
                if(result_backup[x][y][l] == net){
                    z = l;
                    break;
                }
            }
            for(auto &fo : (*fanoutVect)){
                int fx = fo.first.first*2, fy = fo.first.second*2;
                if(result_backup[fx][fy][z] == net){
                    targetx = fx, targety = fy;
                    break;
                }

            }
            queue<pair<int, int>> q;
            vector<vector<pair<int, int>>> front(col, vector<pair<int, int>>(row, make_pair(-1, -1)));
            q.push(make_pair(x, y));
            front[x][y] = make_pair(0, 0);
            int curx = 0, cury = 0;
            while(!q.empty()){
                curx = q.front().first;
                cury = q.front().second;
                q.pop();
                if(curx == targetx && cury == targety){
                    break;
                }
                if(curx > 0 && ((curx-1)%2 != 1 || cury%2 != 1) && front[curx-1][cury].first == -1 && (result[curx-1][cury][z] == -1 || result[curx-1][cury][z] == net) && (pinGrid[curx-1][cury] == -1) /*&& (outGrid[curx-1][cury] == -1 || (outGrid[curx-1][cury] == net))*/){
                    q.push(make_pair(curx-1, cury));
                    front[curx-1][cury] = make_pair(curx, cury);
                }
                if(curx < col-1 && ((curx+1)%2 != 1 || cury%2 != 1) && front[curx+1][cury].first == -1 && (result[curx+1][cury][z] == -1 || result[curx+1][cury][z] == net) && (pinGrid[curx+1][cury] == -1) /*&& (outGrid[curx+1][cury] == -1 || (outGrid[curx+1][cury] == net))*/){
                    q.push(make_pair(curx+1, cury));
                    front[curx+1][cury] =  make_pair(curx, cury);
                }
                if(cury > 0 &&  ((curx)%2 != 1 || (cury-1)%2 != 1)&& front[curx][cury-1].first == -1 && (result[curx][cury-1][z] == -1 || result[curx][cury-1][z] == net) && (pinGrid[curx][cury-1] == -1) /*&& (outGrid[curx][cury-1] == -1 || (outGrid[curx][cury-1] == net))*/){
                    q.push(make_pair(curx, cury-1));
                    front[curx][cury-1] =  make_pair(curx, cury);
                }
                if(cury < row-1 && ((curx)%2 != 1 || (cury+1)%2 != 1) && front[curx][cury+1].first == -1 && (result[curx][cury+1][z] == -1 || result[curx][cury+1][z] == net) && (pinGrid[curx][cury+1] == -1) /*&& (outGrid[curx][cury+1] == -1 || (outGrid[curx][cury+1] == net))*/){
                    q.push(make_pair(curx, cury+1));
                    front[curx][cury+1] =  make_pair(curx, cury);
                }
            }
            if(curx != targetx || cury != targety){
                cout << net << " no path" << endl;
                cout << endl;
            } 
            int tmpx = targetx, tmpy = targety;
            //cout << "x: " << tmpx << " y: " << tmpy << endl;
            //cout << "x: " << x << " y: " << y << endl;
            while((tmpx != x) || (tmpy != y)){
                //cout << "x: " << tmpx << " y: " << tmpy << endl;
                rt[tmpx][tmpy][z] = net;
                int tx = front[tmpx][tmpy].first, ty = front[tmpx][tmpy].second;
                tmpx = tx, tmpy = ty;
            }
            rt[x][y][z] = net;
        }
        result = rt;
    }
}

    