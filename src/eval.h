#pragma once
#include <stdint.h>

#include "board.h"
#include "movegen.h"

static const float INF = 1000000.0f;

extern const float PAWN_VALUE;
extern const float KNIGHT_VALUE;
extern const float BISHOP_VALUE;
extern const float ROOK_VALUE;
extern const float QUEEN_VALUE;
extern const float KING_VALUE;

extern const float PIECE_VALUES[12];

extern const float PAWN_TABLE[64];
extern const float KNIGHT_TABLE[64];
extern const float BISHOP_TABLE[64];
extern const float ROOK_TABLE[64];
extern const float QUEEN_TABLE[64];
extern const float KING_TABLE[64];
extern const float KING_TABLE_ENDGAME[64];

extern const uint8_t FLIP_TABLE[64];

extern float HISTORY_TABLE[2][64][64];

float eval_pawns(const Board* board);
float eval_knights(const Board* board);
float eval_bishops(const Board* board);
float eval_rooks(const Board* board);
float eval_queens(const Board* board);
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
float minimax_with_pvs(
		// const Board* board, 
		Board* board, 
		enum Color color, 
		int depth, 
		int max_depth, 
		float alpha, 
		float beta,
		uint64_t* nodes_visited,
		bool is_pv_node,
		Move* pv_moves
		);

Move get_best_move(
		const Board* board, 
		enum Color color, 
		int depth,
		uint64_t* nodes_visited
		);
Move get_best_move_id(
		Board* board, 
		enum Color color, 
		int max_depth,
		uint64_t* nodes_visited
		);
float calc_mvv_lva_score(
		const Board* board, 
		enum Color color,
		uint8_t from,
		uint8_t to
		);
BitBoard get_pawn_attacks(BitBoard pawns, enum Color color);
