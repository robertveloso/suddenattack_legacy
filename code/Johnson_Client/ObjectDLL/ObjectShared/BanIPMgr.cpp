// ----------------------------------------------------------------------- //
//
// MODULE  : BanIPMgr.cpp
//
// PURPOSE : Handles banned IP's.
//
// CREATED : 10/24/02
//
// (c) 1999-2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#include "stdafx.h"
#include "BanIPMgr.h"

class BanIPMgr_Impl : public BanIPMgr
{
	friend class BanIPMgr;

	protected:

		// Not allowed to create directly.  Use Instance().
		BanIPMgr_Impl( );

		// Copy ctor and assignment operator not implemented and should never be used.
		BanIPMgr_Impl( BanIPMgr_Impl const& other );
		BanIPMgr_Impl& operator=( BanIPMgr_Impl const& other );

	public:

		// This destructor should be private, but if it is, the compiler complains
		// that the Instance function does not have access to it.  Instance should
		// have access since it's a member function.  Compiler bug?
		virtual ~BanIPMgr_Impl();

	public:

		// Initializes the object.
		virtual bool Init( );

		// Terminates the object.
		virtual void Term( );

		// Called when client is added.  returns true if client allowed.
		virtual bool OnAddClient( HCLIENT hClient );

		// Call to add a ban.
		virtual bool AddBan( char const* pszBanIP );
		virtual bool AddBan( ClientIP const& bannedIP );

		// Call to remove a ban.
		virtual bool RemoveBan( char const* pszBanIP );
		virtual bool RemoveBan( ClientIP const& bannedIP );

		// Get the list of bans.
		virtual BanList const& GetBanList( ) { return m_BanList; }

	protected:

		// Checks if the client matches banned IP's.
		bool IsClientBanned( HCLIENT hClient );

		// Reads current bans from bute file.
		bool ReadBans( );

		// Writes current bans to bute file.
		bool WriteBans( );

	protected:

		bool m_bInitialized;
		BanList	m_BanList;
};


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr::Instance()
//
//	PURPOSE:	Instatiator of singleton
//
//  This function is the only way to instatiate this object.  It
//  ensures that there is only one object, the singleton.
//
// ----------------------------------------------------------------------- //

