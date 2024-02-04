#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "board.h"

struct GUI {
	SDL_Window*   window;
	SDL_Renderer* renderer;
	SDL_Texture*  textures[12];
};

SDL_Texture* load_texture(SDL_Renderer* renderer, const char* path);
void gui_init(struct GUI* gui);
void gui_draw(struct GUI* gui);
void gui_quit(struct GUI* gui);

void gui_draw_board(struct GUI* gui, const Board* board, uint8_t tinted_square);
bool gui_handle_player_loop(struct GUI* gui, Board* board);
