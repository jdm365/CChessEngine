#include <stdint.h>
#include <stdlib.h>
#include <stdio.h>
#include <immintrin.h>
#include <math.h>
#include <time.h>

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

inline void matrix_multiply_16_8_16(
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


inline void matrix_multiply_8_8_16(
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

inline void matrix_multiply_8_8_32(
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

inline float scale_int32(int32_t x) {
	/************************************************************
	 * Scale an int32_t value to allow range ~[-100.0f, 100.0f].
	************************************************************/
	return (float)x * SCALE_FACTOR_32;
}

inline void clamped_relu_16_8(const int16_t* A, int8_t* B, int n) {
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

inline void nnue_first_layer() {
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

float forward_incremental(const Board* board) {
	uint16_t flipped_bits[3] = {0, 0, 0};
	int8_t   sign[3] = {1, 1, 1};
	uint8_t  flipped_bits_count = 0;
	for (int idx = 0; idx < 12; ++idx) {
		// Compare board state with the previous state
		// and find the flipped bits
		uint64_t diff = board->pieces[idx] ^ nnue_state.BOARD_STATE[idx];

		while (diff) {
			uint64_t bit = diff & -diff;
			int square = __builtin_ctzll(bit);
			flipped_bits[flipped_bits_count] = square + idx * 64;
			sign[flipped_bits_count] = (board->pieces[idx] & bit) ? -1 : 1;
			flipped_bits_count++;
			diff &= diff - 1;
		}
		if (flipped_bits_count == 3) {
			break;
		}
	}

	// Now vector add/substract the weights from the current value in the hidden layer
	for (int i = 0; i < flipped_bits_count; ++i) {
		int square = flipped_bits[i];
		int8_t s = sign[i];
		// for (int j = 0; j < HIDDEN_LAYER_DIM_1; ++j) {
			// nnue_state.HIDDEN_LAYER_1[j] += s * nnue.W0[square * HIDDEN_LAYER_DIM_1 + j];
		// }
		// Vectorized version
		for (int j = 0; j < HIDDEN_LAYER_DIM_1; j += 16) {
			__m256i a = _mm256_loadu_si256((__m256i*)&nnue_state.HIDDEN_LAYER_1[j]);
			__m256i b = _mm256_loadu_si256((__m256i*)&nnue.W0[square * HIDDEN_LAYER_DIM_1 + j]);
			__m256i c = _mm256_mullo_epi16(b, _mm256_set1_epi8(s));
			__m256i d = _mm256_add_epi16(a, c);
			_mm256_storeu_si256((__m256i*)&nnue_state.HIDDEN_LAYER_1[j], d);
		}
	}

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

void encode_board_state_full(const Board* board) {
	/************************************************************
	 * Encode the board state.
	 * Modified NNUE encoding skips the mirrored state for the 
	   other color.
	 * Encode the board state with 45056 bits conditional 
	   on the following:
	   - (White king on square idx, piece X on square jdx); if True 1 else 0.
	   - 11 other piece types (excluding self), 64 self squares and 64 
	     other squares. 11 * 64 * 64 = 45056 bits.
	 * This strange encoding is designed to allow a large number of parameters
	   in the network enabling a more robust evaluation function, but also 
	   guarantees that the input layer is very sparse, and that very few bits
	   in the board representation and thus input layer change from move to move.
	 * This allows for an extremely efficient forward pass on the next move given
	   the previous value of layer 1 from the last board state.
	 * See https://arxiv.org/pdf/2209.01506.pdf chapter 4.6 for a more in 
	   depth explanation.

	 * **NOTE: I don't really buy this "overparameterization" argument so much.
	   I still think this is a good idea, but this is much more akin to a sparse
	   expert routing architecture where 64 similar but allowably subtly different
	   models can be learned for the king on each square. The sparse expert 
	   architecture allows for much faster inference times which is great here,
	   but I suspect this isn't wildly better than some other structure.
	
	* **NEXT NOTE: Fuck it, I talked myself out of it. I want to try ditching the
	  sparse MOE design. I don't want to have to spend the time learning 64 different
	  representations anyways (one for each king position). I'd have to train on 8 
	  quadrillion positions anyways before that gave great results. (Maybe not)
	
	* New architecture: Instead I am going to try the obvious default of stacking
	  together all the bitboards to get a 64 * 12 = 768 dimensional input.
	  On a given move, there will still be at most 3 bits of board state change 
	  (Move from current square 1, add to new square 2, remove captured piece square 3)
	  so we can still use the NNUE efficient update, just ignoring the overly
	  complex encoding.
	************************************************************/
	for (int idx = 0; idx < 12; ++idx) {
		nnue_state.BOARD_STATE[idx] = board->pieces[idx];
	}
}

void test_nnue() {
	init_nnue();
	Board board;
	init_board(&board);

	populate_board_from_fen(&board, STARTING_FEN);

	encode_board_state_full(&board);

	clock_t start = clock();

	float eval = 0.0f;
	for (int idx = 0; idx < 1000000; ++idx) {
		eval = full_forward();
	}
	printf("Eval: %f\n", eval);
	printf("Kilo Evaluations per second: %f\n", 0.001f * 1000000.0f / ((float)(clock() - start) / CLOCKS_PER_SEC));

	start = clock();
	for (int idx = 0; idx < 1000000; ++idx) {
		eval = forward_incremental(&board);
	}
	printf("Kilo Evaluations per second: %f\n", 0.001f * 1000000.0f / ((float)(clock() - start) / CLOCKS_PER_SEC));
	printf("Eval: %f\n", eval);
}
