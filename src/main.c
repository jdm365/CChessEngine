#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "board.h"
#include "movegen.h"
#include "eval.h"
#include "gui.h"


int main() {

	Board board;
	init_board(&board);
	populate_board_from_fen(&board, STARTING_FEN);

	struct GUI gui;
	gui_init(&gui);

	MoveList moves;

	enum Color color = WHITE;
	const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);
	while (1) {

		if (keyboard_state[SDL_SCANCODE_ESCAPE]) {
			goto quit;
		}
		if (keyboard_state[SDL_SCANCODE_SPACE]) {
			while (1) {
				if (keyboard_state[SDL_SCANCODE_SPACE]) {
					break;
				}
				if (keyboard_state[SDL_SCANCODE_ESCAPE]) {
					goto quit;
				}
				SDL_PumpEvents();
			}
		}
		SDL_PumpEvents();

		gui_draw_board(&gui, &board);

		Move best_move = get_best_move(&board, color, 4);
		_make_move(&board, best_move.from, best_move.to);
		printf("\nMove: %s", translate_square_from_index(best_move.from));
		printf("%s\n\n", translate_square_from_index(best_move.to));
		reset_move_list(&moves);

		if (game_over(&board)) {
			printf("White kings remaining: %d\n", __builtin_popcountll(board.white_king));
			printf("Black kings remaining: %d\n", __builtin_popcountll(board.black_king));
			SDL_Delay(2500);
			goto quit;
		}
		float eval = eval_board(&board);
		char title[64];
		sprintf(title, "Eval: %.2f", eval);
		SDL_SetWindowTitle(gui.window, title);

		color = !color;
		// SDL_Delay(1000 / 60);
		SDL_Delay(750);
	}
	quit:
		gui_quit(&gui);

	gui_quit(&gui);
	return 0;
}
