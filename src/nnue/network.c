#include <stdint.h>
#include <stdio.h>
#include <immintrin.h>
#include <math.h>

#include "network.h"

NNUE      nnue;
NNUEState nnue_state;


void init_nnue() {
	/************************************************************
	 * Initialize the NNUE model and its state.
	************************************************************/

	////////////////
	// Init State //
	////////////////

	// Board State 
	for (int i = 0; i < BOARD_STATE_SIZE; ++i) {
		nnue_state.BOARD_STATE[i] = 0;
	}

	// HIDDEN_LAYER_1 output
	for (int i = 0; i < HIDDEN_LAYER_DIM_1; ++i) {
		nnue_state.HIDDEN_LAYER_1[i] = 0;
	}

	// HIDDEN_LAYER_2 output
	for (int i = 0; i < HIDDEN_LAYER_DIM_23; ++i) {
		nnue_state.HIDDEN_LAYER_2[i] = 0;
	}

	// HIDDEN_LAYER_3 output
	for (int i = 0; i < HIDDEN_LAYER_DIM_23; ++i) {
		nnue_state.HIDDEN_LAYER_3[i] = 0;
	}

	// OUTPUT_LAYER outputs
	nnue_state.OUTPUT_LAYER[0] = 0;
	nnue_state.EVAL 		   = 0.0f;


	////////////////
	// Init Model //
	////////////////

	// W0
	for (int i = 0; i < INPUT_LAYER_DIM; ++i) {
		for (int j = 0; j < HIDDEN_LAYER_DIM_1; ++j) {
			// nnue.W0[i * HIDDEN_LAYER_DIM_1 + j] = 0;
			nnue.W0[i * HIDDEN_LAYER_DIM_1 + j] = rand();
		}
	}

	// W1
	for (int i = 0; i < HIDDEN_LAYER_DIM_1; ++i) {
		for (int j = 0; j < HIDDEN_LAYER_DIM_23; ++j) {
			// nnue.W1[i * HIDDEN_LAYER_DIM_23 + j] = 0;
			nnue.W1[i * HIDDEN_LAYER_DIM_23 + j] = rand();
		}
	}

	// W2
	for (int i = 0; i < HIDDEN_LAYER_DIM_23; ++i) {
		for (int j = 0; j < HIDDEN_LAYER_DIM_23; ++j) {
			// nnue.W2[i * HIDDEN_LAYER_DIM_23 + j] = 0;
			nnue.W2[i * HIDDEN_LAYER_DIM_23 + j] = rand();
		}
	}

	// W3
	for (int i = 0; i < HIDDEN_LAYER_DIM_23; ++i) {
		// nnue.W3[i] = 0;
		nnue.W3[i] = rand();
	}
}

void matrix_multiply_16_8_16(
		const int16_t* A, 
		const int8_t* B, 
		int16_t* C, 
		int m, 
		int n, 
		int k
		) {
	/************************************************************
	 * Multiply two matrices A and B and store the result in C.
	 * Use cache blocking to improve performance.
	************************************************************/

	// Cache blocking parameters
	const int BLOCK_SIZE = 64;

	// Cache blocking
	for (int i = 0; i < m; i += BLOCK_SIZE) {
		for (int j = 0; j < n; j += BLOCK_SIZE) {
			for (int l = 0; l < k; l += BLOCK_SIZE) {
				// Compute the dimensions of the block
				int m_lim = fmin(i + BLOCK_SIZE, m);
				int n_lim = fmin(j + BLOCK_SIZE, n);
				int k_lim = fmin(l + BLOCK_SIZE, k);

				// Multiply the block
				for (int i1 = i; i1 < m_lim; i1++) {
					for (int j1 = j; j1 < n_lim; j1++) {
						int16_t sum = 0;
						for (int l1 = l; l1 < k_lim; l1++) {
							sum += A[i1 * k + l1] * B[l1 * n + j1];
						}
						C[i1 * n + j1] += sum;
					}
				}
			}
		}
	}
}


void matrix_multiply_8_8_16(
		const int8_t* A, 
		const int8_t* B, 
		int16_t* C,
		int m, 
		int n, 
		int k
		) {
	/************************************************************
	 * Multiply two matrices A and B and store the result in C.
	 * Use cache blocking to improve performance.
	************************************************************/

	// Cache blocking parameters
	const int BLOCK_SIZE = 64;

	// Cache blocking
	for (int i = 0; i < m; i += BLOCK_SIZE) {
		for (int j = 0; j < n; j += BLOCK_SIZE) {
			for (int l = 0; l < k; l += BLOCK_SIZE) {
				// Compute the dimensions of the block
				int m_lim = fmin(i + BLOCK_SIZE, m);
				int n_lim = fmin(j + BLOCK_SIZE, n);
				int k_lim = fmin(l + BLOCK_SIZE, k);

				// Multiply the block
				for (int i1 = i; i1 < m_lim; i1++) {
					for (int j1 = j; j1 < n_lim; j1++) {
						int16_t sum = 0;
						for (int l1 = l; l1 < k_lim; l1++) {
							sum += A[i1 * k + l1] * B[l1 * n + j1];
						}
						C[i1 * n + j1] += sum;
					}
				}
			}
		}
	}
}

