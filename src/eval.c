#include <stdio.h>
#include <omp.h>

#include "board.h"
#include "movegen.h"
#include "eval.h"


float eval_board(const Board* board) {
	float eval = 0.0f;

	// White
	eval += PAWN_VALUE   * __builtin_popcountll(board->white_pawns);
	eval += KNIGHT_VALUE * __builtin_popcountll(board->white_knights);
	eval += BISHOP_VALUE * __builtin_popcountll(board->white_bishops);
	eval += ROOK_VALUE   * __builtin_popcountll(board->white_rooks);
	eval += QUEEN_VALUE  * __builtin_popcountll(board->white_queens);
	eval += KING_VALUE   * __builtin_popcountll(board->white_king);

	// Black
	eval -= PAWN_VALUE   * __builtin_popcountll(board->black_pawns);
	eval -= KNIGHT_VALUE * __builtin_popcountll(board->black_knights);
	eval -= BISHOP_VALUE * __builtin_popcountll(board->black_bishops);
	eval -= ROOK_VALUE   * __builtin_popcountll(board->black_rooks);
	eval -= QUEEN_VALUE  * __builtin_popcountll(board->black_queens);
	eval -= KING_VALUE   * __builtin_popcountll(board->black_king);
	return eval;
}


float minimax(
		const Board* board, 
		enum Color color, 
		int depth, 
		int max_depth, 
		float alpha, 
		float beta,
		uint64_t* nodes_visited
		) {
	float maximizing_factor = (color == WHITE) ? 1.0f : -1.0f;

	if (depth == max_depth) {
		return eval_board(board) * maximizing_factor;
	}

	MoveList moves;
	init_move_list(&moves);
	get_legal_moves(board, &moves, color);

	if (moves.count == 0) {
		return eval_board(board) * maximizing_factor;
	}

	float best_score = -INF;
	for (int idx = 0; idx < moves.count; ++idx) {
		++*nodes_visited;
		Move move = moves.moves[idx];
		Board new_board = *board;
		_make_move(&new_board, move.from, move.to);
		float score = -minimax(
				&new_board, 
				!color, 
				depth + 1, 
				max_depth, 
				-beta, 
				-alpha,
				nodes_visited
				);
		if (score > best_score) {
			best_score = score;
		}
		if (score > alpha) {
			alpha = score;
		}
		if (alpha >= beta) {
			break;
		}
	}
	return best_score;
}

Move get_best_move(const Board* board, enum Color color, int depth) {
	MoveList moves;
	init_move_list(&moves);
	get_legal_moves(board, &moves, color);

	float best_score = -INF;
	Move best_move = moves.moves[0];

	uint64_t nodes_visited = 0;

	for (int idx = 0; idx < moves.count; ++idx) {
		++nodes_visited;

		Move move = moves.moves[idx];
		Board new_board = *board;
		_make_move(&new_board, move.from, move.to);
		float score = -minimax(
				&new_board, 
				!color, 
				1, 
				depth, 
				-INF, 
				INF,
				&nodes_visited
				);
		if (score > best_score) {
			best_score = score;
			best_move = move;
		}
	}

	printf("Nodes visited: %llu\n", nodes_visited);
	return best_move;
}
