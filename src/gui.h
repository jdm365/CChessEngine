#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "board.h"

static const char* WHITE_PAWN_PATH = "res/images/white/pawn.png";
static const char* WHITE_KNIGHT_PATH = "res/images/white/knight.png";
static const char* WHITE_BISHOP_PATH = "res/images/white/bishop.png";
static const char* WHITE_ROOK_PATH = "res/images/white/rook.png";
static const char* WHITE_QUEEN_PATH = "res/images/white/queen.png";
static const char* WHITE_KING_PATH = "res/images/white/king.png";

static const char* BLACK_PAWN_PATH = "res/images/black/pawn.png";
static const char* BLACK_KNIGHT_PATH = "res/images/black/knight.png";	
static const char* BLACK_BISHOP_PATH = "res/images/black/bishop.png";
static const char* BLACK_ROOK_PATH = "res/images/black/rook.png";
static const char* BLACK_QUEEN_PATH = "res/images/black/queen.png";
static const char* BLACK_KING_PATH = "res/images/black/king.png";

struct GUI {
	SDL_Window*   window;
	SDL_Renderer* renderer;
	SDL_Texture*  textures[12];
};

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path);
void gui_init(struct GUI* gui);
void gui_draw(struct GUI* gui);
void gui_quit(struct GUI* gui);

void gui_draw_board(struct GUI* gui, Board* board);
