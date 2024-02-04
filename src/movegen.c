#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
#include <omp.h>

#include "board.h"
#include "movegen.h"
#include "eval.h"

// static inline uint8_t min(const uint8_t a, const uint8_t b) {
	// return a < b ? a : b;
// }

inline void create_move(Move* move, uint8_t from, uint8_t to) {
	*move = (from & SQUARE_IDX_MASK) | (to << DST_SQUARE_SHIFT);
}

inline void decode_move(const Move* move, uint8_t* from, uint8_t* to) {
	*from = *move & SQUARE_IDX_MASK;
	*to   = (*move >> DST_SQUARE_SHIFT) & SQUARE_IDX_MASK;
}

void init_move_list(MoveList* list) {
	list->count = 0;
}

inline void reset_move_list(MoveList* list) {
	list->count = 0;
}

void add_move(MoveList* list, Move move) {
	list->moves[list->count] = move;
	++list->count;

	if (list->count == 267) {
		printf("Error: Move list overflow\n");
		exit(1);
	}

	list->move_scores[list->count] = 0.0f;
}

Move get_move_from_string(const char* move) {
	char from[2] = {move[0], move[1]};
	char to[2]   = {move[2], move[3]};
	uint8_t from_index = translate_square_from_char(from);
	uint8_t to_index   = translate_square_from_char(to);

	Move m;
	create_move(&m, from_index, to_index);

	return m;
}

const char* get_string_from_move(Move move) {
	uint8_t from, to;
	decode_move(&move, &from, &to);

	static char str[5];
	str[0] = translate_square_from_index(from)[0];
	str[1] = translate_square_from_index(from)[1];
	str[2] = translate_square_from_index(to)[0];
	str[3] = translate_square_from_index(to)[1];
	str[4] = '\0';
	return str;
}

