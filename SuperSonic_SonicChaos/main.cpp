#include "Header.h"

void bogusSDL_LTGRenderPresent(SDL_Renderer* x)
{
	if (lowTier)
	{
		SDL_RenderCopy(x, lowTier, NULL, &rect);
	}
	SDL_RenderPresent(x);
}

void __declspec(naked) SuperForm()
{
	if (superState && gameTimer->pause)
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
	else if (transfState)
	{
		if (frameTimer > 59)
		{
			transfState = false;
			superState = true;
			*(double*)gravityAddress = 0.21875;
			frameTimer = 0;
		}
		else
		{
			frameTimer++;
		}

		__asm {
			mov cx, 25
			jmp [superFormJMPBack]
		}
	}
		
	else
	{
		WORD saveCX = 0; //STACK, BITCH
		__asm
		{
			mov saveCX, cx
		}

		frameTimer = 0;
	/*	if (!lowTier)
		{
			render = SDL_GetRenderer(bogusWindow);

			SDL_Surface* surf = SDL_LoadBMP("C:/rtdl_decompilation/LTG.bmp");
			if (surf)
			{
				SDL_SetWindowTitle(bogusWindow, "YOU SHOULD KYS... NOW!");

				lowTier = SDL_CreateTextureFromSurface(render, surf);
				SDL_FreeSurface(surf);
			}
		}
	*/
		__asm {
			mov cx, saveCX
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
		mov eax, superState
		cmp eax, 1
		je exit_transform

		mov eax, [shieldAddress]
		mov eax, [eax]
		mov shieldStore, eax

		mov eax, 0x90000000
		mov eax, [hurtTimer]

		mov eax, 1
		mov transfState, eax
		jmp resetConstants

		exit_transform:
		mov eax, edx
		jmp [transformJMPBack]

		resetConstants:
		mov eax, 0
		mov ecx, accelerationConst
		mov [ecx], eax
		mov ecx, gravityAddress
		mov [ecx+04], eax
		mov ecx, dy
		mov [ecx], eax
		mov [ecx-04], eax
		jmp exit_transform
	}
} 
//hooks at drop dash start address, goes over to the sprite setting address and then replaces that instruction momentarily
//alternatively you could modify the gateway by having it link to the very end of the jump/dropdash function after two hooks

void __declspec(naked) jump()
{
	if (superState)
	{
		__asm {
			mulss xmm0, SUPER_JUMP
			mov ecx, jumpJMPBack
			add ecx, 5
			jmp ecx
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
	gameTimer = (sonicTimer*)(baseAddress + 0x196FCC);	
	baseOffset(gravityAddress);
	baseOffset(accelerationConst);
	baseOffset(accelerationMax);
	baseOffset(ring_counter);
	baseOffset(shieldAddress);
	baseOffset(hurtTimer);
	baseOffset(dy);

	transformJMPBack = Hook((void*)transformAddress, transform, 7);
	jumpJMPBack = Hook((void*)jumpAddress, jump, 5);
	superFormJMPBack = Hook((void*)superAddress, SuperForm, 7);

	DWORD createWin = 0x9409F + baseAddress, x = 0, updateFrame = 0x94015 + baseAddress;
/*	VirtualProtect((LPVOID)createWin, 6, PAGE_EXECUTE_READWRITE, &x);
	VirtualProtect((LPVOID)updateFrame, 6, PAGE_EXECUTE_READWRITE, &x);

	DWORD* y = (DWORD*)&bogusSDL_CreateWin;
	*(DWORD*)((BYTE*)createWin + 2) = (DWORD)&y; //that was a reference to a function, by the way

	DWORD* z = (DWORD*)&bogusSDL_LTGRenderPresent;
	*(DWORD*)((BYTE*)updateFrame + 2) = (DWORD)&z;
*/
	if (!(transformJMPBack && jumpJMPBack && superFormJMPBack && VirtualProtect((LPVOID)gravityAddress, 8, PAGE_READWRITE, &x)))
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