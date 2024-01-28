#pragma once

#include <stdint.h>
#include <stdbool.h>

static const char* STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

enum Piece {
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING
};
enum Color {
	WHITE,
	BLACK,
	EMPTY
};
enum ColoredPiece {
	WHITE_PAWN,
	WHITE_KNIGHT,
	WHITE_BISHOP,
	WHITE_ROOK,
	WHITE_QUEEN,
	WHITE_KING,

	BLACK_PAWN,
	BLACK_KNIGHT,
	BLACK_BISHOP,
	BLACK_ROOK,
	BLACK_QUEEN,
	BLACK_KING,

	EMPTY_SQUARE
};

typedef uint64_t BitBoard;
typedef struct {
	BitBoard white_pawns;
	BitBoard white_knights;
	BitBoard white_bishops;
	BitBoard white_rooks;
	BitBoard white_queens;
	BitBoard white_king;

	BitBoard black_pawns;
	BitBoard black_knights;
	BitBoard black_bishops;
	BitBoard black_rooks;
	BitBoard black_queens;
	BitBoard black_king;
} Board;

void init_board(Board* board);
void set_bit(BitBoard* board, int index);
uint8_t translate_square_from_char(const char* square);
const char* translate_square_from_index(uint8_t square);
void populate_board_from_fen(Board* board, const char* fen);
void print_board(Board* board);
void print_bitboard(uint64_t bit_board);
void make_move(Board* board, const char* move);
void _make_move(
		Board* board, 
		uint8_t from_square, 
		uint8_t to_square
		);
void _castle(Board* board, uint8_t from_square, uint8_t to_square);
enum Color is_occupied_by(const Board* board, uint8_t square);
enum ColoredPiece piece_at(const Board* board, uint8_t square);
bool is_empty(const Board* board, uint8_t square);
BitBoard get_occupied_squares(const Board* board);