bool is_move_legal(MoveList* legal_moves, Move move) {
	for (int idx = 0; idx < legal_moves->count; ++idx) {
		Move m = legal_moves->moves[idx];
		if (m == move) {
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
		printf("%s", translate_square_from_index(move & SQUARE_IDX_MASK));
		printf("%s\n", translate_square_from_index((move >> DST_SQUARE_SHIFT) & SQUARE_IDX_MASK));
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

	Move move = 0;
	if (is_empty(board, one_forward)) {
		create_move(&move, square, one_forward);
		add_move(list, move);

		if (is_starting_rank) {
			uint8_t two_forward = square + (color == WHITE ? 16 : -16);
			if (is_empty(board, two_forward)) {
				create_move(&move, square, two_forward);
				add_move(list, move);
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
				create_move(&move, square, capture_left);
				add_move(list, move);
			}
		}

		if (can_capture_right) {
			uint8_t capture_right = square + 9;
			if (is_occupied_by(board, capture_right) == BLACK) {
				create_move(&move, square, capture_right);
				add_move(list, move);
			}
		}
	}
	else {
		if (can_capture_left) {
			uint8_t capture_left = square - 9;
			if (is_occupied_by(board, capture_left) == WHITE) {
				create_move(&move, square, capture_left);
				add_move(list, move);
			}
		}

		if (can_capture_right) {
			uint8_t capture_right = square - 7;
			if (is_occupied_by(board, capture_right) == WHITE) {
				create_move(&move, square, capture_right);
				add_move(list, move);
			}
		}
	}
	// TODO: En passant. In the future. Last move dependence.
}

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

inline void get_knight_moves_piece(
		const Board* board,
		MoveList* list,
		const BitBoard* self_mask,
		uint8_t square,
		enum Color color
		) {
	uint64_t moves = KNIGHT_MOVES[square];
	moves &= ~*self_mask;

	Move move = 0;
	while (moves) {
		uint8_t to = __builtin_ctzll(moves);
		create_move(&move, square, to);
		add_move(list, move);
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

/*
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

	Move move = 0;

	if (upper_left_range > 0) {
		uint8_t local_rank = rank + 1;
		uint8_t local_file = file - 1;

		while (local_rank <= 7 && local_file >= 0) {
			uint8_t to = local_rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, to);
			create_move(&move, square, to);

			if (occupant == EMPTY) {
				add_move(list, move);
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
				add_move(list, move);
				break;
			}
		}
	}

	if (upper_right_range > 0) {
		uint8_t local_rank = rank + 1;
		uint8_t local_file = file + 1;

		while (local_rank <= 7 && local_file <= 7) {
			uint8_t to = local_rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, to);
			create_move(&move, square, to);

			if (occupant == EMPTY) {
				add_move(list, move);
				local_rank++;
				local_file++;
				continue;
			}
			else if (occupant == color) {
				break;
			}
			else {
				add_move(list, move);
				break;
			}
		}
	}

	if (lower_right_range > 0) {
		uint8_t local_rank = rank - 1;
		uint8_t local_file = file + 1;

		while (local_rank >= 0 && local_file <= 7) {
			uint8_t to = local_rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, to);
			create_move(&move, square, to);

			if (occupant == EMPTY) {
				add_move(list, move);
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
				add_move(list, move);
				break;
			}
		}
	}

	if (lower_left_range > 0) {
		uint8_t local_rank = rank - 1;
		uint8_t local_file = file - 1;

		while (local_rank >= 0 && local_file >= 0) {
			uint8_t to = local_rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, to);
			create_move(&move, square, to);

			if (occupant == EMPTY) {
				add_move(list, move);

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
				add_move(list, move);
				break;
			}
		}
	}
}
*/

void get_bishop_moves_piece(
		const Board* board,
		MoveList* list,
		uint8_t square,
		enum Color color
		) {
	BitBoard self_mask = get_occupied_squares_color(board, color);
	BitBoard occupied = get_occupied_squares(board);

	BitBoard occupancy = occupied & BISHOP_MASKS[square];

	uint64_t magic_hash_key = magic_hash(
			occupancy,
			BISHOP_MAGICS[square],
			BISHOP_SHIFT[square]
			);
	BitBoard moves = BISHOP_MOVES[square][magic_hash_key];
	moves &= ~self_mask;

	while (moves) {
		uint8_t to = __builtin_ctzll(moves);
		Move move;
		create_move(&move, square, to);
		add_move(list, move);
		moves &= moves - 1;
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

	Move move = 0;

	if (upper_range > 0) {
		uint8_t local_rank = rank + 1;

		while (local_rank <= 7) {
			uint8_t to = local_rank * 8 + file;
			enum Color occupant = is_occupied_by(board, to);
			create_move(&move, square, to);

			if (occupant == EMPTY) {
				add_move(list, move);
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
				add_move(list, move);
				break;
			}
		}
	}

	if (right_range > 0) {
		uint8_t local_file = file + 1;

		while (local_file <= 7) {
			uint8_t to = rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, to);
			create_move(&move, square, to);

			if (occupant == EMPTY) {
				add_move(list, move);
				local_file++;
				continue;
			}
			else if (occupant == color) {
				break;
			}
			else {
				add_move(list, move);
				break;
			}
		}
	}

	if (lower_range > 0) {
		uint8_t local_rank = rank - 1;

		while (local_rank >= 0) {
			uint8_t to = local_rank * 8 + file;
			enum Color occupant = is_occupied_by(board, to);
			create_move(&move, square, to);

			if (occupant == EMPTY) {
				add_move(list, move);
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
				add_move(list, move);
				break;
			}
		}
	}

	if (left_range > 0) {
		uint8_t local_file = file - 1;

		while (local_file >= 0) {
			uint8_t to = rank * 8 + local_file;
			enum Color occupant = is_occupied_by(board, to);
			create_move(&move, square, to);

			if (occupant == EMPTY) {
				add_move(list, move);
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
				add_move(list, move);
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

	Move move = 0;
	while (moves) {
		uint8_t to = __builtin_ctzll(moves);
		create_move(&move, square, to);
		add_move(list, move);
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
		create_move(&move, square, square + 2);
		add_move(list, move);
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
		create_move(&move, square, square - 2);
		add_move(list, move);
	}
}

/*
static inline void swap(MoveList* list, int a, int b) {
	// Swap move and move_scores
	Move temp_move   = list->moves[a];
	float temp_score = list->move_scores[a];

	list->moves[a] = list->moves[b];
	list->move_scores[a] = list->move_scores[b];

	list->moves[b] = temp_move;
	list->move_scores[b] = temp_score;
}

static inline int median_of_three(MoveList* list, int low, int high) {
    int mid = low + (high - low) / 2;
    if (list->move_scores[low] > list->move_scores[mid])
        swap(list, low, mid);
    if (list->move_scores[low] > list->move_scores[high])
        swap(list, low, high);
    if (list->move_scores[mid] > list->move_scores[high])
        swap(list, mid, high);
    return mid;
}

static inline int partition(MoveList* list, int low, int high) {
	// int pivot = list->move_scores[high];
	int pivot_index = median_of_three(list, low, high);
	float pivot = list->move_scores[pivot_index];
	int i = low - 1;

	for (int j = low; j <= high - 1; ++j) {
		if (list->move_scores[j] >= pivot) {
			++i;
			swap(list, i, j);
		}
	}

	swap(list, i + 1, high);
	return i + 1;
}

static inline void quicksort(MoveList* list, int low, int high) {
    while (low < high) {
        int pi = partition(list, low, high);

        // Optimize for tail recursion
        if (pi - low < high - pi) {
            quicksort(list, low, pi - 1);
            low = pi + 1;
        } else {
            quicksort(list, pi + 1, high);
            high = pi - 1;
        }
    }
}
*/

static inline void insertion_sort(MoveList* list, int n) {
	Move key;
	float key_score;
	int j;
	for (int i = 1; i < n; ++i) {
		key = list->moves[i];
		key_score = list->move_scores[i];
		j = i - 1;

		while (j >= 0 && list->move_scores[j] < key_score) {
			list->moves[j + 1] = list->moves[j];
			list->move_scores[j + 1] = list->move_scores[j];
			--j;
		}

		list->moves[j + 1] = key;
		list->move_scores[j + 1] = key_score;
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
		list->move_scores[idx] = calc_mvv_lva_score(board, &list->moves[idx]);
	}

	// Sort moves
	// quicksort(list, 0, list->count - 1);
	insertion_sort(list, list->count);
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
		uint8_t from, to;
		decode_move(&move, &from, &to);

		Board new_board = *board;
		_make_move(&new_board, from, to);

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
	printf("==                   MOVEGEN PERF                  ==\n");
	printf("=====================================================\n");
	printf("Time spent:    %f\n", time_spent);
	printf("Nodes visited: %lu\n", nodes_visited);
	printf("MN/s:          %f\n", nodes_visited / time_spent / 1000000);
	printf("=====================================================\n");
}
