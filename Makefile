test: main.cpp GEQuadTree.h GEQuadTree.cpp
	g++ -std=c++11 GEQuadTree.cpp main.cpp -D DEBUG -o test.exe
testmemory: main.cpp GEQuadTree.h GEQuadTree.cpp
	g++ -std=c++11 GEQuadTree.cpp main.cpp -D DEBUG -D TESTMEMORY -o test.exe 