#pragma once

#include "board.h"
#include "movegen.h"

static const float INF = 1000000.0f;

static const float PAWN_VALUE   = 1.0f;
static const float KNIGHT_VALUE = 3.0f;
static const float BISHOP_VALUE = 3.0f;
static const float ROOK_VALUE   = 5.0f;
static const float QUEEN_VALUE  = 9.0f;
static const float KING_VALUE   = 1000.0f;

static const float PIECE_VALUES[7] = {
	PAWN_VALUE,
	KNIGHT_VALUE,
	BISHOP_VALUE,
	ROOK_VALUE,
	QUEEN_VALUE,
	KING_VALUE,
	0.0f
};

static const float PAWN_TABLE[64] = {
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
	0.5f, 1.0f, 1.0f,-2.0f,-2.0f,  1.0f, 1.0f, 0.5f,
	0.5f,-0.5f,-1.0f, 0.0f, 0.0f, -1.0f,-0.5f, 0.5f,
	0.0f, 0.0f, 0.0f, 2.0f, 2.0f,  0.0f, 0.0f, 0.0f,
	0.5f, 0.5f, 1.0f, 3.0f, 3.0f,  1.0f, 0.5f, 0.5f,
	1.0f, 1.0f, 2.0f, 3.5f, 3.5f,  2.0f, 1.0f, 1.0f,
	5.0f, 5.0f, 5.0f, 5.0f, 5.0f,  5.0f, 5.0f, 5.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f
};

static const float KNIGHT_TABLE[64] = {
	-5.0f,-4.0f,-3.0f,-3.0f,-3.0f,-3.0f,-4.0f,-5.0f,
	-4.0f,-2.0f, 0.0f, 0.0f, 0.0f, 0.0f,-2.0f,-4.0f,
	-3.0f, 0.0f, 1.0f, 1.5f, 1.5f, 1.0f, 0.0f,-3.0f,
	-3.0f, 0.5f, 1.5f, 2.0f, 2.0f, 1.5f, 0.5f,-3.0f,
	-3.0f, 0.0f, 1.5f, 2.0f, 2.0f, 1.5f, 0.0f,-3.0f,
	-3.0f, 0.5f, 1.0f, 1.5f, 1.5f, 1.0f, 0.5f,-3.0f,
	-4.0f,-2.0f, 0.0f, 0.5f, 0.5f, 0.0f,-2.0f,-4.0f,
	-5.0f,-4.0f,-3.0f,-3.0f,-3.0f,-3.0f,-4.0f,-5.0f
};

static const float KING_TABLE[64] = {
	4.0f, 6.0f, -1.0f, 0.0f, 0.0f, -1.0f, 6.0f, 4.0f,
	4.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 4.0f,
	-2.0f,-4.0f,-4.0f,-4.0f,-4.0f,-4.0f,-4.0f,-2.0f,
	-4.0f,-6.0f,-6.0f,-8.0f,-8.0f,-6.0f,-6.0f,-4.0f,
	-6.0f,-8.0f,-8.0f,-10.0f,-10.0f,-8.0f,-8.0f,-6.0f,
	-6.0f,-8.0f,-8.0f,-10.0f,-10.0f,-8.0f,-8.0f,-6.0f,
	-6.0f,-8.0f,-8.0f,-10.0f,-10.0f,-8.0f,-8.0f,-6.0f,
	-6.0f,-8.0f,-8.0f,-10.0f,-10.0f,-8.0f,-8.0f,-6.0f
};

static const uint8_t FLIP_TABLE[64] = {
	56, 57, 58, 59, 60, 61, 62, 63,
	48, 49, 50, 51, 52, 53, 54, 55,
	40, 41, 42, 43, 44, 45, 46, 47,
	32, 33, 34, 35, 36, 37, 38, 39,
	24, 25, 26, 27, 28, 29, 30, 31,
	16, 17, 18, 19, 20, 21, 22, 23,
	 8,  9, 10, 11, 12, 13, 14, 15,
	 0,  1,  2,  3,  4,  5,  6,  7
};


float eval_pawns(const Board* board);
float eval_knights(const Board* board);
float eval_kings(const Board* board);
float eval_board(const Board* board);
float minimax(
		const Board* board, 
		enum Color color, 
		int depth, 
		int max_depth, 
		float alpha, 
		float beta,
		uint64_t* nodes_visited
		);

Move get_best_move(const Board* board, enum Color color, int depth);
void calc_mvv_lva_score(const Board* board, Move* move);
BitBoard get_pawn_attacks(BitBoard pawns, enum Color color);
