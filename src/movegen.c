#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <omp.h>

#include "board.h"
#include "movegen.h"
#include "eval.h"

static inline uint8_t min(const uint8_t a, const uint8_t b) {
	return a < b ? a : b;
}

void init_move_list(MoveList* list) {
	list->count = 0;
}

inline void reset_move_list(MoveList* list) {
	list->count = 0;
}

void add_move(MoveList* list, uint8_t from, uint8_t to) {
	list->moves[list->count].from = from;
	list->moves[list->count].to   = to;
	++list->count;

	if (list->count == 267) {
		printf("Error: Move list overflow\n");
		exit(1);
	}

	list->moves[list->count].mvv_lva_score = 0.0f;
}

Move get_move_from_string(const char* move) {
	char from[2] = {move[0], move[1]};
	char to[2]   = {move[2], move[3]};
	Move m = {
		translate_square_from_char(from),
		translate_square_from_char(to)
	};
	return m;
}

const char* get_string_from_move(Move move) {
	static char str[5];
	str[0] = translate_square_from_index(move.from)[0];
	str[1] = translate_square_from_index(move.from)[1];
	str[2] = translate_square_from_index(move.to)[0];
	str[3] = translate_square_from_index(move.to)[1];
	str[4] = '\0';
	return str;
}

bool is_move_legal(MoveList* legal_moves, Move move) {
	for (int idx = 0; idx < legal_moves->count; ++idx) {
		Move m = legal_moves->moves[idx];
		if (m.from == move.from && m.to == move.to) {
			return true;
		}
	}
	return false;
}

bool game_over(const Board* board) {
	return (__builtin_popcountll(board->pieces[WHITE_KING] | board->pieces[BLACK_KING]) != 2);
}

void print_legal_moves(const Board* board, enum Color color) {
	MoveList list;
	init_move_list(&list);

	get_legal_moves(board, &list, color);

	printf("Number of moves: %d\n", list.count);
	for (int idx = 0; idx < list.count; ++idx) {
		Move move = list.moves[idx];
		printf("%s", translate_square_from_index(move.from));
		printf("%s\n", translate_square_from_index(move.to));
	}
}

void get_pawn_moves_piece(
		const Board* board, 
		MoveList* list, 
		uint8_t square,
		enum Color color
		) {
	uint8_t rank = square / 8;
	uint8_t file = square % 8;

	const uint8_t starting_rank = color == WHITE ? 1 : 6;
	bool is_starting_rank = (rank == starting_rank);

	// Forward moves
	uint8_t one_forward = square + (color == WHITE ? 8 : -8);
	if (is_empty(board, one_forward)) {
		add_move(list, square, one_forward);

		if (is_starting_rank) {
			uint8_t two_forward = square + (color == WHITE ? 16 : -16);
			if (is_empty(board, two_forward)) {
				add_move(list, square, two_forward);
			}
		}
	}

	// Captures
	bool can_capture_left  = (file > 0);
	bool can_capture_right = (file < 7);

	if (color == WHITE) {
		if (can_capture_left) {
			uint8_t capture_left = square + 7;
			if (is_occupied_by(board, capture_left) == BLACK) {
				add_move(list, square, capture_left);
			}
		}

		if (can_capture_right) {
			uint8_t capture_right = square + 9;
			if (is_occupied_by(board, capture_right) == BLACK) {
				add_move(list, square, capture_right);
			}
		}
	}
	else {
		if (can_capture_left) {
			uint8_t capture_left = square - 9;
			if (is_occupied_by(board, capture_left) == WHITE) {
				add_move(list, square, capture_left);
			}
		}

		if (can_capture_right) {
			uint8_t capture_right = square - 7;
			if (is_occupied_by(board, capture_right) == WHITE) {
				add_move(list, square, capture_right);
			}
		}
	}
	// TODO: En passant. In the future. Last move dependence.
}

/*
void get_pawn_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		) {
	BitBoard pawns = board->pieces[WHITE_PAWN + 6 * color];

	while (pawns) {
		uint8_t square = __builtin_ctzll(pawns);
		get_pawn_moves_piece(board, list, square, color);
		pawns &= pawns - 1;
	}
}
*/

