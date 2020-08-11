
//-------------------------------
//---------| 8 x 8  |------------
//-------------------------------

// #define WIDTH	8
// #define	LENGTH	8
//
// #define LATENCY_PARAM 4
//
// #define CSR_OFFSETS_LENGTH 8
// #define CSR_COL_INX_LENGTH 64
// #define CSR_VAL_LENGTH 64
//
// #define CSC_OFFSETS_LENGTH 8
// #define CSC_ROW_INX_LENGTH 64
// #define CSC_VAL_LENGTH 64
//
// #define BCSR_OFFSETS_LENGTH 2 // 8/4
// #define BCSR_COL_INX_LENGTH 4 // (8/4) x (8/4)
// #define BCSR_VAL_LENGTH 8
// #define BCSR_VAL_WIDTH 8
//
// #define BCSR_BLOCK_LENGTH 4
//
// #define LIL_LENGTH	8 // it is same as LENGTH, we eliminate zero rows dynamically
//
// #define COO_NUM_TUPLES 64
//
// #define NUM_DIAGONALS 15
// #define MAX_DIAGONAL_LEN 9
//
// #define ELL_MAX_COMP_ROW_LENGTH 8

//-------------------------------
//--------| 16 x 16  |-----------
//-------------------------------

#define WIDTH	16
#define	LENGTH	16

#define LATENCY_PARAM 4

#define CSR_OFFSETS_LENGTH 16
#define CSR_COL_INX_LENGTH 256
#define CSR_VAL_LENGTH 256

#define CSC_OFFSETS_LENGTH 16
#define CSC_ROW_INX_LENGTH 256
#define CSC_VAL_LENGTH 256

#define BCSR_OFFSETS_LENGTH 4 // 16/4
#define BCSR_COL_INX_LENGTH 16 // (16/4) x (16/4)
#define BCSR_VAL_LENGTH 16
#define BCSR_VAL_WIDTH 16

#define BCSR_BLOCK_LENGTH 4

#define LIL_LENGTH	16 // it is same as LENGTH, we eliminate zero rows dynamically

#define COO_NUM_TUPLES 256

#define NUM_DIAGONALS 31 // 16x2 - 1
#define MAX_DIAGONAL_LEN 17 // 16+1

#define ELL_MAX_COMP_ROW_LENGTH 16

//-------------------------------
//--------| 32 x 32  |-----------
//-------------------------------

// #define WIDTH	32
// #define	LENGTH	32
//
// #define LATENCY_PARAM 4
//
// #define CSR_OFFSETS_LENGTH 32
// #define CSR_COL_INX_LENGTH 1024
// #define CSR_VAL_LENGTH 1024
//
// #define CSC_OFFSETS_LENGTH 32
// #define CSC_ROW_INX_LENGTH 1024
// #define CSC_VAL_LENGTH 1024
//
// #define BCSR_OFFSETS_LENGTH 8 // 32/4
// #define BCSR_COL_INX_LENGTH 64 // (32/4) x (32/4)
// #define BCSR_VAL_LENGTH 32
// #define BCSR_VAL_WIDTH 32
//
// #define BCSR_BLOCK_LENGTH 4
//
// #define LIL_LENGTH	32 // it is same as LENGTH, we eliminate zero rows dynamically
//
// #define COO_NUM_TUPLES 1024
//
// #define NUM_DIAGONALS 63 // 32x2 - 1
// #define MAX_DIAGONAL_LEN 33 // 32+1
//
// #define ELL_MAX_COMP_ROW_LENGTH 32

//-------------------------------
//---------| COMMON |------------
//-------------------------------

int DotProduct(int A[WIDTH],
			   int B[WIDTH]);

//-------------------------------
//--------| 0. DENSE |-----------
//-------------------------------

void SpMV(int A[LENGTH][WIDTH],
		  int B[WIDTH],
		  int out[LENGTH],
			int out_latency[LATENCY_PARAM],
			int latency[LATENCY_PARAM]);

void top(int A[LENGTH][WIDTH],
		 int B[WIDTH],
		 int out[LENGTH],
		 int out_latency[LATENCY_PARAM],
		 int latency[LATENCY_PARAM]);

//-------------------------------
//-----------| 1. LIL |----------
//-------------------------------

void LilTop(int indices[LIL_LENGTH][WIDTH],
			int values[LIL_LENGTH][WIDTH],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
			int latency[LATENCY_PARAM]);

void LilSpMV(int indices[LIL_LENGTH][WIDTH],
			 int values[LIL_LENGTH][WIDTH],
			 int B[WIDTH],
			 int out[LENGTH],
			 int out_latency[LATENCY_PARAM],
			 int latency[LATENCY_PARAM]);

int LilDecompress(int indices[LIL_LENGTH][WIDTH],
		 		 	 	   int values[LIL_LENGTH][WIDTH],
		 				   int read_inx[WIDTH],
		 		 	 	   int A[WIDTH],
							 int out_latency[LATENCY_PARAM],
				 		   int latency[LATENCY_PARAM]);

//-------------------------------
//-----------| 2. CSR |----------
//-------------------------------

void CsrTop(int offsets[CSR_OFFSETS_LENGTH],
		 	int col_indices[CSR_COL_INX_LENGTH],
			int values[CSR_VAL_LENGTH],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
		  int latency[LATENCY_PARAM]);

