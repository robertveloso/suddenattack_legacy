#pragma once

#ifdef _SERVER

	#include "../../../Jolyo/NJean/Application/Packet.h"
	#include "../../LogicObject/Player/Type.h"
	#include "../../LogicObject/Player/Result.h"
	#include "../../LogicObject/Player/Stock.h"
	#include "../../LogicObject/Item.h"
	#include "../../LogicObject/Time.h"

#elif _TESTCLIENT

	#include "../../../../SAClient/Network/NJean/Application/Packet.h"
	#include "../../LogicObject/Player/Type.h"
	#include "../../LogicObject/Player/Stock.h"
	#include "../../LogicObject/Item.h"
	#include "../../LogicObject/Time.h"

#else

	#include "../../Lib_Network/NJean/Application/Packet.h"
	#include "../../Lib_LogicObject/Player/Type.h"
	#include "../../Lib_LogicObject/Player/Result.h"
	#include "../../Lib_LogicObject/Player/Stock.h"
	#include "../../Lib_LogicObject/Item.h"
	#include "../../Lib_LogicObject/Time.h"

#endif

namespace Packet {			namespace Service {			namespace Item {

enum						{ s_nItemCode = 700 };

struct CN_EnterShop
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 0 };
};

struct SN_Shop
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 1 };

	void					SetTable( const char * pTable )
	{
		int i;
		for( i = 0; i < 4096 - 1; i++ )
		{
			m_strTable[ i ]	= pTable[ i ];
			if( m_strTable[ i ] == '\0' )
			{
				break;
			}
		}
		m_strTable[ 4096 - 1 ] = '\0';

		this->SetLength( sizeof( SN_Shop ) - ( 4096 - 1 - i ) );
	}
	char					m_strTable[ 4096 ];
};

struct SN_EnterShop
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 2 };
};

struct CN_ExitShop
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 3 };
};

struct SN_ExitShop
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 4 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL_NOT_EXIST,		// 존재하지 않는 아이템이 포함되어 있다.
		FAIL_DEFICIT,		// 금액이 부족하다.
		FAIL_DB,			// 쿼리 실패
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	__int64					m_llRestMoney;

};

struct SS_Inven
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 5 };
};

struct SN_Inven
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 5 };

	typedef					::LogicObject::Time
							TIME;

	short					m_nCount;
	TIME					m_stServerTime;
	unsigned char			m_byServerMin;
};

struct SN_Item
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 6 };

	typedef					::LogicObject::Item
							ITEM;
	typedef					::LogicObject::Time
							TIME;

	ITEM					m_stItem;
	TIME					m_stExpireTime;
};

struct CQ_Buy
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 7 };

	typedef					::LogicObject::Item
							ITEM;

	ITEM					m_stItem;
	int						m_nPeriod;
	char					m_cDefault;
	char					m_cDisplay;			
};

struct SA_Buy
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 8 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL_NOT_EXIST,		// 아이템이 존재하지 않다.
		FAIL_OWN,			// 이미 소유한 아이템
		FAIL_DEFICIT,		// 금액이 부족하다.
		FAIL_DB1,			// 쿼리 실패
		FAIL_DB2,			// 쿼리 실패
		FAIL_DB3,			// 쿼리 실패
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	__int64					m_llRestMoney;

};

struct CQ_Discard
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 9 };

	typedef					::LogicObject::Item
							ITEM;

	ITEM					m_stItem;
};

struct SA_Discard
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 10 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL_NOT_EXIST,		// 아이템이 존재하지 않다.
		FAIL_DB1,			// 쿼리 실패
		FAIL_DB2,			// 쿼리 실패
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
};

struct SN_AddItem
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 11 };

	typedef					::LogicObject::Item
							ITEM;
	typedef					::LogicObject::Time
							TIME;

	ITEM					m_stItem;
	TIME					m_stExpireTime;
};

struct SN_RemoveItem
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 12 };

	typedef					::LogicObject::Item
							ITEM;

	ITEM					m_stItem;
};

