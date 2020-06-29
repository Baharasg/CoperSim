#include "CoperSim.h"
#include <stdio.h>
#include <string.h>

//-------------------------------
//---------| COMMON |------------
//-------------------------------

int DotProduct(int A[WIDTH], int B[WIDTH]){
	int out = 0;
	for (int i = 0; i < WIDTH; i++){
		// #pragma HLS unroll
		out = out + A[i] * B[i];
	}
	return out;
}

//-------------------------------
//----------| DENSE |------------
//-------------------------------

void SpMV(int A[LENGTH][WIDTH],
					int B[WIDTH],
					int out[LENGTH],
					int out_latency[LATENCY_PARAM],
					int latency[LATENCY_PARAM]){

	for (int i = 0; i < LENGTH; i++){
		// #pragma HLS unroll
		out[i] = DotProduct(A[i],B);

		// BRAM access to read offsets occurs in either way of above if condition.
		out_latency[2] = out_latency[2] + latency[2];

		// add dot-product latency
		out_latency[0] = out_latency[0] + latency[0];
	}
}

//................................

void top(int A[LENGTH][WIDTH],
		 int B[WIDTH],
		 int out[LENGTH],
		 int out_latency[LATENCY_PARAM],
		 int latency[LATENCY_PARAM]){

// #pragma HLS INTERFACE axis port=A
// #pragma HLS INTERFACE axis port=B
// #pragma HLS INTERFACE axis port=out

	int buff_A[LENGTH][WIDTH];
	int buff_B[WIDTH];

// #pragma HLS dataflow

	for (int i = 0; i<LENGTH; i++){
		for (int j = 0; j<WIDTH; j++){
			buff_A[i][j] = A[i][j];

			// add memory latency
			out_latency[1] = out_latency[1] + latency [1];
		}
	}
	for (int j = 0; j<WIDTH; j++){
		buff_B[j] = B[j];
	}

	SpMV(buff_A, buff_B, out, out_latency, latency);
}

//-------------------------------
//-----------| 1. LIL |----------
//-------------------------------

int LilDecompress(int indices[LIL_LENGTH][WIDTH],
		 	 	   int values[LIL_LENGTH][WIDTH],
				   int read_inx[WIDTH],
		 	 	   int A[WIDTH],
					 int out_latency[LATENCY_PARAM],
		 		   int latency[LATENCY_PARAM]){

	int min_inx = 100;

	for (int i = 0; i<WIDTH; i++){
		// #pragma HLS unroll
		if (read_inx[i] < LIL_LENGTH-1 and indices[read_inx[i]][i] < min_inx){
			min_inx = indices[read_inx[i]][i];

			// BRAM access
			out_latency[2] = out_latency[2] + latency[2];
		}
	}

	for (int i = 0; i<WIDTH; i++){
		// #pragma HLS unroll
		if (indices[read_inx[i]][i] == min_inx){
			A[i] = values[read_inx[i]][i];
			read_inx[i]++;

			// BRAM access
			out_latency[2] = out_latency[2] + latency[2];
		}
		else {
			A[i] = 0;
		}
	}

	return min_inx;
}

//................................

void LilSpMV(int indices[LIL_LENGTH][WIDTH],
			 int values[LIL_LENGTH][WIDTH],
			 int B[WIDTH],
			 int out[LENGTH],
			 int out_latency[LATENCY_PARAM],
			 int latency[LATENCY_PARAM]){

	int A[WIDTH];
	int read_inx[WIDTH];
	int min_inx = 0;
	int min_inx_old = 0;

	for (int i = 0; i<WIDTH; i++)
	{
		// #pragma HLS unroll
		read_inx[i] = 0;
	}

	for (int i = 0; i < LENGTH; i++){
		// #pragma HLS pipeline

		min_inx = LilDecompress(indices, values, read_inx, A, out_latency, latency);
		if (min_inx != 100){
			if (min_inx > min_inx_old + 1){
				out[i] = 0;
			}
			out[min_inx] = DotProduct(A,B);

			// add dot-product latency
			out_latency[0] = out_latency[0] + latency[0];
		}
		min_inx_old = min_inx;
	}
}

