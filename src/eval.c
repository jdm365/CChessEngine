#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#include "board.h"
#include "eval.h"

#define DEBUG 0


const float PAWN_VALUE   = 1.0f;
const float KNIGHT_VALUE = 3.0f;
const float BISHOP_VALUE = 3.15f;
const float ROOK_VALUE   = 5.0f;
const float QUEEN_VALUE  = 9.0f;
const float KING_VALUE   = 1000.0f;

// Do array of 12 for convernience. Potentially fix in the future.
const float PIECE_VALUES[12] = {
	PAWN_VALUE,
	PAWN_VALUE,
	KNIGHT_VALUE,
	KNIGHT_VALUE,
	BISHOP_VALUE,
	BISHOP_VALUE,
	ROOK_VALUE,
	ROOK_VALUE,
	QUEEN_VALUE,
	QUEEN_VALUE,
	KING_VALUE,
	KING_VALUE
};

const float PAWN_TABLE[64] = {
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f,
	0.5f, 1.0f, 1.0f,-2.0f,-2.0f,  1.0f, 1.0f, 0.5f,
	0.5f, 0.5f,-1.0f, 0.0f, 0.0f, -1.0f, 0.5f, 0.5f,
	0.0f, 0.0f, 1.5f, 2.5f, 2.5f,  1.5f, 0.0f, 0.0f,
	0.5f, 0.5f, 1.0f, 3.0f, 3.0f,  1.0f, 0.5f, 0.5f,
	1.0f, 1.0f, 2.0f, 3.5f, 3.5f,  2.0f, 1.0f, 1.0f,
	5.0f, 5.0f, 5.0f, 5.0f, 5.0f,  5.0f, 5.0f, 5.0f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.0f,  0.0f, 0.0f, 0.0f
};

const float KNIGHT_TABLE[64] = {
	-5.0f,-4.0f,-3.0f,-3.0f,-3.0f,-3.0f,-4.0f,-5.0f,
	-4.0f,-2.0f, 0.0f, 0.0f, 0.0f, 0.0f,-2.0f,-4.0f,
	-4.0f, 0.0f, 1.0f, 1.5f, 1.5f, 1.0f, 0.0f,-4.0f,
	-3.0f, 0.5f, 1.5f, 2.0f, 2.0f, 1.5f, 0.5f,-3.0f,
	-3.0f, 0.0f, 1.5f, 2.5f, 2.5f, 1.5f, 0.0f,-3.0f,
	-1.0f, 1.5f, 2.0f, 3.5f, 3.5f, 2.0f, 1.5f,-1.0f,
	 0.0f, 2.0f, 0.0f, 0.5f, 0.5f, 0.0f, 2.0f, 0.0f,
	-5.0f,-4.0f,-3.0f,-3.0f,-3.0f,-3.0f,-4.0f,-5.0f
};

const float BISHOP_TABLE[64] = {
	-2.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-2.0f,
	-1.0f, 2.0f, 0.0f, 0.0f, 0.0f, 0.0f, 2.0f,-1.0f,
	-1.0f, 0.0f, 0.5f, 1.0f, 1.0f, 0.5f, 0.0f,-1.0f,
	-1.0f, 0.5f, 0.5f, 1.0f, 1.0f, 0.5f, 0.5f,-1.0f,
	-1.0f, 0.0f, 1.0f, 1.0f, 1.0f, 1.0f, 0.0f,-1.0f,
	-1.0f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,-0.0f,-1.0f,
	-1.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.5f,-1.0f,
	-2.0f, 0.0f,-1.0f,-1.0f,-1.0f,-1.0f,-0.0f,-2.0f
};

const float ROOK_TABLE[64] = {
	-1.0f, 0.0f, 1.0f, 3.0f, 2.0f, 1.0f, 2.0f,-1.0f,
	-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,-0.0f,-0.5f,
	-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,-0.0f,-0.5f,
	-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,-0.0f,-0.5f,
	-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,-0.0f,-0.5f,
	-0.5f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,-0.0f,-0.5f,
	0.5f, 1.0f, 1.0f, 1.0f, 1.0f, 1.0f,-0.0f, 0.5f,
	0.0f, 0.0f, 0.0f, 0.0f, 0.5f, 0.0f,-0.0f, 0.0f
};

