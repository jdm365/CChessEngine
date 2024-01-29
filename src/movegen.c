#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <stdbool.h>

#include "board.h"
#include "movegen.h"
#include "eval.h"

static uint8_t min(const uint8_t a, const uint8_t b) {
	return a < b ? a : b;
}

void init_move_list(MoveList* list) {
	list->count = 0;
	for (int idx = 0; idx < 267; ++idx) {
		list->moves[idx].mvv_lva_score = 0.0f;
	}
}

void reset_move_list(MoveList* list) {
	list->count = 0;
	for (int idx = 0; idx < 267; ++idx) {
		list->moves[idx].mvv_lva_score = 0.0f;
	}
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
	return (__builtin_popcountll(board->white_king | board->black_king) != 2);
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

	if (color == BLACK) {
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

void get_pawn_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		) {
	BitBoard pawns = color == WHITE ? board->white_pawns : board->black_pawns;

	while (pawns) {
		uint8_t square = __builtin_ctzll(pawns);
		get_pawn_moves_piece(board, list, square, color);
		pawns &= pawns - 1;
	}
}

void get_knight_moves_piece(
		const Board* board,
		MoveList* list,
		uint8_t square,
		enum Color color
		) {
	uint8_t rank = square / 8;
	uint8_t file = square % 8;

	bool can_move_two_up    = (rank < 6);
	bool can_move_two_down  = (rank > 1);
	bool can_move_two_left  = (file > 1);
	bool can_move_two_right = (file < 6);

	bool can_move_one_up    = (rank < 7);
	bool can_move_one_down  = (rank > 0);
	bool can_move_one_left  = (file > 0);
	bool can_move_one_right = (file < 7);

	if (can_move_two_up) {
		if (can_move_one_left) {
			uint8_t move = square + 15;
			if (is_occupied_by(board, move) != color) {
				add_move(list, square, move);
			}
		}

		if (can_move_one_right) {
			uint8_t move = square + 17;
			if (is_occupied_by(board, move) != color) {
				add_move(list, square, move);
			}
		}
	}

	if (can_move_two_down) {
		if (can_move_one_left) {
			uint8_t move = square - 17;
			if (is_occupied_by(board, move) != color) {
				add_move(list, square, move);
			}
		}

		if (can_move_one_right) {
			uint8_t move = square - 15;
			if (is_occupied_by(board, move) != color) {
				add_move(list, square, move);
			}
		}
	}

	if (can_move_two_left) {
		if (can_move_one_up) {
			uint8_t move = square + 6;
			if (is_occupied_by(board, move) != color) {
				add_move(list, square, move);
			}
		}

		if (can_move_one_down) {
			uint8_t move = square - 10;
			if (is_occupied_by(board, move) != color) {
				add_move(list, square, move);
			}
		}
	}

	if (can_move_two_right) {
		if (can_move_one_up) {
			uint8_t move = square + 10;
			if (is_occupied_by(board, move) != color) {
				add_move(list, square, move);
			}
		}

		if (can_move_one_down) {
			uint8_t move = square - 6;
			if (is_occupied_by(board, move) != color) {
				add_move(list, square, move);
			}
		}
	}
}

void get_knight_moves(
		const Board* board,
		MoveList* list,
		enum Color color
		) {
	BitBoard knights = (color == WHITE) ? board->white_knights : board->black_knights;

	while (knights) {
		uint8_t square = __builtin_ctzll(knights);
		get_knight_moves_piece(board, list, square, color);
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
	BitBoard bishops = (color == WHITE) ? board->white_bishops : board->black_bishops;

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
	BitBoard rooks = (color == WHITE) ? board->white_rooks : board->black_rooks;

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
	BitBoard queens = (color == WHITE) ? board->white_queens : board->black_queens;

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
	if (__builtin_popcountll(board->white_king | board->black_king) != 2) {
		return;
	}
	uint8_t square;
	if (color == WHITE) {
		square = __builtin_ctzll(board->white_king);
	}
	else {
		square = __builtin_ctzll(board->black_king);
	}

	uint8_t rank = square / 8;
	uint8_t file = square % 8;

	bool upper = (rank < 7);
	bool right = (file < 7);
	bool lower = (rank > 0);
	bool left  = (file > 0);

	bool upper_left  = (rank < 7) && (file > 0);
	bool upper_right = (rank < 7) && (file < 7);
	bool lower_right = (rank > 0) && (file < 7);
	bool lower_left  = (rank > 0) && (file > 0);

	if (upper) {
		uint8_t move = (rank + 1) * 8 + file;
		enum Color occupant = is_occupied_by(board, move);
		if (occupant != color) {
			add_move(list, square, move);
		}
	}

	if (upper_right) {
		uint8_t move = (rank + 1) * 8 + file + 1;
		enum Color occupant = is_occupied_by(board, move);
		if (occupant != color) {
			add_move(list, square, move);
		}
	}

	if (right) {
		uint8_t move = rank * 8 + file + 1;
		enum Color occupant = is_occupied_by(board, move);
		if (occupant != color) {
			add_move(list, square, move);
		}
	}

	if (lower_right) {
		uint8_t move = (rank - 1) * 8 + file + 1;
		enum Color occupant = is_occupied_by(board, move);
		if (occupant != color) {
			add_move(list, square, move);
		}
	}

	if (lower) {
		uint8_t move = (rank - 1) * 8 + file;
		enum Color occupant = is_occupied_by(board, move);
		if (occupant != color) {
			add_move(list, square, move);
		}
	}

	if (lower_left) {
		uint8_t move = (rank - 1) * 8 + file - 1;
		enum Color occupant = is_occupied_by(board, move);
		if (occupant != color) {
			add_move(list, square, move);
		}
	}

	if (left) {
		uint8_t move = rank * 8 + file - 1;
		enum Color occupant = is_occupied_by(board, move);
		if (occupant != color) {
			add_move(list, square, move);
		}
	}

	if (upper_left) {
		uint8_t move = (rank + 1) * 8 + file - 1;
		enum Color occupant = is_occupied_by(board, move);
		if (occupant != color) {
			add_move(list, square, move);
		}
	}

	// Check castling rights
	uint8_t starting_square_king = (color == WHITE) ? 4 : 60;
	if (square != starting_square_king) return;

	uint8_t starting_queens_rook = (color == WHITE) ? 0 : 56;
	uint8_t starting_kings_rook  = (color == WHITE) ? 7 : 63;

	BitBoard rooks = (color == WHITE) ? board->white_rooks : board->black_rooks;
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
	quicksort(list->moves, 0, list->count - 1);
}

