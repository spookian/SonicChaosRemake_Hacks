#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>

#define baseOffset(A) A+=baseAddress
#define moduleOffset(A,B) A+=B

DWORD baseAddress;
DWORD jmpBackAddy;

char cmdMsg[200] = "Injection complete!\nX Velocity: ";
const char* spaceClear = "            ";

void clearLine(HANDLE console, COORD oldPos)
{
	SetConsoleCursorPosition(console, oldPos);
	WriteConsole(console, spaceClear, strlen(spaceClear), 0, 0);
	SetConsoleCursorPosition(console, oldPos);
}

DWORD Hook(void* toHook, void* ourFunct, int len)
{
	if (len < 5)
	{
		return false;
	}

	DWORD gateway = (DWORD)VirtualAlloc(NULL, len + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	memcpy((LPVOID)gateway, toHook, len);

	*(BYTE*)(gateway + len) = 0xE9;
	*(DWORD*)(gateway + len + 1) = (jmpBackAddy - (gateway + len + 5));

	DWORD curProtection;
	VirtualProtect(toHook, len, PAGE_EXECUTE_READWRITE, &curProtection);

	memset(toHook, 0x90, len);

	DWORD relativeAddress = ((DWORD)ourFunct - (DWORD)toHook) - 5;

	*(BYTE*)toHook = 0xE9;
	*(DWORD*)((DWORD)toHook + 1) = relativeAddress;

	DWORD temp;
	VirtualProtect(toHook, len, curProtection, &temp);

	return gateway;
}