const float QUEEN_TABLE[64] = {
	-2.0f,-1.0f,-1.0f,-0.5f,-0.5f,-1.0f,-1.0f,-2.0f,
	-1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f,-0.0f,-1.0f,
	-1.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f,-1.0f,
	-0.5f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f,-0.5f,
	-0.0f, 0.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f,-0.5f,
	-1.0f, 0.5f, 0.5f, 0.5f, 0.5f, 0.5f, 0.0f,-1.0f,
	-1.0f, 0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 0.0f,-1.0f,
	-2.0f,-1.0f,-1.0f,-0.5f,-0.5f,-1.0f,-1.0f,-2.0f
};

const float KING_TABLE[64] = {
	4.0f, 6.0f, 4.0f, 0.0f, 0.0f, -1.0f, 6.0f, 4.0f,
	4.0f, 4.0f, 0.0f, 0.0f, 0.0f, 0.0f, 4.0f, 4.0f,
	-4.0f,-4.0f,-4.0f,-4.0f,-4.0f,-4.0f,-4.0f,-4.0f,
	-6.0f,-6.0f,-6.0f,-8.0f,-8.0f,-6.0f,-6.0f,-6.0f,
	-6.0f,-8.0f,-8.0f,-10.0f,-10.0f,-8.0f,-8.0f,-6.0f,
	-6.0f,-8.0f,-8.0f,-10.0f,-10.0f,-8.0f,-8.0f,-6.0f,
	-6.0f,-8.0f,-8.0f,-10.0f,-10.0f,-8.0f,-8.0f,-6.0f,
	-6.0f,-8.0f,-8.0f,-10.0f,-10.0f,-8.0f,-8.0f,-6.0f
};

const float KING_TABLE_ENDGAME[64] = {
	-5.0f,-4.0f,-3.0f,-2.0f,-2.0f,-3.0f,-4.0f,-5.0f,
	-3.0f,-2.0f,-1.0f, 0.0f, 0.0f,-1.0f,-2.0f,-3.0f,
	-1.0f,-1.0f, 2.0f, 2.0f, 2.0f, 2.0f,-1.0f,-1.0f,
	 1.0f, 1.0f, 3.0f, 4.0f, 4.0f, 3.0f, 1.0f, 1.0f,
	 1.0f,1.0f, 3.0f, 4.0f, 4.0f, 3.0f,1.0f,1.0f,
	 1.0f,1.0f, 2.0f, 3.0f, 3.0f, 2.0f,1.0f,1.0f,
	 1.0f,1.0f, 2.0f, 2.0f, 2.0f, 2.0f, 1.0f,1.0f,
	-1.0f,-1.0f,-1.0f,-1.0f,-1.0f,-1.0f, -1.0f,-1.0f
};

const uint8_t FLIP_TABLE[64] = {
	56, 57, 58, 59, 60, 61, 62, 63,
	48, 49, 50, 51, 52, 53, 54, 55,
	40, 41, 42, 43, 44, 45, 46, 47,
	32, 33, 34, 35, 36, 37, 38, 39,
	24, 25, 26, 27, 28, 29, 30, 31,
	16, 17, 18, 19, 20, 21, 22, 23,
	 8,  9, 10, 11, 12, 13, 14, 15,
	 0,  1,  2,  3,  4,  5,  6,  7
};

float HISTORY_TABLE[64][64] = {{0.0f}};

static uint8_t get_diagonal_range(BitBoard movers, BitBoard blockers) {
	uint8_t range_sum = 0;
	while (movers) {
		uint8_t square = __builtin_ctzll(movers);
		movers &= movers - 1;

		// Get range of bishop
		uint8_t range = 0;

		uint8_t rank = square / 8;
		uint8_t file = square % 8;
		uint8_t start_rank = rank;
		uint8_t start_file = file;

		while (rank < 7 && file < 7) {
			++rank;
			++file;
			if (blockers & (1ULL << (8 * rank + file))) {
				break;
			}
			++range;
		}

		rank = start_rank;
		file = start_file;

		while (rank > 0 && file < 7) {
			--rank;
			++file;
			if (blockers & (1ULL << (8 * rank + file))) {
				break;
			}
			++range;
		}

		rank = start_rank;
		file = start_file;

		while (rank < 7 && file > 0) {
			++rank;
			--file;
			if (blockers & (1ULL << (8 * rank + file))) {
				break;
			}
			++range;
		}

		rank = start_rank;
		file = start_file;

		while (rank > 0 && file > 0) {
			--rank;
			--file;
			if (blockers & (1ULL << (8 * rank + file))) {
				break;
			}
			++range;
		}
		range_sum += range;
	}
	return range_sum;
}

