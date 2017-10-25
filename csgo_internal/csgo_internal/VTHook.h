#pragma once

class VTHook
{
public:
	VTHook()
	{
		memset( this, 0, sizeof( VTHook) );
	}

	VTHook( PDWORD* ppdwClassBase )
	{
		bInitialize( ppdwClassBase );
	}

	~VTHook()
	{
		UnHook();
	}

	bool bInitialize( PDWORD* ppdwClassBase )
	{
		m_ClassBase = ppdwClassBase;
		m_OldVT = *ppdwClassBase;
		m_VTSize = GetVTCount( *ppdwClassBase );
		m_NewVT = new DWORD[ m_VTSize ];
		memcpy( m_NewVT, m_OldVT, sizeof( DWORD) * m_VTSize );
		*ppdwClassBase = m_NewVT;
		return true;
	}

	bool bInitialize( PDWORD** pppdwClassBase ) // fix for pp
	{
		return bInitialize( *pppdwClassBase );
	}

	void UnHook()
	{
		if( m_ClassBase )
		{
			*m_ClassBase = m_OldVT;
		}
	}

	void ReHook()
	{
		if( m_ClassBase )
		{
			*m_ClassBase = m_NewVT;
		}
	}

	int iGetFuncCount()
	{
		return ( int )m_VTSize;
	}

	DWORD GetFuncAddress( int Index )
	{
		if( Index >= 0 && Index <= ( int )m_VTSize && m_OldVT != NULL )
		{
			return m_OldVT[ Index ];
		}
		return NULL;
	}

	PDWORD GetOldVT()
	{
		return m_OldVT;
	}

	DWORD HookFunction( DWORD dwNewFunc, unsigned int iIndex )
	{
		if( m_NewVT && m_OldVT && iIndex <= m_VTSize && iIndex >= 0 )
		{
			m_NewVT[ iIndex ] = dwNewFunc;
			return m_OldVT[ iIndex ];
		}

		return NULL;
	}

private:
	DWORD GetVTCount( PDWORD pdwVMT )
	{
		DWORD dwIndex = 0;

		while (IsBadCodePtr((FARPROC)pdwVMT[dwIndex]) == false) {
			dwIndex++;
		}
		return dwIndex;
	}

	PDWORD* m_ClassBase;
	PDWORD m_NewVT, m_OldVT;
	DWORD m_VTSize;
};
