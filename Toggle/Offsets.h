#pragma once
#include <windows.h>
#include "CGlowObjectManager.h"
#include "CUtility.h"

namespace OFFSETS
{
	CGlowObjectManager* pGlowObjectManager = nullptr;
	uintptr_t pLocalPlayer;
	uintptr_t oEntTeam = 0xF4;
	DWORD pEntityList = 0x0;
}