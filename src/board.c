#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

#include "board.h"


void init_board(Board* board) {
	board->white_pawns   = 0;
	board->white_knights = 0;
	board->white_bishops = 0;
	board->white_rooks   = 0;
	board->white_queens  = 0;
	board->white_king    = 0;

	board->black_pawns   = 0;
	board->black_knights = 0;
	board->black_bishops = 0;
	board->black_rooks   = 0;
	board->black_queens  = 0;
	board->black_king    = 0;
}

void set_bit(BitBoard* board, int index) {
	*board |= (1ULL << index);
}


uint8_t translate_square_from_char(const char* square) {
	char file = square[0];
	char rank = square[1];

	uint8_t square_index = 0;

	switch (rank) {
		case '1':
			square_index += 0;
			break;
		case '2':
			square_index += 8;
			break;
		case '3':
			square_index += 16;
			break;
		case '4':
			square_index += 24;
			break;
		case '5':
			square_index += 32;
			break;
		case '6':
			square_index += 40;
			break;
		case '7':
			square_index += 48;
			break;
		case '8':
			square_index += 56;
			break;
		default:
			printf("Invalid rank: %c\n", rank);
			exit(1);
	}

	switch (file) {
		case 'a':
			square_index += 0;
			break;
		case 'b':
			square_index += 1;
			break;
		case 'c':
			square_index += 2;
			break;
		case 'd':
			square_index += 3;
			break;
		case 'e':
			square_index += 4;
			break;
		case 'f':
			square_index += 5;
			break;
		case 'g':
			square_index += 6;
			break;
		case 'h':
			square_index += 7;
			break;
		default:
			printf("Invalid file: %c\n", file);
			exit(1);
	}

	return square_index;
}

const char* translate_square_from_index(uint8_t square) {
	static char square_str[3];
	square_str[0] = (square % 8) + 'a';
	square_str[1] = (square / 8) + '1';
	square_str[2] = '\0';
	return square_str;
}


void populate_board_from_fen(Board* board, const char* fen) {
	int square_cntr = 0;
	int fen_idx = 0;
	while (square_cntr < 64) {
		char piece = fen[fen_idx++];
		if (piece == '/') {
			continue;
		}
		if (piece == ' ') {
			break;
		}
		int rank = square_cntr / 8;
		int file = square_cntr % 8;
		int square = 63 - (rank * 8) - (7 - file);
		switch (piece) {
			case 'P':
				set_bit(&board->white_pawns,   square);
				break;
			case 'N':
				set_bit(&board->white_knights, square);
				break;
			case 'B':
				set_bit(&board->white_bishops, square);
				break;
			case 'R':
				set_bit(&board->white_rooks,   square);
				break;
			case 'Q':
				set_bit(&board->white_queens,  square);
				break;
			case 'K':
				set_bit(&board->white_king,    square);
				break;
			case 'p':
				set_bit(&board->black_pawns,   square);
				break;
			case 'n':
				set_bit(&board->black_knights, square);
				break;
			case 'b':
				set_bit(&board->black_bishops, square);
				break;
			case 'r':
				set_bit(&board->black_rooks,   square);
				break;
			case 'q':
				set_bit(&board->black_queens,  square);
				break;
			case 'k':
				set_bit(&board->black_king,    square);
				break;
			case '1':
				square_cntr += 0;
				break;
			case '2':
				square_cntr += 1;
				break;
			case '3':
				square_cntr += 2;
				break;
			case '4':
				square_cntr += 3;
				break;
			case '5':
				square_cntr += 4;
				break;
			case '6':
				square_cntr += 5;
				break;
			case '7':
				square_cntr += 6;
				break;
			case '8':
				square_cntr += 7;
				break;
			default:
				printf("Invalid piece: %c\n", piece);
				exit(1);
		}
		square_cntr++;
	}
}

