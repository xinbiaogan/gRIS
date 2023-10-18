PARA = -std=c++11 -O3 -Wall
com: rwgraph.cpp Hyper.cpp el2bin.cpp option.cpp GLib.hpp
	g++ -c GLib.hpp -o GLib.o $(PARA)
	g++ -c mappedHeap.hpp -o mappedHeap.o $(PARA)
	g++ -c HeapData.hpp -o HeadData.o $(PARA)
	g++ -c option.cpp -o option.o $(PARA)
	g++ -c rwgraph.cpp -o rwgraph.o $(PARA)
	g++ Hyper.cpp rwgraph.o option.o -o Hyper -fopenmp $(PARA)
	g++ el2bin.cpp -o el2bin $(PARA)
