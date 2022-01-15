#pragma once
#include <Windows.h>
#define GRAVITY_CONST 0x3FCC0000 //it's a float
#define CLASSICGRAV_CONST 0.21875
#define GROUNDACCEL_CONST 0.046875
#define AIRACCEL_CONST 0.09375

const DWORD SUPER_JUMP = 0x41000000;
const DWORD SUPERACCEL_MAX = 0x41200000;
const DWORD SUPERACCEL_CONST = 0x3e400000;

const DWORD CLASSIC_JUMP = 0x40d00000;
const DWORD CLASSICACCEL_MAX = 0x40c00000;
const DWORD CLASSICACCEL_CONST = 0x3d400000;

typedef struct {
	UINT32 minute;
	UINT32 seconds;
	UINT32 milliseconds;

	UINT32 pause;
} sonicTimer;

BOOL compareTimers(sonicTimer x, sonicTimer y)
{
	if (x.minute == y.minute)
	{
		if (x.seconds == y.seconds)
		{
			if (x.milliseconds == y.milliseconds)
			{
				return true;
			}
		}
	}
	return false;
}



typedef struct {
	UINT32 character;
	BYTE unk_0;
	BYTE unk_1;
	BYTE unk_2;
	BYTE depth;
	UINT32 unk_word1;
	float x;
	float y;

	float x_totalvelo; //read only
	float y_totalvelo; //reads and writes
	
	float x_airvelo; //read only
	float y_airvelo; //read only

	float x_groundvelo; //reads and writes

} playerStruct;

sonicTimer* gameTimer;

float reserve_xvel = 0;
sonicTimer reserve_timer = { 0,0,0 };
DWORD ring_counter = (DWORD)0x196F80;
DWORD shieldAddress = (DWORD)0x19F984;
DWORD hurtTimer = (DWORD)0x19F988;
double* gravityAddress = (double*)0x97918;
DWORD accelerationConst = 0x19F990;
DWORD accelerationMax = 0x19F998;