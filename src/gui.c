#include <stdio.h>
#include <stdbool.h>

#include "board.h"
#include "gui.h"
#include "movegen.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>


static const char* WHITE_PAWN_PATH   = "res/images/white/pawn.png";
static const char* WHITE_KNIGHT_PATH = "res/images/white/knight.png";
static const char* WHITE_BISHOP_PATH = "res/images/white/bishop.png";
static const char* WHITE_ROOK_PATH   = "res/images/white/rook.png";
static const char* WHITE_QUEEN_PATH  = "res/images/white/queen.png";
static const char* WHITE_KING_PATH   = "res/images/white/king.png";

static const char* BLACK_PAWN_PATH   = "res/images/black/pawn.png";
static const char* BLACK_KNIGHT_PATH = "res/images/black/knight.png";	
static const char* BLACK_BISHOP_PATH = "res/images/black/bishop.png";
static const char* BLACK_ROOK_PATH   = "res/images/black/rook.png";
static const char* BLACK_QUEEN_PATH  = "res/images/black/queen.png";
static const char* BLACK_KING_PATH   = "res/images/black/king.png";

static const int WINDOW_WIDTH  = 600;
static const int WINDOW_HEIGHT = 600;
static const int SQUARE_SIZE   = WINDOW_WIDTH / 8;


SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path) {
	SDL_Texture* texture = IMG_LoadTexture(renderer, path);
	if (texture == NULL) {
		printf("Failed to load texture: %s\n", path);
		exit(1);
	}
	return texture;
}

void gui_init(struct GUI* gui) {
	SDL_Init(SDL_INIT_VIDEO);
	gui->window = SDL_CreateWindow(
			"Chess", 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN
			);
	gui->renderer = SDL_CreateRenderer(
			gui->window, 
			-1, 
			SDL_RENDERER_ACCELERATED
			);

	// Init textures
	gui->textures[WHITE_PAWN] = load_texture(
			gui->renderer, 
			WHITE_PAWN_PATH
			);
	gui->textures[WHITE_KNIGHT] = load_texture(
			gui->renderer, 
			WHITE_KNIGHT_PATH
			);
	gui->textures[WHITE_BISHOP] = load_texture(
			gui->renderer, 
			WHITE_BISHOP_PATH
			);
	gui->textures[WHITE_ROOK] = load_texture(
			gui->renderer, 
			WHITE_ROOK_PATH
			);
	gui->textures[WHITE_QUEEN] = load_texture(
			gui->renderer, 
			WHITE_QUEEN_PATH
			);
	gui->textures[WHITE_KING] = load_texture(
			gui->renderer, 
			WHITE_KING_PATH
			);

	gui->textures[BLACK_PAWN] = load_texture(
			gui->renderer, 
			BLACK_PAWN_PATH
			);
	gui->textures[BLACK_KNIGHT] = load_texture(
			gui->renderer, 
			BLACK_KNIGHT_PATH
			);
	gui->textures[BLACK_BISHOP] = load_texture(
			gui->renderer, 
			BLACK_BISHOP_PATH
			);
	gui->textures[BLACK_ROOK] = load_texture(
			gui->renderer, 
			BLACK_ROOK_PATH
			);
	gui->textures[BLACK_QUEEN] = load_texture(
			gui->renderer, 
			BLACK_QUEEN_PATH
			);
	gui->textures[BLACK_KING] = load_texture(
			gui->renderer, 
			BLACK_KING_PATH
			);

	if (gui->window == NULL) {
		printf("Window could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}

	if (gui->renderer == NULL) {
		printf("Renderer could not be created! SDL_Error: %s\n", SDL_GetError());
		exit(1);
	}
}

void gui_draw_board(struct GUI* gui, Board* board, uint8_t tinted_square) {
	SDL_SetRenderDrawColor(gui->renderer, 0, 0, 0, 255);
	SDL_RenderClear(gui->renderer);

	uint8_t tinted_square_rank = tinted_square / 8;
	uint8_t tinted_square_file = tinted_square % 8;

	tinted_square = (7 - tinted_square_rank) * 8 + tinted_square_file;

	SDL_Rect rect;
	rect.w = SQUARE_SIZE;
	rect.h = SQUARE_SIZE;
	for (int i = 0; i < 8; ++i) {
		rect.y = i * SQUARE_SIZE;
		for (int j = 0; j < 8; ++j) {
			rect.x = j * SQUARE_SIZE;
			int tint = (i * 8 + j == tinted_square) ? 64 : 0;
			if ((i + j) % 2 == 0) {
				SDL_SetRenderDrawColor(
						gui->renderer, 
						255, 
						255 - tint, 
						255, 
						255
						);
			} 
			else {
				SDL_SetRenderDrawColor(
						gui->renderer, 
						196, 
						164 - tint, 
						132, 
						255
						);
			}
			SDL_RenderFillRect(gui->renderer, &rect);
			enum ColoredPiece piece = colored_piece_at(board, (7 - i) * 8 + j);
			if (piece != EMPTY_SQUARE) {
				SDL_RenderCopy(
						gui->renderer, 
						gui->textures[piece], 
						NULL, 
						&rect
						);
			}
		}
	}

	SDL_RenderPresent(gui->renderer);
}

void gui_quit(struct GUI* gui) {
	SDL_DestroyRenderer(gui->renderer);
	SDL_DestroyWindow(gui->window);
	SDL_Quit();
}

bool gui_handle_player_loop(struct GUI* gui, Board* board) {
	SDL_Event event;

	uint8_t num_clicks = 0;

	uint8_t src_square = 64;
	uint8_t dst_square = 64;

	MoveList moves;
	init_move_list(&moves);
	get_legal_moves(board, &moves, WHITE);

	while (1) {
		while (SDL_PollEvent(&event) != 0) {
			if (event.type == SDL_QUIT) {
				return true;
			}

			// Detect mouse click
			if (event.type == SDL_MOUSEBUTTONDOWN) {
				int x, y;
				SDL_GetMouseState(&x, &y);
				uint8_t file = x / SQUARE_SIZE;
				uint8_t rank = 7 - y / SQUARE_SIZE;
				uint8_t square = rank * 8 + file;
				if (square >= 0 && square < 64) {
					if (num_clicks == 0) {
						src_square = square;
						++num_clicks;
					} 
					else if (num_clicks == 1) {
						if (square == src_square) {
							src_square = 64;
							num_clicks = 0;
							continue;
						}

						dst_square = square;
						++num_clicks;
						goto make_move;
					}
				}
				gui_draw_board(gui, board, src_square);
				continue;
			}
			gui_draw_board(gui, board, src_square);

			SDL_Delay(10);
			continue;

			make_move: {
				Move move;
				create_move(&move, src_square, dst_square);

				if (is_move_legal(&moves, move)) {
					_make_move(board, src_square, dst_square);
					return false;
				}
				printf("Illegal move!\n");
				printf("Move: %s", translate_square_from_index(src_square));
				printf("%s\n", translate_square_from_index(dst_square));


				num_clicks = 0;
			}
		}
	}
}

