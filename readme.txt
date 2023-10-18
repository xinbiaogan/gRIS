Compile:
--------------------------------------------------------
Use `make' command to compile everything


How to use:
--------------------------------------------------------

1. Conversion from a text format to binary file
	./el2bin <input file> <output file>
    <input file>: the path to text file in edge list format: the first line contains the number of nodes n and number of edges m, each of the next m lines describes an edge following the format: <src> <dest> <weight>. Node index starts from 1.
    <output file>: the path to binary output file

2. Run Hyper to find the seed sets
	./Hyper [Options]

    Options:

        -i <binary graph file>
            specify the path to the binary graph file (default: network.bin)

        -o <seed output file>
            specify the path to the output file containing selected seeds (default: seeds.out)

        -k <number of seeds>
            number of selected seed nodes(default: 1)


        -e <epsilon>
            epsilon value in (epsilon)-approximation (see our paper for more details, default: 0.1)
	


Examples: find seed sets with number of seeds ranging from 1 to n on the graph network.txt:
The sample network network.txt in this case contains only 4 nodes and 4 edges and is formated as follows:
		4 4
		1 2 0.3
		2 3 0.5
		1 3 0.6
		1 4 0.2
	1. Convert to binary file:
		./el2bin network.txt network.bin
	
	2. Run Hyper with k=1, epsilon=0.1，deafult dataset is soc-Epinions1:
		./Hyper -i network.bin -k 1 -e 0.1 