//................................

void LilTop(int indices[LIL_LENGTH][WIDTH],
			int values[LIL_LENGTH][WIDTH],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
			int latency[LATENCY_PARAM]){

	// #pragma HLS INTERFACE axis port=indices
	// #pragma HLS INTERFACE axis port=values
	// #pragma HLS INTERFACE axis port=B
	// #pragma HLS INTERFACE axis port=out


	int buff_indices[LIL_LENGTH][WIDTH];
	// #pragma HLS ARRAY_PARTITION variable=buff_indices complete dim=2
	int buff_values[LIL_LENGTH][WIDTH];
	// #pragma HLS ARRAY_PARTITION variable=buff_values complete dim=2
	int buff_B[WIDTH];

	int row_counter = 0;


	// #pragma HLS dataflow

	for (int i = 0; i<LIL_LENGTH; i++){
		for (int j = 0; j<WIDTH; j++){
			buff_indices[i][j] = indices[i][j];

			// add memory latency
			if (buff_indices[i][j] != 100){
				out_latency[1] = out_latency[1] + latency [1];
			}
		}
	}

	for (int i = 0; i<LIL_LENGTH; i++){
		for (int j = 0; j<WIDTH; j++){
			buff_values[i][j] = values[i][j];
			// no need to add memory latency due to parallelism (same size indices and values)
		}
	}
	for (int j = 0; j<WIDTH; j++){
		buff_B[j] = B[j];
	}

	LilSpMV(buff_indices, buff_values, buff_B, out, out_latency, latency);
}

//---------------------------------
//-----------| 2. CSR |------------
//---------------------------------


int CsrDecompress(int offsets[CSR_OFFSETS_LENGTH],
		 	 	   int col_indices[CSR_COL_INX_LENGTH],
				   int values[CSR_VAL_LENGTH],
				   int read_inx,
				   int old_inx,
				   int A[WIDTH],
					 int out_latency[LATENCY_PARAM],
				   int latency[LATENCY_PARAM]){

	int num_val = 0;

	if (read_inx == 0){
		num_val = offsets[read_inx];
	}
	else {
		num_val = offsets[read_inx] - offsets[read_inx - 1];
	}
	// BRAM access
	out_latency[2] = out_latency[2] + latency[2];


	for (int i = 0; i<num_val; i++){
		// #pragma HLS pipeline
		A[col_indices[old_inx + i]] = values[old_inx + i];

		// BRAM access to read values and indices (2 times)
		out_latency[2] = out_latency[2] + 2 * latency[2];

		// add latency for decompressing a single non-zero values
		// out_latency[3] = out_latency[3] + latency[3];
	}

	return num_val;
}

//................................

void CsrSpMV(int offsets[CSR_OFFSETS_LENGTH],
			 int col_indices[CSR_COL_INX_LENGTH],
			 int values[CSR_VAL_LENGTH],
			 int B[WIDTH],
			 int out[LENGTH],
			 int out_latency[LATENCY_PARAM],
		   int latency[LATENCY_PARAM]){

	int A[WIDTH];
	int num_val = 0;
	int old_inx = 0;

	for (int i = 0; i < LENGTH; i++){
		// #pragma HLS pipeline

		for (int j = 0; j<WIDTH; j++){
			// #pragma HLS unroll
			A[j] = 0;
		}

		num_val = CsrDecompress(offsets, col_indices, values, i, old_inx,  A, out_latency, latency);

		if (num_val == 0){
			out[i] = 0;
		} else {
			out[i] = DotProduct(A,B);

			// add dot-product latency
			out_latency[0] = out_latency[0] + latency[0];
		}
		old_inx = old_inx + num_val;
	}
}

//................................

