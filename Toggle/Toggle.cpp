// Toggle.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include "CUtility.h"
#include "Offsets.h"

CUtility Util;

// Potential Improvements: Move to separate file & class
int GetClassID(uintptr_t ent)
{
	auto pIClientNetworkableVtable = Util.Read<uintptr_t>(ent + 0x8);
	auto pGetClientClass = Util.Read<uintptr_t>(pIClientNetworkableVtable + 2 * 0x4); // 3rd Func
	auto pClientClass = Util.Read<uintptr_t>(pGetClientClass + 0x1); // Pointer to struct out of mov eax
	return Util.Read<uintptr_t>(pClientClass + 0x14);
}

// Potential Improvements: Move to separate file & class
// Potential Improvements: Read whole chunk instead of per-player
VOID DoGlow()
{
	Utility::GlowObjectManager GlowObjData = Util.Read<Utility::GlowObjectManager>((uintptr_t)OFFSETS::pGlowObjectManager);
	Utility::playerClass localPlayer = Util.Read<Utility::playerClass>((uintptr_t)OFFSETS::pLocalPlayer + 0x4);
	if (GlowObjData.pObjects == nullptr) return;
	for (int i = 0; i < GlowObjData.ObjectCount ; ++i)
	{
		uintptr_t *GlowDefOffset = (uintptr_t *)((byte*)GlowObjData.pObjects + i * 0x38);
		Utility::glowDefStruct glowData = Util.Read<Utility::glowDefStruct>((uintptr_t)GlowDefOffset);

		if (glowData.m_hEntity == nullptr) continue;
		int iClassID = GetClassID((uintptr_t)glowData.m_hEntity);
		if (iClassID != 40) continue;
		Utility::playerClass currentPlayer = Util.Read<Utility::playerClass>((uintptr_t)glowData.m_hEntity + 0x4);
		if (currentPlayer.m_iTeamNum == localPlayer.m_iTeamNum) continue;

		glowData.m_vGlowColor = Utility::Vector3(
			(255 - (currentPlayer.m_iHealth *2.55))/255,
			(currentPlayer.m_iHealth*2.55) / 255,
			0.f);
		glowData.m_flGlowAlpha = .7f;
		glowData.m_bRenderWhenOccluded = true;
		glowData.m_bRenderWhenUnoccluded = false;
		glowData.m_bFullBloomRender = false;

		Util.Write<Utility::glowDefStruct>((uintptr_t)GlowDefOffset, glowData);
	}
}

int main()
{
	Util.runProcess();
	OFFSETS::pGlowObjectManager = (CGlowObjectManager*)Util.FindPattern(Util.modClient, (byte*)"\xA1\x00\x00\x00\x00\xA8\x01\x75\x4B", "x????xxxx", 4, 1);
	OFFSETS::pLocalPlayer = Util.Read<uintptr_t>((uintptr_t)Util.FindPattern(Util.modClient, (byte*)"\x8D\x34\x85\x00\x00\x00\x00\x89\x15\x00\x00\x00\x00\x8B\x41\x08\x8B\x48\x04\x83\xF9\xFF", "xxx????xx????xxxxxxxxx", 4, 3));

	while (true) DoGlow();
	
}