void print_board(Board* board) {
	printf("\n  +---+---+---+---+---+---+---+---+\n");
	for (int rank = 7; rank >= 0; --rank) {
		printf("%d |", rank + 1);
		for (int file = 0; file < 8; ++file) {
			int square_index = rank * 8 + file;
			if (board->white_pawns & (1ULL << square_index)) {
				printf(" P |");
			} else if (board->white_knights & (1ULL << square_index)) {
				printf(" N |");
			} else if (board->white_bishops & (1ULL << square_index)) {
				printf(" B |");
			} else if (board->white_rooks & (1ULL << square_index)) {
				printf(" R |");
			} else if (board->white_queens & (1ULL << square_index)) {
				printf(" Q |");
			} else if (board->white_king & (1ULL << square_index)) {
				printf(" K |");
			} else if (board->black_pawns & (1ULL << square_index)) {
				printf(" p |");
			} else if (board->black_knights & (1ULL << square_index)) {
				printf(" n |");
			} else if (board->black_bishops & (1ULL << square_index)) {
				printf(" b |");
			} else if (board->black_rooks & (1ULL << square_index)) {
				printf(" r |");
			} else if (board->black_queens & (1ULL << square_index)) {
				printf(" q |");
			} else if (board->black_king & (1ULL << square_index)) {
				printf(" k |");
			} else {
				printf("   |");
			}
		}
		printf("\n");
		printf("  +---+---+---+---+---+---+---+---+\n");
	}
	printf("    a   b   c   d   e   f   g   h\n");
}

void print_bitboard(uint64_t bit_board) {
	printf("\n  +---+---+---+---+---+---+---+---+\n");
	for (int rank = 7; rank >= 0; --rank) {
		printf("%d |", rank + 1);
		for (int file = 0; file < 8; ++file) {
			int square_index = rank * 8 + file;
			if (bit_board & (1ULL << square_index)) {
				printf(" X |");
			} else {
				printf("   |");
			}
		}
		printf("\n");
		printf("  +---+---+---+---+---+---+---+---+\n");
	}
	printf("    a   b   c   d   e   f   g   h\n");
}

void make_move(Board* board, const char* move) {
	char* first_two_chars = malloc(3);
	strncpy(first_two_chars, move, 2);
	uint8_t from_square = translate_square_from_char(first_two_chars);
	free(first_two_chars);

	char* last_two_chars = malloc(3);
	strncpy(last_two_chars, move + 2, 2);
	uint8_t to_square = translate_square_from_char(last_two_chars);
	free(last_two_chars);

	_make_move(board, from_square, to_square);
}

