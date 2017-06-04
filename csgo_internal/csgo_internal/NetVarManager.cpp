#include "SDK.h"

void CNetVarManager::Initialize()
{
	m_tables.clear();

	ClientClass* clientClass = I::Client->GetAllClasses();
	if( !clientClass )
		return;

	while( clientClass )
	{
		RecvTable* recvTable = clientClass->m_pRecvTable;
		m_tables.push_back( recvTable );

		clientClass = clientClass->m_pNext;
	}
}

int CNetVarManager::GetOffset( const char* tableName, const char* propName )
{
	int offset = Get_Prop( tableName, propName );
	if( !offset )
	{
		return 0;
	}
	return offset;
}

bool CNetVarManager::HookProp( const char* tableName, const char* propName, RecvVarProxyFn fun )
{
	RecvProp* recvProp = 0;
	Get_Prop( tableName, propName, &recvProp );
	if( !recvProp )
		return false;

	recvProp->m_ProxyFn = fun;

	return true;
}

int CNetVarManager::Get_Prop( const char* tableName, const char* propName, RecvProp** prop )
{
	RecvTable* recvTable = GetTable( tableName );
	if( !recvTable )
		return 0;

	int offset = Get_Prop( recvTable, propName, prop );
	if( !offset )
		return 0;

	return offset;
}

int CNetVarManager::Get_Prop( RecvTable* recvTable, const char* propName, RecvProp** prop )
{
	int extraOffset = 0;
	for( int i = 0; i < recvTable->m_nProps; ++i )
	{
		RecvProp* recvProp = &recvTable->m_pProps[ i ];
		RecvTable* child = recvProp->m_pDataTable;

		if( child && ( child->m_nProps > 0 ) )
		{
			int tmp = Get_Prop( child, propName, prop );
			if( tmp )
				extraOffset += ( recvProp->m_Offset + tmp );
		}

		if( stricmp( recvProp->m_pVarName, propName ) )
			continue;

		if( prop )
			*prop = recvProp;

		return ( recvProp->m_Offset + extraOffset );
	}

	return extraOffset;
}

RecvTable* CNetVarManager::GetTable( const char* tableName )
{
	if( m_tables.empty() )
		return 0;

	for each( RecvTable* table in m_tables )
	{
		if( !table )
			continue;

		if( stricmp( table->m_pNetTableName, tableName ) == 0 )
			return table;
	}

	return 0;
}

void CNetVarManager::DumpTable( RecvTable* table, int depth )
{
	std::string pre( "" );
	for( int i = 0; i < depth; i++ )
		pre.append( "\t" );

	m_file << pre << table->m_pNetTableName << "\n";

	for( int i = 0; i < table->m_nProps; i++ )
	{
		RecvProp* prop = &table->m_pProps[ i ];
		if( !prop )
			continue;

		std::string varName( prop->m_pVarName );

		if( varName.find( strenc( "baseclass" ) ) == 0 || varName.find( "0" ) == 0 || varName.find( "1" ) == 0 || varName.find( "2" ) == 0 )
			continue;

		m_file << pre << "\t " << varName << " "
				<< std::setfill( '_' ) << std::setw( 60 - varName.length() - ( depth * 4 ) )
				<< "[0x" << std::setfill( '0' ) << std::setw( 8 ) << std::hex
				<< std::uppercase << prop->m_Offset << "]\n";

		if( prop->m_pDataTable )
			DumpTable( prop->m_pDataTable, depth + 1 );
	}
}

void CNetVarManager::DumpNetvars()
{
	m_file.open( "C:/Users/Admin/Desktop/netdump.txt" );
	m_file << strenc( "NetVar Dump by A5 / teamgamerfood\n\nhaha look proper formatting \n\n" );

	for( ClientClass* pClass = I::Client->GetAllClasses(); pClass != NULL; pClass = pClass->m_pNext )
	{
		RecvTable* table = pClass->m_pRecvTable;
		DumpTable( table, 0 );
	}

	m_file.close();
}

CNetVarManager* NetVarManager = new CNetVarManager;
