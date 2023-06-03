#include "include/UI.hpp"
#include <iostream>
#include <chrono>
#define BOUND 20
using namespace std;
int main(){
    auto start_time = std::chrono::high_resolution_clock::now();
    UI *sper = new UI();
    sper->createBoard(BOUND, BOUND, 2);
    sper->addPin(2, 3, 0);
    sper->addPin(1, 1, 1);
    sper->addPin(3, 2, 2);
    sper->addPin(5, 3, 3);
    sper->addPin(2, 1, 4);
    sper->addPin(4, 4, 5);
    sper->addPin(8, 8, 6);
    sper->addPin(10, 6, 7);
    sper->addPin(13, 10, 8);
    sper->addPin(16, 13, 9);
    sper->addFanout(2, BOUND-1);
    sper->addFanout(3, BOUND-1);
    sper->addFanout(BOUND-1, 2);
    sper->addFanout(BOUND-1, 6);
    sper->addFanout(BOUND-2, 0);
    sper->addFanout(6, BOUND-1);
    sper->addFanout(10, BOUND-1);
    sper->addFanout(11, BOUND-1);
    sper->addFanout(15, BOUND-1);
    sper->addFanout(BOUND-1, 15);
    /*sper->addPin(1, 1, 0);
    sper->addPin(2, 2, 1);
    sper->addPin(4, 2, 2);
    sper->addPin(5, 1, 3);
    sper->addPin(2, 4, 4);
    sper->addPin(3, 5, 5);
    sper->addPin(5, 5, 6);
    sper->addPin(6, 4, 7);
    sper->addPin(2, 6, 8);
    sper->addFanout(1, 0, 0);
    sper->addFanout(2, 0, 2);
    sper->addFanout(4, 0, 3);
    sper->addFanout(6, 0, 7);
    sper->addFanout(0, 2, 1);
    sper->addFanout(0, 5, 4);
    sper->addFanout(3, 7, 8);
    sper->addFanout(5, 7, 5);
    sper->addFanout(7, 6, 6);*/



    
    sper->setMode(3);
    sper->getResult();
    sper->outputResult();
    auto end_time = std::chrono::high_resolution_clock::now();
    auto duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count()/1000000.0;

    // Print the duration
    std::cout << "Time taken: " << duration << " seconds" << std::endl;
}