void CsrTop(int offsets[CSR_OFFSETS_LENGTH],
		 	int col_indices[CSR_COL_INX_LENGTH],
			int values[CSR_VAL_LENGTH],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
		  int latency[LATENCY_PARAM]){

	// #pragma HLS INTERFACE axis port=row_indices
	// #pragma HLS INTERFACE axis port=col_indices
	// #pragma HLS INTERFACE axis port=values
	// #pragma HLS INTERFACE axis port=B
	// #pragma HLS INTERFACE axis port=out

	int buff_offsets[CSR_OFFSETS_LENGTH];
	int buff_col_indices[CSR_COL_INX_LENGTH];
	int buff_values[CSR_VAL_LENGTH];
	int buff_B[WIDTH];

	// #pragma HLS dataflow

	for (int i = 0; i<CSR_OFFSETS_LENGTH; i++){
		buff_offsets[i] = offsets[i];

		// add the latency for streaming four bytes (offsets are the longest for csr)
		if (buff_offsets[i] != 100){
			out_latency[1] = out_latency[1] + latency [1];
		}
	}
	for (int i = 0; i<CSR_COL_INX_LENGTH; i++){
		buff_col_indices[i] = col_indices[i];
		// no need to count latency because of parallelism
	}
	for (int i = 0; i<CSR_VAL_LENGTH; i++){
		buff_values[i] = values[i];
		// no need to count latency because of parallelism
	}
	for (int j = 0; j<WIDTH; j++){
		buff_B[j] = B[j];
		// no need to count latency because of parallelism
	}

	CsrSpMV(buff_offsets, buff_col_indices, buff_values, buff_B, out, out_latency, latency);
}

//-------------------------------
//-----------| 3. BCSR |---------
//-------------------------------

int BcsrDecompress(int offsets[BCSR_OFFSETS_LENGTH],
			 	   	   	   int col_indices[BCSR_COL_INX_LENGTH],
								   int values[BCSR_VAL_LENGTH][BCSR_VAL_WIDTH],
								   int read_inx,
								   int old_inx,
								   int A[BCSR_BLOCK_LENGTH][WIDTH],
									 int out_latency[LATENCY_PARAM],
									 int latency[LATENCY_PARAM]){
	int num_blocks = 0;

	if (read_inx == 0){
		num_blocks = offsets[read_inx];
	}
	else {
		num_blocks = offsets[read_inx] - offsets[read_inx - 1];
	}

	for (int i = 0; i<num_blocks; i++){
		#pragma HLS unroll
		for (int j = 0; j<BCSR_VAL_WIDTH; j++){
			#pragma HLS unroll
			A[j / BCSR_BLOCK_LENGTH][col_indices[old_inx + i]+j % BCSR_BLOCK_LENGTH] = values[old_inx + i][j];

			// BRAM access to read values and indices (2 times)
			out_latency[2] = out_latency[2] + 2 * latency[2];
		}
	}

	return num_blocks;
}

//................................

void BcsrSpMV(int offsets[BCSR_OFFSETS_LENGTH],
				  	  int col_indices[BCSR_COL_INX_LENGTH],
						  int values[BCSR_VAL_LENGTH][BCSR_VAL_WIDTH],
						  int B[WIDTH],
						  int out[LENGTH],
							int out_latency[LATENCY_PARAM],
							int latency[LATENCY_PARAM]){

	int A[BCSR_BLOCK_LENGTH][WIDTH];
	// #pragma HLS ARRAY_PARTITION variable=A complete dim=2
	int num_blocks = 0;
	int old_inx = 0;

	for (int i = 0; i < BCSR_OFFSETS_LENGTH; i++){
		// #pragma HLS pipeline


		for (int j = 0; j<BCSR_BLOCK_LENGTH; j++){
			// #pragma HLS unroll
			for (int k = 0; k<WIDTH; k++){
				// #pragma HLS unroll
				A[j][k] = 0;
			}
		}

		num_blocks = BcsrDecompress(offsets, col_indices, values, i, old_inx,  A, out_latency, latency);

		if (num_blocks == 0){
			for (int j = 0; j<BCSR_BLOCK_LENGTH; j++){
				// #pragma HLS unroll
				out[i * BCSR_BLOCK_LENGTH + j] = 0;
			}
		}
		else {
			for (int j = 0; j<BCSR_BLOCK_LENGTH; j++){
				// #pragma HLS unroll
				out[i * BCSR_BLOCK_LENGTH + j] = DotProduct(A[j], B);

				// add dot-product latency
				out_latency[0] = out_latency[0] + latency[0];
			}
		}
		old_inx = old_inx + num_blocks;
	}

}

