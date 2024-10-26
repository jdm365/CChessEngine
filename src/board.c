#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <stdint.h>
#include <time.h>

#include "board.h"
#include "movegen.h"


uint16_t MOVE_NUMBER = 0;

const char* STARTING_FEN = "rnbqkbnr/pppppppp/8/8/8/8/PPPPPPPP/RNBQKBNR";


const uint64_t FILE_A = 0x0101010101010101;
const uint64_t FILE_B = 0x0202020202020202;
const uint64_t FILE_C = 0x0404040404040404;
const uint64_t FILE_D = 0x0808080808080808;
const uint64_t FILE_E = 0x1010101010101010;
const uint64_t FILE_F = 0x2020202020202020;
const uint64_t FILE_G = 0x4040404040404040;
const uint64_t FILE_H = 0x8080808080808080;

const uint64_t RANK_1 = 0x00000000000000FF;
const uint64_t RANK_2 = 0x000000000000FF00;
const uint64_t RANK_3 = 0x0000000000FF0000;
const uint64_t RANK_4 = 0x00000000FF000000;
const uint64_t RANK_5 = 0x000000FF00000000;
const uint64_t RANK_6 = 0x0000FF0000000000;
const uint64_t RANK_7 = 0x00FF000000000000;
const uint64_t RANK_8 = 0xFF00000000000000;

uint64_t PAWN_MOVES[2][64];
uint64_t PAWN_ATTACKS[2][64];
uint64_t KNIGHT_MOVES[64];
uint64_t KING_MOVES[64];

BitBoard BISHOP_MASKS[64];
uint64_t BISHOP_MAGICS[64];
uint8_t  BISHOP_SHIFT[64];
BitBoard BISHOP_MOVES[64][4096];

BitBoard ROOK_MASKS[64];
uint64_t ROOK_MAGICS[64];
uint8_t  ROOK_SHIFT[64];
BitBoard ROOK_MOVES[64][4096];

uint64_t ZOBRIST[64][12];
TTEntry* TT;

void init_pawn_moves() {
	for (int square = 0; square < 64; square++) {
		uint64_t white_moves = 0;
		uint64_t white_attacks = 0;
		uint64_t black_moves = 0;
		uint64_t black_attacks = 0;

		int rank = square / 8;
		int file = square % 8;

		if (rank == 1) {
			white_moves |= (1ULL << (square + 8));
			white_moves |= (1ULL << (square + 16));
		} else if (rank < 7) {
			white_moves |= (1ULL << (square + 8));
		} else {
			white_moves = 0;
		}

		if (rank == 6) {
			black_moves |= (1ULL << (square - 8));
			black_moves |= (1ULL << (square - 16));
		} else if (rank > 0) {
			black_moves |= (1ULL << (square - 8));
		} else {
			black_moves = 0;
		}

		if (file > 0) {
			white_attacks |= (1ULL << (square + 7));
			black_attacks |= (1ULL << (square - 9));
		}
		if (file < 7) {
			white_attacks |= (1ULL << (square + 9));
			black_attacks |= (1ULL << (square - 7));
		}

		PAWN_MOVES[WHITE][square] = white_moves;
		PAWN_MOVES[BLACK][square] = black_moves;
		PAWN_ATTACKS[WHITE][square] = white_attacks;
		PAWN_ATTACKS[BLACK][square] = black_attacks;
	}
}

void init_knight_moves() {
	for (int square = 0; square < 64; ++square) {
		uint64_t knight_moves = 0;
		int rank = square / 8;
		int file = square % 8;

		if (rank >= 2 && file >= 1) {
			knight_moves |= (1ULL << (square - 17));
		}
		if (rank >= 2 && file <= 6) {
			knight_moves |= (1ULL << (square - 15));
		}
		if (rank >= 1 && file >= 2) {
			knight_moves |= (1ULL << (square - 10));
		}
		if (rank >= 1 && file <= 5) {
			knight_moves |= (1ULL << (square - 6));
		}
		if (rank <= 6 && file >= 2) {
			knight_moves |= (1ULL << (square + 6));
		}
		if (rank <= 6 && file <= 5) {
			knight_moves |= (1ULL << (square + 10));
		}
		if (rank <= 5 && file >= 1) {
			knight_moves |= (1ULL << (square + 15));
		}
		if (rank <= 5 && file <= 6) {
			knight_moves |= (1ULL << (square + 17));
		}
		KNIGHT_MOVES[square] = knight_moves;
	}
}