static uint8_t get_straight_range(BitBoard movers, BitBoard blockers) {
	uint8_t range_sum = 0;

	while (movers) {
		uint8_t square = __builtin_ctzll(movers);
		movers &= movers - 1;

		// Get range of rook 
		uint8_t range = 0;

		uint8_t rank = square / 8;
		uint8_t file = square % 8;
		uint8_t start_rank = rank;
		uint8_t start_file = file;

		while (rank < 7) {
			++rank;
			if (blockers & (1ULL << (8 * rank + file))) {
				break;
			}
			++range;
		}

		rank = start_rank;
		file = start_file;

		while (file < 7) {
			++file;
			if (blockers & (1ULL << (8 * rank + file))) {
				break;
			}
			++range;
		}

		rank = start_rank;
		file = start_file;

		while (rank > 0) {
			--rank;
			if (blockers & (1ULL << (8 * rank + file))) {
				break;
			}
			++range;
		}

		rank = start_rank;
		file = start_file;

		while (file > 0) {
			--file;
			if (blockers & (1ULL << (8 * rank + file))) {
				break;
			}
			++range;
		}

		range_sum += range;
	}

	return range_sum;
}

inline BitBoard get_pawn_attacks(BitBoard pawns, enum Color color) {
	BitBoard left_attacks = 0;
	BitBoard right_attacks = 0;
	if (color == WHITE) {
		left_attacks  = (pawns & ~FILE_A) << 7;
		right_attacks = (pawns & ~FILE_H) << 9;
	}
	else if (color == BLACK) {
		left_attacks  = (pawns & ~FILE_A) >> 9;
		right_attacks = (pawns & ~FILE_H) >> 7;
	}
	return left_attacks | right_attacks;
}

float eval_pawns(const Board* board) {
	float value = 0.0f;

	BitBoard white_pawns = board->pieces[WHITE_PAWN];
	BitBoard black_pawns = board->pieces[BLACK_PAWN];

	while (white_pawns) {
		uint8_t square = __builtin_ctzll(white_pawns);
		white_pawns &= white_pawns - 1;
		value += PAWN_VALUE + 0.125f * PAWN_TABLE[square];
	}

	while (black_pawns) {
		uint8_t square = __builtin_ctzll(black_pawns);
		black_pawns &= black_pawns - 1;
		value -= PAWN_VALUE + 0.125f * PAWN_TABLE[FLIP_TABLE[square]];
	}

	// Supporting pawns bonus
	BitBoard white_board = get_occupied_squares_color(board, WHITE);
	BitBoard black_board = get_occupied_squares_color(board, BLACK);

	BitBoard white_pawn_attacks = get_pawn_attacks(board->pieces[WHITE_PAWN], WHITE);
	BitBoard black_pawn_attacks = get_pawn_attacks(board->pieces[BLACK_PAWN], BLACK);

	value += 0.1f * __builtin_popcountll(white_pawn_attacks & white_board);
	value -= 0.1f * __builtin_popcountll(black_pawn_attacks & black_board);

	BitBoard white_pawns_blocked = (white_pawns >> 8) & white_board;
	BitBoard black_pawns_blocked = (black_pawns << 8) & black_board;

	value -= 0.25f * __builtin_popcountll(white_pawns_blocked);
	value += 0.25f * __builtin_popcountll(black_pawns_blocked);

	return value;
}

float eval_knights(const Board* board) {
	float value = 0.0f;

	BitBoard white_knights = board->pieces[WHITE_KNIGHT];
	BitBoard black_knights = board->pieces[BLACK_KNIGHT];

	while (white_knights) {
		uint8_t square = __builtin_ctzll(white_knights);
		white_knights &= white_knights - 1;
		value += KNIGHT_VALUE + 0.075f * KNIGHT_TABLE[square];
	}

	while (black_knights) {
		uint8_t square = __builtin_ctzll(black_knights);
		black_knights &= black_knights - 1;
		value -= KNIGHT_VALUE + 0.075f * KNIGHT_TABLE[FLIP_TABLE[square]];
	}

	return value;
}


