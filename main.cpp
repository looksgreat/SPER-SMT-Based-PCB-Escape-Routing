#include "include/UI.hpp"
#include <iostream>
using namespace std;
int main(){
    UI *sper = new UI();
    sper->createBoard(6, 6, 2);
    sper->addPin(3, 3, 0);
    sper->addPin(4, 4, 1);
    sper->addFanout(0, 3);
    sper->addFanout(3, 0);
    sper->setMode(0);
    sper->getResult();
    sper->outputResult();
}