void CsrSpMV(int offsets[CSR_OFFSETS_LENGTH],
			int col_indices[CSR_COL_INX_LENGTH],
			int values[CSR_VAL_LENGTH],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
		  int latency[LATENCY_PARAM]);

int CsrDecompress(int offsets[CSR_OFFSETS_LENGTH],
 	 	  int col_indices[CSR_COL_INX_LENGTH],
		  int values[CSR_VAL_LENGTH][BCSR_VAL_WIDTH],
		  int read_inx,
		  int A[WIDTH],
			int out_latency[LATENCY_PARAM],
		  int latency[LATENCY_PARAM]);

//-------------------------------
//-----------| 3. BCSR |---------
//-------------------------------

void BcsrTop(int offsets[BCSR_OFFSETS_LENGTH],
				int col_indices[BCSR_COL_INX_LENGTH],
				int values[BCSR_VAL_LENGTH][BCSR_VAL_WIDTH],
				int B[WIDTH],
				int out[LENGTH],
				int out_latency[LATENCY_PARAM],
				int latency[LATENCY_PARAM]);

void BcsrSpMV(int offsets[BCSR_OFFSETS_LENGTH],
				int col_indices[BCSR_COL_INX_LENGTH],
				int values[BCSR_VAL_LENGTH][BCSR_VAL_WIDTH],
				int B[WIDTH],
				int out[LENGTH],
				int out_latency[LATENCY_PARAM],
				int latency[LATENCY_PARAM]);

int BcsrDecompress(int offsets[BCSR_OFFSETS_LENGTH],
	   	   	   	   int col_indices[BCSR_COL_INX_LENGTH],
							   int values[BCSR_VAL_LENGTH][BCSR_VAL_WIDTH],
							   int read_inx,
							   int old_inx,
							   int A[BCSR_BLOCK_LENGTH][WIDTH],
								 int out_latency[LATENCY_PARAM],
								 int latency[LATENCY_PARAM]);

//-------------------------------
//-----------| 4. COO |----------
//-------------------------------
void CooTop(int coo_tuples[COO_NUM_TUPLES][3],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
			int latency[LATENCY_PARAM]);

void CooSpMV(int coo_tuples[COO_NUM_TUPLES][3],
			 int B[WIDTH],
			 int out[LENGTH],
			 int out_latency[LATENCY_PARAM],
			 int latency[LATENCY_PARAM]);

int CooDecompress(int coo_tuples[COO_NUM_TUPLES],
				   int row,
				   int A[WIDTH],
					 int out_latency[LATENCY_PARAM],
					 int latency[LATENCY_PARAM]);

//-------------------------------
//-----------| 5. CSC |----------
//-------------------------------

void CscTop(int row_indices[CSC_ROW_INX_LENGTH],
           int offsets[CSC_OFFSETS_LENGTH],
           int values[CSC_VAL_LENGTH],
           int B[WIDTH],
           int out[LENGTH],
					 int out_latency[LATENCY_PARAM],
		 		   int latency[LATENCY_PARAM]);

void CscSpMV(int row_indices[CSC_ROW_INX_LENGTH],
            int offsets[CSC_OFFSETS_LENGTH],
            int values[CSC_VAL_LENGTH],
            int B[WIDTH],
            int out[LENGTH],
						int out_latency[LATENCY_PARAM],
					  int latency[LATENCY_PARAM]);

int CscDecompress(int row_indices[CSC_ROW_INX_LENGTH],
                  int offsets[CSC_OFFSETS_LENGTH],
                  int values[CSC_VAL_LENGTH],
                  int read_inx,
                  int A[WIDTH],
									int out_latency[LATENCY_PARAM],
								  int latency[LATENCY_PARAM]);

//-------------------------------
//-----------| 6. DIA |----------
//-------------------------------

void DiaTop(int DIA_diagonals[NUM_DIAGONALS][MAX_DIAGONAL_LEN],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
			int latency[LATENCY_PARAM]);

void DiaSpMV(int DIA_diagonals[NUM_DIAGONALS][MAX_DIAGONAL_LEN],
			 int B[WIDTH],
			 int out[LENGTH],
			 int out_latency[LATENCY_PARAM],
			 int latency[LATENCY_PARAM]);

int DiaDecompress(int DIA_diagonals[NUM_DIAGONALS][MAX_DIAGONAL_LEN],
				  int A[WIDTH],
					int out_latency[LATENCY_PARAM],
					int latency[LATENCY_PARAM]);

//-------------------------------
//-----------| 7. ELL |----------
//-------------------------------

void EllTop(int ell_values[LENGTH][ELL_MAX_COMP_ROW_LENGTH],
			int ell_col_indices[LENGTH][ELL_MAX_COMP_ROW_LENGTH],
			int B[WIDTH],
			int out[LENGTH],
			int out_latency[LATENCY_PARAM],
			int latency[LATENCY_PARAM]);

void EllSpMV(int ell_values[LENGTH][ELL_MAX_COMP_ROW_LENGTH],
			 int ell_col_indices[LENGTH][ELL_MAX_COMP_ROW_LENGTH],
			 int B[WIDTH],
			 int out[LENGTH],
			 int out_latency[LATENCY_PARAM],
			 int latency[LATENCY_PARAM]);

int EllDecompress(int ell_values[ELL_MAX_COMP_ROW_LENGTH],
				  int ell_col_indices[ELL_MAX_COMP_ROW_LENGTH],
				  int A[WIDTH],
					int out_latency[LATENCY_PARAM],
					int latency[LATENCY_PARAM]);
