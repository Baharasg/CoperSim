

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
int total_latency = 0;
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
		printf("Block: %d \n", line);

		istringstream ss(dense_line);
		while(ss >> input_dense)
		{
			A[ii/WIDTH][ii%WIDTH] = input_dense;
			ii ++;
		}
		ii = 0;
		printf("Reading DENSE file is done... \n");
		line ++;

		// Call the DENSE HWA:
		top(A, B, out, out_latency, latency);

		for (int i = 0; i<LATENCY_PARAM; i++){
			 // printf("latency: %d \n", out_latency[i]);
			 if (i == 1){ // memory
				 memory_latency = memory_latency + out_latency[i];
			 } else { // compute
				 compute_latency = compute_latency + out_latency[i];
			 }
			 out_latency[i] = 0;
		 }
		 printf("latency: %d \n", max(compute_latency,memory_latency));
		 total_latency = total_latency + max(compute_latency,memory_latency);
		 memory_latency = 0;
		 compute_latency = 0;
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

		printf("Block: %d \n", line);

		istringstream ss(indices_line);
		while(ss >> input_inx)
		{
			if (input_inx == 0){
				LIL_indices[ii / WIDTH][ii % WIDTH] = 100;
			}
			else{
				LIL_indices[ii / WIDTH][ii % WIDTH] = input_inx;
			}
			ii ++;
		}
		ii = 0;
		printf("Reading LIL-indices file is done... ");

		getline(values_file, values_line);
		istringstream ss2(values_line);
		while(ss2 >> input_val)
		{
			LIL_values[ii / WIDTH][ii % WIDTH] = input_val;
			ii ++;
		}
		ii = 0;
		printf("Reading LIL-values file is done... \n");
		line ++;

   // Call the LIL HWA:
	 LilTop(LIL_indices, LIL_values, B, LIL_out, out_latency, latency);

	 for (int i = 0; i<LATENCY_PARAM; i++){
		 // printf("latency: %d \n", out_latency[i]);
		 if (i == 1){ // memory
			 printf("memory latency: %d \n", out_latency[i]);
			 memory_latency = memory_latency + out_latency[i];
		 } else { // compute
			 compute_latency = compute_latency + out_latency[i];
		 }
		 out_latency[i] = 0;
	 }
	 printf("latency: %d \n", max(compute_latency,memory_latency));
	 total_latency = total_latency + max(compute_latency,memory_latency);
	 memory_latency = 0;
	 compute_latency = 0;


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

		printf("Block: %d \n", line);

		istringstream ss(offsets_line);
		while(ss >> input_offset)
		{
			CSR_offsets[ii] = input_offset;
			ii ++;
		}
		ii = 0;
		printf("Reading CSR-offsets file is done... ");

		getline(col_indices_file, col_indices_line);
		istringstream ss1(col_indices_line);
		while(ss1 >> input_col_inx)
		{
			CSR_col_indices[ii] = input_col_inx;
			ii ++;
		}
		ii = 0;
		printf("Reading CSR-coloumn-indices file is done... ");

		getline(values_file, values_line);
		istringstream ss2(values_line);
		while(ss2 >> input_val)
		{
			CSR_values[ii] = input_val;
			ii ++;
		}
		ii = 0;
		printf("Reading CSR-values is done... \n");
		line ++;


	 // Call the CSR HWA:
	 CsrTop(CSR_offsets, CSR_col_indices, CSR_values, B, CSR_out, out_latency, latency);


	 for (int i = 0; i<LATENCY_PARAM; i++){
		 // printf("latency: %d \n", out_latency[i]);
		 if (i == 1){ // memory
			 printf("memory latency: %d \n", out_latency[i]);
			 memory_latency = memory_latency + out_latency[i];
		 } else { // compute
			 compute_latency = compute_latency + out_latency[i];
		 }
		 out_latency[i] = 0;
	 }
	 printf("latency: %d \n", max(compute_latency,memory_latency));
	 total_latency = total_latency + max(compute_latency,memory_latency);
	 memory_latency = 0;
	 compute_latency = 0;


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

		printf("Block: %d \n", line);

		istringstream ss(offsets_line);
		while(ss >> input_offset)
		{
			if (ii < BCSR_OFFSETS_LENGTH)
			{
				BCSR_offsets[ii] = input_offset;
				ii ++;
			}
		}
		ii = 0;
		printf("Reading BCSR-offsets file is done... ");

		getline(col_indices_file, col_indices_line);
		istringstream ss1(col_indices_line);
		while(ss1 >> input_col_inx)
		{
			BCSR_col_indices[ii] = input_col_inx;
			ii ++;
		}
		ii = 0;
		printf("Reading BCSR-coloumn-indices file is done... ");

		getline(values_file, values_line);
		istringstream ss2(values_line);
		while(ss2 >> input_val)
		{
			BCSR_values[ii / BCSR_VAL_WIDTH][ii % BCSR_VAL_WIDTH] = input_val;
			ii ++;
		}
		ii = 0;
		printf("Reading BCSR-values is done... \n");
		line ++;

	  // Call the BCSR HWA:
	  BcsrTop(BCSR_offsets, BCSR_col_indices, BCSR_values, B, BCSR_out, out_latency, latency);


	 for (int i = 0; i<LATENCY_PARAM; i++){
		 // printf("latency: %d \n", out_latency[i]);
		 if (i == 1){ // memory
			 printf("memory latency: %d \n", out_latency[i]);
			 memory_latency = memory_latency + out_latency[i];
		 } else { // compute
			 compute_latency = compute_latency + out_latency[i];
		 }
		 out_latency[i] = 0;
	 }
	 printf("latency: %d \n", max(compute_latency,memory_latency));
	 total_latency = total_latency + max(compute_latency,memory_latency);
	 memory_latency = 0;
	 compute_latency = 0;

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

		printf("Block: %d \n", line);

		istringstream ss(coo_line);
		while(ss >> input_coo)
		{
			COO_tuples[ii / 3][ii % 3] = input_coo;
			ii ++;
		}
		ii = 0;
		printf("Reading COO file is done... \n");
		line ++;

		// Call the COO HWA:
	  CooTop(COO_tuples, B, COO_out, out_latency, latency);

		for (int i = 0; i<LATENCY_PARAM; i++){
 			 // printf("latency: %d \n", out_latency[i]);
 			 if (i == 1){ // memory
				 printf("memory latency: %d \n", out_latency[i]);
 				 memory_latency = memory_latency + out_latency[i];
 			 } else { // compute
 				 compute_latency = compute_latency + out_latency[i];
 			 }
 			 out_latency[i] = 0;
 		 }
 		 printf("latency: %d \n", max(compute_latency,memory_latency));
 		 total_latency = total_latency + max(compute_latency,memory_latency);
 		 memory_latency = 0;
 		 compute_latency = 0;
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

		printf("Block: %d \n", line);

		istringstream ss(offsets_line);
		while(ss >> input_offset)
		{
			CSC_offsets[ii] = input_offset;
			ii ++;
		}
		ii = 0;
		printf("Reading CSC offsets file is done... ");

		getline(row_indices_file, row_indices_line);
		istringstream ss1(row_indices_line);
		while(ss1 >> input_row_inx)
		{
			CSC_row_indices[ii] = input_row_inx;
			ii ++;
		}
		ii = 0;
		printf("Reading CSC-row-indices file is done... ");

		getline(values_file, values_line);
		istringstream ss2(values_line);
		while(ss2 >> input_val)
		{
			CSC_values[ii] = input_val;
			ii ++;
		}
		ii = 0;
		printf("Reading CSC-values file is done... \n");
		line ++;

		// Call the CSC HWA:
	  CscTop(CSC_row_indices, CSC_offsets, CSC_values, B, CSC_out, out_latency, latency);

		for (int i = 0; i<LATENCY_PARAM; i++){
 			 // printf("latency: %d \n", out_latency[i]);
 			 if (i == 1){ // memory
				 printf("memory latency: %d \n", out_latency[i]);
 				 memory_latency = memory_latency + out_latency[i];
 			 } else { // compute
 				 compute_latency = compute_latency + out_latency[i];
 			 }
 			 out_latency[i] = 0;
 		 }
 		 printf("latency: %d \n", max(compute_latency,memory_latency));
 		 total_latency = total_latency + max(compute_latency,memory_latency);
 		 memory_latency = 0;
 		 compute_latency = 0;
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

		printf("Block: %d \n", line);

		istringstream ss(dia_line);
		while(ss >> input_dia)
		{
			DIA_diagonals[ii / MAX_DIAGONAL_LEN][ii % MAX_DIAGONAL_LEN] = input_dia;
			ii ++;
		}
		ii = 0;
		printf("Reading DIA file is done... \n");
		line ++;

		// Call the DIA HWA:
	  DiaTop(DIA_diagonals, B, DIA_out, out_latency, latency);

		for (int i = 0; i<LATENCY_PARAM; i++){
 			 // printf("latency: %d \n", out_latency[i]);
 			 if (i == 1){ // memory
				 printf("memory latency: %d \n", out_latency[i]);
 				 memory_latency = memory_latency + out_latency[i];
 			 } else { // compute
 				 compute_latency = compute_latency + out_latency[i];
 			 }
 			 out_latency[i] = 0;
 		 }
 		 printf("latency: %d \n", max(compute_latency,memory_latency));
 		 total_latency = total_latency + max(compute_latency,memory_latency);
 		 memory_latency = 0;
 		 compute_latency = 0;
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

		printf("Block: %d \n", line);

		istringstream ss(indices_line);
		while(ss >> input_inx)
		{
			ELL_col_indices[ii/ELL_MAX_COMP_ROW_LENGTH][ii%ELL_MAX_COMP_ROW_LENGTH] = input_inx;
			ii ++;
		}
		ii = 0;
		printf("Reading ELL-indices file is done... ");

		getline(values_file, values_line);
		istringstream ss2(values_line);
		while(ss2 >> input_val)
		{
			ELL_values[ii/ELL_MAX_COMP_ROW_LENGTH][ii%ELL_MAX_COMP_ROW_LENGTH] = input_val;
			ii ++;
		}
		ii = 0;
		printf("Reading ELL-values file is done... \n");
		line ++;

		// Call the ELL HWA:
	  EllTop(ELL_values, ELL_col_indices, B, ELL_out, out_latency, latency);

		for (int i = 0; i<LATENCY_PARAM; i++){
 			 // printf("latency: %d \n", out_latency[i]);
 			 if (i == 1){ // memory
				 printf("memory latency: %d \n", out_latency[i]);
 				 memory_latency = memory_latency + out_latency[i];
 			 } else { // compute
 				 compute_latency = compute_latency + out_latency[i];
 			 }
 			 out_latency[i] = 0;
 		 }
 		 printf("latency: %d \n", max(compute_latency,memory_latency));
 		 total_latency = total_latency + max(compute_latency,memory_latency);
 		 memory_latency = 0;
 		 compute_latency = 0;
	}
	indices_file.close();
	values_file.close();
}

printf("done \n");
printf("total latency: %d cycles\n", total_latency);

return 0;
}
