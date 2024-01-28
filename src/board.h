#pragma once

#include <stdint.h>
#include <stdbool.h>
#include <wchar.h>

static const char* STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";

static const uint64_t FILE_A = 0x0101010101010101;
static const uint64_t FILE_B = 0x0202020202020202;
static const uint64_t FILE_C = 0x0404040404040404;
static const uint64_t FILE_D = 0x0808080808080808;
static const uint64_t FILE_E = 0x1010101010101010;
static const uint64_t FILE_F = 0x2020202020202020;
static const uint64_t FILE_G = 0x4040404040404040;
static const uint64_t FILE_H = 0x8080808080808080;

static const uint64_t RANK_1 = 0x00000000000000FF;
static const uint64_t RANK_2 = 0x000000000000FF00;
static const uint64_t RANK_3 = 0x0000000000FF0000;
static const uint64_t RANK_4 = 0x00000000FF000000;
static const uint64_t RANK_5 = 0x000000FF00000000;
static const uint64_t RANK_6 = 0x0000FF0000000000;
static const uint64_t RANK_7 = 0x00FF000000000000;
static const uint64_t RANK_8 = 0xFF00000000000000;

enum Piece {
	PAWN,
	KNIGHT,
	BISHOP,
	ROOK,
	QUEEN,
	KING,
	EMPTY_PIECE
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
enum ColoredPiece colored_piece_at(const Board* board, uint8_t square);
enum Piece piece_at(const Board* board, uint8_t square);
bool is_empty(const Board* board, uint8_t square);
BitBoard get_occupied_squares(const Board* board);
BitBoard get_occupied_squares_color(const Board* board, enum Color color);
