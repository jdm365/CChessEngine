#include <stdio.h>

#include "gui.h"

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "board.h"

static const int WINDOW_WIDTH  = 600;
static const int WINDOW_HEIGHT = 600;
static const int SQUARE_SIZE = WINDOW_WIDTH / 8;

SDL_Texture* load_texture(
		SDL_Renderer* renderer, 
		const char* path
		) {
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

void gui_draw_board(struct GUI* gui, Board* board) {
	SDL_SetRenderDrawColor(gui->renderer, 0, 0, 0, 255);
	SDL_RenderClear(gui->renderer);

	SDL_Rect rect;
	rect.w = SQUARE_SIZE;
	rect.h = SQUARE_SIZE;
	for (int i = 0; i < 8; ++i) {
		rect.y = i * SQUARE_SIZE;
		for (int j = 0; j < 8; ++j) {
			rect.x = j * SQUARE_SIZE;
			if ((i + j) % 2 == 0) {
				SDL_SetRenderDrawColor(
						gui->renderer, 
						255, 
						255, 
						255, 
						255
						);
			} 
			else {
				SDL_SetRenderDrawColor(
						gui->renderer, 
						196, 
						164, 
						132, 
						255
						);
			}
			SDL_RenderFillRect(gui->renderer, &rect);
			enum ColoredPiece piece = piece_at(board, (7 - i) * 8 + j);
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