void get_pawn_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		) {
	BitBoard pawns = board->pieces[WHITE_PAWN + 6 * color];

	BitBoard occupied = get_occupied_squares(board);
	BitBoard opposing = get_occupied_squares_color(board, !color);

	BitBoard one_forward = (color == WHITE) ? (pawns << 8) : (pawns >> 8);

	BitBoard left_capture  = (color == WHITE) ? pawns << 7 : pawns >> 9;
	BitBoard right_capture = (color == WHITE) ? pawns << 9 : pawns >> 7;

	BitBoard one_forward_moves = one_forward & ~occupied;
	BitBoard two_forward_moves = (color == WHITE) 
								 ? ((one_forward_moves & RANK_3) << 8) 
								 : ((one_forward_moves & RANK_6) >> 8);
	two_forward_moves &= ~occupied;

	BitBoard left_capture_moves  = left_capture & opposing;
	BitBoard right_capture_moves = right_capture & opposing;

	while (one_forward_moves) {
		uint8_t square = __builtin_ctzll(one_forward_moves);
		add_move(list, square - (color == WHITE ? 8 : -8), square);
		one_forward_moves &= one_forward_moves - 1;
	}

	while (two_forward_moves) {
		uint8_t square = __builtin_ctzll(two_forward_moves);
		add_move(list, square - (color == WHITE ? 16 : -16), square);
		two_forward_moves &= two_forward_moves - 1;
	}

	while (left_capture_moves) {
		uint8_t square = __builtin_ctzll(left_capture_moves);
		add_move(list, square - (color == WHITE ? 7 : -7), square);
		left_capture_moves &= left_capture_moves - 1;
	}

	while (right_capture_moves) {
		uint8_t square = __builtin_ctzll(right_capture_moves);
		add_move(list, square - (color == WHITE ? 9 : -9), square);
		right_capture_moves &= right_capture_moves - 1;
	}
}

void get_knight_moves_piece(
		const Board* board,
		MoveList* list,
		const BitBoard* self_mask,
		uint8_t square,
		enum Color color
		) {
	uint64_t moves = KNIGHT_MOVES[square];
	moves &= ~*self_mask;

	while (moves) {
		uint8_t move = __builtin_ctzll(moves);
		add_move(list, square, move);
		moves &= moves - 1;
	}
}

void get_knight_moves(
		const Board* board,
		MoveList* list,
		enum Color color
		) {
	BitBoard knights = board->pieces[WHITE_KNIGHT + 6 * color];
	BitBoard self_mask = get_occupied_squares_color(board, color);

	while (knights) {
		uint8_t square = __builtin_ctzll(knights);
		get_knight_moves_piece(board, list, &self_mask, square, color);
		knights &= knights - 1;
	}
}

void get_bishop_moves_piece(
		const Board* board,
		MoveList* list,
		uint8_t square,
		enum Color color
		) {
	uint8_t rank = square / 8;
	uint8_t file = square % 8;

	uint8_t upper_right_range = min(7 - rank, 7 - file);
	uint8_t upper_left_range  = min(7 - rank, file);
	uint8_t lower_right_range = min(rank, 7 - file);
	uint8_t lower_left_range  = min(rank, file);

	if (upper_left_range > 0) {
		uint8_t local_rank = rank + 1;
		uint8_t local_file = file - 1;

		while (local_rank <= 7 && local_file >= 0) {
			uint8_t move = local_rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, move);
			if (occupant == EMPTY) {
				add_move(list, square, move);
				if (local_file == 0) {
					break;
				}
				local_rank++;
				local_file--;
				continue;
			}
			else if (occupant == color) {
				break;
			}
			else {
				add_move(list, square, move);
				break;
			}
		}
	}

	if (upper_right_range > 0) {
		uint8_t local_rank = rank + 1;
		uint8_t local_file = file + 1;

		while (local_rank <= 7 && local_file <= 7) {
			uint8_t move = local_rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, move);
			if (occupant == EMPTY) {
				add_move(list, square, move);
				local_rank++;
				local_file++;
				continue;
			}
			else if (occupant == color) {
				break;
			}
			else {
				add_move(list, square, move);
				break;
			}
		}
	}

	if (lower_right_range > 0) {
		uint8_t local_rank = rank - 1;
		uint8_t local_file = file + 1;

		while (local_rank >= 0 && local_file <= 7) {
			uint8_t move = local_rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, move);
			if (occupant == EMPTY) {
				add_move(list, square, move);
				if (local_rank == 0) {
					break;
				}
				local_rank--;
				local_file++;
				continue;
			}
			else if (occupant == color) {
				break;
			}
			else {
				add_move(list, square, move);
				break;
			}
		}
	}

	if (lower_left_range > 0) {
		uint8_t local_rank = rank - 1;
		uint8_t local_file = file - 1;

		while (local_rank >= 0 && local_file >= 0) {
			uint8_t move = local_rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, move);
			if (occupant == EMPTY) {
				add_move(list, square, move);

				if (local_rank == 0 || local_file == 0) {
					break;
				}
				local_rank--;
				local_file--;
				continue;
			}
			else if (occupant == color) {
				break;
			}
			else {
				add_move(list, square, move);
				break;
			}
		}
	}
}