//................................

void BcsrTop(int offsets[BCSR_OFFSETS_LENGTH],
		  	 int col_indices[BCSR_COL_INX_LENGTH],
				 int values[BCSR_VAL_LENGTH][BCSR_VAL_WIDTH],
				 int B[WIDTH],
				 int out[LENGTH],
				 int out_latency[LATENCY_PARAM],
				 int latency[LATENCY_PARAM]){

	// #pragma HLS INTERFACE axis port=row_indices
	// #pragma HLS INTERFACE axis port=col_indices
	// #pragma HLS INTERFACE axis port=values
	// #pragma HLS INTERFACE axis port=B
	// #pragma HLS INTERFACE axis port=out

	int buff_offsets[BCSR_OFFSETS_LENGTH];
	int buff_col_indices[BCSR_COL_INX_LENGTH];
	int buff_values[BCSR_VAL_LENGTH][BCSR_VAL_WIDTH];
	// #pragma HLS ARRAY_PARTITION variable=buff_values complete dim=2
	int buff_B[WIDTH];

	// #pragma HLS dataflow

	for (int i = 0; i < BCSR_OFFSETS_LENGTH; i++){
		buff_offsets[i] = offsets[i];
	}
	for (int i = 0; i < BCSR_COL_INX_LENGTH; i++){
		buff_col_indices[i] = col_indices[i];

		// add the memory streaming latency four bytes
		if (buff_col_indices[i] != 100){
			out_latency[1] = out_latency[1] + 16 * latency [1]; // x16 because we need to transfer 16 values per each index
		}

	}
	for (int i = 0; i < BCSR_VAL_LENGTH; i++){
		for (int j = 0; j< BCSR_VAL_WIDTH; j++){
			buff_values[i][j] = values[i][j];
		}
	}
	for (int j = 0; j < WIDTH; j++){
		buff_B[j] = B[j];
	}

	BcsrSpMV(buff_offsets, buff_col_indices, buff_values, buff_B, out, out_latency, latency);
}

//---------------------------------
//-----------| 4. COO |------------
//---------------------------------

int CooDecompress(int coo_tuples[COO_NUM_TUPLES][3],
				   int row,
				   int A[WIDTH],
					 int out_latency[LATENCY_PARAM],
					 int latency[LATENCY_PARAM]){

	int vector_exist = 0;

	for (int i = 0; i<COO_NUM_TUPLES; i++){
		// #pragma HLS pipeline
		if (coo_tuples[i][0] != 100){
			if (coo_tuples[i][0] == row ){ // if the tuple row matches our target row
				A[coo_tuples[i][1]] = coo_tuples[i][2]; // go to the column index in the row and put the value

				// BRAM access
				out_latency[2] = out_latency[2] + latency[2];
		  }
			vector_exist = 1;
		}
	}

	// the rest will be 0

	return vector_exist;
}

//................................

void CooSpMV(int coo_tuples[COO_NUM_TUPLES][3],
			 int B[WIDTH],
			 int out[LENGTH],
			 int out_latency[LATENCY_PARAM],
			 int latency[LATENCY_PARAM]){

	int A[WIDTH];
	int vector_exist = 0;

	for (int i = 0; i < LENGTH; i++){
		// #pragma HLS pipeline

		for (int i = 0; i<WIDTH; i++){
			// #pragma HLS unroll
			A[i] = 0;
		}

		vector_exist = CooDecompress(coo_tuples, i, A, out_latency, latency);

		out[i] = DotProduct(A,B);

		if (vector_exist == 1){
			// add dot-product latency
			out_latency[0] = out_latency[0] + latency[0];
		}
	}
}

//................................