float eval_bishops(const Board* board) {
	float value = 0.0f;

	BitBoard white_bishops = board->pieces[WHITE_BISHOP];
	BitBoard black_bishops = board->pieces[BLACK_BISHOP];

	BitBoard occupied = get_occupied_squares(board);

	uint8_t white_bishops_range_sum = get_diagonal_range(white_bishops, occupied);
	uint8_t black_bishops_range_sum = get_diagonal_range(black_bishops, occupied);

	value += 0.1f * (white_bishops_range_sum - black_bishops_range_sum);

	while (white_bishops) {
		uint8_t square = __builtin_ctzll(white_bishops);
		white_bishops &= white_bishops - 1;
		value += BISHOP_VALUE + 0.075f * BISHOP_TABLE[square];
	}

	while (black_bishops) {
		uint8_t square = __builtin_ctzll(black_bishops);
		black_bishops &= black_bishops - 1;
		value -= BISHOP_VALUE + 0.075f * BISHOP_TABLE[FLIP_TABLE[square]];
	}
	return value;
}

float eval_rooks(const Board* board) {
	float value = 0.0f;

	BitBoard white_rooks = board->pieces[WHITE_ROOK];
	BitBoard black_rooks = board->pieces[BLACK_ROOK];

	BitBoard occupied = get_occupied_squares(board);

	uint8_t white_rooks_range_sum = get_straight_range(white_rooks, occupied);
	uint8_t black_rooks_range_sum = get_straight_range(black_rooks, occupied);

	value += 0.05f * (white_rooks_range_sum - black_rooks_range_sum);

	while (white_rooks) {
		uint8_t square = __builtin_ctzll(white_rooks);
		white_rooks &= white_rooks - 1;
		value += ROOK_VALUE + 0.075f * ROOK_TABLE[square];
	}

	while (black_rooks) {
		uint8_t square = __builtin_ctzll(black_rooks);
		black_rooks &= black_rooks - 1;
		value -= ROOK_VALUE + 0.075f * ROOK_TABLE[FLIP_TABLE[square]];
	}
	return value;
}

float eval_queens(const Board* board) {
	float value = 0.0f;

	BitBoard white_queens = board->pieces[WHITE_QUEEN];
	BitBoard black_queens = board->pieces[BLACK_QUEEN];

	while (white_queens) {
		uint8_t square = __builtin_ctzll(white_queens);
		white_queens &= white_queens - 1;
		value += QUEEN_VALUE + 0.075f * QUEEN_TABLE[square];
	}

	while (black_queens) {
		uint8_t square = __builtin_ctzll(black_queens);
		black_queens &= black_queens - 1;
		value -= QUEEN_VALUE + 0.075f * QUEEN_TABLE[FLIP_TABLE[square]];
	}

	return value;
}

float eval_kings(const Board* board) {
	float value = 0.0f;

	const float* table = (__builtin_popcountll(get_occupied_squares(board)) > 16) ? KING_TABLE : KING_TABLE_ENDGAME;

	BitBoard white_king = board->pieces[WHITE_KING];
	BitBoard black_king = board->pieces[BLACK_KING];

	while (white_king) {
		uint8_t square = __builtin_ctzll(white_king);
		white_king &= white_king - 1;
		value += (KING_VALUE + 0.1f * table[square]);
	}

	while (black_king) {
		uint8_t square = __builtin_ctzll(black_king);
		black_king &= black_king - 1;
		value -= (KING_VALUE + 0.1f * table[FLIP_TABLE[square]]);
	}

	return value;
}

float eval_board(const Board* board) {
	float eval = 0.0f;

	eval += eval_pawns(board);
	eval += eval_knights(board);
	eval += eval_bishops(board);
	eval += eval_rooks(board);
	eval += eval_queens(board);
	eval += eval_kings(board);

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
	float maximizing_factor = (1.0f - 2.0f * color);

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
		// TODO: LMR

		++*nodes_visited;
		Move move = moves.moves[idx];
		Board new_board = *board;
		uint8_t from, to;
		decode_move(move, &from, &to);

		_make_move(&new_board, from, to);
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
			float inc = 0.000001f * ((float)depth * (float)depth);
			HISTORY_TABLE[from][to] += inc;

			if (HISTORY_TABLE[from][to] > 10.0f) {
				printf("HISTORY_TABLE overflow\n");
				exit(1);
			}
			break;
		}
	}
	return best_score;
}

