

#include "CoperSim.h"
#include <stdio.h>
#include <fstream>
#include <sstream>
#include <iostream>
#include <string>
using namespace std;

int main(int argc, char** argv)
{

int B[WIDTH];

for (int i = 0; i<WIDTH; i++)
{
	B[i] = 1;
}

int compute_latency = 0;
int memory_latency = 0;
float mem_comp_ratio = 0;
int transferred = 0;
int useful_transferred = 0;
float bw_util = 0;
int total_blocks = 0;
int T_dot_total = 0;
int T_mem_total = 0;
int T_BRAM_total = 0;
int T_decoprs_total = 0;
int T_compute_total = 0;
int total_latency = 0;
float total_mem_comp_ratio = 0;
float avg_mem_comp_ratio = 0;
int total_transferred = 0;
int total_useful_transferred = 0;
float total_bw_util = 0;
float avg_bw_util = 0;


int latency [LATENCY_PARAM];
int out_latency [LATENCY_PARAM];

for (int i = 0; i<LATENCY_PARAM; i++){
	 out_latency[i] = 0;
 }

// Inject latency (added for simulation):
string temp;
string word;
int inx = 0;
ifstream conf_file (argv[2]);
if (!conf_file) {
	printf("Missing latency file. Quitting.\n");
	return 1;
}
if (conf_file.is_open())
{
	while ( getline (conf_file,temp) && inx < LATENCY_PARAM)
	{
		istringstream iss(temp);
		iss >> word;
		cout << word << "\t";
		iss >> word;
		latency[inx] = std::stoi(word);
		printf("latency[%d]: %d\n", inx, latency[inx]);
		inx++;
	}
	conf_file.close();
}

// print out the header of the table
printf("Block\t T_dot\t T_mem\t T_BRAM\t T_decoprs\t T_compute\t T_max\t mem/comp\t Data\t Usf_data\t BW_util\n");

//---------------------------------
//---------| 0. DENSE |------------
//---------------------------------

if (argv[1] == std::string("dense"))
{
	int A[LENGTH][WIDTH];
	int out[LENGTH];

	// read inputs
	int input_dense = 0;
	string dense_line;

	int ii = 0;
	int line = 0;

	ifstream dense_file;
	dense_file.open(argv[3]);

	if (!dense_file) {
		printf("Missing dense file. Quitting.\n");
		return 1;
	}

	while(getline(dense_file, dense_line))
	{
		printf("%d \t", line); // block

		istringstream ss(dense_line);
		while(ss >> input_dense)
		{
			A[ii / WIDTH][ii % WIDTH] = input_dense;
			ii ++;
			transferred ++;
			if (input_dense != 0){
				useful_transferred ++;
			}
		}
		ii = 0;
		// printf("Reading DENSE file is done... \n");
		total_blocks = line;
		line ++;


		// Call the DENSE HWA:
		top(A, B, out, out_latency, latency);

		for (int i = 0; i<LATENCY_PARAM; i++){
			 printf("%d \t", out_latency[i]); // respectively: T_dot, T_mem, T_BRAM, T_decoprs

 			 if (i == 1){ // memory
 				 memory_latency = memory_latency + out_latency[i];
				 T_mem_total = T_mem_total + memory_latency;
 			 } else { // compute
 				 compute_latency = compute_latency + out_latency[i];
				 if (i == 0){
					 T_dot_total = T_dot_total + out_latency[i];
				 }
				 else if (i == 2){
					 T_BRAM_total = T_BRAM_total + out_latency[i];
				 }
				 else if (i == 3){
					 T_decoprs_total = T_decoprs_total + out_latency[i];
				 }
 			 }
 			 out_latency[i] = 0;
 		 }
		printf("%d \t", compute_latency);  // T_compute
 		T_compute_total = T_compute_total + compute_latency;
 		printf("%d \t", max(compute_latency,memory_latency)); // T_max
 		mem_comp_ratio = (float) memory_latency / compute_latency;
 		total_mem_comp_ratio = total_mem_comp_ratio + mem_comp_ratio;
 		printf("%1.2f \t", mem_comp_ratio); // mem/comp
 		printf("%d \t", transferred); // data
 		printf("%d \t", useful_transferred); // usf_data
 		total_transferred = total_transferred + transferred;
 		total_useful_transferred = total_useful_transferred + useful_transferred;
 		bw_util = (float) useful_transferred / transferred;
 		total_bw_util = total_bw_util + bw_util;
 		printf("%1.2f \t", bw_util); // BW_util
 		printf("\n");

 		 total_latency = total_latency + max(compute_latency,memory_latency);
 		 memory_latency = 0;
 		 compute_latency = 0;
		 transferred = 0;
		 useful_transferred = 0;
		 bw_util = 0;
	}
	dense_file.close();

}

//---------------------------------
//-----------| 1. LIL |------------
//---------------------------------

else if (argv[1] == std::string("lil"))
{
	int LIL_indices[LIL_LENGTH][WIDTH];
	int LIL_values[LIL_LENGTH][WIDTH];
	int LIL_out[LIL_LENGTH];


	// read inputs
	int input_inx = 0;
	int input_val = 0;

	string indices_line;
	string values_line;

	int ii = 0;
	int line = 0;

	ifstream indices_file;
	indices_file.open(argv[3]);

	ifstream values_file;
	values_file.open(argv[4]);


	if (!indices_file) {
		printf("Missing indices file. Quitting. \n");
		return 1;
	}

	if (!values_file) {
		printf("Missing values file.  Quitting. \n");
		return 1;
	}


	while(getline(indices_file, indices_line))
	{
		// initialization to prevent large padded input files
		for (int i = 0; i<LIL_LENGTH; i++){
			for (int j = 0; j<WIDTH; j++){
				LIL_indices[i][j] = 100;
				LIL_values[i][j] = 100;
			}
			LIL_out [i] = 0;
		}

		printf("%d \t", line); //block

		istringstream ss(indices_line);
		while(ss >> input_inx)
		{
			if (input_inx == 0){
				LIL_indices[ii / WIDTH][ii % WIDTH] = 100;
			}
			else{
				LIL_indices[ii / WIDTH][ii % WIDTH] = input_inx;
				transferred ++;
			}
			ii ++;
		}
		ii = 0;
		// printf("Reading LIL-indices file is done... ");

		getline(values_file, values_line);
		istringstream ss2(values_line);
		while(ss2 >> input_val)
		{
			LIL_values[ii / WIDTH][ii % WIDTH] = input_val;
			ii ++;
			transferred ++;
			if (input_val != 0){
				useful_transferred ++;
			}
		}
		ii = 0;
		// printf("Reading LIL-values file is done... \n");
		total_blocks = line;
		line ++;


   // Call the LIL HWA:
	 LilTop(LIL_indices, LIL_values, B, LIL_out, out_latency, latency);

	 for (int i = 0; i<LATENCY_PARAM; i++){
			printf("%d \t", out_latency[i]); // respectively: T_dot, T_mem, T_BRAM, T_decoprs

			if (i == 1){ // memory
				memory_latency = memory_latency + out_latency[i];
				T_mem_total = T_mem_total + memory_latency;
			} else { // compute
				compute_latency = compute_latency + out_latency[i];
				if (i == 0){
					T_dot_total = T_dot_total + out_latency[i];
				}
				else if (i == 2){
					T_BRAM_total = T_BRAM_total + out_latency[i];
				}
				else if (i == 3){
					T_decoprs_total = T_decoprs_total + out_latency[i];
				}
			}
			out_latency[i] = 0;
		}
		printf("%d \t", compute_latency);  // T_compute
		T_compute_total = T_compute_total + compute_latency;
		printf("%d \t", max(compute_latency,memory_latency)); // T_max
		mem_comp_ratio = (float) memory_latency / compute_latency;
		total_mem_comp_ratio = total_mem_comp_ratio + mem_comp_ratio;
		printf("%1.2f \t", mem_comp_ratio); // mem/comp
		printf("%d \t", transferred); // data
		printf("%d \t", useful_transferred); // usf_data
		total_transferred = total_transferred + transferred;
		total_useful_transferred = total_useful_transferred + useful_transferred;
		bw_util = (float) useful_transferred / transferred;
		total_bw_util = total_bw_util + bw_util;
		printf("%1.2f \t", bw_util); // BW_util
		printf("\n");

		total_latency = total_latency + max(compute_latency,memory_latency);
		memory_latency = 0;
		compute_latency = 0;
		transferred = 0;
		useful_transferred = 0;
		bw_util = 0;


	}
	indices_file.close();
	values_file.close();

}

//---------------------------------
//-----------| 2. CSR |------------
//---------------------------------

else if (argv[1] == std::string("csr"))
{

	int CSR_offsets[CSR_OFFSETS_LENGTH];
	int CSR_col_indices[CSR_COL_INX_LENGTH];
	int CSR_values[CSR_VAL_LENGTH];
	int CSR_out[LENGTH];

	// read inputs

	int input_offset = 0;
	int input_col_inx = 0;
	int input_val = 0;

	string offsets_line;
	string col_indices_line;
	string values_line;

	int ii = 0;
	int line = 0;

	ifstream offsets_file;
	offsets_file.open(argv[3]);

	ifstream col_indices_file;
	col_indices_file.open(argv[4]);

	ifstream values_file;
	values_file.open(argv[5]);

	if (!offsets_file) {
		printf("Missing offsets file. Quitting.\n");
		return 1;
	}

	if (!col_indices_file) {
		printf("Missing col indices file. Quitting. \n");
		return 1;
	}

	if (!values_file) {
		printf("Missing values file.  Quitting. \n");
		return 1;
	}


	while(getline(offsets_file, offsets_line))
	{
		// initialization to prevent large padded input files
		for (int i = 0; i<CSR_OFFSETS_LENGTH; i++){
			CSR_offsets[i] = 100;
		}
		for (int i = 0; i<CSR_COL_INX_LENGTH; i++){
			CSR_col_indices[i] = 100;
		}
		for (int i = 0; i<CSR_VAL_LENGTH; i++){
			CSR_values[i] = 100;
		}

		printf("%d \t", line); // block

		istringstream ss(offsets_line);
		while(ss >> input_offset)
		{
			CSR_offsets[ii] = input_offset;
			ii ++;
			transferred ++;
		}
		ii = 0;
		// printf("Reading CSR-offsets file is done... ");

		getline(col_indices_file, col_indices_line);
		istringstream ss1(col_indices_line);
		while(ss1 >> input_col_inx)
		{
			CSR_col_indices[ii] = input_col_inx;
			ii ++;
			transferred ++;
		}
		ii = 0;
		// printf("Reading CSR-coloumn-indices file is done... ");

		getline(values_file, values_line);
		istringstream ss2(values_line);
		while(ss2 >> input_val)
		{
			CSR_values[ii] = input_val;
			ii ++;
			transferred ++;
			if (input_val != 0){
				useful_transferred ++;
			}
		}
		ii = 0;
		// printf("Reading CSR-values is done... \n");
		total_blocks = line;
		line ++;



	 // Call the CSR HWA:
	 CsrTop(CSR_offsets, CSR_col_indices, CSR_values, B, CSR_out, out_latency, latency);


	 for (int i = 0; i<LATENCY_PARAM; i++){
			printf("%d \t", out_latency[i]); // respectively: T_dot, T_mem, T_BRAM, T_decoprs

			if (i == 1){ // memory
				memory_latency = memory_latency + out_latency[i];
				T_mem_total = T_mem_total + memory_latency;
			} else { // compute
				compute_latency = compute_latency + out_latency[i];
				if (i == 0){
					T_dot_total = T_dot_total + out_latency[i];
				}
				else if (i == 2){
					T_BRAM_total = T_BRAM_total + out_latency[i];
				}
				else if (i == 3){
					T_decoprs_total = T_decoprs_total + out_latency[i];
				}
			}
			out_latency[i] = 0;
		}
		printf("%d \t", compute_latency);  // T_compute
		T_compute_total = T_compute_total + compute_latency;
		printf("%d \t", max(compute_latency,memory_latency)); // T_max
		mem_comp_ratio = (float) memory_latency / compute_latency;
		total_mem_comp_ratio = total_mem_comp_ratio + mem_comp_ratio;
		printf("%1.2f \t", mem_comp_ratio); // mem/comp
		printf("%d \t", transferred); // data
		printf("%d \t", useful_transferred); // usf_data
		total_transferred = total_transferred + transferred;
		total_useful_transferred = total_useful_transferred + useful_transferred;
		bw_util = (float) useful_transferred / transferred;
		total_bw_util = total_bw_util + bw_util;
		printf("%1.2f \t", bw_util); // BW_util
		printf("\n");

		total_latency = total_latency + max(compute_latency,memory_latency);
		memory_latency = 0;
		compute_latency = 0;
		transferred = 0;
		useful_transferred = 0;
		bw_util = 0;

	}
	offsets_file.close();
	col_indices_file.close();
	values_file.close();

}

//---------------------------------
//----------| 3. BCSR |------------
//---------------------------------

else if (argv[1] == std::string("bcsr"))
{
	int BCSR_offsets[BCSR_OFFSETS_LENGTH];
	int BCSR_col_indices[BCSR_COL_INX_LENGTH];
  int BCSR_values[BCSR_VAL_LENGTH][BCSR_VAL_WIDTH];
	int BCSR_out[LENGTH];

	// read inputs

	int input_offset = 0;
	int input_col_inx = 0;
	int input_val = 0;

	string offsets_line;
	string col_indices_line;
	string values_line;

	int ii = 0;
	int line = 0;

	ifstream offsets_file;
	offsets_file.open(argv[3]);

	ifstream col_indices_file;
	col_indices_file.open(argv[4]);

	ifstream values_file;
	values_file.open(argv[5]);

	if (!offsets_file) {
		printf("Missing offsets file. Quitting.\n");
		return 1;
	}

	if (!col_indices_file) {
		printf("Missing col indices file. Quitting. \n");
		return 1;
	}

	if (!values_file) {
		printf("Missing values file.  Quitting. \n");
		return 1;
	}


	while(getline(offsets_file, offsets_line))
	{
		// initialization to prevent large padded input files
		for (int i = 0; i<BCSR_OFFSETS_LENGTH; i++){
			BCSR_offsets[i] = 100;
		}
		for (int i = 0; i<BCSR_COL_INX_LENGTH; i++){
			BCSR_col_indices[i] = 100;
		}
		for (int i = 0; i<BCSR_VAL_LENGTH; i++){
			for (int j = 0; j<BCSR_VAL_WIDTH; j++){
				BCSR_values[i][j] = 100;
			}
		}

		printf("%d \t", line); // block

		istringstream ss(offsets_line);
		while(ss >> input_offset)
		{
			if (ii < BCSR_OFFSETS_LENGTH)
			{
				BCSR_offsets[ii] = input_offset;
				ii ++;
				transferred ++;
			}
		}
		ii = 0;
		// printf("Reading BCSR-offsets file is done... ");

		getline(col_indices_file, col_indices_line);
		istringstream ss1(col_indices_line);
		while(ss1 >> input_col_inx)
		{
			BCSR_col_indices[ii] = input_col_inx;
			ii ++;
			transferred ++;
		}
		ii = 0;
		// printf("Reading BCSR-coloumn-indices file is done... ");

		getline(values_file, values_line);
		istringstream ss2(values_line);
		while(ss2 >> input_val)
		{
			BCSR_values[ii / BCSR_VAL_WIDTH][ii % BCSR_VAL_WIDTH] = input_val;
			ii ++;
			transferred ++;
			if (input_val != 0){
				useful_transferred ++;
			}
		}
		ii = 0;
		// printf("Reading BCSR-values is done... \n");
		total_blocks = line;
		line ++;


	  // Call the BCSR HWA:
	  BcsrTop(BCSR_offsets, BCSR_col_indices, BCSR_values, B, BCSR_out, out_latency, latency);


		for (int i = 0; i<LATENCY_PARAM; i++){
 			printf("%d \t", out_latency[i]); // respectively: T_dot, T_mem, T_BRAM, T_decoprs

 			if (i == 1){ // memory
 				memory_latency = memory_latency + out_latency[i];
				T_mem_total = T_mem_total + memory_latency;
 			} else { // compute
 				compute_latency = compute_latency + out_latency[i];
 				if (i == 0){
 					T_dot_total = T_dot_total + out_latency[i];
 				}
 				else if (i == 2){
 					T_BRAM_total = T_BRAM_total + out_latency[i];
 				}
 				else if (i == 3){
 					T_decoprs_total = T_decoprs_total + out_latency[i];
 				}
 			}
 			out_latency[i] = 0;
 		}
		printf("%d \t", compute_latency);  // T_compute
		T_compute_total = T_compute_total + compute_latency;
		printf("%d \t", max(compute_latency,memory_latency)); // T_max
		mem_comp_ratio = (float) memory_latency / compute_latency;
		total_mem_comp_ratio = total_mem_comp_ratio + mem_comp_ratio;
		printf("%1.2f \t", mem_comp_ratio); // mem/comp
		printf("%d \t", transferred); // data
		printf("%d \t", useful_transferred); // usf_data
		total_transferred = total_transferred + transferred;
		total_useful_transferred = total_useful_transferred + useful_transferred;
		bw_util = (float) useful_transferred / transferred;
		total_bw_util = total_bw_util + bw_util;
		printf("%1.2f \t", bw_util); // BW_util
		printf("\n");

 		total_latency = total_latency + max(compute_latency,memory_latency);
 		memory_latency = 0;
 		compute_latency = 0;
 		transferred = 0;
 		useful_transferred = 0;
 		bw_util = 0;

	}
	offsets_file.close();
	col_indices_file.close();
	values_file.close();

}

//-----------------------------------
//-----------| 4. COO |--------------
//-----------------------------------

else if (argv[1] == std::string("coo"))
{
	int COO_tuples[COO_NUM_TUPLES][3];
	int COO_out[LENGTH];

	// read inputs
	int input_coo = 0;
	string coo_line;

	int ii = 0;
	int line = 0;

	ifstream coo_file;
	coo_file.open(argv[3]);

	if (!coo_file) {
		printf("Missing coo tuples file. Quitting.\n");
		return 1;
	}

	while(getline(coo_file, coo_line))
	{
		// initialization to prevent large padded input files
		for (int i = 0; i<COO_NUM_TUPLES; i++){
			for (int j = 0; j<3; j++){
				COO_tuples[i][j] = 100;
			}
		}

		printf("%d \t", line); //block

		istringstream ss(coo_line);
		while(ss >> input_coo)
		{
			COO_tuples[ii / 3][ii % 3] = input_coo;
			ii ++;
			transferred ++;
			if (ii % 3 == 0){
				useful_transferred ++;
			}
		}
		ii = 0;
		// printf("Reading COO file is done... \n");
		total_blocks = line;
		line ++;


		// Call the COO HWA:
	  CooTop(COO_tuples, B, COO_out, out_latency, latency);

		for (int i = 0; i<LATENCY_PARAM; i++){
			 printf("%d \t", out_latency[i]); // respectively: T_dot, T_mem, T_BRAM, T_decoprs

 			 if (i == 1){ // memory
 				 memory_latency = memory_latency + out_latency[i];
				 T_mem_total = T_mem_total + memory_latency;
 			 } else { // compute
 				 compute_latency = compute_latency + out_latency[i];
				 if (i == 0){
					 T_dot_total = T_dot_total + out_latency[i];
				 }
				 else if (i == 2){
					 T_BRAM_total = T_BRAM_total + out_latency[i];
				 }
				 else if (i == 3){
					 T_decoprs_total = T_decoprs_total + out_latency[i];
				 }
 			 }
 			 out_latency[i] = 0;
 		 }
		  printf("%d \t", compute_latency);  // T_compute
	 		T_compute_total = T_compute_total + compute_latency;
	 		printf("%d \t", max(compute_latency,memory_latency)); // T_max
	 		mem_comp_ratio = (float) memory_latency / compute_latency;
	 		total_mem_comp_ratio = total_mem_comp_ratio + mem_comp_ratio;
	 		printf("%1.2f \t", mem_comp_ratio); // mem/comp
	 		printf("%d \t", transferred); // data
	 		printf("%d \t", useful_transferred); // usf_data
	 		total_transferred = total_transferred + transferred;
	 		total_useful_transferred = total_useful_transferred + useful_transferred;
	 		bw_util = (float) useful_transferred / transferred;
	 		total_bw_util = total_bw_util + bw_util;
	 		printf("%1.2f \t", bw_util); // BW_util
	 		printf("\n");

 		 total_latency = total_latency + max(compute_latency,memory_latency);
 		 memory_latency = 0;
 		 compute_latency = 0;
		 transferred = 0;
		 useful_transferred = 0;
		 bw_util = 0;
	}
	coo_file.close();

}

//-----------------------------------
//-----------| 5. CSC |--------------
//-----------------------------------

else if (argv[1] == std::string("csc"))
{
	int CSC_row_indices[CSC_ROW_INX_LENGTH];
	int CSC_offsets[CSC_OFFSETS_LENGTH];
	int CSC_values[CSC_VAL_LENGTH];
	int CSC_out[LENGTH];

	// read inputs

	int input_row_inx = 0;
	int input_offset = 0;
	int input_val = 0;

	string offsets_line;
	string row_indices_line;
	string values_line;

	int ii = 0;
	int line = 0;

	ifstream offsets_file;
	offsets_file.open(argv[3]);

	ifstream row_indices_file;
	row_indices_file.open(argv[4]);

	ifstream values_file;
	values_file.open(argv[5]);

	if (!offsets_file) {
		printf("Missing offsets file. Quitting.\n");
		return 1;
	}
	if (!row_indices_file) {
		printf("Missing row indices file. Quitting. \n");
		return 1;
	}
	if (!values_file) {
		printf("Missing values file.  Quitting. \n");
		return 1;
	}


	while(getline(offsets_file, offsets_line))
	{
		// initialization to prevent large padded input files
		for (int i = 0; i<CSC_OFFSETS_LENGTH; i++){
			CSC_offsets[i] = 100;
		}
		for (int i = 0; i<CSC_ROW_INX_LENGTH; i++){
			CSC_row_indices[i] = 100;
		}
		for (int i = 0; i<CSC_VAL_LENGTH; i++){
			CSC_values[i] = 100;
		}

		printf("%d \t", line);

		istringstream ss(offsets_line);
		while(ss >> input_offset)
		{
			CSC_offsets[ii] = input_offset;
			ii ++;
			transferred ++;
		}
		ii = 0;
		// printf("Reading CSC offsets file is done... ");

		getline(row_indices_file, row_indices_line);
		istringstream ss1(row_indices_line);
		while(ss1 >> input_row_inx)
		{
			CSC_row_indices[ii] = input_row_inx;
			ii ++;
			transferred ++;
		}
		ii = 0;
		// printf("Reading CSC-row-indices file is done... ");

		getline(values_file, values_line);
		istringstream ss2(values_line);
		while(ss2 >> input_val)
		{
			CSC_values[ii] = input_val;
			ii ++;
			transferred ++;
			if (input_val != 0){
				useful_transferred ++;
			}
		}
		ii = 0;
		// printf("Reading CSC-values file is done... \n");
		total_blocks = line;
		line ++;


		// Call the CSC HWA:
	  CscTop(CSC_row_indices, CSC_offsets, CSC_values, B, CSC_out, out_latency, latency);

		for (int i = 0; i<LATENCY_PARAM; i++){
			 printf("%d \t", out_latency[i]); // respectively: T_dot, T_mem, T_BRAM, T_decoprs

 			 if (i == 1){ // memory
 				 memory_latency = memory_latency + out_latency[i];
				 T_mem_total = T_mem_total + memory_latency;
 			 } else { // compute
 				 compute_latency = compute_latency + out_latency[i];
				 if (i == 0){
					 T_dot_total = T_dot_total + out_latency[i];
				 }
				 else if (i == 2){
					 T_BRAM_total = T_BRAM_total + out_latency[i];
				 }
				 else if (i == 3){
					 T_decoprs_total = T_decoprs_total + out_latency[i];
				 }
 			 }
 			 out_latency[i] = 0;
 		 }
		  printf("%d \t", compute_latency);  // T_compute
	 		T_compute_total = T_compute_total + compute_latency;
	 		printf("%d \t", max(compute_latency,memory_latency)); // T_max
	 		mem_comp_ratio = (float) memory_latency / compute_latency;
	 		total_mem_comp_ratio = total_mem_comp_ratio + mem_comp_ratio;
	 		printf("%1.2f \t", mem_comp_ratio); // mem/comp
	 		printf("%d \t", transferred); // data
	 		printf("%d \t", useful_transferred); // usf_data
	 		total_transferred = total_transferred + transferred;
	 		total_useful_transferred = total_useful_transferred + useful_transferred;
	 		bw_util = (float) useful_transferred / transferred;
	 		total_bw_util = total_bw_util + bw_util;
	 		printf("%1.2f \t", bw_util); // BW_util
	 		printf("\n");

 		 total_latency = total_latency + max(compute_latency,memory_latency);
 		 memory_latency = 0;
 		 compute_latency = 0;
		 transferred = 0;
		 useful_transferred = 0;
		 bw_util = 0;
	}
	offsets_file.close();
	row_indices_file.close();
	values_file.close();

}

//---------------------------------
//-----------| 6. DIA |------------
//---------------------------------

else if (argv[1] == std::string("dia"))
{
	int DIA_diagonals[NUM_DIAGONALS][MAX_DIAGONAL_LEN];
	int DIA_out[LENGTH];

	// read inputs
	int input_dia = 0;
	string dia_line;

	int ii = 0;
	int line = 0;

	ifstream dia_file;
	dia_file.open(argv[3]);

	if (!dia_file) {
		printf("Missing dia file. Quitting.\n");
		return 1;
	}

	while(getline(dia_file, dia_line))
	{
		// initialization to prevent large padded input files
		for (int i = 0; i<NUM_DIAGONALS; i++){
			for (int j = 0; j<MAX_DIAGONAL_LEN; j++){
				DIA_diagonals[i][j] = 100;
			}
		}

		printf("%d \t", line); // block

		istringstream ss(dia_line);
		while(ss >> input_dia)
		{
			DIA_diagonals[ii / MAX_DIAGONAL_LEN][ii % MAX_DIAGONAL_LEN] = input_dia;
			ii ++;
			transferred ++;
			if (input_dia != 100){
				if (input_dia > 0){
					useful_transferred ++;
				}
			}
		}
		ii = 0;
		// printf("Reading DIA file is done... \n");
		total_blocks = line;
		line ++;

		// Call the DIA HWA:
	  DiaTop(DIA_diagonals, B, DIA_out, out_latency, latency);

		for (int i = 0; i<LATENCY_PARAM; i++){
			 printf("%d \t", out_latency[i]); // respectively: T_dot, T_mem, T_BRAM, T_decoprs

 			 if (i == 1){ // memory
 				 memory_latency = memory_latency + out_latency[i];
				 T_mem_total = T_mem_total + memory_latency;
 			 } else { // compute
 				 compute_latency = compute_latency + out_latency[i];
				 if (i == 0){
					 T_dot_total = T_dot_total + out_latency[i];
				 }
				 else if (i == 2){
					 T_BRAM_total = T_BRAM_total + out_latency[i];
				 }
				 else if (i == 3){
					 T_decoprs_total = T_decoprs_total + out_latency[i];
				 }
 			 }
 			 out_latency[i] = 0;
 		 }
		  printf("%d \t", compute_latency);  // T_compute
	 		T_compute_total = T_compute_total + compute_latency;
	 		printf("%d \t", max(compute_latency,memory_latency)); // T_max
	 		mem_comp_ratio = (float) memory_latency / compute_latency;
	 		total_mem_comp_ratio = total_mem_comp_ratio + mem_comp_ratio;
	 		printf("%1.2f \t", mem_comp_ratio); // mem/comp
	 		printf("%d \t", transferred); // data
	 		printf("%d \t", useful_transferred); // usf_data
	 		total_transferred = total_transferred + transferred;
	 		total_useful_transferred = total_useful_transferred + useful_transferred;
	 		bw_util = (float) useful_transferred / transferred;
	 		total_bw_util = total_bw_util + bw_util;
	 		printf("%1.2f \t", bw_util); // BW_util
	 		printf("\n");

 		 total_latency = total_latency + max(compute_latency,memory_latency);
 		 memory_latency = 0;
 		 compute_latency = 0;
		 transferred = 0;
		 useful_transferred = 0;
		 bw_util = 0;
	}
	dia_file.close();
}

//---------------------------------
//-----------| 7. ELL |------------
//---------------------------------

else if (argv[1] == std::string("ell"))
{
	int ELL_values[LENGTH][ELL_MAX_COMP_ROW_LENGTH];
	int ELL_col_indices[LENGTH][ELL_MAX_COMP_ROW_LENGTH];
	int ELL_out[LENGTH];

	// read inputs
	int input_inx = 0;
	int input_val = 0;

	string indices_line;
	string values_line;

	int ii = 0;
	int line = 0;

	ifstream indices_file;
	indices_file.open(argv[3]);

	ifstream values_file;
	values_file.open(argv[4]);


	if (!indices_file) {
		printf("Missing indices file. Quitting. \n");
		return 1;
	}
	if (!values_file) {
		printf("Missing values file.  Quitting. \n");
		return 1;
	}

	while(getline(indices_file, indices_line))
	{
		// initialization to prevent large padded input files
		for (int i = 0; i<LENGTH; i++){
			for (int j = 0; j<ELL_MAX_COMP_ROW_LENGTH; j++){
				ELL_values[i][j] = 100;
				ELL_col_indices[i][j] = 100;
			}
		}

		printf("%d \t", line); //Block

		istringstream ss(indices_line);
		while(ss >> input_inx)
		{
			ELL_col_indices[ii / ELL_MAX_COMP_ROW_LENGTH][ii % ELL_MAX_COMP_ROW_LENGTH] = input_inx;
			ii ++;
			transferred ++;
		}
		ii = 0;
		// printf("Reading ELL-indices file is done... ");

		getline(values_file, values_line);
		istringstream ss2(values_line);
		while(ss2 >> input_val)
		{
			ELL_values[ii / ELL_MAX_COMP_ROW_LENGTH][ii % ELL_MAX_COMP_ROW_LENGTH] = input_val;
			ii ++;
			transferred ++;
			if (input_val != 0){
				useful_transferred ++;
			}
		}
		ii = 0;
		// printf("Reading ELL-values file is done... \n");
		total_blocks = line;
		line ++;


		// Call the ELL HWA:
	  EllTop(ELL_values, ELL_col_indices, B, ELL_out, out_latency, latency);

		for (int i = 0; i<LATENCY_PARAM; i++){
			 printf("%d \t", out_latency[i]); // respectively: T_dot, T_mem, T_BRAM, T_decoprs

 			 if (i == 1){ // memory
 				 memory_latency = memory_latency + out_latency[i];
				 T_mem_total = T_mem_total + memory_latency;
 			 } else { // compute
 				 compute_latency = compute_latency + out_latency[i];
				 if (i == 0){
					 T_dot_total = T_dot_total + out_latency[i];
				 }
				 else if (i == 2){
					 T_BRAM_total = T_BRAM_total + out_latency[i];
				 }
				 else if (i == 3){
					 T_decoprs_total = T_decoprs_total + out_latency[i];
				 }
 			 }
 			 out_latency[i] = 0;
 		 }
		  printf("%d \t", compute_latency);  // T_compute
	 		T_compute_total = T_compute_total + compute_latency;
	 		printf("%d \t", max(compute_latency,memory_latency)); // T_max
	 		mem_comp_ratio = (float) memory_latency / compute_latency;
	 		total_mem_comp_ratio = total_mem_comp_ratio + mem_comp_ratio;
	 		printf("%1.2f \t", mem_comp_ratio); // mem/comp
	 		printf("%d \t", transferred); // data
	 		printf("%d \t", useful_transferred); // usf_data
	 		total_transferred = total_transferred + transferred;
	 		total_useful_transferred = total_useful_transferred + useful_transferred;
	 		bw_util = (float) useful_transferred / transferred;
	 		total_bw_util = total_bw_util + bw_util;
	 		printf("%1.2f \t", bw_util); // BW_util
	 		printf("\n");

 		 total_latency = total_latency + max(compute_latency,memory_latency);
 		 memory_latency = 0;
 		 compute_latency = 0;
		 transferred = 0;
		 useful_transferred = 0;
		 bw_util = 0;
	}
	indices_file.close();
	values_file.close();
}

// =========== All Done!

avg_mem_comp_ratio = (float) total_mem_comp_ratio / total_blocks;
avg_bw_util = (float) total_bw_util / total_blocks;

printf("----------\n");
printf("Block\t T_dot\t T_mem\t T_BRAM\t T_decoprs\t T_compute\t total\t avg_mem/comp\t Data\t usf_data\t avg_BW_util\n");
printf("%d\t %d\t %d\t %d\t %d\t %d\t %d\t %1.2f\t %d\t %d\t %1.2f\n",
				total_blocks, T_dot_total, T_mem_total, T_BRAM_total, T_decoprs_total, T_compute_total,
				total_latency, avg_mem_comp_ratio, total_transferred, total_useful_transferred, avg_bw_util);

return 0;
}