void _make_move(
		Board* board, 
		uint8_t from_square, 
		uint8_t to_square
		) {
	// Check if castling
	if (
			(from_square == 4 || from_square == 60)
				&& 
			(abs(from_square - to_square) == 2)
				&&
			((board->white_king & (1ULL << from_square)) || (board->black_king & (1ULL << from_square)))
		) {
		_castle(board, from_square, to_square);
		return;
	}

	uint64_t from_square_mask = (1ULL << from_square);
	uint64_t to_square_mask   = (1ULL << to_square);

	// Get piece at from_square and remove
	enum ColoredPiece piece;
	if (board->white_pawns & from_square_mask) {
		piece = WHITE_PAWN;
		board->white_pawns &= ~from_square_mask;
	} else if (board->white_knights & from_square_mask) {
		piece = WHITE_KNIGHT;
		board->white_knights &= ~from_square_mask;
	} else if (board->white_bishops & from_square_mask) {
		piece = WHITE_BISHOP;
		board->white_bishops &= ~from_square_mask;
	} else if (board->white_rooks & from_square_mask) {
		piece = WHITE_ROOK;
		board->white_rooks &= ~from_square_mask;
	} else if (board->white_queens & from_square_mask) {
		piece = WHITE_QUEEN;
		board->white_queens &= ~from_square_mask;
	} else if (board->white_king & from_square_mask) {
		piece = WHITE_KING;
		board->white_king &= ~from_square_mask;
	} 

	else if (board->black_pawns & from_square_mask) {
		piece = BLACK_PAWN;
		board->black_pawns &= ~from_square_mask;
	} else if (board->black_knights & from_square_mask) {
		piece = BLACK_KNIGHT;
		board->black_knights &= ~from_square_mask;
	} else if (board->black_bishops & from_square_mask) {
		piece = BLACK_BISHOP;
		board->black_bishops &= ~from_square_mask;
	} else if (board->black_rooks & from_square_mask) {
		piece = BLACK_ROOK;
		board->black_rooks &= ~from_square_mask;
	} else if (board->black_queens & from_square_mask) {
		piece = BLACK_QUEEN;
		board->black_queens &= ~from_square_mask;
	} else if (board->black_king & from_square_mask) {
		piece = BLACK_KING;
		board->black_king &= ~from_square_mask;
	} 

	else {
		printf("No piece found at from_square\n");
		printf("from_square: %d\n", from_square);
		printf("to_square: %d\n", to_square);
		print_board(board);
		// print_bitboard(board->white_king);
		exit(1);
	}

	// Check promotion
	if (piece == WHITE_PAWN && to_square >= 56) {
		piece = WHITE_QUEEN;
	} else if (piece == BLACK_PAWN && to_square <= 7) {
		piece = BLACK_QUEEN;
	}

	// Remove any pieces from to_square
	board->white_pawns   &= ~to_square_mask;
	board->white_knights &= ~to_square_mask;
	board->white_bishops &= ~to_square_mask;
	board->white_rooks   &= ~to_square_mask;
	board->white_queens  &= ~to_square_mask;
	board->white_king    &= ~to_square_mask;

	board->black_pawns   &= ~to_square_mask;
	board->black_knights &= ~to_square_mask;
	board->black_bishops &= ~to_square_mask;
	board->black_rooks   &= ~to_square_mask;
	board->black_queens  &= ~to_square_mask;
	board->black_king    &= ~to_square_mask;

	// Move piece to to_square
	switch (piece) {
		case WHITE_PAWN:
			board->white_pawns |= to_square_mask;
			break;
		case WHITE_KNIGHT:
			board->white_knights |= to_square_mask;
			break;
		case WHITE_BISHOP:
			board->white_bishops |= to_square_mask;
			break;
		case WHITE_ROOK:
			board->white_rooks |= to_square_mask;
			break;
		case WHITE_QUEEN:
			board->white_queens |= to_square_mask;
			break;
		case WHITE_KING:
			board->white_king |= to_square_mask;
			break;

		case BLACK_PAWN:
			board->black_pawns |= to_square_mask;
			break;
		case BLACK_KNIGHT:
			board->black_knights |= to_square_mask;
			break;
		case BLACK_BISHOP:
			board->black_bishops |= to_square_mask;
			break;
		case BLACK_ROOK:
			board->black_rooks |= to_square_mask;
			break;
		case BLACK_QUEEN:
			board->black_queens |= to_square_mask;
			break;
		case BLACK_KING:
			board->black_king |= to_square_mask;
			break;

		default:
			printf("Invalid piece: %llu\n", (uint64_t)piece);
			exit(1);
	}
}

void _castle(Board* board, uint8_t from_square, uint8_t to_square) {
	uint64_t from_square_mask = (1ULL << from_square);
	uint64_t to_square_mask   = (1ULL << to_square);

	uint8_t rook_from_square = (to_square > from_square) ? to_square + 1 : to_square - 2;
	uint8_t rook_to_square   = (to_square > from_square) ? to_square - 1 : to_square + 1;
	bool is_white = (from_square == 4);

	// Remove king from from_square and rook from appropriate square
	// Guaranteed no pieces captured, so don't remove anything from to squares.

	if (is_white) {
		board->white_king &= ~from_square_mask;
		board->white_rooks &= ~(1ULL << rook_from_square);

		board->white_king |= to_square_mask;
		board->white_rooks |= (1ULL << rook_to_square);
	} 
	else {
		board->black_king &= ~from_square_mask;
		board->black_rooks &= ~(1ULL << rook_from_square);

		board->black_king |= to_square_mask;
		board->black_rooks |= (1ULL << rook_to_square);
	}
}

