#include "rwgraph.h"
#include <vector>
#include <iostream>
#include <fstream>
#include <queue>
#include <cstdlib>
#include <unistd.h>
#include <sstream>

using namespace std;

const vector<int> &Graph::operator[](int u) const
{
        return adjList[u];
}

/*
 * get the list of neighbors of node u
 */
const vector<int> &Graph::operator[](int u)
{
        return adjList[u];
}

/*
 * get the list neighbor weights of node u
 */
const vector<float> &Graph::getWeight(int u) const
{
        return weights[u];
}

const vector<float> &Graph::getWeight(int u)
{
        return weights[u];
}

/*
 * get degree of node u
 */
int Graph::getDegree(int u) const
{
        return adjList[u].size();
}

/*
 * get the number of nodes
 */
int Graph::getSize() const
{
        return numNodes;
}

/*
 * get the number of edges
 */
int Graph::getEdge() const
{
        return numEdges;
}

/*
 * read binary graph input for LT model
 * difference between LT and IC is for LT we accumulate the weights for fast choosing a random node
 */
void Graph::readGraphLT(const char *filename)
{
        FILE *pFile;
        pFile = fopen(filename, "rb");
        fread(&numNodes, sizeof(int), 1, pFile);
        fread(&numEdges, sizeof(long long), 1, pFile);
        vector<int> degree(numNodes + 1);
        fread(&degree[1], sizeof(int), numNodes, pFile);

        vector<int> a;
        vector<float> b;
        adjList.push_back(a);
        weights.push_back(b);

        for (unsigned int i = 1; i <= numNodes; ++i)
        {
                vector<int> tmp(degree[i]);
                fread(&tmp[0], sizeof(int), degree[i], pFile);

                adjList.push_back(tmp);
        }

        for (unsigned int i = 1; i <= numNodes; ++i)
        {
                vector<float> tmp(degree[i] + 1, 0);
                fread(&tmp[1], sizeof(float), degree[i], pFile);

                for (int j = 1; j < degree[i] + 1; ++j)
                {
                        tmp[j] += tmp[j - 1];
                }

                weights.push_back(tmp);
        }
}

/*
 * read input graph for IC model
 */
void Graph::readGraphIC(const char *filename)
{
        FILE *pFile;
        pFile = fopen(filename, "rb");
        fread(&numNodes, sizeof(int), 1, pFile);
        fread(&numEdges, sizeof(long long), 1, pFile);
        vector<int> degree(numNodes + 1);
        fread(&degree[1], sizeof(int), numNodes, pFile);

        vector<int> a;
        vector<float> b;
        adjList.push_back(a);
        weights.push_back(b);

        for (unsigned int i = 1; i <= numNodes; ++i)
        {
                vector<int> tmp(degree[i]);
                fread(&tmp[0], sizeof(int), degree[i], pFile);
                adjList.push_back(tmp);
        }

        for (unsigned int i = 1; i <= numNodes; ++i)
        {
                vector<float> tmp(degree[i] + 1, 0);
                fread(&tmp[1], sizeof(float), degree[i], pFile);
                weights.push_back(tmp);
        }
}

void Graph::writeToFile(const char *filename)
{ /*
        ofstream output(filename);
        for (unsigned int i = 0; i < numNodes; ++i){
                for (unsigned int j = 0; j < adjList[i].size(); ++j){
                        if (adjList[i][j] > i){
                                output << adjList[i][j] << " " << i << " " << weights[i][j] << endl;
                        }
                }
        }
        output.close();
*/
}

// choose a random edge in LT model based on linear search
int randIndex_lin(vector<float> &w, std::mt19937_64 &mt, std::uniform_real_distribution<double> &dist)
{
        double ranNum = dist(mt);
        if (w.size() <= 1 || ranNum > w[w.size() - 1])
                return -1;

        for (unsigned int i = 1; i < w.size(); ++i)
        {
                if (ranNum <= w[i])
                        return i;
        }
        return -1;
}

// choose a random live edge in LT model based on binary search
int randIndex_bin(vector<float> &w, std::mt19937_64 &mt, std::uniform_real_distribution<double> &dist)
{
        double ran = dist(mt);
        if (w.size() <= 1 || ran > w[w.size() - 1])
                return -1;
        int left = 1;
        int right = w.size() - 1;
        int prob;
        for (unsigned int i = 0; i < w.size(); ++i)
        {
                prob = (left + right) / 2;
                if (w[prob - 1] > ran)
                {
                        right = prob - 1;
                        continue;
                }
                if (w[prob] <= ran)
                {
                        left = prob + 1;
                        continue;
                }
                break;
        }
        return prob;
}

HyperGraph::HyperGraph(unsigned int n)
{
        for (unsigned int i = 0; i <= n; ++i)
        {
                vector<int> tmp;
                node_edge.push_back(tmp);
        }
        maxDegree = 0;
        numNodes = n;
        num_hyedge.resize(n);
}

/*
 * Add a hyperedge into the hypergraph
 */
void HyperGraph::addEdge(vector<int> edge)
{
        edge_node.push_back(edge);
        unsigned int ind = edge_node.size() - 1;
        for (unsigned int i = 0; i < edge.size(); ++i)
                node_edge[edge[i]].push_back(ind);
}

