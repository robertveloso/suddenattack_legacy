#ifndef __SUDDEN_ATTACK_MAP_LIST_BY_NOVICE__
#define __SUDDEN_ATTACK_MAP_LIST_BY_NOVICE__

#include "LTGUIMgr.h"

//////////////////////////////////////////////////////////////////////////

#define MAP_NAME_MAX_LENGTH			64

struct MISSION;

typedef struct _tag_MapData
{
	uint32		dwMapIndex;
	char		szMapName[MAP_NAME_MAX_LENGTH];
	char		szMapType[MAP_NAME_MAX_LENGTH];

	MISSION*	pMission;

	_tag_MapData()	{	ZeroMemory( this, sizeof(this) );	}
} MapData, *LPMapData;

//////////////////////////////////////////////////////////////////////////


class CSAMapList : public CLTGUIListView
{
public:
	
	CSAMapList();
	~CSAMapList();

	LTBOOL Create(	LTRect rt, uint16 nItemHeight,
					uint16 nShowCount, int nCommand1,
					CLTGUICommandHandler* pCommand = NULL,
					LTBOOL	bDrawBack = LTFALSE,
					uint32	nBackColor = 0xFF000000,
					uint32	nBorderColor = 0xFFFFFFFF );

	LTBOOL Create( uint16 nPosX, uint16 nPosY,
				   uint16 nWidth, uint16 nHeight,
				   uint16 nItemHeight,
				   CLTGUICommandHandler* pCommandHandler,
				   uint32	nCommandID,
				   LTBOOL	bDrawBack = LTFALSE,
				   uint32	nBackColor = 0xFF000000,
				   uint32	nBorderColor = 0xFFFFFFFF );

	LTBOOL Insert( LPMapData lpMapData );
	LTBOOL Remove( uint32 dwMapIndex );
	
	LPMapData Get( uint32 dwMapIndex );		// Map Index 로 데이터 가져오기
	LPMapData Get( void );					// 현재 위치의 데이터 가져오기

	LTBOOL	SetItemText( uint32 dwPos, char* szMapName, char* szMapType );

	void	SetSelect( LTBOOL bSelected )			{ m_bSelected = bSelected;		}
	LTBOOL	IsSelected()							{ return m_bSelected;			}
	
	void	SetScrollEnable( LTBOOL bScroll )		{ m_bScrollEnable = bScroll;	}
	LTBOOL	IsScrollEnable()						{ return m_bScrollEnable;		}

	LTBOOL		RenderItem( void* pListItem, const LTIntPt* ptfPosition,
								int32 nIndex, bool bSelect, bool bOver );
	void	RenderItemText();

	LTBOOL	OnLButtonDown( int x, int y );

	void	SetPreIndex( int nIndex );
	
protected:
	CSAD3DText*	m_pMapNameText;
	CSAD3DText* m_pMapTypeText;

	int		m_nCommand1;
	LTBOOL	m_bSelected;
	LTBOOL	m_bScrollEnable;

	LTPoly_GT4 m_PolyBar;

	int32	m_nPreIndex;
};



#endif //__SUDDEN_ATTACK_MAP_LIST_BY_NOVICE__
