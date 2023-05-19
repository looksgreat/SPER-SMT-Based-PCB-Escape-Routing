CXX = g++
CXXFLAGS = -std=c++14 -g -o0 -L/DATA/lllll/lib
HEADERS = -I./include -I/DATA/lllll/include
SOURCES = src/routing.cpp src/solver.cpp src/UI.cpp
OBJECTS = build/routing.o build/solver.o build/UI.o
EXECUTABLE = build/sper

all: main.cpp $(SOURCES) $(EXECUTABLE)

build/sper: main.cpp $(OBJECTS) 
	${CXX} ${CXXFLAGS}  $^ ${HEADERS} -lz3  -o $@

build/%.o : src/%.cpp include/%.hpp
	mkdir -p build
	${CXX} ${CXXFLAGS}  $< ${HEADERS} -c -lz3  -o $@

.PHONY:clean
clean:
	rm build -r -f
