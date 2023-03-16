
.PHONY: all build test

all: build test

build: main

test: main
	./main

main: *.hpp *.cpp
	g++ *.cpp `quantlib-config --cflags` -g0 -O3 `quantlib-config --libs` -I/opt/homebrew/Cellar/boost/1.81.0_1/include/ -L/opt/homebrew/Cellar/boost/1.81.0_1/lib -g0 -O3 -o main -lboost_system
	
	
	
	
