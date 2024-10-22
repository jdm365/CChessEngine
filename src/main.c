#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <stdint.h>
#include <string.h>
#include <time.h>

#include "board.h"
#include "movegen.h"
#include "eval.h"
#include "gui.h"
// #include "depth_test.h"
// #include "nnue/network.h"


#define min(a, b) ((a) < (b) ? (a) : (b))
#define max(a, b) ((a) > (b) ? (a) : (b)

#define INPUT_BUFFER_SIZE 4096

void uci_loop() {
    char input[INPUT_BUFFER_SIZE];
    MoveList moves;

    Board board;
	init_board(&board);
	populate_board_from_fen(&board, STARTING_FEN);

    while (fgets(input, INPUT_BUFFER_SIZE, stdin)) {
        if (strncmp(input, "uci", 3) == 0) {
            // UCI initialization, engine identification
            printf("id name cchessengine\n");
            printf("id author Jake Mehlman\n");
            printf("uciok\n");
			fflush(stdout);

        } else if (strncmp(input, "isready", 7) == 0) {
            // Respond to readiness check
            printf("readyok\n");
			fflush(stdout);

        } else if (strncmp(input, "position", 8) == 0) {
            // Parse 'position' command (e.g., 'position startpos' or 'position fen')
            char* ptr = input + 9;
            if (strncmp(ptr, "startpos", 8) == 0) {
                // Use the starting position FEN
                init_board(&board);
                populate_board_from_fen(&board, STARTING_FEN);
                ptr += 8;
            } else if (strncmp(ptr, "fen", 3) == 0) {
                // Custom FEN string
                ptr += 4; // Skip 'fen '
                // init_board(&board);
                populate_board_from_fen(&board, ptr); // Assuming your populate_board_from_fen function works correctly
            }

        } else if (strncmp(input, "go", 2) == 0) {

            // Process the 'go' command and search for the best move
            uint64_t nodes_searched;
            Move best_move = get_best_move_id(&board, WHITE, 4, &nodes_searched); // Example: depth=4
            uint8_t from, to;
            decode_move(best_move, &from, &to);

            // Output the best move in UCI format
            printf("bestmove %s", translate_square_from_index(from));
			printf("%s\n", translate_square_from_index(to));
			fflush(stdout);

			reset_move_list(&moves);

        } else if (strncmp(input, "quit", 4) == 0) {
            // Graceful exit
            break;
		}
    }
}

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

	float seconds = 0.0f;

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
		SDL_Delay(max(0, 500 - (int)(1000 * seconds))));

		// start timer
		clock_t start = clock();

		uint64_t nodes_searched;
		Move best_move = get_best_move_id(&board, BLACK, max_depth, &nodes_searched);
		// Move best_move = get_best_move(&board, BLACK, max_depth, &nodes_searched);
		uint8_t from, to;
		decode_move(best_move, &from, &to);

		// end timer
		clock_t end = clock();
		seconds = (float)(end - start) / CLOCKS_PER_SEC;

		_make_move(&board, from, to);
		printf("\nMove: %s", translate_square_from_index(from));
		printf("%s\n\n", translate_square_from_index(to));
		printf("Think time: %f\n", seconds);
		printf("KNodes searched: %lu\n", nodes_searched / 1000);
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

	const int MAX_DEPTH = 4;

	uci_loop();
	// get_average_num_nodes(MAX_DEPTH);
	// perf_test(MAX_DEPTH);
	// play_self_with_gui(MAX_DEPTH);
	// play_against_engine(MAX_DEPTH);

	// test_nnue();
	return 0;
}
