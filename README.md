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
