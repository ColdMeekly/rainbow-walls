#pragma once
#include <windows.h>
#include "CUtility.h"
#include <vector>
#include <cassert>

class CGlowObjectManager
{
public:
	int	RegisterGlowObject(/*C_BaseEntity*/DWORD pEntity, const Utility::Vector3 &vGlowColor, float flGlowAlpha, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot);
	void	UnregisterGlowObject(int nGlowObjectHandle);
	int	HasGlowEffect(/*C_BaseEntity*/DWORD  *pEntity) const;

	class GlowObjectDefinition_t
	{
	public:
		void	set(float r, float g, float b, float a);
		DWORD	getEnt();
		bool	IsUnused() const;

	public:
		DWORD	m_hEntity;
		Utility::Vector3	m_vGlowColor;
		float	m_flGlowAlpha;

		char	unknown[4]; //pad 
		float	flUnk; //confirmed to be treated as a float while reversing glow functions 
		float	m_flBloomAmount;
		float	localplayeriszeropoint3;


		bool	m_bRenderWhenOccluded;
		bool	m_bRenderWhenUnoccluded;
		bool	m_bFullBloomRender;
		char	unknown1[1]; //pad 


		int	m_nFullBloomStencilTestValue; // 0x28 
		int	iUnk; //appears like it needs to be zero  
		int	m_nSplitScreenSlot; //Should be -1 

		// Linked list of free slots 
		int	m_nNextFreeSlot;

		// Special values for GlowObjectDefinition_t::m_nNextFreeSlot 
		static const int END_OF_FREE_LIST = -1;
		static const int ENTRY_IN_USE = -2;
	};

	int m_nFirstFreeSlot;
	std::vector< GlowObjectDefinition_t > m_GlowObjectDefinitions;
};

