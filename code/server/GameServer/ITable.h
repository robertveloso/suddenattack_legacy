#pragma once

#include "../System.h"

namespace GameServer {

class ITable
{
public:
	class					IFactory
	:	public				Singleton<IFactory>
	{
	public:
		virtual				~IFactory() {}

		virtual ITable *	Create( const CHAR * i_strSource, CULONG i_ulSourceLength, CHAR i_cRowSplitter, CHAR i_cColumnSplitter, CBOOL i_bTrim ) = 0;
		virtual void		Destroy( ITable * i_pTable ) = 0;

		virtual STRING *	CreateItem() = 0;
		virtual void		DestroyItem( STRING * i_pItem ) = 0;
	};
	static inline			IFactory *
	IFACTORY()				{ return IFactory::GetInstance(); }

public:
	virtual					~ITable() {}

	virtual CULONG			GetMaxRow() = 0;
	virtual CULONG			GetMaxColumn() = 0;
	
	virtual STRING const *	GetItem( CULONG i_ulRow, CULONG i_ulColumn ) = 0;

};

} /* GameServer */