void CooTop(int coo_tuples[COO_NUM_TUPLES][3],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
			int latency[LATENCY_PARAM]){

	// #pragma HLS INTERFACE axis port=coo_tuples
	// #pragma HLS INTERFACE axis port=B
	// #pragma HLS INTERFACE axis port=out

	int buff_coo_tuples[COO_NUM_TUPLES][3];
	int buff_B[WIDTH];

	// #pragma HLS dataflow

	for (int i = 0; i<COO_NUM_TUPLES; i++){
		for (int j = 0; j < 3; j++) {
			buff_coo_tuples[i][j] = coo_tuples[i][j];

			if (buff_coo_tuples[i][j] != 100){
				// memory
				out_latency[1] = out_latency[1] + latency[1];
			}
		}
	}
	for (int j = 0; j<WIDTH; j++){
		buff_B[j] = B[j];
	}

	CooSpMV(buff_coo_tuples, buff_B, out, out_latency, latency);
}

//---------------------------------
//-----------| 5. CSC |------------
//---------------------------------

int CscDecompress(int row_indices[CSC_ROW_INX_LENGTH],
                   int offsets[CSC_OFFSETS_LENGTH], //col ptrs
                   int values[CSC_VAL_LENGTH],
                   int read_inx,
                   int A[LENGTH],
						 			 int out_latency[LATENCY_PARAM],
						 		   int latency[LATENCY_PARAM]){

    Decompress_Zero:for (int i = 0; i<LENGTH; i++) {
        // #pragma HLS pipeline
        A[i] = 0;
    }

    int col_ptr = 1;
    int num_val = offsets[col_ptr] - offsets[col_ptr-1];
    Decompress_Row:for(int i = 0; i < CSC_ROW_INX_LENGTH && col_ptr < CSC_OFFSETS_LENGTH;) {
    	int start_index = i;
    	Get_Indices:while(i < start_index + num_val) {
			if(row_indices[i] == read_inx) {
				A[col_ptr-1] = values[i];

				// BRAM
				out_latency[2] = out_latency[2] + latency[2];

				break;
			}
			i++;
    	}
		i = offsets[col_ptr++];
    num_val = offsets[col_ptr] - offsets[col_ptr-1];

		// BRAM
		out_latency[2] = out_latency[2] + latency[2];

    }
    return num_val;
}

//................................

//TODO: make this not have to decompress the entire matrix
void CscSpMV(int row_indices[CSC_ROW_INX_LENGTH],
             int offsets[CSC_OFFSETS_LENGTH],
             int values[CSC_VAL_LENGTH],
             int B[WIDTH],
             int out[LENGTH],
			 			 int out_latency[LATENCY_PARAM],
			 		   int latency[LATENCY_PARAM]){

    int A[LENGTH];
    SpMV_Decompress:for (int i = 0; i<LENGTH; i++){ //iterate across rows...
      int num_val = CscDecompress(row_indices, offsets, values, i, A, out_latency, latency);
      // #pragma HLS unroll
      out[i] = DotProduct(A,B);

			if (num_val > 0){
				// add dot-product latency
				out_latency[0] = out_latency[0] + latency[0];
			}
    }
}

//................................

void CscTop(int row_indices[CSC_ROW_INX_LENGTH],
            int offsets[CSC_OFFSETS_LENGTH],
            int values[CSC_VAL_LENGTH],
            int B[WIDTH],
            int out[LENGTH],
						int out_latency[LATENCY_PARAM],
		 		    int latency[LATENCY_PARAM]){

    // #pragma HLS INTERFACE axis port=row_indices
    // #pragma HLS INTERFACE axis port=col_indices
    // #pragma HLS INTERFACE axis port=values
    // #pragma HLS INTERFACE axis port=B
    // #pragma HLS INTERFACE axis port=out

    int buff_row_indices[CSC_ROW_INX_LENGTH];
    int buff_offsets[CSC_OFFSETS_LENGTH];
    int buff_values[CSC_VAL_LENGTH];
    int buff_B[WIDTH];

    // #pragma HLS dataflow

    Setup_Rows:for (int i = 0; i<CSC_ROW_INX_LENGTH; i++){
        buff_row_indices[i] = row_indices[i];
    }
    Setup_Cols:for (int i = 0; i<CSC_OFFSETS_LENGTH; i++){
        buff_offsets[i] = offsets[i];

        if (buff_offsets[i] == 100){
            break;
        }

				// add the memory streaming latency four bytes
				out_latency[1] = out_latency[1] + latency [1];
    }
    Setup_Vals:for (int i = 0; i<CSC_VAL_LENGTH; i++){
        buff_values[i] = values[i];
        if (buff_values[i] == 100){
            break;
        }
    }
    Setup_Vector:for (int j = 0; j<WIDTH; j++){
        buff_B[j] = B[j];
    }

    CscSpMV(buff_row_indices, buff_offsets, buff_values, buff_B, out, out_latency, latency);
}

