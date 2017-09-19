RouterSim.exe	:	RouterSim.o
	g++ RouterSim.o -std=c++11 -o RouterSim.exe

RouterSim.o	:	RouterSim.cpp RouterSim.h
	g++ -std=c++11 -c RouterSim.cpp
	