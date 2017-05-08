#pragma once

class CGlowObjectManager
{
public:
	class GlowObjectDefinition_t
	{
	public:
		void set( Color color )
		{
			m_vGlowColor = Vector( color.rBase(), color.gBase(), color.bBase() );
			m_flGlowAlpha = color.aBase();
			m_bRenderWhenOccluded = true;
			m_bRenderWhenUnoccluded = false;
			m_flBloomAmount = 1.f;
		}

		CBaseEntity* getEntity()
		{
			return m_hEntity;
		}

		bool IsEmpty() const
		{
			return m_nNextFreeSlot != GlowObjectDefinition_t::ENTRY_IN_USE;
		}

	public:
		CBaseEntity* m_hEntity;
		Vector m_vGlowColor;
		float m_flGlowAlpha;

		char unknown[ 4 ];
		float flUnk;
		float m_flBloomAmount;
		float localplayeriszeropoint3;

		bool m_bRenderWhenOccluded;
		bool m_bRenderWhenUnoccluded;
		bool m_bFullBloomRender;
		char unknown1[ 1 ];

		int m_nFullBloomStencilTestValue;
		int iUnk;
		int m_nSplitScreenSlot;
		int m_nNextFreeSlot;

		// Special values for GlowObjectDefinition_t::m_nNextFreeSlot
		static const int END_OF_FREE_LIST = -1;
		static const int ENTRY_IN_USE = -2;
	};

	GlowObjectDefinition_t* m_GlowObjectDefinitions;
	int max_size;
	int pad;
	int size;
	GlowObjectDefinition_t* m_GlowObjectDefinitions2;
	int currentObjects;
};
