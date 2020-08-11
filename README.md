# CoperSim
The FPGA emulator for Copernicus: Characterizing the Performance Implications of Compression Formats Used in Sparse Workloads

### To Compile:

`g++ -o CoperSim CoperSim_host.cpp CoperSim.cpp`

## To run:

### 0. DENSE

`./CoperSim dense <latency_file> \<dense_matrix>`

### 1. LIL

`./CoperSim lil <latency_file> \<indices> \<values>`

### 2. CSR

`./CoperSim csr <latency_file> \<offsets> \<indices> \<values>`

### 3. BCSR

`./CoperSim bcsr <latency_file> \<offsets> \<indices> \<values>`

### 4. COO

`./CoperSim coo <latency_file> \<tuples>`

### 5. CSC

`./CoperSim csc <latency_file> \<offsets> \<indices> \<values>`

### 6. DIA

`./CoperSim dia <latency_file> \<diagonals>`

### 7. ELL

`./CoperSim ell <latency_file> \<indices> \<values>`


## Some example matrices in seven formats:

## DENSE:

#### example 1:

1 1 0 1 0 0 1 0

1 1 1 1 0 1 1 0

0 1 1 0 1 1 0 0

1 1 0 1 0 0 1 0

0 0 1 0 1 0 0 1

0 1 1 0 0 1 0 0

1 1 0 1 0 0 1 1

0 0 0 0 1 0 1 1

#### example 2:

0 0 0 0 0 0 0 0

0 0 0 0 0 0 0 0

0 0 0 0 0 0 0 0

0 0 0 0 1 0 0 0

0 0 0 0 0 0 0 1

0 0 0 0 0 0 0 0

0 0 0 0 0 0 0 0

0 0 0 0 0 0 0 1

## LIL:

### indices:

#### expample 1:

1 1 2 1 3 2 1 5

2 2 3 2 5 3 2 7

4 3 5 4 8 6 4 8

7 4 6 7 0 0 7 0

0 6 0 0 0 0 8 0

0 7 0 0 0 0 0 0

#### expample 2:

0 0 0 0 4 0 0 5

0 0 0 0 0 0 0 8

### values:

#### example 1:

1 1 1 1 1 1 1 1

1 1 1 1 1 1 1 1

1 1 1 1 1 1 1 1

1 1 1 1 0 0 1 0

0 1 0 0 0 0 1 0

0 1 0 0 0 0 0 0

#### example 2:

0 0 0 0 1 0 0 1

0 0 0 0 0 0 0 1

## ELL(WIDTH=6):

### indices:

#### example 1:

1 2 4 7 0 0

1 2 3 4 6 7

2 3 5 6 0 0

1 2 4 7 0 0

3 5 8 0 0 0

2 3 6 0 0 0

1 2 4 7 8 0

5 7 8 0 0 0

#### example 2:

0 0 0 0 0 0

0 0 0 0 0 0

0 0 0 0 0 0

5 0 0 0 0 0

8 0 0 0 0 0

0 0 0 0 0 0

0 0 0 0 0 0

8 0 0 0 0 0

### values:

#### example 1:

1 1 1 1 0 0 0 0

1 1 1 1 1 1 0 0

1 1 1 1 0 0 0 0

1 1 1 1 0 0 0 0

1 1 1 0 0 0 0 0

1 1 1 0 0 0 0 0

1 1 1 1 1 0 0 0

1 1 1 0 0 0 0 0

#### example 2:

0 0 0 0 0 0

0 0 0 0 0 0

0 0 0 0 0 0

1 0 0 0 0 0

1 0 0 0 0 0

0 0 0 0 0 0

0 0 0 0 0 0

1 0 0 0 0 0

## DIA:

#### example 1:

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

#### example 2:

0 0 0 0 0 0 0 0 1

1 0 0 0 1 0 0 0 100

3 0 0 0 0 1 100 100 100


## CSR:

### offsets:

#### example 1: 
4 10 14 18 21 24 29 32

#### example 2: 
0 0 0 1 2 2 2 3

### column indices:

#### example 1: 
1 2 4 7 1 2 3 4 6 7 2 3 5 6 1 2 4 7 3 5 8 2 3 6 1 2 4 7 8 5 7 8

#### example 2: 
5 8 8

### values:

#### example 1: 
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1

#### example 2: 
1 1 1

## BCSR:

### offsets:

#### example 1: 
2 4

#### example 2: 
1 2

### column indices:

#### example 1: 
0 4 0 4

#### example 2: 
4 4

### values:

#### example 1: 
1 1 0 1 1 1 1 1 0 1 1 0 1 1 0 1 0 0 1 0 0 1 1 0 1 1 0 1 0 0 0 0 0 0 1 0 0 1 1 0 1 1 0 0 0 0 1 0 1 0 0 1 0 1 0 0 0 0 1 1 1 0 1 1

#### example 2: 
0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 1

## CSC:

### offsets:

#### example 1: 
4 10 14 18 21 24 29 32

#### example 1: 
0 0 0 0 1 1 1 3

### row indices:

#### example 1: 
1 2 4 7 1 2 3 4 6 7 2 3 5 6 1 2 4 7 3 5 8 2 3 6 1 2 4 7 8 5 7 8

#### example 2: 
4 5 8

### values:

#### example 1: 
1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1

#### example 2: 
1 1 1