//-------------------------------
//-----------| 6. DIA |----------
//-------------------------------

/*
the following is a matrix where each entry is its index on its diagonal. the number before
the ":" is the row

0: 0 0 0 0 0
1: 0 1 1 1 1
2: 0 1 2 2 2
3: 0 1 2 3 3
4: 0 1 2 3 4

e.g., the index into the -2 diagonal for the 3rd row is 1. for the 0 diagonal it is 3. for the 1 it is 3.
the index into the -1 diagonal for the 4th row is 3. the index into the -3 diagonal for the 4th row is 1.

the index is never higher than the row number

the index is min(row + dia, row)

*/
int DiagonalIndexForRow(int row, int diagonal) {
	return (row + diagonal < row) ? row + diagonal : row; // min(row + diagonal, row)
}

/*
the following is a matrix where each entry is the diagonal it is on. the number before
the ":" is the row

0:  0  1  2  3  4
1: -1  0  1  2  3
2: -2 -1  0  1  2
3: -3 -2 -1  0  1
4: -4 -3 -2 -1  0

for an nxn matrix, a row is on a diagonal if:
- the diagonal is >= (-row) and the diagonal is < (n - 1) - row
*/
bool IsRowOnDiagonal(int row, int diagonal) {
	return diagonal <= WIDTH - 1 - row && diagonal >= -row;
}

//................................

int DiaDecompress(int dia_diagonals[NUM_DIAGONALS][MAX_DIAGONAL_LEN],
				  int row,
				  int A[WIDTH],
					int out_latency[LATENCY_PARAM],
					int latency[LATENCY_PARAM]){

	// todo: this
	decompress_loop:
	for (int i = 0; i < NUM_DIAGONALS; i++){
		// #pragma HLS pipeline
		int dia = dia_diagonals[i][0];
		if (dia != 100){
			if (!IsRowOnDiagonal(row, dia)) continue;
			// column index is the row + dia
			A[row + dia] = dia_diagonals[dia + 6][DiagonalIndexForRow(row, dia) + 1]; // + 1 since 0 element is the diagonal number

			// BRAM access
			out_latency[2] = out_latency[2] + latency[2];
		}
	}

	return 1;
}

//................................

void DiaSpMV(int dia_diagonals[NUM_DIAGONALS][MAX_DIAGONAL_LEN],
			 int B[WIDTH],
			 int out[LENGTH],
			 int out_latency[LATENCY_PARAM],
			 int latency[LATENCY_PARAM]){

	int A[WIDTH];

	sp_mv_loop:
	for (int i = 0; i < LENGTH; i++){
		// #pragma HLS pipeline

		clear_a_loop:
		for (int i = 0; i<WIDTH; i++){
			// #pragma HLS unroll
			A[i] = 0;
		}

		DiaDecompress(dia_diagonals, i, A, out_latency, latency);

		out[i] = DotProduct(A,B);

		// dot product latency
		out_latency[0] = out_latency[0] + latency[0];

	}
}

//................................

void DiaTop(int dia_diagonals[NUM_DIAGONALS][MAX_DIAGONAL_LEN],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
			int latency[LATENCY_PARAM]){

	// #pragma HLS INTERFACE axis port=dia_diagonals
	// #pragma HLS INTERFACE axis port=B
	// #pragma HLS INTERFACE axis port=out

	int buff_dia_diagonals[NUM_DIAGONALS][MAX_DIAGONAL_LEN];
	int buff_B[WIDTH];

	// #pragma HLS dataflow

	dia_copy_loop:
	for (int i = 0; i < NUM_DIAGONALS; i++){
		for (int j = 0; j < MAX_DIAGONAL_LEN; j++) {
			buff_dia_diagonals[i][j] = dia_diagonals[i][j];

			// add memory streaming latency four bytes
			out_latency[1] = out_latency[1] + latency [1];
			}
			if (buff_dia_diagonals[i][0] == 100){
				break;
			}
	}

	b_copy_loop:
	for (int j = 0; j<WIDTH; j++){
		buff_B[j] = B[j];
	}

	DiaSpMV(buff_dia_diagonals, buff_B, out, out_latency, latency);
}

