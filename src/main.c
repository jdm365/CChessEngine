#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>

#include "board.h"
#include "movegen.h"
#include "eval.h"
#include "gui.h"


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

		// Move best_move = get_best_move(&board, color, max_depth);
		Move best_move = get_best_move_id(&board, color, max_depth);
		_make_move(&board, best_move.from, best_move.to);
		printf("\nMove: %s", translate_square_from_index(best_move.from));
		printf("%s\n\n", translate_square_from_index(best_move.to));
		reset_move_list(&moves);

		gui_draw_board(&gui, &board, 64);

		if (game_over(&board)) {
			if (__builtin_popcountll(board.white_king) > __builtin_popcountll(board.black_king)) {
				SDL_SetWindowTitle(gui.window, "Game Over - White Wins");
			} 
			else if (__builtin_popcountll(board.white_king) < __builtin_popcountll(board.black_king)) {
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
		sprintf(title, "Eval: %.2f", eval);
		SDL_SetWindowTitle(gui.window, title);

		color = !color;
		SDL_Delay(750);
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

		Move best_move = get_best_move(&board, BLACK, max_depth);
		_make_move(&board, best_move.from, best_move.to);
		printf("\nMove: %s", translate_square_from_index(best_move.from));
		printf("%s\n\n", translate_square_from_index(best_move.to));
		reset_move_list(&moves);

		gui_draw_board(&gui, &board, 64);

		if (game_over(&board)) {
			if (__builtin_popcountll(board.white_king) > __builtin_popcountll(board.black_king)) {
				SDL_SetWindowTitle(gui.window, "Game Over - White Wins");
			} 
			else if (__builtin_popcountll(board.white_king) < __builtin_popcountll(board.black_king)) {
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
		sprintf(title, "Eval: %.2f", eval);
		SDL_SetWindowTitle(gui.window, title);

		SDL_Delay(250);
	}
	quit:
		gui_quit(&gui);
}

int main() {
	const int MAX_DEPTH = 4;

	play_self_with_gui(MAX_DEPTH);
	// play_against_engine(MAX_DEPTH);
	return 0;
}