/*
 * Add a hyperedge into the hypergraph while keeping track of the node with max degree
 */
void HyperGraph::addEdgeD(vector<int> edge)
{
        edge_node.push_back(edge);
        int ind = edge_node.size() - 1;
        for (unsigned int i = 0; i < edge.size(); ++i)
        {
                node_edge[edge[i]].push_back(ind);
                if (node_edge[edge[i]].size() > maxDegree)
                        maxDegree = node_edge[edge[i]].size();
        }
}

/*
 * get an edge from the hypergraph
 */
const vector<int> &HyperGraph::getEdge(int e) const
{
        return edge_node[e];
}

const vector<int> &HyperGraph::getEdge(int e)
{
        return edge_node[e];
}

/*
 * get the list of hyperedges incident to node n
 */
const vector<int> &HyperGraph::getNode(int n) const
{
        return node_edge[n];
}

const vector<int> &HyperGraph::getNode(int n)
{
        return node_edge[n];
}

/*
 * get the number of hyperedges
 */
int HyperGraph::getNumEdge() const
{
        return edge_node.size();
}

/*
 * get the maximum degree
 */
int HyperGraph::getMaxDegree()
{
        return maxDegree;
}

/*
 * remove all the hyperedges
 */
void HyperGraph::clearEdges()
{
        edge_node.clear();
        node_edge.clear();
        for (unsigned int i = 0; i <= numNodes; ++i)
        {
                vector<int> tmp;
                node_edge.push_back(tmp);
        }
        maxDegree = 0;
}

/*
 * polling process under LT model
 */
void PollingLT::polling(const Graph &g, int source, vector<int> &res, std::mt19937_64 &mt, std::uniform_real_distribution<double> &dist, vector<bool> &visit, vector<int> &visit_mark)
{
//doing for feature work
}

/*
 * polling process under IC model
 */
void PollingIC::polling(const Graph &g, int source, vector<int> &res, std::mt19937_64 &mt, std::uniform_real_distribution<double> &dist, vector<bool> &visit, vector<int> &visit_mark)
{
        unsigned int i;
        unsigned int cur;
        queue<unsigned int> q1, q2;
        q1.push(source);
        unsigned int num_marked = 1;
        visit[source] = true;
        visit_mark[0] = source;
        // res.push_back(source);
        res[source]++;
        vector<float> w;
        vector<int> neigh;

        neigh = g[source];
        w = g.getWeight(source);
        for (i = 0; i < neigh.size(); i++)
        {
                if (!visit[neigh[i]])
                {
                        if (dist(mt) < w[i + 1])
                        {
                                q1.push(neigh[i]);
                                res[neigh[i]]++;
                                visit[neigh[i]] = true;
                                visit_mark[num_marked++] = neigh[i];
                        }
                }
        }

        while (!q1.empty())
        {
                cur = q1.front();
                q1.pop();
                w = g.getWeight(cur);
                neigh = g[cur];
                for (i = 0; i < neigh.size(); ++i)
                {
                        if (!visit[neigh[i]])
                        {
                                if (dist(mt) < w[i + 1])
                                {
                                        q2.push(neigh[i]);
                                        res[neigh[i]]++;
                                        visit[neigh[i]] = true;
                                        visit_mark[num_marked++] = neigh[i];
                                }
                        }
                }
        }

        while (!q2.empty())
        {
                cur = q2.front();
                q2.pop();
                w = g.getWeight(cur);
                neigh = g[cur];
                for (i = 0; i < neigh.size(); ++i)
                {
                        if (!visit[neigh[i]])
                        {
                                if (dist(mt) < w[i + 1])
                                {
                                        res[neigh[i]]++;
                                        visit[neigh[i]] = true;
                                        visit_mark[num_marked++] = neigh[i];
                                }
                        }
                }
        }

        for (i = 0; i < num_marked; ++i)
        {
                visit[visit_mark[i]] = false;
        }
}

/*
 * convert from an integer to a string
 */
string intToStr(int i)
{
        stringstream ss;
        ss << i;
        return ss.str();
}

/*
 * convert from a strong to an integer
 */
unsigned int strToInt(string s)
{
        unsigned int i;
        istringstream myStream(s);

        if (myStream >> i)
        {
                return i;
        }
        else
        {
                cout << "String " << s << " is not a number." << endl;
                return atoi(s.c_str());
        }
        return i;
}

/*
 * measure the consumed memory
 */
float getCurrentMemoryUsage()
{

        string pid = intToStr(unsigned(getpid()));
        string outfile = "tmp_" + pid + ".txt";
        string command = "pmap " + pid + " | grep -i Total | awk '{print $2}' > " + outfile;
        system(command.c_str());

        string mem_str;
        ifstream ifs(outfile.c_str());
        std::getline(ifs, mem_str);
        ifs.close();

        mem_str = mem_str.substr(0, mem_str.size() - 1);
        cout << "get mem\n";
        cout << mem_str << "KB" << endl;
        float mem = (float)strToInt(mem_str);

        command = "rm " + outfile;
        system(command.c_str());

        return mem / 1024;

        return 0;
}