void init_king_moves() {
	for (int square = 0; square < 64; square++) {
		uint64_t king_moves = 0;
		int rank = square / 8;
		int file = square % 8;
		if (rank >= 1 && file >= 1) {
			king_moves |= (1ULL << (square - 9));
		}
		if (rank >= 1) {
			king_moves |= (1ULL << (square - 8));
		}
		if (rank >= 1 && file <= 6) {
			king_moves |= (1ULL << (square - 7));
		}
		if (file >= 1) {
			king_moves |= (1ULL << (square - 1));
		}
		if (file <= 6) {
			king_moves |= (1ULL << (square + 1));
		}
		if (rank <= 6 && file >= 1) {
			king_moves |= (1ULL << (square + 7));
		}
		if (rank <= 6) {
			king_moves |= (1ULL << (square + 8));
		}
		if (rank <= 6 && file <= 6) {
			king_moves |= (1ULL << (square + 9));
		}
		KING_MOVES[square] = king_moves;
	}
}

static void init_bishop_masks() {
	for (int square_idx = 0; square_idx < 64; ++square_idx) {
		BISHOP_MASKS[square_idx] = 0;

		int rank = square_idx / 8;
		int file = square_idx % 8;
		int start_rank = rank;
		int start_file = file;

		// while (rank < 7 && file < 7) {
		while (rank < 6 && file < 6) {
			++rank;
			++file;
			BISHOP_MASKS[square_idx] |= (1ULL << (8 * rank + file));
		}

		rank = start_rank;
		file = start_file;

		// while (rank > 0 && file < 7) {
		while (rank > 1 && file < 6) {
			--rank;
			++file;
			BISHOP_MASKS[square_idx] |= (1ULL << (8 * rank + file));
		}

		rank = start_rank;
		file = start_file;

		// while (rank < 7 && file > 0) {
		while (rank < 6 && file > 1) {
			++rank;
			--file;
			BISHOP_MASKS[square_idx] |= (1ULL << (8 * rank + file));
		}

		rank = start_rank;
		file = start_file;

		// while (rank > 0 && file > 0) {
		while (rank > 1 && file > 1) {
			--rank;
			--file;
			BISHOP_MASKS[square_idx] |= (1ULL << (8 * rank + file));
		}
	}
}

static BitBoard generate_bishop_moves(BitBoard* occupancy, int square) {
	BitBoard moves = 0;

	int rank = square / 8;
	int file = square % 8;
	int start_rank = rank;
	int start_file = file;

	while (rank < 7 && file < 7) {
		++rank;
		++file;
		if (*occupancy & (1ULL << (8 * rank + file))) {
			moves |= (1ULL << (8 * rank + file));
			break;
		}
		moves |= (1ULL << (8 * rank + file));
	}

	rank = start_rank;
	file = start_file;

	while (rank > 0 && file < 7) {
		--rank;
		++file;
		if (*occupancy & (1ULL << (8 * rank + file))) {
			moves |= (1ULL << (8 * rank + file));
			break;
		}
		moves |= (1ULL << (8 * rank + file));
	}

	rank = start_rank;
	file = start_file;

	while (rank < 7 && file > 0) {
		++rank;
		--file;
		if (*occupancy & (1ULL << (8 * rank + file))) {
			moves |= (1ULL << (8 * rank + file));
			break;
		}
		moves |= (1ULL << (8 * rank + file));
	}

	rank = start_rank;
	file = start_file;

	while (rank > 0 && file > 0) {
		--rank;
		--file;
		if (*occupancy & (1ULL << (8 * rank + file))) {
			moves |= (1ULL << (8 * rank + file));
			break;
		}
		moves |= (1ULL << (8 * rank + file));
	}

	return moves;
}


