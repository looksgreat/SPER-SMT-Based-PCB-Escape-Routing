CXX = g++
CXXFLAGS = -std=c++14 -g -O0 -shared -m64 -L/DATA/lllll/lib
HEADERS = -I./include -I/DATA/lllll/include -I/DATA/lllll/.local/lib/python3.6/site-packages/pybind11/include/pybind11/ -I.
SOURCES = src/routing.cpp src/solver.cpp src/UI.cpp
OBJECTS = build/routing.o build/solver.o build/UI.o
EXECUTABLE = build/sper.so
PIBINDFLAG = -fPIC `python3 -m pybind11 --includes` `python3-config --includes --ldflags`

all: main.cpp $(SOURCES) $(EXECUTABLE) 

build/sper.so: main.cpp $(OBJECTS) 
	${CXX} ${CXXFLAGS}  $^ ${HEADERS} -lz3  $(PIBINDFLAG) -o $@

build/%.o : src/%.cpp include/%.hpp
	mkdir -p build
	${CXX} ${CXXFLAGS}  $< ${HEADERS} -c -lz3  $(PIBINDFLAG) -o $@

.PHONY:clean
clean:
	rm build -r -f