void get_bishop_moves(
		const Board* board,
		MoveList* list,
		enum Color color
		) {
	BitBoard bishops = (color == WHITE) ? board->pieces[WHITE_BISHOP] : board->pieces[BLACK_BISHOP];

	while (bishops) {
		uint8_t square = __builtin_ctzll(bishops);
		get_bishop_moves_piece(board, list, square, color);
		bishops &= bishops - 1;
	}
}


void get_rook_moves_piece(
		const Board* board, 
		MoveList* list, 
		uint8_t square,
		enum Color color
		) {
	uint8_t rank = square / 8;
	uint8_t file = square % 8;

	uint8_t upper_range = 7 - rank;
	uint8_t right_range = 7 - file;
	uint8_t lower_range = rank;
	uint8_t left_range  = file;

	if (upper_range > 0) {
		uint8_t local_rank = rank + 1;

		while (local_rank <= 7) {
			uint8_t move = local_rank * 8 + file;
			enum Color occupant = is_occupied_by(board, move);
			if (occupant == EMPTY) {
				add_move(list, square, move);
				if (local_rank == 0) {
					break;
				}
				local_rank++;
				continue;
			}
			else if (occupant == color) {
				break;
			}
			else {
				add_move(list, square, move);
				break;
			}
		}
	}

	if (right_range > 0) {
		uint8_t local_file = file + 1;

		while (local_file <= 7) {
			uint8_t move = rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, move);
			if (occupant == EMPTY) {
				add_move(list, square, move);
				local_file++;
				continue;
			}
			else if (occupant == color) {
				break;
			}
			else {
				add_move(list, square, move);
				break;
			}
		}
	}

	if (lower_range > 0) {
		uint8_t local_rank = rank - 1;

		while (local_rank >= 0) {
			uint8_t move = local_rank * 8 + file;
			enum Color occupant = is_occupied_by(board, move);
			if (occupant == EMPTY) {
				add_move(list, square, move);
				if (local_rank == 0) {
					break;
				}
				local_rank--;
				continue;
			}
			else if (occupant == color) {
				break;
			}
			else {
				add_move(list, square, move);
				break;
			}
		}
	}

	if (left_range > 0) {
		uint8_t local_file = file - 1;

		while (local_file >= 0) {
			uint8_t move = rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, move);
			if (occupant == EMPTY) {
				add_move(list, square, move);
				if (local_file == 0) {
					break;
				}
				local_file--;
				continue;
			}
			else if (occupant == color) {
				break;
			}
			else {
				add_move(list, square, move);
				break;
			}
		}
	}
}

void get_rook_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		) {
	BitBoard rooks = (color == WHITE) ? board->pieces[WHITE_ROOK] : board->pieces[BLACK_ROOK];

	while (rooks) {
		uint8_t square = __builtin_ctzll(rooks);
		get_rook_moves_piece(board, list, square, color);
		rooks &= rooks - 1;
	}
}

void get_queen_moves_piece(
		const Board* board, 
		MoveList* list, 
		uint8_t square,
		enum Color color
		) {
	get_bishop_moves_piece(board, list, square, color);
	get_rook_moves_piece(board, list, square, color);
}

void get_queen_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		) {
	BitBoard queens = (color == WHITE) ? board->pieces[WHITE_QUEEN] : board->pieces[BLACK_QUEEN];

	while (queens) {
		uint8_t square = __builtin_ctzll(queens);
		get_queen_moves_piece(board, list, square, color);
		queens &= queens - 1;
	}
}