static void init_rook_masks() {
	for (int square_idx = 0; square_idx < 64; ++square_idx) {
		ROOK_MASKS[square_idx] = 0;

		int rank = square_idx / 8;
		int file = square_idx % 8;
		int start_rank = rank;
		int start_file = file;

		while (rank < 6) {
			++rank;
			ROOK_MASKS[square_idx] |= (1ULL << (8 * rank + file));
		}

		rank = start_rank;
		file = start_file;

		while (file < 6) {
			++file;
			ROOK_MASKS[square_idx] |= (1ULL << (8 * rank + file));
		}

		rank = start_rank;
		file = start_file;

		while (file > 1) {
			--file;
			ROOK_MASKS[square_idx] |= (1ULL << (8 * rank + file));
		}

		rank = start_rank;
		file = start_file;

		while (rank > 1) {
			--rank;
			ROOK_MASKS[square_idx] |= (1ULL << (8 * rank + file));
		}
	}
}

static BitBoard generate_rook_moves(BitBoard* occupancy, int square) {
	BitBoard moves = 0;

	int rank = square / 8;
	int file = square % 8;
	int start_rank = rank;
	int start_file = file;

	while (rank < 7) {
		++rank;
		if (*occupancy & (1ULL << (8 * rank + file))) {
			moves |= (1ULL << (8 * rank + file));
			break;
		}
		moves |= (1ULL << (8 * rank + file));
	}

	rank = start_rank;
	file = start_file;

	while (file < 7) {
		++file;
		if (*occupancy & (1ULL << (8 * rank + file))) {
			moves |= (1ULL << (8 * rank + file));
			break;
		}
		moves |= (1ULL << (8 * rank + file));
	}

	rank = start_rank;
	file = start_file;

	while (file > 0) {
		--file;
		if (*occupancy & (1ULL << (8 * rank + file))) {
			moves |= (1ULL << (8 * rank + file));
			break;
		}
		moves |= (1ULL << (8 * rank + file));
	}

	rank = start_rank;
	file = start_file;

	while (rank > 0) {
		--rank;
		if (*occupancy & (1ULL << (8 * rank + file))) {
			moves |= (1ULL << (8 * rank + file));
			break;
		}
		moves |= (1ULL << (8 * rank + file));
	}

	return moves;
}


static void generate_occupancy_variants(
		BitBoard mask, 
		BitBoard* variants, 
		int* num_variants
		) {
	int n = __builtin_popcountll(mask);
	*num_variants = 1 << n;

    for (int idx = 0; idx < *num_variants; ++idx) {
        BitBoard subset    = 0;
        BitBoard temp_mask = mask;

        for (int jdx = 0; jdx < n; ++jdx) {
            int square = __builtin_ctzll(temp_mask);
            temp_mask &= ~(1ULL << square);

            if (idx & (1 << jdx)) {
                subset |= (1ULL << square);
            }
        }

		if (idx >= 4096) {
			printf("Index overflow generating occupancy variants\n");
			printf("Num variants: %d\n", *num_variants);
			exit(1);
		}
        variants[idx] = subset;
    }
}

static uint64_t xorshift64_state = 88172645463325252ULL;
static uint64_t random_uint64() {
	xorshift64_state ^= xorshift64_state >> 12;
    xorshift64_state ^= xorshift64_state << 25;
    xorshift64_state ^= xorshift64_state >> 27;
    return xorshift64_state * 2685821657736338717LL;

}

inline uint64_t magic_hash(uint64_t occupancy, uint64_t magic, uint8_t bits) {
	return (occupancy * magic) >> (64 - bits);
}

static bool validate_magic_number(
		int square, 
		uint64_t magic, 
		BitBoard* variants, 
		int num_variants,
		enum Piece piece_type
		) {
	/************************************************************
	For a bishop on the given square, generate all possible relevant occupancies.
	For each occupancy and the candidate magic number, compute the index by calculating
	the magic hash.
	If the hash for each occupancy and the given magic number is unique, then the magic
	number is valid.
	************************************************************/

	uint64_t attack_table[4096] = {0};

	uint64_t mask;
	BitBoard (*generate_moves)(BitBoard*, int) = NULL;
	if (piece_type == BISHOP) {
		mask = BISHOP_MASKS[square];
		generate_moves = generate_bishop_moves;
	} else {
		mask = ROOK_MASKS[square];
		generate_moves = generate_rook_moves;
	}

	for (int idx = 0; idx < num_variants; ++idx) {
		BitBoard occupancy = variants[idx];

		uint64_t index = magic_hash(
				occupancy, 
				magic, 
				// __builtin_popcountll(BISHOP_MASKS[square])
				__builtin_popcountll(mask)
				);

		if (attack_table[index] == 0) {
			// First time we've seen this index. Store the attack table
			// attack_table[index] = generate_bishop_moves(&occupancy, square);
			attack_table[index] = generate_moves(&occupancy, square);
		} 
		// else if (attack_table[index] != generate_bishop_moves(&occupancy, square)) {
		else if (attack_table[index] != generate_moves(&occupancy, square)) {
			// This magic number produces more than one variant with the same index.
			// It's not a valid magic number.
			return false;
		}
	}

	// All variants produce unique attack tables for the given index.
	// This is a valid magic number.

	return true;
}

