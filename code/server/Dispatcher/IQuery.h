#pragma once

#include "../System.h"

#import "c:\Program Files\Common Files\System\ADO\msado15.dll" \
    no_namespace rename("EOF", "EndOfFile")

namespace Dispatcher {
class IClient;
} /* Dispatcher */

namespace Session {			namespace Manage {
class CDbObject;
} /* Manage */				} /* Session */
	
namespace Dispatcher {

class IQuery
{
public:
	struct E_RESULT {		enum ENUM {
		SUCCESS				= 0,
		FAIL,
		__MAX__,
	}; /* ENUM */			}; /* E_RESULT */

	typedef					::Session::Manage::CDbObject
							COMMAND;

public:
	class					IObserver
	{
	public:
		virtual				~IObserver() {}

		virtual void		OnResponse( IQuery * i_pQuery ) = 0;

	};

	class					IFactory
	:	public				Singleton<IFactory>
	{
	public:
		virtual				~IFactory() {}

		virtual IQuery *	Create( CDWORD i_dwCode ) = 0;
		virtual void		Destroy( IQuery * i_pQuery ) = 0;

	};
	static inline			IFactory *
	IFACTORY()				{ return IFactory::GetInstance(); }

	class					IBuilder
	:	public				Singleton<IBuilder>
	{
	public:
		virtual				~IBuilder() {}
		
		virtual void		Build( IQuery * i_pQuery, CSTRING & i_strSP ) = 0;
		virtual void		Build( IQuery * i_pQuery, CSTRING & i_strParam, CHAR i_cData, CBOOL i_bInput ) = 0;
		virtual void		BuildTiny( IQuery * i_pQuery, CSTRING & i_strParam, BYTE i_byData, CBOOL i_bInput ) = 0;
		virtual void		Build( IQuery * i_pQuery, CSTRING & i_strParam, SHORT i_nData, CBOOL i_bInput ) = 0;
		virtual void		Build( IQuery * i_pQuery, CSTRING & i_strParam, INT i_nData, CBOOL i_bInput ) = 0;
		virtual void		Build( IQuery * i_pQuery, CSTRING & i_strParam, __int64 i_nData, CBOOL i_bInput ) = 0;
		virtual void		Build( IQuery * i_pQuery, CSTRING & i_strParam, CSTRING & i_strData, CBOOL i_bInput, INT i_nDataLen ) = 0;
		virtual void		BuildLowStr( IQuery * i_pQuery, CSTRING & i_strParam, const CHAR * i_strData, CBOOL i_bInput, INT i_nDataLen ) = 0;

	};
	static inline			IBuilder *
	IBUILDER()				{ return IBuilder::GetInstance(); }

	class					IResponser
	:	public				Singleton<IResponser>
	{
	public:
		virtual				~IResponser() {}

		virtual CHAR		ReadChar( IQuery * i_pQuery, CSTRING & i_strParam ) = 0;
		virtual SHORT		ReadShort( IQuery * i_pQuery, CSTRING & i_strParam ) = 0;
		virtual INT			ReadInt( IQuery * i_pQuery, CSTRING & i_strParam ) = 0;
		virtual	LONGLONG 	ReadInt64( IQuery * i_pQuery, CSTRING & i_strParam ) = 0;
		virtual CSTRING &	ReadString( IQuery * i_pQuery, CSTRING & i_strParam ) = 0;
		virtual const char*	ReadRawString( IQuery * i_pQuery, CSTRING & i_strParam ) = 0;

	};
	static inline			IResponser *
	IRESPONSER()			{ return IResponser::GetInstance(); }

	class					IRequester
	:	public				Singleton<IRequester>
	{
	public:
		typedef				IQuery::IObserver
							IOBSERVER;

	public:
		virtual				~IRequester() {}

		virtual IOBSERVER *	GetObserver() = 0;
		virtual void		SetObserver( IOBSERVER * i_pObserver ) = 0;

		virtual void		Regist( IQuery * i_pQuery, IClient * i_pClient ) = 0;
		virtual void		Regist( IQuery * i_pQuery ) = 0;
//		virtual IQuery *	Unregist( CDWORD i_dwCode ) = 0;

	};
	static inline			IRequester *
	IREQUESTER()			{ return IRequester::GetInstance(); }

public:
	virtual					~IQuery() {}

	virtual CDWORD			GetCode() = 0;
	virtual COMMAND *		GetCommand() = 0;

	virtual STRING &		GetLog() = 0;

	virtual E_RESULT::ENUM	GetResult() = 0;
	virtual void			SetResult( E_RESULT::ENUM i_eResult ) = 0;

};

} /* Dispatcher */