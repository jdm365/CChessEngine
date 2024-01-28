#include <stdio.h>
#include <time.h>

#include "board.h"
#include "eval.h"


float eval_pawns(const Board* board) {
	float value = 0.0f;

	BitBoard white_pawns = board->white_pawns;
	BitBoard black_pawns = board->black_pawns;

	while (white_pawns) {
		uint8_t square = __builtin_ctzll(white_pawns);
		white_pawns &= white_pawns - 1;
		value += PAWN_VALUE + 0.1f * PAWN_TABLE[square];
	}

	while (black_pawns) {
		uint8_t square = __builtin_ctzll(black_pawns);
		black_pawns &= black_pawns - 1;
		value -= PAWN_VALUE + 0.1f * PAWN_TABLE[FLIP_TABLE[square]];
	}

	return value;
}

float eval_knights(const Board* board) {
	float value = 0.0f;

	BitBoard white_knights = board->white_knights;
	BitBoard black_knights = board->black_knights;

	while (white_knights) {
		uint8_t square = __builtin_ctzll(white_knights);
		white_knights &= white_knights - 1;
		value += KNIGHT_VALUE + 0.1f * KNIGHT_TABLE[square];
	}

	while (black_knights) {
		uint8_t square = __builtin_ctzll(black_knights);
		black_knights &= black_knights - 1;
		value -= KNIGHT_VALUE + 0.1f * KNIGHT_TABLE[FLIP_TABLE[square]];
	}

	return value;
}

float eval_kings(const Board* board) {
	float value = 0.0f;

	BitBoard white_king = board->white_king;
	BitBoard black_king = board->black_king;

	while (white_king) {
		uint8_t square = __builtin_ctzll(white_king);
		white_king &= white_king - 1;
		value += KING_VALUE + 0.1f * KING_TABLE[square];
	}

	while (black_king) {
		uint8_t square = __builtin_ctzll(black_king);
		black_king &= black_king - 1;
		value -= KING_VALUE + 0.1f * KING_TABLE[FLIP_TABLE[square]];
	}

	return value;
}

float eval_board(const Board* board) {
	float eval = 0.0f;

	eval += eval_pawns(board);
	eval += eval_knights(board);
	eval += eval_kings(board);

	// White
	eval += BISHOP_VALUE * __builtin_popcountll(board->white_bishops);
	eval += ROOK_VALUE   * __builtin_popcountll(board->white_rooks);
	eval += QUEEN_VALUE  * __builtin_popcountll(board->white_queens);

	// Black
	eval -= BISHOP_VALUE * __builtin_popcountll(board->black_bishops);
	eval -= ROOK_VALUE   * __builtin_popcountll(board->black_rooks);
	eval -= QUEEN_VALUE  * __builtin_popcountll(board->black_queens);
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

	if ((depth == max_depth) || game_over(board)) {
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

	clock_t start = clock();
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
	printf("MNps: %f\n", (float)nodes_visited / ((float)(clock() - start) / CLOCKS_PER_SEC) / 1000000.0f);
	return best_move;
}