//-------------------------------
//-----------| 7. ELL |----------
//-------------------------------


int EllDecompress(int ell_values[ELL_MAX_COMP_ROW_LENGTH],
				  int ell_col_indices[ELL_MAX_COMP_ROW_LENGTH],
				  int A[WIDTH],
					int out_latency[LATENCY_PARAM],
					int latency[LATENCY_PARAM]){

	int vector_exist = 0;

	// todo: this
	decompress_loop:
	for (int i = 0; i < ELL_MAX_COMP_ROW_LENGTH; i++){
		// #pragma HLS pipeline
		if (ell_values[i] == 100) break; // reach end of values
			A[ell_col_indices[i]] = ell_values[i]; // go to the column index in the row and put the value
			vector_exist = 1;

			// BRAM access
			out_latency[2] = out_latency[2] + 2 * latency[2];
	}

	// the rest will be 0

	return vector_exist;
}

//................................

void EllSpMV(int ell_values[LENGTH][ELL_MAX_COMP_ROW_LENGTH],
			 int ell_col_indices[LENGTH][ELL_MAX_COMP_ROW_LENGTH],
			 int B[WIDTH],
			 int out[LENGTH],
			 int out_latency[LATENCY_PARAM],
			 int latency[LATENCY_PARAM]){

	int A[WIDTH];

	spmv_loop:
	for (int i = 0; i < LENGTH; i++){
		// #pragma HLS pipeline

		if (ell_col_indices[i][0] != 100){

			clear_a_loop:
			for (int i = 0; i<WIDTH; i++){
				// #pragma HLS unroll
				A[i] = 0;
			}

			int vector_exist = EllDecompress(ell_values[i], ell_col_indices[i], A, out_latency, latency);

			out[i] = DotProduct(A,B);

			if (vector_exist == 1){
				// add dot-product latency
				out_latency[0] = out_latency[0] + latency[0];
			}
		}
	}
}

//................................

void EllTop(int ell_values[LENGTH][ELL_MAX_COMP_ROW_LENGTH],
			int ell_col_indices[LENGTH][ELL_MAX_COMP_ROW_LENGTH],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
			int latency[LATENCY_PARAM]){

	// #pragma HLS INTERFACE axis port=ell_values
	// #pragma HLS INTERFACE axis port=ell_col_indices
	// #pragma HLS INTERFACE axis port=B
	// #pragma HLS INTERFACE axis port=out

	int buff_ell_values[LENGTH][ELL_MAX_COMP_ROW_LENGTH];
	int buff_ell_col_indices[LENGTH][ELL_MAX_COMP_ROW_LENGTH];
	int buff_B[WIDTH];

	// #pragma HLS dataflow

	copy_ell_loop:
	for (int i = 0; i < LENGTH; i++){
		for (int j = 0; j < ELL_MAX_COMP_ROW_LENGTH; j++) {
			buff_ell_values[i][j] = ell_values[i][j];
		}
		if (buff_ell_values[i][0] == 100){
			break;
		}

		// add memory streaming four bytes
		out_latency[1] = out_latency[1] + latency [1];
	}

	copy_col_ind_loop:
	for (int i = 0; i < LENGTH; i++){
		for (int j = 0; j < ELL_MAX_COMP_ROW_LENGTH; j++) {
			buff_ell_col_indices[i][j] = ell_col_indices[i][j];
		}
		// no need to add memory latency here, dure to parallelism (similar for for indices and values)
	}

	copy_b_loop:
	for (int j = 0; j<WIDTH; j++){
		buff_B[j] = B[j];
	}

	EllSpMV(buff_ell_values, buff_ell_col_indices, buff_B, out, out_latency, latency);
}
