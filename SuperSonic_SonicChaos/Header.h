#pragma once
#include <Windows.h>
#include <Core.h>
#include "SonicChaosRemake.h"

BOOL transfState = false;
BOOL superState = false;

DWORD transformAddress = 0x6F1C1;
DWORD jumpAddress = 0x6E887;
DWORD superAddress = 0x6CBDF;
DWORD inputHandler = 0x31ECE;
DWORD dy = 0x197CD8;

DWORD shieldStore = 0;

DWORD transformJMPBack;
DWORD superFormJMPBack;
DWORD jumpJMPBack;

BYTE frameTimer = 0;
SDL_Texture* lowTier = 0;
SDL_Renderer* render;
SDL_Rect rect = { 0, 0, 100, 300 };