static uint64_t find_magic_number(int square, enum Piece piece_type) {
	BitBoard variants[4096];
	int num_variants;

	uint64_t mask;
	if (piece_type == BISHOP) {
		mask = BISHOP_MASKS[square];
	} else {
		mask = ROOK_MASKS[square];
	}
	generate_occupancy_variants(
			// BISHOP_MASKS[square],
			mask,
			variants,
			&num_variants
			);

	int num_trials = 0;
    while (true) {
        uint64_t candidate_magic_number = random_uint64() & random_uint64() & random_uint64();
        if (validate_magic_number(square, candidate_magic_number, variants, num_variants, piece_type)) {
            return candidate_magic_number;
        }
		++num_trials;
    }

	printf("Failed to find a magic number for square %d\n", square);
	exit(1);
}

static void init_bishop_magics() {
	for (int square = 0; square < 64; ++square) {
		BISHOP_MAGICS[square] = find_magic_number(square, BISHOP);
	}
}

static void init_rook_magics() {
	for (int square = 0; square < 64; ++square) {
		ROOK_MAGICS[square] = find_magic_number(square, ROOK);
	}
}

void init_bishop_moves() {
	init_bishop_masks();
	init_bishop_magics();

	for (int square = 0; square < 64; ++square) {
		BitBoard occupancy_variants[4096];
		int num_variants;
		generate_occupancy_variants(
				BISHOP_MASKS[square], 
				occupancy_variants, 
				&num_variants
				);

		BISHOP_SHIFT[square] = __builtin_popcountll(BISHOP_MASKS[square]);
		for (int idx = 0; idx < num_variants; ++idx) {
			BitBoard occupancy = occupancy_variants[idx];
			uint64_t index = magic_hash(
					occupancy, 
					BISHOP_MAGICS[square], 
					BISHOP_SHIFT[square]
					);

			BISHOP_MOVES[square][index] = generate_bishop_moves(&occupancy, square);
		}
	}
	// Now to get move at runtime, we can do:
	// BISHOP_MOVES[square][magic_hash(occupancy, BISHOP_MAGICS[square], __builtin_popcountll(BISHOP_MASKS[square]))]
}

void init_rook_moves() {
	init_rook_masks();
	init_rook_magics();

	for (int square = 0; square < 64; ++square) {
		BitBoard occupancy_variants[4096];
		int num_variants;
		generate_occupancy_variants(
				ROOK_MASKS[square], 
				occupancy_variants, 
				&num_variants
				);

		ROOK_SHIFT[square] = __builtin_popcountll(ROOK_MASKS[square]);
		for (int idx = 0; idx < num_variants; ++idx) {
			BitBoard occupancy = occupancy_variants[idx];
			uint64_t index = magic_hash(
					occupancy, 
					ROOK_MAGICS[square],
					ROOK_SHIFT[square]
					);

			ROOK_MOVES[square][index] = generate_rook_moves(&occupancy, square);
		}
	}
	// Now to get move at runtime, we can do:
	// ROOK_MOVES[square][magic_hash(occupancy, ROOK_MAGICS[square], __builtin_popcountll(ROOK_MASKS[square]))]
}

void init_zobrist_table() {
	srand(time(NULL));
    for (int square = 0; square < 64; ++square) {
        for (int piece = 0; piece < 12; ++piece) {
            ZOBRIST[square][piece] = ((uint64_t)rand() << 32) | rand();
        }
    }
}

