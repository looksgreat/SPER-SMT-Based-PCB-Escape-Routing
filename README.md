# **SPER-SMT-Based-PCB-Escape-Routing**
PCB escape routing is one of the problems in EDA. In this problem, we have a multilayer grid map with multiple pins on it, and several fan-out around the border of the grid map. The goal is to find a path for each pin to fan-out, and each path on the same layer should not cross each other. 

The problem is NP-hard, and there are many heuristic algorithms to solve it. However, the heuristic algorithms are not always optimal, and it is hard to find a good heuristic algorithm.

In this project, I will transform the problem to SMT problem, and use SMT solver to solve it. The SMT solver I use here will be Z3(https://github.com/Z3Prover/z3).

## **System Architecture**
This software will be developed in C++ and use the API provided by z3, and use pybind to support python user.

The system architecture consist of three parts:

+ A class serve as an interface between z3 API and our software. Since z3 is a general tool for many problems, this interface will help us develop easier.
+ A core part that transform the problem to SMT problem.
+ An interface that allow user to use the software.

## **Building SPER using make**

First you have to install z3 on your device. Z3 is a open-source program from Microsoft Research. See https://github.com/Z3Prover/z3 for detail.

After you install Z3 and clone our program from github, you can simply build our program using make.

For C++ user:
```bash
make
```
For python user:
```bash
cd python code
make
```

## **API Functions**
1. `UI()`
    + Constructor function that initializes the UI object.
2. `createBoard(int width, int height, int layers)`
    + Creates a board with the specified dimensions and number of layers.
    + Parameters:
        + width (int): The width of the board.
        + height (int): The height of the board.
        + layers (int): The number of layers in the board.
3. `addPin(int x, int y, int layer)`
    + Adds a pin to the board at the specified coordinates and layer.
    + Parameters:
        + x (int): The x-coordinate of the pin.
        + y (int): The y-coordinate of the pin.
        + layer (int): The layer in which the pin is added.
4. `addFanout(int x, int y, int s (optional))`
    + Adds a fanout to the board at the specified coordinates.
    + Parameters:
        + x (int): The x-coordinate of the fanout.
        + y (int): The y-coordinate of the fanout.
        + s (int): Specify this fanout's signal.If signal is not given, the software will decide the signal for this fanout.
5. `setMode(int mode)`
    + Sets the mode of the board to the specified value.
    + Parameters:
        + mode (int): The mode to set for the board.
    + We provide three mode:
        + Minimize mode (mode = 0): minimize total path length
        + Balance mode (mode = 1): minimize maximum difference between each path length
        + Fast mode (mode = 3): run fast
6. `getResult()`
    + Retrieves the result of the operations performed on the board, visulize using klayout. Also run testing.
7. `outputResult()`
    + Output result on stdout.
## **Example**
### C++
```c++
#include "include/UI.hpp"
#include <iostream>
#define BOUND 20
int main(){
    UI *sper = new UI();
    sper->createBoard(BOUND, BOUND, 2);
    sper->addPin(2, 3, 0);
    sper->addPin(1, 1, 1);
    sper->addFanout(2, BOUND-1);
    sper->addFanout(3, BOUND-1);
    sper->setMode(3);
    sper->getResult();
    sper->outputResult();
}
```
### Python
```python
import sper
ui = sper.UI()
ui.createBoard(5, 5, 1)
ui.addPin(2, 2, 0)
ui.addFanout(4, 2, 0)
ui.setMode(0)
ui.getResult()
ui.outputResult()
```