struct SN_Equipment
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 13 };

	enum					{ s_nNull = 255 };

	unsigned char			m_byCount;
	unsigned char			m_byCode[ 12 ];

	void					Optimize()
	{
		char n;
		for( n = 11; n >= 0; n -- )
		{
			if( m_byCode[ n ] < s_nNull )
			{
				break;
			}
		}

		m_byCount			= n + 1;

		int nCodeSize		= sizeof( unsigned char ) * m_byCount;
		int nMaxSize		= sizeof( unsigned char ) * 12;
		this->SetLength( sizeof( SN_Equipment ) - ( nMaxSize - nCodeSize ) );
	}
};
//
//struct SN_PlayerEquipment
//:	public					CPacket
//{
//	enum					{ s_nCode = s_nItemCode + 14 };
//
//	unsigned char			m_byCount;
//	unsigned char			m_byCode[ 12 ];
//};

struct CQ_AddEquipment
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 15 };

	typedef					::LogicObject::Item
							ITEM;

	unsigned char			m_byIndex;
	ITEM					m_stItem;
};

struct SA_AddEquipment
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 16 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL_NO_INVEN,
		FAIL_WRONG_INDEX,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	unsigned char			m_byIndex;
	unsigned char			m_byCode;

};

struct CQ_RemoveEquipment
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 17 };

	unsigned char			m_byIndex;
};

struct SA_RemoveEquipment
:	public					CPacket
{
	enum					{ s_nCode = s_nItemCode + 18 };

	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL_NO_SELECTED,
	}; /* ENUM */			}; /* E_RESULT */

	unsigned char			m_byResult;
	unsigned char			m_byIndex;
};


//struct SN_Equipment
//:	public					CPacket
//{
//	enum					{ s_nCode = s_nItemCode + 13 };
//
//	typedef					::LogicObject::Item
//							ITEM;
//
//	struct					Stock
//	{
//		unsigned char		byClass;
//		ITEM				stCode;
//	};
//
//	unsigned char			m_byCount;
//	Stock					m_stStock[ 16 ];
//};
//
//struct SN_PlayerEquipment
//:	public					CPacket
//{
//	enum					{ s_nCode = s_nItemCode + 14 };
//
//	typedef					::LogicObject::Item
//							ITEM;
//
//	struct					Stock
//	{
//		unsigned char		byClass;
//		ITEM				stCode;
//	};
//
//	int						m_nPlayerIndex;
//	unsigned char			m_byCount;
//	Stock					m_stStock[ 16 ];
//};
//
//struct CQ_AddEquipment
//:	public					CPacket
//{
//	enum					{ s_nCode = s_nItemCode + 15 };
//
//	typedef					::LogicObject::Item
//							ITEM;
//
//	ITEM					m_stItem;
//	unsigned char			m_byClass;
//};
//
//struct SA_AddEquipment
//:	public					CPacket
//{
//	enum					{ s_nCode = s_nItemCode + 16 };
//
//	struct E_RESULT {		enum ENUM {
//		SUCCESS				= 0,
//		FAIL_NO_INVEN,
////		FAIL_SELECTED,		-> 교체
//		FAIL_MAX,
//	}; /* ENUM */			}; /* E_RESULT */
//
//	unsigned char			m_byResult;
//	unsigned char			m_byClass;
//};
//
//struct CQ_RemoveEquipment
//:	public					CPacket
//{
//	enum					{ s_nCode = s_nItemCode + 17 };
//
//	unsigned char			m_byClass;
//};
//
//struct SA_RemoveEquipment
//:	public					CPacket
//{
//	enum					{ s_nCode = s_nItemCode + 18 };
//
//	struct E_RESULT {		enum ENUM {
//		SUCCESS				= 0,
//		FAIL_NO_SELECTED,
//	}; /* ENUM */			}; /* E_RESULT */
//
//	unsigned char			m_byResult;
//	unsigned char			m_byClass;
//};


} /* Item */				} /* Service */				} /* Packet */
