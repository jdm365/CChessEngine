#pragma once

#include <stdint.h>

#include "board.h"


typedef struct {
	uint8_t from;
	uint8_t to;
} Move;

typedef struct {
	uint16_t count;
	Move moves[267];
} MoveList;

void init_move_list(MoveList* list);
void reset_move_list(MoveList* list);
void add_move(MoveList* list, uint8_t from, uint8_t to);
void print_legal_moves(const Board* board, enum Color color);
Move get_move_from_string(const char* move);
const char* get_string_from_move(Move move);
bool is_move_legal(MoveList* legal_moves, Move move);
bool game_over(const Board* board);

/****************************************************************

// Move generation functions
// Define two functions per piece. One to generate moves for a
// particular piece, and one to generate moves for all pieces of
// that type for that color.

****************************************************************/
void get_pawn_moves_piece(
		const Board* board, 
		MoveList* list, 
		uint8_t square,
		enum Color color
		);
void get_pawn_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		);

void get_knight_moves_piece(
		const Board* board, 
		MoveList* list, 
		uint8_t square,
		enum Color color
		);
void get_knight_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		);

void get_bishop_moves_piece(
		const Board* board, 
		MoveList* list, 
		uint8_t square,
		enum Color color
		);
void get_bishop_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		);

void get_rook_moves_piece(
		const Board* board, 
		MoveList* list, 
		uint8_t square,
		enum Color color
		);
void get_rook_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		);

void get_queen_moves_piece(
		const Board* board, 
		MoveList* list, 
		uint8_t square,
		enum Color color
		);
void get_queen_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		);

void get_king_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		);

void get_legal_moves(
		const Board* board, 
		MoveList* list, 
		enum Color color
		);