BanIPMgr& BanIPMgr::Instance( )
{
	// Putting the singleton as a static function variable ensures that this
	// object is only created if it is used.
	static BanIPMgr_Impl sSingleton;
	return sSingleton;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr::ConvertClientIPFromString
//
//	PURPOSE:	Converts string to ClientIP.
//
//	RETURN:		true on success.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr::ConvertClientIPFromString( char const* pszBanIP, ClientIP& bannedIP )
{
	// Default to all parts banned.
	bannedIP.m_nPart[0] = ( uint8 )-1;
	bannedIP.m_nPart[1] = ( uint8 )-1;
	bannedIP.m_nPart[2] = ( uint8 )-1;
	bannedIP.m_nPart[3] = ( uint8 )-1;

	// Check inputs.
	if( !pszBanIP || !pszBanIP[0] )
		return false;

	// Copy the string so we can strtok.
	char szBanIP[16] = "";
	strncpy( szBanIP, pszBanIP, ARRAY_LEN( szBanIP ));

	// Break the string up into the four parts.
	char* pszToken = strtok( szBanIP, "." );
	bool bWildCard = false;
	for( int i = 0; i < 4 && pszToken; i++ )
	{
		// Check if there's a wildcard character in there.
		// If so, we can stop converting since the rest
		// will be banned.
		if( strstr( pszToken, "*" ))
		{
			bWildCard = true;
			break;
		}

		// Make sure the number is within range.
		int nNum = atoi( pszToken );
		if( nNum < 0 || nNum > 255 )
			return false;

		bannedIP.m_nPart[i] = ( uint8 )nNum;

		// Get next token.
		pszToken = strtok( NULL, "." );
	}

	// See if it's an invalid IP.
	if( !bWildCard && i < 4 )
		return false;

	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr::ConvertClientIPToString
//
//	PURPOSE:	Converts ClientIP to a string.
//
//	RETURN:		true on success.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr::ConvertClientIPToString( ClientIP const& bannedIP, char* pszClientIP, uint32 nSize )
{
	// Check inputs.
	if( !pszClientIP )
		return false;

	std::string sClientIP = "";

	for( int i = 0; i < 4; i++ )
	{
		// Check if this part is using a wildcard.
		if( bannedIP.m_nPart[i] == ( uint8 )-1 )
		{
			sClientIP += "*";
			break;
		}
		// A real number.
		else
		{
			char szNum[16] = "";
			sprintf( szNum, "%d", bannedIP.m_nPart[i] );
			sClientIP += szNum;
		}

		// If not the last part, put in a separator.
		if( i < 3 )
		{
			sClientIP += ".";
		}
	}

	// If there isn't room, then return failure.
	if( sClientIP.length( ) > nSize - 1 )
		return false;

	strcpy( pszClientIP, sClientIP.c_str( ));

	return true;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::BanIPMgr_Impl
//
//	PURPOSE:	Constructor
//
// ----------------------------------------------------------------------- //

BanIPMgr_Impl::BanIPMgr_Impl( )
{
	m_bInitialized = false;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::~BanIPMgr_Impl
//
//	PURPOSE:	Destructor
//
// ----------------------------------------------------------------------- //

BanIPMgr_Impl::~BanIPMgr_Impl( )
{
	Term( );
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::Init
//
//	PURPOSE:	Initializes object.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr_Impl::Init( )
{
	// Start fresh.
	Term( );

	// Read in the bans from the bute.
	if( !ReadBans( ))
		return false;

	m_bInitialized = true;

	return true;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::Term
//
//	PURPOSE:	Terminates object.
//
// ----------------------------------------------------------------------- //

void BanIPMgr_Impl::Term( )
{
	// Write the current set of bans.
	WriteBans( );

	m_bInitialized = false;

	m_BanList.clear( );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::OnAddClient
//
//	PURPOSE:	Called when client is added.  returns true if client allowed.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr_Impl::OnAddClient( HCLIENT hClient )
{
	return !IsClientBanned( hClient );
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::AddBan
//
//	PURPOSE:	Adds a ban to the list.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr_Impl::AddBan( char const* pszBanIP )
{
	ClientIP bannedIP;
	if( !ConvertClientIPFromString( pszBanIP, bannedIP ))
		return false;

	if( !AddBan( bannedIP ))
		return false;
	
	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::AddBan
//
//	PURPOSE:	Adds a ban to the list.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr_Impl::AddBan( ClientIP const& bannedIP )
{
	// Don't allow banning of everyone.
	if( bannedIP.m_nPart[0] == 255 )
	{
		return false;
	}

	// Don't allow banning of local host, which has a special IP.
	if( bannedIP.m_nPart[0] == 0 && bannedIP.m_nPart[1] == 0 && 
		bannedIP.m_nPart[2] == 0 && bannedIP.m_nPart[3] == 0 )
	{
		return false;
	}

	// Add it to the list.
	m_BanList.insert( bannedIP );

	// Iterate over all the clients and see if any match the new banned IP.
    HCLIENT hIterClient = g_pLTServer->GetNextClient( NULL );
    while( hIterClient )
	{
		HCLIENT hNextIterClient = g_pLTServer->GetNextClient( hIterClient );

		if( IsClientBanned( hIterClient ))
		{
			g_pLTServer->CPrint( "KickClient %s %d", __FILE__, __LINE__ );
			g_pLTServer->KickClient( hIterClient );
		}

	    hIterClient = hNextIterClient;
	}
	
	return true;
}

// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::RemoveBan
//
//	PURPOSE:	Removes a ban from the list.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr_Impl::RemoveBan( char const* pszBanIP )
{
	ClientIP bannedIP;
	if( !ConvertClientIPFromString( pszBanIP, bannedIP ))
		return false;

	if( !RemoveBan( bannedIP ))
		return false;

	return true;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::RemoveBan
//
//	PURPOSE:	Removes a ban from the list.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr_Impl::RemoveBan( ClientIP const& bannedIP )
{
	m_BanList.erase( bannedIP );

	return true;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::IsClientBanned
//
//	PURPOSE:	Check if client matches banned IP's.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr_Impl::IsClientBanned( HCLIENT hClient )
{
	// Get the clients IP.
	uint8 aClientIP[4];
	uint16 nPort;
	g_pLTServer->GetClientAddr( hClient, aClientIP, &nPort );

	// Look through list of banned ip's.
	for( BanList::iterator iter = m_BanList.begin( ); iter != m_BanList.end( ); iter++ )
	{
		ClientIP const& bannedIP = *iter;
		bool bBanned = false;

		for( int i = 0; i < 4; i++ )
		{
			// Check if no IP's allowed with this part.
			if( bannedIP.m_nPart[i] == ( uint8 )-1 )
			{
				bBanned = true;
				break;
			}

			// Check if this part doesn't match.
			if( bannedIP.m_nPart[i] != aClientIP[i] )
				break;
		}

		// Check if they matched the ban.
		if( bBanned || i == 4 )
			return true;
	}

	// Client not banned.
	return false;
}


// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::ReadBans
//
//	PURPOSE:	Reads current bans from bute.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr_Impl::ReadBans( )
{
	CButeMgr banBute;
	banBute.Parse( "BanList.txt" );

	char szBanKey[128] = "";
	char szClientIP[16] = "";
	int nBanIndex = 0;

	// Read in the current set of bans.
	while( 1 )
	{
		// Read the next banned IP.
		sprintf( szBanKey, "Ban%d", nBanIndex );
		banBute.GetString( "Bans", szBanKey, "", szClientIP, ARRAY_LEN( szClientIP ));

		// If we didn't find one, the stop looking.
		if( !banBute.Success( ))
			break;

		// Add the ban.
		AddBan( szClientIP );
		nBanIndex++;
	}	

	return true;
}



// ----------------------------------------------------------------------- //
//
//	ROUTINE:	BanIPMgr_Impl::WriteBans
//
//	PURPOSE:	Writes current bans to bute.
//
// ----------------------------------------------------------------------- //

bool BanIPMgr_Impl::WriteBans( )
{
	// If not initialized, we can't write out the bans yet.
	if( !m_bInitialized )
		  

		return false;

	CButeMgr banBute;
	char szBanKey[128] = "";
	char szClientIP[16] = "";
	int nBanIndex = 0;

	// Look through list of banned ip's.
	for( BanList::iterator iter = m_BanList.begin( ); iter != m_BanList.end( ); iter++ )
	{
		ClientIP const& bannedIP = *iter;

		// Convert it to a string.
		if( !ConvertClientIPToString( bannedIP, szClientIP, ARRAY_LEN( szClientIP )))
			return false;

		// Write the banned IP.
		sprintf( szBanKey, "Ban%d", nBanIndex );
		banBute.SetString( "Bans", szBanKey, szClientIP );

		nBanIndex++;
	}

	// Save the list.
	banBute.Save( "BanList.txt" );

	return true;
}
