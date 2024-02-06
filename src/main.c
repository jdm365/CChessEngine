#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "board.h"
#include "movegen.h"
#include "eval.h"
#include "gui.h"
#include "depth_test.h"
#include "nnue/network.h"



void play_self_with_gui(int max_depth) {
	Board board;
	init_board(&board);
	populate_board_from_fen(&board, STARTING_FEN);

	struct GUI gui;
	gui_init(&gui);

	MoveList moves;

	enum Color color = WHITE;
	const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);
	while (1) {
		SDL_PumpEvents();

		if (keyboard_state[SDL_SCANCODE_ESCAPE]) {
			goto quit;
		}

		gui_draw_board(&gui, &board, 64);

		uint64_t nodes_searched;
		Move best_move = get_best_move_id(&board, color, max_depth, &nodes_searched);
		uint8_t from, to;
		decode_move(best_move, &from, &to);

		_make_move(&board, from, to);
		reset_move_list(&moves);

		MOVE_NUMBER += (color == BLACK);

		printf("\nMove: %s", translate_square_from_index(from));
		printf("%s\n\n", translate_square_from_index(to));

		float eval = eval_board(&board);
		char title[64];
		sprintf(title, "Eval: %.2f  Move: %d", eval, MOVE_NUMBER);
		SDL_SetWindowTitle(gui.window, title);

		gui_draw_board(&gui, &board, 64);

		if (game_over(&board)) {
			// if (__builtin_popcountll(board.white_king) > __builtin_popcountll(board.black_king)) {
			if (__builtin_popcountll(board.pieces[WHITE_KING]) > __builtin_popcountll(board.pieces[BLACK_KING])) {
				SDL_SetWindowTitle(gui.window, "Game Over - White Wins");
			} 
			else if (__builtin_popcountll(board.pieces[WHITE_KING]) < __builtin_popcountll(board.pieces[BLACK_KING])) {
				SDL_SetWindowTitle(gui.window, "Game Over - Black Wins");
			} 
			else {
				SDL_SetWindowTitle(gui.window, "Game Over - Draw");
			}

			SDL_Delay(2500);
			goto quit;
		}

		color = (enum Color)!color;
		SDL_Delay(500);
	}
	quit:
		gui_quit(&gui);
}

void play_against_engine(int max_depth) {
	Board board;
	init_board(&board);
	populate_board_from_fen(&board, STARTING_FEN);

	struct GUI gui;
	gui_init(&gui);

	MoveList moves;

	const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);
	while (1) {
		SDL_PumpEvents();

		if (keyboard_state[SDL_SCANCODE_ESCAPE]) {
			goto quit;
		}
		bool _quit = gui_handle_player_loop(&gui, &board);
		if (_quit) {
			goto quit;
		}

		gui_draw_board(&gui, &board, 64);
		SDL_Delay(500);

		uint64_t nodes_searched;
		Move best_move = get_best_move_id(&board, BLACK, max_depth, &nodes_searched);
		// Move best_move = get_best_move(&board, BLACK, max_depth, &nodes_searched);
		uint8_t from, to;
		decode_move(best_move, &from, &to);

		_make_move(&board, from, to);
		printf("\nMove: %s", translate_square_from_index(from));
		printf("%s\n\n", translate_square_from_index(to));
		reset_move_list(&moves);
		MOVE_NUMBER++;

		gui_draw_board(&gui, &board, 64);

		if (game_over(&board)) {
			if (__builtin_popcountll(board.pieces[WHITE_KING]) > __builtin_popcountll(board.pieces[BLACK_KING])) {
				SDL_SetWindowTitle(gui.window, "Game Over - White Wins");
			} 
			else if (__builtin_popcountll(board.pieces[WHITE_KING]) < __builtin_popcountll(board.pieces[BLACK_KING])) {
				SDL_SetWindowTitle(gui.window, "Game Over - Black Wins");
			} 
			else {
				SDL_SetWindowTitle(gui.window, "Game Over - Draw");
			}

			SDL_Delay(2500);
			goto quit;
		}
		float eval = eval_board(&board);
		char title[64];
		sprintf(title, "Eval: %.2f  Move: %d", eval, MOVE_NUMBER);
		SDL_SetWindowTitle(gui.window, title);
	}
	quit:
		gui_quit(&gui);
}

int main() {
	init_pawn_moves();
	init_knight_moves();
	init_king_moves();

	init_bishop_moves();
	init_rook_moves();

	// const int MAX_DEPTH = 4;

	// get_average_num_nodes(MAX_DEPTH);
	// perf_test(MAX_DEPTH);
	// play_self_with_gui(MAX_DEPTH);
	// play_against_engine(MAX_DEPTH);

	test_nnue();
	return 0;
}
