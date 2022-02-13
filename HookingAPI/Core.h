#pragma once
#include <Windows.h>
#include <TlHelp32.h>
#include "SDL_Hook.h"
#include "D3D_Hook.h"

#define baseOffset(A) A+=baseAddress
#define moduleOffset(A,B) A+=B

DWORD baseAddress;

DWORD Hook(void* toHook, void* ourFunct, int len)
{
	DWORD jmpBackAddy = (DWORD)toHook + len;
	if (len < 5)
	{
		return false;
	}

	DWORD gateway = (DWORD)VirtualAlloc(NULL, len + 5, MEM_COMMIT, PAGE_EXECUTE_READWRITE);
	if (!gateway)
	{
		return false;
	}
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

void Hook_AbsoluteJump(LPVOID fromAddress, LPVOID toAddress)
{
	DWORD x = 0;
	VirtualProtect(fromAddress, 6, PAGE_EXECUTE_READWRITE, &x);

	DWORD* y = (DWORD*)toAddress;
	*(DWORD*)((BYTE*)fromAddress + 2) = (DWORD)&y;
}