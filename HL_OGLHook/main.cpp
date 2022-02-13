#include <Windows.h>
#include <Core.h>
#include "SonicChaosRemake.h"

void updateXVelocity(HANDLE console, COORD oldPos, float* xPos, char* floater);
void updateTimer(HANDLE console, COORD oldPos, sonicTimer* gameTimer);

DWORD gateHook = 0;
char cmdMsg[200] = "Injection complete!\nX Velocity: ";
const char* spaceClear = "            ";

void clearLine(HANDLE console, COORD oldPos)
{
	SetConsoleCursorPosition(console, oldPos);
	WriteConsole(console, spaceClear, strlen(spaceClear), 0, 0);
	SetConsoleCursorPosition(console, oldPos);
}

void __declspec(naked) ourFunct()
{
	__asm {
		add eax, 2
		jmp [gateHook]
	}
}

DWORD WINAPI MainThread(LPVOID param)
{
	baseAddress = (DWORD)GetModuleHandleA(NULL);

	CONSOLE_SCREEN_BUFFER_INFO consoleSBI;
	float xPos = 0;
	char floater[50];

	AllocConsole();
	HANDLE console = GetStdHandle(STD_OUTPUT_HANDLE);

	int ringLength = 5;
	DWORD ringAddress = 0x5B436;
	baseOffset(ringAddress);
	gateHook = Hook((void*)ringAddress, ourFunct, ringLength);

	if (!gateHook)
	{
		FreeLibraryAndExitThread((HMODULE)param, 0);
		return 0;
	}
	WriteConsole(console, cmdMsg, strlen(cmdMsg), 0, 0);
	GetConsoleScreenBufferInfo(console, &consoleSBI);
	COORD oldPos = consoleSBI.dwCursorPosition;

	gameTimer = (sonicTimer*)(baseAddress + 0x196FCC);
	WriteConsole(console, "\nMinutes:\nSeconds:\nMilliseconds: \n", 32, 0, 0);
	GetConsoleScreenBufferInfo(console, &consoleSBI);
	COORD newPos = consoleSBI.dwCursorPosition;
	newPos.Y -= 2;

	char y[30];
	_itoa_s((DWORD)baseAddress, y, _countof(y), 16);
	WriteConsole(console, y, strlen(y), 0, 0);


	while (true)
	{
		updateXVelocity(console, oldPos, (float*)(baseAddress + 0x197CE4), floater);
		updateTimer(console, newPos, gameTimer);

		if (GetAsyncKeyState(VK_ESCAPE)) break;
		if (GetAsyncKeyState(0x45)) *(DWORD*)(baseAddress + 0x197CD8) = 0;
		Sleep(60);
	}

	FreeLibraryAndExitThread((HMODULE)param, 0);
	
	return 0;
}

BOOL WINAPI DllMain(HINSTANCE hinstDLL, DWORD fdwReason, LPVOID lpvReserved)
{
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		CreateThread(0, 0, MainThread, hinstDLL, 0, 0);
	}

	return TRUE;
}

void updateTimer(HANDLE console, COORD oldPos, sonicTimer* gameTimer)
{
	char x[10];
	DWORD y = (DWORD)gameTimer;

	if (!compareTimers(*gameTimer, reserve_timer))
	{
		for (int i = 0; i < 3; i++)
		{
			clearLine(console, oldPos);

			_itoa_s(*(PUINT32)y, x, _countof(x), 10);
			WriteConsole(console, x, strlen(x), 0, 0);

			y += 4;
			oldPos.Y++;
		}
		reserve_timer = *gameTimer;
	}
}

void updateXVelocity(HANDLE console, COORD oldPos, float* xPos, char* floater)
{
	if (*xPos != reserve_xvel)
	{
		clearLine(console, oldPos);
		_gcvt_s(floater, 50, (double)*xPos, 5);
		WriteConsole(console, floater, strlen(floater), 0, 0);
	}

	reserve_xvel = *xPos;
}