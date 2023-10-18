#ifndef _HYPERGRAPH_H_
#define _HYPERGRAPH_H_

#include "rwgraph.h"
#include <iostream>
#include <cstdlib>
#include <cstdio>
#include <cmath>
#include <vector>
#include <fstream>
#include <cstring>
#include <random>
#include <queue>
#include "mappedHeap.hpp"
#include "HeapData.hpp"

#if defined(_OPENMP)
#include <omp.h>
#else
typedef int omp_int_t;
inline omp_int_t omp_set_num_threads(int t) { return 1; }
inline omp_int_t omp_get_thread_num() { return 0; }
#endif

using namespace std;


template <class PollingProcess>
long long addHyperedge(Graph &g, HyperGraph &hg, int t, long long num, std::mt19937_64 &mt, std::uniform_real_distribution<double> &dist, PollingProcess &p)
{
	int numNodes = g.getSize();

	std::uniform_int_distribution<int> intGen(1, numNodes);

	// temporary array for holding hyperedges before put them into the hypergraph
	vector<vector<vector<int>>> tmp(t, vector<vector<int>>(numNodes / t + 1));

	omp_set_num_threads(t);

	long long iter = 0;
	int c = 10000;

#pragma omp parallel
	{
		int id = omp_get_thread_num();
		int ver;

		vector<int> visit_mark(numNodes + 1, 0);
		vector<bool> visit(numNodes + 1, false);

		while (iter < num)
		{

			for (int i = 0; i < c; ++i)
			{
				vector<int> he;

				ver = intGen(mt);

				// generate a hyperedge and store in he variable
				p.polling(g, ver, hg.num_hyedge, mt, dist, visit, visit_mark);

				// tmp[id].push_back(he);
			}

#pragma omp atomic
			iter += c;
		}
	}
	return iter;
}

void buildSeedSet(Graph &g, HyperGraph &hg, vector<int> &seeds, unsigned int n, int k, double &ept)
{
	vector<bool> nodeMark(n + 1, true);
	seeds.clear();
	vector<float> w;
	queue<unsigned int> q1, q2;
	queue<double> q3, q4;
	vector<int> neigh;

	unsigned int cur;
	int i;
	int totalCost = 0;
	while (totalCost < k)
	{
		vector<bool> visitMark(n + 1, false);
		int max = -1;
		int index = 0;
		for (i = 0; i < n; i++)
		{
			if (max < hg.num_hyedge[i] && nodeMark[i])
			{
				max =  hg.num_hyedge[i];
				index = i;
			}
		}
		nodeMark[index] = false;
		totalCost++;
		seeds.push_back(index);

		neigh = g[index];
		w = g.getWeight(index);
		for (i = 0; i < neigh.size(); i++)
		{
			if (nodeMark[neigh[i]])
			{
				q1.push(neigh[i]);
				q3.push(w[i + 1]);
			}
		}
		while (!q1.empty())
		{
			cur = q1.front();
			q1.pop();
			double p = q3.front();
			q3.pop();
			hg.num_hyedge[cur] -= int(p *  hg.num_hyedge[index]);
			visitMark[cur] = true;
			ept += p *  hg.num_hyedge[index] > 1?p *  hg.num_hyedge[index]:1;
			w = g.getWeight(cur);
			neigh = g[cur];
			for (i = 0; i < neigh.size(); ++i)
			{
				if (!visitMark[neigh[i]] && nodeMark[neigh[i]])
				{
					q2.push(neigh[i]);
					q4.push(w[i + 1] * p);
				}
			}
		}
		while (!q2.empty())
		{
			cur = q2.front();
			q2.pop();
			double p = q4.front();
			q4.pop();
			ept +=  p *  hg.num_hyedge[index] > 1?p *  hg.num_hyedge[index]:1;
			hg.num_hyedge[cur] -= int(p *  hg.num_hyedge[index]);
		}
	}
}

#endif