void init_TT() {
	TT = (TTEntry*)malloc(TT_SIZE * sizeof(TTEntry));
	for (int idx = 0; idx < TT_SIZE; ++idx) {
		TT[idx].key = 0;
		TT[idx].depth = 0;
		TT[idx].flag = 0;
		TT[idx].score = 0;
		TT[idx].move_number = 0;
		TT[idx].best_move = (Move){
			.from = 0,
			.to = 0,
			.is_killer = false,
			.padding = 0,
		};

		TT[idx].color = (uint8_t)EMPTY;
	}
}

uint64_t zobrist_hash(const Board* board) {
	uint64_t hash = 0;
	for (int square = 0; square < 64; ++square) {
		uint8_t piece = board->piece_at[square];

		if (piece != EMPTY_SQUARE) {
			hash ^= ZOBRIST[square][piece];
		}
	}
	return hash;
}

void store_TT_entry(
		const Board* board, 
		float score, 
		uint8_t depth, 
		uint8_t flag,
		uint16_t move_number,
		Move best_move,
		enum Color color
		) {
	uint64_t key = zobrist_hash(board);

	uint64_t index = key % TT_SIZE;
	TTEntry* entry = &TT[index];

	if (entry->depth <= depth || entry->depth == 0) {
        entry->key = key;
        entry->depth = depth;
        entry->flag  = flag;
        entry->score = score;
		entry->move_number = MOVE_NUMBER;
		entry->best_move = best_move;
		entry->color = (uint8_t)color;
    }
}

void init_board(Board* board) {
	memset(board->pieces, 0, sizeof(board->pieces));
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
				set_bit(&board->pieces[WHITE_PAWN], square);
				break;
			case 'N':
				set_bit(&board->pieces[WHITE_KNIGHT], square);
				break;
			case 'B':
				set_bit(&board->pieces[WHITE_BISHOP], square);
				break;
			case 'R':
				set_bit(&board->pieces[WHITE_ROOK], square);
				break;
			case 'Q':
				set_bit(&board->pieces[WHITE_QUEEN], square);
				break;
			case 'K':
				set_bit(&board->pieces[WHITE_KING], square);
				break;
			case 'p':
				set_bit(&board->pieces[BLACK_PAWN], square);
				break;
			case 'n':
				set_bit(&board->pieces[BLACK_KNIGHT], square);
				break;
			case 'b':
				set_bit(&board->pieces[BLACK_BISHOP], square);
				break;
			case 'r':
				set_bit(&board->pieces[BLACK_ROOK], square);
				break;
			case 'q':
				set_bit(&board->pieces[BLACK_QUEEN], square);
				break;
			case 'k':
				set_bit(&board->pieces[BLACK_KING], square);
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
				printf("Invalid piece: %c - %d\n", piece, square_cntr);
				exit(1);
		}
		square_cntr++;
	}

	for (int idx = 0; idx < 64; ++idx) {
		board->piece_at[idx] = (uint8_t)colored_piece_at(board, idx);
	}
}

