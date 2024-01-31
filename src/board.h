#pragma once

#include <stdint.h>
#include <stdbool.h>

extern const char* STARTING_FEN;

extern const uint64_t FILE_A;
extern const uint64_t FILE_B;
extern const uint64_t FILE_C;
extern const uint64_t FILE_D;
extern const uint64_t FILE_E;
extern const uint64_t FILE_F;
extern const uint64_t FILE_G;
extern const uint64_t FILE_H;

extern const uint64_t RANK_1;
extern const uint64_t RANK_2;
extern const uint64_t RANK_3;
extern const uint64_t RANK_4;
extern const uint64_t RANK_5;
extern const uint64_t RANK_6;
extern const uint64_t RANK_7;
extern const uint64_t RANK_8;

// precompute knight moves
extern uint64_t PAWN_MOVES[2][64];
extern uint64_t PAWN_ATTACKS[2][64];
extern uint64_t KNIGHT_MOVES[64];
extern uint64_t KING_MOVES[64];
void init_pawn_moves();
void init_knight_moves();
void init_king_moves();

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
BitBoard* get_bitboard_by_index(const Board* board, int index);
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
