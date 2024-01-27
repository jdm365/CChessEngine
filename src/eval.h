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
