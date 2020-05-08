#ifndef __MURSUM_USERLIST_H__
#define __MURSUM_USERLIST_H__

#include "LTGUIMgr.h"

typedef struct _tagUSERITEM
{
	int		nIndex;
	char	szNick[MAX_NICK_LEN];
	int		nGrade;
} T_USER_ITEM, *T_USER_ITEMptr;

typedef struct _tagFRIENDITEM : _tagUSERITEM
{
	uint16		nStatus;	//Login, Logout ...
} T_FRIEND_ITEM, *T_FRIEND_ITEMptr;

class CSaListUser : public CLTGUIListView
{
public:
	CSaListUser();
	virtual ~CSaListUser();

	LTBOOL			Create( uint16 nPosX, uint16 nPosY,
							uint16 nWidth, uint16 nHeight,
							uint16 nItemHeight,
							CLTGUICommandHandler* pCommandHandler = LTNULL,
							LTBOOL	bDrawBack = LTFALSE,
							uint32	nBackColor = 0xFF000000,
							uint32	nBorderColor = 0xFFFFFFFF );
	
	LTBOOL			AddUser( T_USER_ITEMptr pUserItem );
	LTBOOL			RemoveUser( int nIndex );
	T_USER_ITEMptr	GetUserData( int nIndex );
	T_USER_ITEMptr	GetSelectedUserData();

	LTBOOL			OnLButtonDblClick( int x, int y );
	
	LTBOOL			OnRButtonDown(int x, int y);
	
	LTBOOL			OnWheelUp( int x, int y );
	LTBOOL			OnWheelDown( int x, int y );

protected:
	LTBOOL			RenderItem( void* pListItem, const LTIntPt* ptItemPos,
								int32 nIndex, bool bSelect, bool bOver );
	void			RenderItemText();
				
	CSAD3DText*		m_pNickList;
	
	LTPoly_GT4		m_PolyBar;
	LTPoly_GT4		m_PolyGrade;

};



class CSaListFriend : public CSaListUser
{
public:
	CSaListFriend();
	virtual ~CSaListFriend();

	LTBOOL				AddUser( T_FRIEND_ITEMptr pFriendItem );
	T_FRIEND_ITEMptr	GetUserData( int nIndex );
	T_FRIEND_ITEMptr	GetSelectedUserData();

	LTBOOL				SetStatus( uint16 nStatus );
		
	LTBOOL				OnWheelUp( int x, int y );
	LTBOOL				OnWheelDown( int x, int y );

protected:
	LTBOOL				RenderItem( void* pListItem, const LTIntPt* ptItemPos,
									int32 nIndex, bool bSelect, bool bOver );
	void				RenderItemText();
};

#endif //#ifndef __MURSUM_USERLIST_H__