void print_board(const Board* board) {
	printf("\n  +---+---+---+---+---+---+---+---+\n");
	for (int rank = 7; rank >= 0; --rank) {
		printf("%d |", rank + 1);
		for (int file = 0; file < 8; ++file) {
			int square_index = rank * 8 + file;
			if (board->pieces[WHITE_PAWN] & (1ULL << square_index)) {
				printf(" P |");
			} else if (board->pieces[WHITE_KNIGHT] & (1ULL << square_index)) {
				printf(" N |");
			} else if (board->pieces[WHITE_BISHOP] & (1ULL << square_index)) {
				printf(" B |");
			} else if (board->pieces[WHITE_ROOK] & (1ULL << square_index)) {
				printf(" R |");
			} else if (board->pieces[WHITE_QUEEN] & (1ULL << square_index)) {
				printf(" Q |");
			} else if (board->pieces[WHITE_KING] & (1ULL << square_index)) {
				printf(" K |");
			} else if (board->pieces[BLACK_PAWN] & (1ULL << square_index)) {
				printf(" p |");
			} else if (board->pieces[BLACK_KNIGHT] & (1ULL << square_index)) {
				printf(" n |");
			} else if (board->pieces[BLACK_BISHOP] & (1ULL << square_index)) {
				printf(" b |");
			} else if (board->pieces[BLACK_ROOK] & (1ULL << square_index)) {
				printf(" r |");
			} else if (board->pieces[BLACK_QUEEN] & (1ULL << square_index)) {
				printf(" q |");
			} else if (board->pieces[BLACK_KING] & (1ULL << square_index)) {
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
	char first_two_chars[3];
	strncpy(first_two_chars, move, 2);
	uint8_t from_square = translate_square_from_char(first_two_chars);

	char last_two_chars[3];
	strncpy(last_two_chars, move + 2, 2);
	uint8_t to_square = translate_square_from_char(last_two_chars);

	_make_move(board, from_square, to_square);
}

uint8_t _make_move(
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
			((board->pieces[WHITE_KING] | board->pieces[BLACK_KING]) & (1ULL << from_square))
		) {
		_castle(board, from_square, to_square);
		return 0;
	}

	uint64_t from_square_mask = (1ULL << from_square);
	uint64_t to_square_mask   = (1ULL << to_square);

	// Get piece at from_square
	uint8_t from_piece = board->piece_at[from_square];
	uint8_t to_piece   = board->piece_at[to_square];
	
	// Remove piece from from_square and to_square
	board->pieces[from_piece] &= ~from_square_mask;
	
	// Check promotion
	uint8_t promotion = (from_piece == WHITE_PAWN && to_square >= 56) 
						|| (from_piece == BLACK_PAWN && to_square <= 7);
	from_piece += 4 * promotion;

	if (to_piece != EMPTY_SQUARE) {
		board->pieces[to_piece]   &= ~to_square_mask;
	}

	// Move piece to to_square
	board->pieces[from_piece] |= to_square_mask;

	board->piece_at[from_square] = (uint8_t)EMPTY_SQUARE;
	board->piece_at[to_square]   = (uint8_t)from_piece;

	return promotion;
}


void unmake_move(
		Board* board, 
		enum ColoredPiece captured_piece,
		uint8_t promotion,
		uint8_t from_square, 
		uint8_t to_square
		) {
	// Check if casted
	if (
			(from_square == 4 || from_square == 60)
				&&
			(abs(from_square - to_square) == 2)
				&&
			((board->pieces[WHITE_KING] | board->pieces[BLACK_KING]) & (1ULL << to_square))
		) {
		un_castle(board, from_square, to_square);
		return;
	}
	uint64_t from_square_mask = (1ULL << from_square);
	uint64_t to_square_mask   = (1ULL << to_square);

	// Remove piece from to square and place on from square
	uint8_t to_piece = board->piece_at[to_square];

	board->pieces[to_piece] &= ~to_square_mask;
	to_piece -= 4 * promotion;
	board->pieces[to_piece] |= from_square_mask;

	board->pieces[captured_piece] |= to_square_mask;

	board->piece_at[to_square]   = (uint8_t)captured_piece;
	board->piece_at[from_square] = (uint8_t)colored_piece_at(board, from_square);
}


inline void _castle(Board* board, uint8_t from_square, uint8_t to_square) {
	uint64_t from_square_mask = (1ULL << from_square);
	uint64_t to_square_mask   = (1ULL << to_square);

	uint8_t rook_from_square = (to_square > from_square) ? to_square + 1 : to_square - 2;
	uint8_t rook_to_square   = (to_square > from_square) ? to_square - 1 : to_square + 1;

	// bool is_white = (from_square == 4);

	// Remove king from from_square and rook from appropriate square
	// Guaranteed no pieces captured, so don't remove anything from to squares.
	// enum ColoredPiece king = WHITE_KING + 6 * !is_white;
	// enum ColoredPiece rook = WHITE_ROOK + 6 * !is_white;
	uint8_t king = (uint8_t)WHITE_KING + 6 * (from_square != 4);
	uint8_t rook = (uint8_t)WHITE_ROOK + 6 * (from_square != 4);

	board->pieces[king] &= ~from_square_mask;
	board->pieces[king] |= to_square_mask;

	board->pieces[rook] &= ~(1ULL << rook_from_square);
	board->pieces[rook] |= (1ULL << rook_to_square);

	// Update rook position
	board->piece_at[rook_from_square] = (uint8_t)EMPTY_SQUARE;
	board->piece_at[rook_to_square]   = (uint8_t)rook;

	// Update king position
	board->piece_at[from_square] = (uint8_t)EMPTY_SQUARE;
	board->piece_at[to_square]   = (uint8_t)king;
}

inline void un_castle(Board* board, uint8_t from_square, uint8_t to_square) {
	uint64_t from_square_mask = (1ULL << from_square);
	uint64_t to_square_mask   = (1ULL << to_square);

	uint8_t rook_from_square = (to_square > from_square) ? to_square + 1 : to_square - 2;
	uint8_t rook_to_square   = (to_square > from_square) ? to_square - 1 : to_square + 1;

	// bool is_white = (from_square == 4);

	// Remove king from from_square and rook from appropriate square
	// Guaranteed no pieces captured, so don't remove anything from to squares.
	// enum ColoredPiece king = WHITE_KING + 6 * !is_white;
	// enum ColoredPiece rook = WHITE_ROOK + 6 * !is_white;
	uint8_t king = (uint8_t)WHITE_KING + 6 * (from_square != 4);
	uint8_t rook = (uint8_t)WHITE_ROOK + 6 * (from_square != 4);

	// board->pieces[king] &= ~from_square_mask;
	// board->pieces[king] |= to_square_mask;
	board->pieces[king] &= ~to_square_mask;
	board->pieces[king] |= from_square_mask;

	// board->pieces[rook] &= ~(1ULL << rook_from_square);
	// board->pieces[rook] |= (1ULL << rook_to_square);
	board->pieces[rook] &= ~(1ULL << rook_to_square);
	board->pieces[rook] |= (1ULL << rook_from_square);

	// Update rook position
	// board->piece_at[rook_from_square] = (uint8_t)EMPTY_SQUARE;
	// board->piece_at[rook_to_square]   = (uint8_t)rook;
	board->piece_at[rook_to_square]   = (uint8_t)EMPTY_SQUARE;
	board->piece_at[rook_from_square] = (uint8_t)rook;

	// Update king position
	// board->piece_at[from_square] = (uint8_t)EMPTY_SQUARE;
	// board->piece_at[to_square]   = (uint8_t)king;
	board->piece_at[to_square]   = (uint8_t)EMPTY_SQUARE;
	board->piece_at[from_square] = (uint8_t)king;
}

inline enum Color is_occupied_by(const Board* board, uint8_t square) {
	BitBoard white_board = get_occupied_squares_color(board, WHITE);
	if (white_board & (1ULL << square)) {
		return WHITE;
	}

	BitBoard black_board = get_occupied_squares_color(board, BLACK);
	if (black_board & (1ULL << square)) {
		return BLACK;
	}

	return EMPTY;
}

inline enum ColoredPiece colored_piece_at(const Board* board, uint8_t square) {
	for (int idx = 0; idx < 12; ++idx) {
		if (board->pieces[idx] & (1ULL << square)) {
			return (enum ColoredPiece)idx;
		}
	}
	return EMPTY_SQUARE;
}

inline enum Piece piece_at(const Board* board, uint8_t square) {
	for (int idx = 0; idx < 6; ++idx) {
		if ((board->pieces[idx] | board->pieces[idx + 6]) & (1ULL << square)) {
			return (enum Piece)idx;
		}
	}
	return EMPTY_PIECE;
}


inline bool is_empty(const Board* board, uint8_t square) {
	return is_occupied_by(board, square) == EMPTY;
}

inline BitBoard get_occupied_squares(const Board* board) {
	BitBoard occupied_squares = board->pieces[WHITE_PAWN];
	for (int idx = 1; idx < 12; ++idx) {
		occupied_squares |= board->pieces[idx];
	}

	return occupied_squares;
}

inline BitBoard get_occupied_squares_color(const Board* board, enum Color color) {
	uint8_t shift = 6 * (color == BLACK);
	return board->pieces[WHITE_PAWN + shift] |
		   board->pieces[WHITE_KNIGHT + shift] |
		   board->pieces[WHITE_BISHOP + shift] |
		   board->pieces[WHITE_ROOK + shift] |
		   board->pieces[WHITE_QUEEN + shift] |
		   board->pieces[WHITE_KING + shift];
}
