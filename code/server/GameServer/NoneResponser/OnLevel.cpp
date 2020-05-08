#include "StdAfx.h"

#include "./OnLevel.h"

#include "../Query.h"
#include "../IPlayer.h"
#include "../Player/IResult.h"
#include "../ITable.h"

#include "../../Dispatcher/ITime.h"
#include "../../Dispatcher/IConsole.h"


namespace GameServer {		namespace NoneResponser {

OnLevel::OnLevel()
{
}

OnLevel::~OnLevel()
{
}

void
OnLevel::OnResponse( IQUERY * i_pQuery )
{
	typedef					Query::Server
							SERVER;
	Assert( i_pQuery->GetCode() == SERVER::E_CODE::REQ_LEVEL );

	INT nResult				= -99;
	const char * strResult	= NULL;
	try
	{
		nResult				= IQUERY::IRESPONSER()->ReadInt( i_pQuery, STRING( "@o_sp_rtn" ) );
		if( nResult == 0 )
		{
			strResult		= IQUERY::IRESPONSER()->ReadRawString( i_pQuery, STRING( "@o_result_str" ) );
		}

	}
	catch(_com_error &e)
	{
		::_bstr_t clsSource( e.Source() );
		::_bstr_t clsDetail( e.Description() );

		STRING strSource;
		STRING strDetail;
		strSource.Printf( "Dev\t[RESP_ERR] Source: %s\r\n", static_cast<CHAR *>( clsSource ) );
		strDetail.Printf( "Dev\t[RESP_ERR] Detail: %s\r\n", static_cast<CHAR *>( clsDetail ) );

		::Dispatcher::ICONSOLE()->Write( strSource );
		::Dispatcher::ICONSOLE()->Write( strDetail );

		nResult				= -99;

		printf( "Server Start DB Error \n%s\n%s\n", strSource, strDetail );
		DEBUG_BREAK;
	}
	catch( ... )
	{
		nResult				= -99;

		printf( "Server Start DB Error : %d\n", nResult );
		DEBUG_BREAK;
	}

	if( i_pQuery->GetResult() == IQUERY::E_RESULT::FAIL )
	{
		nResult				= -99;
	}

	String<1024> strDbLog;
	strDbLog.Printf( "DB\t%s\t%s\t%d\n%s\t%s",
							::Dispatcher::ITIME()->GetTime().GetCSTR(),
							"SP_GS_GM_LEVEL_R",
							nResult,
							strResult,
							i_pQuery->GetLog().GetCSTR() );
	::Dispatcher::ICONSOLE()->WriteUnsafe( strDbLog.GetCSTR() );

	if( nResult == 0 )
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnLevel Success\r\n" ) );
		cout << "OnLevel Success" << endl;
	}
	else
	{
		::Dispatcher::ICONSOLE()->Output( STRING( "OnLevel Fail\r\n" ) );
		cout << "OnLevel Fail!!!" << endl;
		return;
	}

	ITable * pTable			= ITable::IFACTORY()->Create( strResult, SERVER::s_nLevelLength, '\r', '|', TRUE );
	for( ULONG nRow = 0; nRow < pTable->GetMaxRow(); nRow ++ )
	{
		STRING const * pIndex	= pTable->GetItem( nRow, 0 );
		STRING const * pMin		= pTable->GetItem( nRow, 2 );
		STRING const * pMax		= pTable->GetItem( nRow, 3 );
		STRING const * pAbs		= pTable->GetItem( nRow, 4 );

		if( pIndex == NULL )
		{
			::Dispatcher::ICONSOLE()->Output( STRING( "OnLevel Fail : Index is Null\r\n" ) );
			return;
		}
		if( pMin == NULL )
		{
			::Dispatcher::ICONSOLE()->Output( STRING( "OnLevel Fail : Min is Null\r\n" ) );
			return;
		}
		if( pMax == NULL )
		{
			::Dispatcher::ICONSOLE()->Output( STRING( "OnLevel Fail : Max is Null\r\n" ) );
			return;
		}
		if( pAbs == NULL )
		{
			::Dispatcher::ICONSOLE()->Output( STRING( "OnLevel Fail : Abs is Null\r\n" ) );
			return;
		}

		LONG lIndex			= atol( ( * pIndex ).GetCSTR() );
		LONG lMin			= atol( ( * pMin ).GetCSTR() );
		LONG lMax			= atol( ( * pMax ).GetCSTR() );
		BOOL bAbs			= FALSE;
		if( (* pAbs) == STRING( "0.000" ) )
		{
			bAbs			= TRUE;
		}

		if( lIndex != nRow )
		{
			::Dispatcher::ICONSOLE()->Output( STRING( "OnLevel Fail : Wrong Index\r\n" ) );
			return;
		}

		Player::IResult::IGRADE()->Add( lMin, lMax, bAbs );
	}

	//Player::IResult::IGRADE()->Add( 0, 3000, TRUE );
	//Player::IResult::IGRADE()->Add( 3001, 6000, TRUE );
	//Player::IResult::IGRADE()->Add( 6001, 9000, TRUE );
	//Player::IResult::IGRADE()->Add( 9001, 18000, TRUE );
	//Player::IResult::IGRADE()->Add( 18001, 30000, TRUE );
	//Player::IResult::IGRADE()->Add( 30001, 45000, TRUE );
	//Player::IResult::IGRADE()->Add( 45001, 75000, TRUE );
	//Player::IResult::IGRADE()->Add( 75001, 135000, TRUE );
	//Player::IResult::IGRADE()->Add( 135001, 255000, TRUE );
	//Player::IResult::IGRADE()->Add( 255001, 425000, TRUE );
	//Player::IResult::IGRADE()->Add( 425001, 775000, TRUE );
	//Player::IResult::IGRADE()->Add( 775001, 1275000, TRUE );
	//Player::IResult::IGRADE()->Add( 1275001, 2275000, FALSE );
	//Player::IResult::IGRADE()->Add( 2275001, 3575000, FALSE );
	//Player::IResult::IGRADE()->Add( 3575001, 5075000, FALSE );
	//Player::IResult::IGRADE()->Add( 5075001, 8075000, FALSE );
	//Player::IResult::IGRADE()->Add( 8075001, 12075000, FALSE );
	//Player::IResult::IGRADE()->Add( 12075001, 17075000, FALSE );
	//Player::IResult::IGRADE()->Add( 17075001, 27075000, FALSE );
	//Player::IResult::IGRADE()->Add( 27075001, 999999999, FALSE );

	ITable::IFACTORY()->Destroy( pTable );

	IPlayer::IBUILDER()->SetLock( FALSE, 0, 0 );
}

} /* NoneResponser */		} /* GameServer */