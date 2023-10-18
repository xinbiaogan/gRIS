#include "option.h"
#include "hypergraph.hpp"
#include <iostream>
#include <ctime>
#include <cmath>
#include <algorithm>
using namespace std;
int parseLine(char *line)
{
	// This assumes that a digit will be found and the line ends in " Kb".
	int i = strlen(line);
	const char *p = line;
	while (*p < '0' || *p > '9')
		p++;
	line[i - 3] = '\0';
	i = atoi(p);
	return i;
}
void printInstruction()
{
	printf("Parameters error, please check the readme.txt file for correct format!\n\n");
	printf("./Hyper [Options]\n");
	printf("Options:\n");

	printf("\t-i <binary graph file>\n");
	printf("\t\tspecify the path to the binary graph file (default: network.bin)\n");
	printf("\t-o <seed output file>\n");
	printf("\t\tspecify the path to the output file containing selected seeds (default: seeds.out)\n");
	printf("\t-k <number of seeds>\n");
	printf("\t\tnumber of selected seed nodes\n");
	printf("\t-e <epsilon>\n");
	printf("\t\tepsilon value in (epsilon)-approximation (see our paper for more details, default: 0.1)\n");
}

double log2(int n)
{
	return log(n) / log(2);
}

double logcnk(int n, int k)
{
	double ans = 0;
	for (int i = n - k + 1; i <= n; i++)
	{
		ans += log(i);
	}
	for (int i = 1; i <= k; i++)
	{
		ans -= log(i);
	}
	return ans;
}

int main(int argc, char **argv)
{
	std::random_device rd;
	std::mt19937_64 mt(rd());
	std::uniform_real_distribution<double> dist(0, 1);
	try
	{
		OptionParser op(argc, argv);
		if (!op.validCheck() || argc <= 1)
		{
			printInstruction();
			return -1;
		}
		char *inFile = op.getPara("-i");
		if (inFile == NULL)
		{
			inFile = (char *)"network.bin";
		}

		char *seedFile = op.getPara("-o");
		if (seedFile == NULL)
		{
			seedFile = (char *)"seeds.out";
		}

		char *model;
		model = (char *) "IC";

		Graph g;
		if (strcmp(model, "IC") == 0)
		{
			g.readGraphIC(inFile);
		}
		else
		{
			printf("Incorrect model option!");
			return -1;
		}

		unsigned int n = g.getSize();

		char *tmp = op.getPara("-e");
		float precision = 0.1;
		if (tmp != NULL)
		{
			precision = atof(tmp);
		}

		float delta = 0.1;
		float kl = 1;

		tmp = op.getPara("-d");
		if (tmp != NULL)
		{
			delta = atof(tmp);
		}

		tmp = op.getPara("-k");
		if (tmp != NULL)
		{
			kl = atof(tmp);
		}


		float alpha = 1;
		int t = 1;  //the threads of openmp

		HyperGraph hg(n);

		clock_t start = clock();
		vector<int> seeds;
		long long curSamples = 0;
		long long curTotalDegree = 0;
		long long totalSamples = 0;

		double esp = precision;
		double epsilon_prime = esp * sqrt(2);
		for (int x = 1;; x++)
		{
			curSamples = (2.0 + (2.0 / 3.0) * epsilon_prime) * (log(n) + logcnk(n, kl) + log(log2(n))) *
						 pow(2.0, x) / (epsilon_prime * epsilon_prime);
			seeds.clear();
			if (strcmp(model, "IC") == 0)
			{
				PollingIC p;
				totalSamples += addHyperedge<PollingIC>(g, hg, t, curSamples - totalSamples, mt, dist, p);
			}
			double ept = 0;
			buildSeedSet(g, hg, seeds, n, kl, ept);
			if (ept > 1 / pow(2.0, x))
			{
				double OPT_prime = ept * n / (1 + epsilon_prime);
				seeds.clear();
				double e = exp(1);
				double alpha = sqrt(log(n) + log(2));
				double beta = sqrt((1 - 1 / e) * (logcnk(n, kl) + log(n) + log(2)));
				int R = 2.0 * n * pow((1 - 1 / e) * alpha + beta, 2) / (OPT_prime * esp * esp);
				if (totalSamples < R)
				{
					int need = (int)(R - totalSamples);
					PollingIC p;
					totalSamples += addHyperedge<PollingIC>(g, hg, t, need, mt, dist, p);
					seeds.clear();
					buildSeedSet(g, hg, seeds, n, kl, ept);
				}
				break;
			}
		}

		cout << "Time: " << (float)(clock() - start) / CLOCKS_PER_SEC << "s,";



		FILE *mfile = fopen("/proc/self/status", "r");
		char line[128];
		uint32_t virtualMem = 0;
		uint32_t physicalMem = 0;

		while (fgets(line, 128, mfile) != NULL)
		{
			if (strncmp(line, "VmSize:", 7) == 0)
			{
				virtualMem = parseLine(line);
				break;
			}
			if (strncmp(line, "VmRSS:", 6) == 0)
			{
				physicalMem = parseLine(line);
				break;
			}
		}
		fclose(mfile);
		cout<<" Memory: " << virtualMem / 1024 << " MB" << endl;
		
		sort(seeds.begin(),seeds.end());
		ofstream out(seedFile);
		cout << "First seed set: ";
		
		for (unsigned int i = 0; i < kl; ++i)
		{
			cout << seeds[i] << " ";
			out << seeds[i] << " ";
		}
		cout << endl;
		out.close();
	}
	catch (int n)
	{
		printf("Some error(s) has occurred with syntax or parameters. Please consult the instruction file!");
		printInstruction();
	}
}
