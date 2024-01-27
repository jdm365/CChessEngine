#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

#include "board.h"
#include "movegen.h"
#include "eval.h"



void play_game(Board* board) {
	char move[6];
	MoveList moves;
	init_move_list(&moves);

	while (1) {
		printf("Enter move: ");
		scanf("%s", move);

		get_legal_moves(board, &moves, WHITE);

		Move m = get_move_from_string(move);
		if (!is_move_legal(&moves, m)) {
			printf("Illegal move\n");
			printf("Legal moves:\n");
			print_legal_moves(board, WHITE);
			continue;
		}
		make_move(board, move);
		reset_move_list(&moves);
		print_board(board);

		Move best_move = get_best_move(board, BLACK, 6);
		_make_move(board, best_move.from, best_move.to);
		reset_move_list(&moves);
		printf("Made move: %s\n", get_string_from_move(best_move));

		print_board(board);
		printf("Eval: %f\n", eval_board(board));
	}
}


int main() {

	Board board;
	init_board(&board);
	populate_board_from_fen(&board, STARTING_FEN);

	play_game(&board);
	return 0;
}
