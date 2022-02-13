#pragma once
#pragma once
#include <SDL.h>

SDL_Window* bogusWindow;

SDL_Window* bogusSDL_CreateWin(const char* title, int x, int y, int w, int h, Uint32 flags);

SDL_Window* bogusSDL_CreateWin(const char* title, int x, int y, int w, int h, Uint32 flags)
{
	bogusWindow = SDL_CreateWindow(title, x, y, w * 2, h * 2, flags);

	return bogusWindow;
}
//Applications usually have a jmp table to the dll functions, so this accounts for that
//This file is small, but trust me it will get bigger as I come up with more ideas