void matrix_multiply_8_8_32(
		const int8_t* A, 
		const int8_t* B, 
		int32_t* C,
		int m, 
		int n, 
		int k
		) {
	/************************************************************
	 * Multiply two matrices A and B and store the result in C.
	 * Use cache blocking to improve performance.
	************************************************************/

	// Cache blocking parameters
	const int BLOCK_SIZE = 64;

	// Cache blocking
	for (int i = 0; i < m; i += BLOCK_SIZE) {
		for (int j = 0; j < n; j += BLOCK_SIZE) {
			for (int l = 0; l < k; l += BLOCK_SIZE) {
				// Compute the dimensions of the block
				int m_lim = fmin(i + BLOCK_SIZE, m);
				int n_lim = fmin(j + BLOCK_SIZE, n);
				int k_lim = fmin(l + BLOCK_SIZE, k);

				// Multiply the block
				for (int i1 = i; i1 < m_lim; i1++) {
					for (int j1 = j; j1 < n_lim; j1++) {
						int32_t sum = 0;
						for (int l1 = l; l1 < k_lim; l1++) {
							sum += A[i1 * k + l1] * B[l1 * n + j1];
						}
						C[i1 * n + j1] += sum;
					}
				}
			}
		}
	}
}

float scale_int32(int32_t x) {
	/************************************************************
	 * Scale an int32_t value to allow range ~[-100.0f, 100.0f].
	************************************************************/
	return (float)x * SCALE_FACTOR_32;
}

void clamped_relu_16_8(const int16_t* A, int8_t* B, int n) {
	/************************************************************
	 * Apply the clamped ReLU function to the input matrix A.
	************************************************************/
	/*
	for (int i = 0; i < n; ++i) {
		B[i] = fmin(fmax(A[i], -128), 127);
	}
	*/

	// Vectorized version
	for (int i = 0; i < n; i += 16) {
		__m256i a = _mm256_loadu_si256((__m256i*)&A[i]);
		__m256i b = _mm256_min_epi16(
				_mm256_max_epi16(a, _mm256_set1_epi16(-128)), 
				_mm256_set1_epi16(127)
				);
		_mm256_storeu_si256((__m256i*)&B[i], b);
	}
};

void nnue_first_layer() {
	/************************************************************
	 * Compute the first layer of the neural network.
	 * Sparse mm. Find the non-zero elements of the input matrix
	 * and add the corresponding weights to the hidden layer.
	************************************************************/

    for (int i = 0; i < INPUT_LAYER_DIM; ++i) {
        if (nnue_state.BOARD_STATE[i / 64] & (1ULL << (i % 64))) {
            for (int j = 0; j < HIDDEN_LAYER_DIM_1; ++j) {
                nnue_state.HIDDEN_LAYER_1[j] += nnue.W0[i * HIDDEN_LAYER_DIM_1 + j];
            }
        }
    }
}

float full_forward() {
	/************************************************************
	 * Do a full forward pass of the neural network.
	 * No incremental update is performed.
	************************************************************/
	nnue_first_layer();
	matrix_multiply_8_8_16(
			(const int8_t*)nnue_state.HIDDEN_LAYER_1, 
			(const int8_t*)nnue.W1,
			(int16_t*)nnue_state.HIDDEN_LAYER_2,
			1, 
			HIDDEN_LAYER_DIM_1, 
			HIDDEN_LAYER_DIM_23
			);
	clamped_relu_16_8(
			(const int16_t*)nnue_state.HIDDEN_LAYER_2,
			(int8_t*)nnue_state.HIDDEN_LAYER_2,
			HIDDEN_LAYER_DIM_23
			);
	matrix_multiply_8_8_16(
			(const int8_t*)nnue_state.HIDDEN_LAYER_2,
			(const int8_t*)nnue.W2,
			(int16_t*)nnue_state.HIDDEN_LAYER_3,
			1, 
			HIDDEN_LAYER_DIM_23, 
			HIDDEN_LAYER_DIM_23
			);
	clamped_relu_16_8(
			(const int16_t*)nnue_state.HIDDEN_LAYER_3,
			(int8_t*)nnue_state.HIDDEN_LAYER_3,
			HIDDEN_LAYER_DIM_23
			);
	matrix_multiply_8_8_32(
			(const int8_t*)nnue_state.HIDDEN_LAYER_3,
			(const int8_t*)nnue.W3,
			(int32_t*)nnue_state.OUTPUT_LAYER,
			1, 
			1, 
			HIDDEN_LAYER_DIM_23
			);
	nnue_state.EVAL = scale_int32(nnue_state.OUTPUT_LAYER[0]);
	return nnue_state.EVAL;
}

void test_nnue() {
	init_nnue();

	// Set the board state randomly
	for (int i = 0; i < BOARD_STATE_SIZE; ++i) {
		nnue_state.BOARD_STATE[i] = rand();
	}

	float eval = full_forward();
	printf("Eval: %f\n", eval);
}
