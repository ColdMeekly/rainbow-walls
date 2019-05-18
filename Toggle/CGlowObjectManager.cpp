#include "CGlowObjectManager.h"

int CGlowObjectManager::RegisterGlowObject(DWORD pEntity, const Utility::Vector3 &vGlowColor, float flGlowAlpha, bool bRenderWhenOccluded, bool bRenderWhenUnoccluded, int nSplitScreenSlot)
{
	int nIndex;
	if (m_nFirstFreeSlot == GlowObjectDefinition_t::END_OF_FREE_LIST)
	{
		m_GlowObjectDefinitions.emplace_back();
		nIndex = m_GlowObjectDefinitions.size();
	}
	else
	{
		nIndex = m_nFirstFreeSlot;
		m_nFirstFreeSlot = m_GlowObjectDefinitions[nIndex].m_nNextFreeSlot;
	}

	m_GlowObjectDefinitions[nIndex].m_hEntity = pEntity;
	m_GlowObjectDefinitions[nIndex].m_vGlowColor = vGlowColor;
	m_GlowObjectDefinitions[nIndex].m_flGlowAlpha = flGlowAlpha;
	m_GlowObjectDefinitions[nIndex].flUnk = 0.0f;
	m_GlowObjectDefinitions[nIndex].m_flBloomAmount = 1.0f;
	m_GlowObjectDefinitions[nIndex].localplayeriszeropoint3 = 0.0f;
	m_GlowObjectDefinitions[nIndex].m_bRenderWhenOccluded = bRenderWhenOccluded;
	m_GlowObjectDefinitions[nIndex].m_bRenderWhenUnoccluded = bRenderWhenUnoccluded;
	m_GlowObjectDefinitions[nIndex].m_bFullBloomRender = false;
	m_GlowObjectDefinitions[nIndex].m_nFullBloomStencilTestValue = 0;
	m_GlowObjectDefinitions[nIndex].m_nSplitScreenSlot = nSplitScreenSlot;
	m_GlowObjectDefinitions[nIndex].m_nNextFreeSlot = GlowObjectDefinition_t::ENTRY_IN_USE;

	return nIndex;
}

void CGlowObjectManager::UnregisterGlowObject(int nGlowObjectHandle)
{
	assert(!m_GlowObjectDefinitions[nGlowObjectHandle].IsUnused());

	m_GlowObjectDefinitions[nGlowObjectHandle].m_nNextFreeSlot = m_nFirstFreeSlot;
	m_GlowObjectDefinitions[nGlowObjectHandle].m_hEntity = NULL;
	m_nFirstFreeSlot = nGlowObjectHandle;
}
int CGlowObjectManager::HasGlowEffect(DWORD  *pEntity) const
{
	for (int i = 0; i < m_GlowObjectDefinitions.size(); ++i)
	{
		// Room for error
		if (!m_GlowObjectDefinitions[i].IsUnused() && m_GlowObjectDefinitions[i].m_hEntity == (DWORD)pEntity)
		{
			return i;
		}
	}

	return NULL;
}

void CGlowObjectManager::GlowObjectDefinition_t::set(float r, float g, float b, float a)
{
	m_vGlowColor = Utility::Vector3(r, g, b);
	m_flGlowAlpha = a;
	m_bRenderWhenOccluded = true;
	m_bRenderWhenUnoccluded = false;
	m_flBloomAmount = 1.0f;
}

DWORD CGlowObjectManager::GlowObjectDefinition_t::getEnt()
{
	return m_hEntity;
}

bool CGlowObjectManager::GlowObjectDefinition_t::IsUnused() const
{
	return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE;
}