enum Color is_occupied_by(const Board* board, uint8_t square) {
	BitBoard white_board = board->white_pawns;
	white_board |= board->white_knights;
	white_board |= board->white_bishops;
	white_board |= board->white_rooks;
	white_board |= board->white_queens;
	white_board |= board->white_king;

	if (white_board & (1ULL << square)) {
		return WHITE;
	}

	BitBoard black_board = board->black_pawns;
	black_board |= board->black_knights;
	black_board |= board->black_bishops;
	black_board |= board->black_rooks;
	black_board |= board->black_queens;
	black_board |= board->black_king;

	if (black_board & (1ULL << square)) {
		return BLACK;
	}

	return EMPTY;
}

enum ColoredPiece colored_piece_at(const Board* board, uint8_t square) {
	if (board->white_pawns & (1ULL << square)) {
		return WHITE_PAWN;
	} else if (board->white_knights & (1ULL << square)) {
		return WHITE_KNIGHT;
	} else if (board->white_bishops & (1ULL << square)) {
		return WHITE_BISHOP;
	} else if (board->white_rooks & (1ULL << square)) {
		return WHITE_ROOK;
	} else if (board->white_queens & (1ULL << square)) {
		return WHITE_QUEEN;
	} else if (board->white_king & (1ULL << square)) {
		return WHITE_KING;
	} 

	else if (board->black_pawns & (1ULL << square)) {
		return BLACK_PAWN;
	} else if (board->black_knights & (1ULL << square)) {
		return BLACK_KNIGHT;
	} else if (board->black_bishops & (1ULL << square)) {
		return BLACK_BISHOP;
	} else if (board->black_rooks & (1ULL << square)) {
		return BLACK_ROOK;
	} else if (board->black_queens & (1ULL << square)) {
		return BLACK_QUEEN;
	} else if (board->black_king & (1ULL << square)) {
		return BLACK_KING;
	} 

	else {
		return EMPTY_SQUARE;
	}
}

enum Piece piece_at(const Board* board, uint8_t square) {
	if ((board->white_pawns | board->black_pawns) & (1ULL << square)) {
		return PAWN;
	} else if ((board->white_knights | board->black_knights) & (1ULL << square)) {
		return KNIGHT;
	} else if ((board->white_bishops | board->black_bishops) & (1ULL << square)) {
		return BISHOP;
	} else if ((board->white_rooks | board->black_rooks) & (1ULL << square)) {
		return ROOK;
	} else if ((board->white_queens | board->black_queens) & (1ULL << square)) {
		return QUEEN;
	} else if ((board->white_king | board->black_king) & (1ULL << square)) {
		return KING;
	} 

	else {
		return EMPTY_PIECE;
	}
}


bool is_empty(const Board* board, uint8_t square) {
	return is_occupied_by(board, square) == EMPTY;
}

BitBoard get_occupied_squares(const Board* board) {
	BitBoard occupied_squares = 0;

	occupied_squares |= board->white_pawns;
	occupied_squares |= board->white_knights;
	occupied_squares |= board->white_bishops;
	occupied_squares |= board->white_rooks;
	occupied_squares |= board->white_queens;
	occupied_squares |= board->white_king;

	occupied_squares |= board->black_pawns;
	occupied_squares |= board->black_knights;
	occupied_squares |= board->black_bishops;
	occupied_squares |= board->black_rooks;
	occupied_squares |= board->black_queens;
	occupied_squares |= board->black_king;

	return occupied_squares;
}

BitBoard get_occupied_squares_color(const Board* board, enum Color color) {
	BitBoard occupied_squares = 0;

	if (color == WHITE) {
		occupied_squares |= board->white_pawns;
		occupied_squares |= board->white_knights;
		occupied_squares |= board->white_bishops;
		occupied_squares |= board->white_rooks;
		occupied_squares |= board->white_queens;
		occupied_squares |= board->white_king;
	} 
	else if (color == BLACK) {
		occupied_squares |= board->black_pawns;
		occupied_squares |= board->black_knights;
		occupied_squares |= board->black_bishops;
		occupied_squares |= board->black_rooks;
		occupied_squares |= board->black_queens;
		occupied_squares |= board->black_king;
	}

	return occupied_squares;
}
