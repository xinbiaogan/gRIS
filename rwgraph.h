#ifndef _RWGRAPH_H
#define _RWGRAPH_H
#include <vector>
#include <random>

// Graph class defines fundamental operators on graph
class Graph
{
	private:
		// number of nodes
		unsigned int numNodes;
		// number of edges
		unsigned int numEdges;
		// adjacency list
		std::vector<std::vector<int> > adjList;
		std::vector<std::vector<float> > weights;
	
	public:
		// get a vector of neighbours of node u
		const std::vector<int> & operator [] (int u) const;
		// return weights of neighbours of node u
		const std::vector<float> & getWeight(int u) const;

		// get a vector of neighbours of node u
		const std::vector<int> & operator [] (int u);
		// return weights of neighbours of node u
		const std::vector<float> & getWeight(int u);

		// get degree of node u
		int getDegree(int u) const;
		// get size of the graph
		int getSize() const;
		// get number of edges
		int getEdge() const;
		// read graph from a file
		void readGraphLT(const char * filename);
		// read graph from a file
		void readGraphIC(const char * filename);
		// write the graph to file
		void writeToFile(const char * filename);
		
};

class HyperGraph
{
	private:
		// store the edges that a node is incident to
		std::vector<std::vector<int> > node_edge;
		// store hyperedges
		std::vector<std::vector<int> > edge_node;
		unsigned int maxDegree;
		unsigned int numNodes;
	public:
		std::vector<int> num_hyedge;
		HyperGraph(unsigned int n);
		void addEdge(std::vector<int> edge);
		void addEdgeD(std::vector<int> edge);
		int getMaxDegree();
		const std::vector<int> & getEdge(int e) const;
		const std::vector<int> & getEdge(int e);
		const std::vector<int> & getNode(int n) const;
		const std::vector<int> & getNode(int n);
		// get number of edges
        int getNumEdge() const;
		void clearEdges();

};

// choose an element proportional to distribution in w
int randIndex_bin(std::vector<float> &w,  std::mt19937_64 & mt, std::uniform_real_distribution<double> &dist);

// generate a hyperedge following LT model
class PollingLT{
	public:
		void polling(const Graph &g, int source, std::vector<int> & res, std::mt19937_64 & mt, std::uniform_real_distribution<double> &dist, std::vector<bool> &visit, std::vector<int> &mark_visit);
};

// generate a hyperedge following IC model
class PollingIC{
	public:
		void polling(const Graph &g, int source, std::vector<int> & res,  std::mt19937_64 & mt, std::uniform_real_distribution<double> &dist, std::vector<bool> &visit, std::vector<int> &visit_mark);
};

float getCurrentMemoryUsage();

#endif