Move get_best_move(const Board* board, enum Color color, int depth, uint64_t* nodes_visited) {
	MoveList moves;
	init_move_list(&moves);
	get_legal_moves(board, &moves, color);

	float best_score = -INF;
	Move best_move = moves.moves[0];

	*nodes_visited = 0;

	clock_t start = clock();
	for (int idx = 0; idx < moves.count; ++idx) {
		++*nodes_visited;

		Move move = moves.moves[idx];
		Board new_board = *board;
		uint8_t from, to;
		decode_move(move, &from, &to);
		_make_move(&new_board, from, to);
		float score = -minimax(
				&new_board, 
				!color, 
				1, 
				depth, 
				-INF, 
				INF,
				nodes_visited
				);
		if (score > best_score) {
			best_score = score;
			best_move = move;
		}
	}

	if (DEBUG) {
		printf("Nodes visited: %lu\n", *nodes_visited);
		printf("MNps: %f\n", (float)*nodes_visited / ((float)(clock() - start) / CLOCKS_PER_SEC) / 1000000.0f);
	}
	return best_move;
}


Move get_best_move_id(
		const Board* board, 
		enum Color color, 
		int max_depth,
		uint64_t* nodes_visited
		) {
    MoveList moves;
    init_move_list(&moves);
    get_legal_moves(board, &moves, color);

    Move  best_move = moves.moves[0];
    float best_score;
    *nodes_visited = 0;

	if ((color == WHITE) && (MOVE_NUMBER % 10 == 9)) {
		for (int idx = 0; idx < 4096; ++idx) {
			HISTORY_TABLE[idx / 64][idx % 64] *= 0.5f;
		}
	}

    clock_t start = clock();

    for (int depth = 1; depth <= max_depth; ++depth) {
        best_score = -INF;

        for (int idx = 0; idx < moves.count; ++idx) {
            ++*nodes_visited;

            Move move = moves.moves[idx];
            Board new_board = *board;


			uint8_t from, to;
			decode_move(move, &from, &to);

			float last_history = HISTORY_TABLE[from][to];

            _make_move(&new_board, from, to);
            float score = -minimax(
                &new_board, 
                !color, 
                1, 
                depth, 
                -INF, 
                INF,
                nodes_visited
            );

            if (score > best_score) {
                best_score = score;
                best_move = move;
            }
			moves.move_scores[idx] += HISTORY_TABLE[from][to] - last_history;
        }
		// Reorder moves based on scores
		insertion_sort(&moves, moves.count);

        // Optional: Check time or other stopping criteria to break early
        // if (time_elapsed(start) > time_limit) break;
    }

	if (DEBUG) {
		printf("KNodes visited: %lu\n", *nodes_visited / 1000);
		printf("MNps: %f\n", (float)*nodes_visited / ((float)(clock() - start) / CLOCKS_PER_SEC) / 1000000.0f);
	}

    return best_move;
}

inline float calc_mvv_lva_score(const Board* board, const Move* move) {
	uint8_t from, to;
	decode_move(*move, &from, &to);

	uint8_t attacker = board->piece_at[from];
	uint8_t victim   = board->piece_at[to];

	if (victim == EMPTY_SQUARE) return HISTORY_TABLE[from][to];
	// Offset by QUEEN_VALUE to make sure that the score 
	// is always positive (excluding king sacrifices, lol)
	return HISTORY_TABLE[from][to] + (QUEEN_VALUE + PIECE_VALUES[victim] - PIECE_VALUES[attacker]);
}

inline float _calc_mvv_lva_score(
		const Board* board, 
		uint8_t from,
		uint8_t to
		) {
	uint8_t attacker = board->piece_at[from];
	uint8_t victim   = board->piece_at[to];

	if (victim == EMPTY_SQUARE) return HISTORY_TABLE[from][to];
	// Offset by QUEEN_VALUE to make sure that the score 
	// is always positive (excluding king sacrifices, lol)
	return HISTORY_TABLE[from][to] + (QUEEN_VALUE + PIECE_VALUES[victim] - PIECE_VALUES[attacker]);
}
