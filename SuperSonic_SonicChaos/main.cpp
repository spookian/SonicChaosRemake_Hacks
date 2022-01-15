#include <Windows.h>
#include <TlHelp32.h>
#include <iostream>
#include "Core.h"
#include "SonicChaosRemake.h"

BOOL transfState = false;
BOOL superState = false;

DWORD transformAddress = 0x6F1C1;
DWORD jumpAddress = 0x6E887;
DWORD superAddress = 0x6CBDF;

DWORD shieldStore = 0;

DWORD transformJMPBack;
DWORD superFormJMPBack;
DWORD dropDashJMPBack;
DWORD jumpJMPBack;
DWORD go;
BYTE frameTimer = 0;

void __declspec(naked) SuperForm()
{
	if (superState)
	{
		__asm
		{
			mov ax, cx
			mov dh, frameTimer
			cmp dh, 60
			jge drainRing
			add dh, 1
			jmp rest

			rest:
			mov frameTimer, dh

			mov edx, accelerationConst
			mov ecx, SUPERACCEL_CONST
			mov [edx], ecx
			mov edx, accelerationMax
			mov ecx, SUPERACCEL_MAX
			mov [edx], ecx

			mov ecx, 0x70000000
			mov edx, [shieldAddress]
			mov [edx], ecx
			mov ecx, 0x90000000
			mov edx, [hurtTimer]
			mov [edx], ecx
			jmp end

			drainRing:
			mov dh, 0
			mov ecx, [ring_counter]
			mov ebx, [ecx]
			sub ebx, 1
			mov [ecx], ebx
			cmp ebx, 0
			jle loseSuper
			jmp rest

			loseSuper:
			mov edx, accelerationConst
			mov ecx, CLASSICACCEL_CONST
			mov[edx], ecx
			mov edx, accelerationMax
			mov ecx, CLASSICACCEL_MAX
			mov[edx], ecx

			xor edx, edx
			mov [superState], edx
			mov ecx, [hurtTimer]
			or ecx, 0x00010000
			mov [ecx], edx
			mov edx, shieldStore
			mov ecx, [shieldAddress]
			mov [ecx], edx
			jmp end

			end:
			mov cx, ax
			jmp [superFormJMPBack]
		}
	}
	else
	{
		__asm {
			jmp [superFormJMPBack]
		}
	}

}//hook this to a function that executes every frame
//HOOK IT TO THE ANIMATION FUNCTION DO IT NOOWWWWWWWWWWWWWWW

void __declspec(naked) transform() //so check for super state, then check for transfState. If neither, then carry on
//check for 50 rings, if not then go to gateway
{
	__asm {
		mov edx, eax
		mov eax, ring_counter
		mov eax, [eax]
		cmp eax, 50
		jl exit_transform

		mov eax, [shieldAddress]
		mov eax, [eax]
		mov shieldStore, eax
		mov eax, 1
		mov superState, eax

		exit_transform:
		mov eax, edx
		jmp [transformJMPBack]
	}
} 
//hooks at drop dash start address, goes over to the sprite setting address and then replaces that instruction momentarily
//alternatively you could modify the gateway by having it link to the very end of the jump/dropdash function after two hooks

void __declspec(naked) dropDashTransformSprite()
{
	__asm {
		mov cx, 25;
		jmp [dropDashJMPBack]
	}
}

void __declspec(naked) jump()
{
	if (superState)
	{
		__asm {
			mulss xmm0, SUPER_JUMP
			jmp [go]
		}
	}
	else
	{
		__asm {
			jmp [jumpJMPBack]
		}
	}
}

DWORD WINAPI MainThread(LPVOID param)
{
	baseAddress = (DWORD)GetModuleHandleA(NULL);
	
	baseOffset(transformAddress);
	baseOffset(jumpAddress);
	baseOffset(superAddress);
	
	baseOffset(gravityAddress);
	baseOffset(accelerationConst);
	baseOffset(accelerationMax);

	baseOffset(ring_counter);
	baseOffset(shieldAddress);
	baseOffset(hurtTimer);

	int transformLength = 7, jumpLength = 5, superLength = 7;
	jmpBackAddy = transformAddress + transformLength;
	transformJMPBack = Hook((void*)transformAddress, transform, transformLength);

	jmpBackAddy = jumpAddress + jumpLength;
	jumpJMPBack = Hook((void*)jumpAddress, jump, jumpLength);
	go = jumpJMPBack + 5;

	jmpBackAddy = superAddress + superLength;
	superFormJMPBack = Hook((void*)superAddress, SuperForm, superLength);


	if (!(transformJMPBack && jumpJMPBack && superFormJMPBack))
	{
		FreeLibraryAndExitThread((HMODULE)param, 0);
		return 0;
	}

	while (true)
	{
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