void get_king_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		) {
	uint8_t square;
	if (color == WHITE) {
		square = __builtin_ctzll(board->pieces[WHITE_KING]);
	}
	else {
		square = __builtin_ctzll(board->pieces[BLACK_KING]);
	}

	BitBoard mask = get_occupied_squares_color(board, color);
	uint64_t moves = KING_MOVES[square] & ~mask;
	moves &= ~mask;

	while (moves) {
		uint8_t move = __builtin_ctzll(moves);
		add_move(list, square, move);
		moves &= moves - 1;
	}

	// Check castling rights
	// uint8_t starting_square_king = (color == WHITE) ? 4 : 60;
	uint8_t offset = color * 56;
	uint8_t starting_square_king = 4 + offset;
	if (square != starting_square_king) return;

	// uint8_t starting_queens_rook = (color == WHITE) ? 0 : 56;
	// uint8_t starting_kings_rook  = (color == WHITE) ? 7 : 63;
	uint8_t starting_queens_rook = offset;
	uint8_t starting_kings_rook  = 7 + offset;

	BitBoard rooks = (color == WHITE) ? board->pieces[WHITE_ROOK] : board->pieces[BLACK_ROOK];
	BitBoard occupied = get_occupied_squares(board);

	if (
			(rooks & (1ULL << starting_kings_rook)) 
				&&
			!(occupied & (1ULL << (starting_square_king + 1)))
				&&
			!(occupied & (1ULL << (starting_square_king + 2)))
	   ) {
		add_move(list, square, square + 2);
	}

	if (
			(rooks & (1ULL << starting_queens_rook)) 
				&&
			!(occupied & (1ULL << (starting_square_king - 1)))
				&&
			!(occupied & (1ULL << (starting_square_king - 2)))
				&&
			!(occupied & (1ULL << (starting_square_king - 3)))
	   ) {
		add_move(list, square, square - 2);
	}
}

/*
static inline void swap(Move* a, Move* b) {
	Move tmp = *a;
	*a = *b;
	*b = tmp;
}

static inline int partition(Move* moves, int low, int high) {
	int pivot = moves[high].mvv_lva_score;
	int i = low - 1;

	for (int j = low; j <= high - 1; ++j) {
		if (moves[j].mvv_lva_score >= pivot) {
			++i;
			swap(&moves[i], &moves[j]);
		}
	}

	swap(&moves[i + 1], &moves[high]);
	return i + 1;
}

static inline void quicksort(Move* moves, int low, int high) {
	if (low < high) {
		int pi = partition(moves, low, high);

		quicksort(moves, low, pi - 1);
		quicksort(moves, pi + 1, high);
	}
}
*/

static inline void insertion_sort(Move* moves, int n) {
    for (int i = 1; i < n; i++) {
        Move key = moves[i];
        int j = i - 1;

        while (j >= 0 && moves[j].mvv_lva_score < key.mvv_lva_score) {
            moves[j + 1] = moves[j];
            j = j - 1;
        }
        moves[j + 1] = key;
    }
}


void get_legal_moves(
		const Board* board,
		MoveList* list,
		enum Color color
		) {
	get_pawn_moves(board, list, color);
	get_knight_moves(board, list, color);
	get_bishop_moves(board, list, color);
	get_rook_moves(board, list, color);
	get_queen_moves(board, list, color);
	get_king_moves(board, list, color);

	// Calc mvv lva
	for (int idx = 0; idx < list->count; ++idx) {
		calc_mvv_lva_score(board, &list->moves[idx]);
	}

	// Sort moves
	// quicksort(list->moves, 0, list->count - 1);
	insertion_sort(list->moves, list->count);
}

static void search_legal_moves(
		const Board* board,
		enum Color color,
		int max_depth,
		uint64_t* nodes_visited
		) {
	if (max_depth == 0) return;

	MoveList list;
	init_move_list(&list);
	get_legal_moves(board, &list, color);

	for (int idx = 0; idx < list.count; ++idx) {
		++(*nodes_visited);

		Move move = list.moves[idx];
		Board new_board = *board;
		_make_move(&new_board, move.from, move.to);

		search_legal_moves(&new_board, !color, max_depth - 1, nodes_visited);
	}
}


void perf_test(int max_depth) {
	Board board;
	init_board(&board);
	populate_board_from_fen(&board, STARTING_FEN);

	print_legal_moves(&board, WHITE);

    uint64_t nodes_visited = 0;

	clock_t start = clock();
	search_legal_moves(&board, WHITE, max_depth, &nodes_visited);
	clock_t end = clock();

	double time_spent = (double)(end - start) / CLOCKS_PER_SEC;
	printf("=====================================================\n");
	printf("==                    MOVEGEN PERF                 ==\n");
	printf("=====================================================\n");
	printf("Time spent:    %f\n", time_spent);
	printf("Nodes visited: %llu\n", nodes_visited);
	printf("MN/s:          %f\n", nodes_visited / time_spent / 1000000);
	printf("=====================================================\n");
}
