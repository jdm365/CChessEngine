#pragma once

#include <stdint.h>

// NNUE
// Will try without bias first.
// Also will try without mirrored network heads for 
// white and black pieces.

#define INPUT_LAYER_DIM 40960
#define HIDDEN_LAYER_DIM_1 256
#define HIDDEN_LAYER_DIM_23 32 

#define BOARD_STATE_SIZE INPUT_LAYER_DIM / 64

// scale an int32_t to a range of -100.0f to 100.0f
#define SCALE_FACTOR_32 (100.0f / 2147483648.0f)


typedef struct {
	int16_t W0[INPUT_LAYER_DIM * HIDDEN_LAYER_DIM_1];
	int8_t  W1[HIDDEN_LAYER_DIM_1 * HIDDEN_LAYER_DIM_23];
	int8_t  W2[HIDDEN_LAYER_DIM_23 * HIDDEN_LAYER_DIM_23];
	int8_t  W3[HIDDEN_LAYER_DIM_23];
} NNUE;

typedef struct {
	uint64_t BOARD_STATE[BOARD_STATE_SIZE];
	int16_t  HIDDEN_LAYER_1[HIDDEN_LAYER_DIM_1];
	int8_t   HIDDEN_LAYER_2[HIDDEN_LAYER_DIM_23];
	int8_t   HIDDEN_LAYER_3[HIDDEN_LAYER_DIM_23];
	int8_t   OUTPUT_LAYER[1];
	float    EVAL;
} NNUEState;

extern NNUE      nnue;
extern NNUEState nnue_state;

void init_nnue();
void matrix_multiply_16_8_16(
		const int16_t* A, 
		const int8_t* B, 
		int16_t* C, 
		int m, 
		int n, 
		int k
		);
void matrix_multiply_8_8_16(
		const int8_t* A, 
		const int8_t* B, 
		int16_t* C, 
		int m, 
		int n, 
		int k
		);
void matrix_multiply_8_8_32(
		const int8_t* A, 
		const int8_t* B, 
		int32_t* C, 
		int m, 
		int n, 
		int k
		);
float scale_int32(int32_t x);
void  clamped_relu_16_8(const int16_t* A, int8_t* B, int n);
void  nnue_first_layer();
float full_forward();

void test_nnue();
