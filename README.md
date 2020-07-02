# CoperSim
The FPGA emulator for Copernicus: Characterizing the Performance Implications of Compression Formats Used in Sparse Workloads

To Compile:
g++ -o CoperSim CoperSim_host.cpp CoperSim.cpp

To run:

0. DENSE
./CoperSim dense <latency_file> \<dense_matrix>

1. LIL
./CoperSim lil <latency_file> \<indices> \<values>

2. CSR
./CoperSim csr <latency_file> \<offsets> \<indices> \<values>

3. BCSR
./CoperSim bcsr <latency_file> \<offsets> \<indices> \<values>

4. COO
./CoperSim coo <latency_file> \<tuples>

5. CSC
./CoperSim csc <latency_file> \<offsets> \<indices> \<values>

6. DIA
./CoperSim dia <latency_file> \<diagonals>

7. ELL
./CoperSim ell <latency_file> \<indices> \<values>


Some example matrices in seven formats:

>>>> DENSE:

exp1.
1 1 0 1 0 0 1 0
1 1 1 1 0 1 1 0
0 1 1 0 1 1 0 0
1 1 0 1 0 0 1 0
0 0 1 0 1 0 0 1
0 1 1 0 0 1 0 0
1 1 0 1 0 0 1 1
0 0 0 0 1 0 1 1

exp2.
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 1 0 0 0
0 0 0 0 0 0 0 1
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 0
0 0 0 0 0 0 0 1

>>>>>> LIL:

indices

exp1.
1 1 2 1 3 2 1 5
2 2 3 2 5 3 2 7
4 3 5 4 8 6 4 8
7 4 6 7 0 0 7 0
0 6 0 0 0 0 8 0
0 7 0 0 0 0 0 0

exp2.
0 0 0 0 4 0 0 5
0 0 0 0 0 0 0 8

values

exp1.
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 1 1 1 1
1 1 1 1 0 0 1 0
0 1 0 0 0 0 1 0
0 1 0 0 0 0 0 0

exp2.
0 0 0 0 1 0 0 1
0 0 0 0 0 0 0 1

>>>>>> ELL(WIDTH=6):

indices

exp1.
1 2 4 7 0 0
1 2 3 4 6 7
2 3 5 6 0 0
1 2 4 7 0 0
3 5 8 0 0 0
2 3 6 0 0 0
1 2 4 7 8 0
5 7 8 0 0 0

exp2.
0 0 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
5 0 0 0 0 0
8 0 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
8 0 0 0 0 0

values

exp1.
1 1 1 1 0 0 0 0
1 1 1 1 1 1 0 0
1 1 1 1 0 0 0 0
1 1 1 1 0 0 0 0
1 1 1 0 0 0 0 0
1 1 1 0 0 0 0 0
1 1 1 1 1 0 0 0
1 1 1 0 0 0 0 0

exp2.
0 0 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
1 0 0 0 0 0
1 0 0 0 0 0
0 0 0 0 0 0
0 0 0 0 0 0
1 0 0 0 0 0

>>>>>>> DIA:

exp1.
-6 1 0 100 100 100 100 100 100
-5 0 1 0 100 100 100 100 100
-4 0 1 0 0 100 100 100 100
-3 1 0 1 1 1 100 100 100
-2 0 1 1 0 0 0 100 100
-1 1 1 0 0 0 0 1 100
0 1 1 1 1 1 1 1 1
1 1 1 0 0 0 0 1 100
2 0 1 1 0 0 0 100 100
3 1 0 1 1 1 100 100 100
4 0 1 0 0 100 100 100 100
5 0 1 0 100 100 100 100 100
6 1 0 100 100 100 100 100 100

exp2.
0 0 0 0 0 0 0 0 1
1 0 0 0 1 0 0 0 100
3 0 0 0 0 1 100 100 100


>>>>>>>> CSR:

offsets

exp1. 4 10 14 18 21 24 29 32

exp2. 0 0 0 1 2 2 2 3

column indices

exp1. 1 2 4 7 1 2 3 4 6 7 2 3 5 6 1 2 4 7 3 5 8 2 3 6 1 2 4 7 8 5 7 8

exp2. 5 8 8

values

exp1. 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1

exp2. 1 1 1

>>>>>>> BCSR:

offsets

exp1. 2 4

exp2. 1 2

column indices

exp1. 0 4 0 4

exp2. 4 4

values

exp1. 1 1 0 1 1 1 1 1 0 1 1 0 1 1 0 1 0 0 1 0 0 1 1 0 1 1 0 1 0 0 0 0 0 0 1 0 0 1 1 0 1 1 0 0 0 0 1 0 1 0 0 1 0 1 0 0 0 0 1 1 1 0 1 1

exp2. 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 1

>>>>>>>> CSC:

offsets

exp1. 4 10 14 18 21 24 29 32

exp2. 0 0 0 0 1 1 1 3

row indices

exp1. 1 2 4 7 1 2 3 4 6 7 2 3 5 6 1 2 4 7 3 5 8 2 3 6 1 2 4 7 8 5 7 8

exp2. 4 5 8

values

exp1. 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1

exp2. 1 1 1
