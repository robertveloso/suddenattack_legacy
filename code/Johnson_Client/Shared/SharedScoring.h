// ----------------------------------------------------------------------- //
//
// MODULE  : SharedScoring.h
//
// PURPOSE : SharedScoring - shared mission summary stuff
//
// CREATED : 10/17/02
//
// (c) 2002 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef __SHARED_SCORE_H__
#define __SHARED_SCORE_H__

#include "ltbasetypes.h"
#include "ClientServerShared.h"

class CPlayerScore
{
public:
	CPlayerScore();

    void	Init(uint32 nClientID);
    void    WriteData(ILTMessage_Write *pMsg);
    void    ReadData(ILTMessage_Read *pMsg);

#ifdef _CLIENTBUILD
	//[MURSUM]===========================================
	void	AddClientDeaths();
	void	AddClientScore();
	//===================================================
#endif

#ifndef _CLIENTBUILD
	void	AddFrag();
	void	RemoveFrag();
	void	AddTag();
	void	RemoveTag();
	void	AddBonus(int nBonus);
	
	//-----------------------------------------------------
	// ±¤½Ä
	//-----------------------------------------------------
	void	AddDeaths();
	//-----------------------------------------------------
#endif

	int32	GetScore() {return m_nScore;}
	int32	GetFrags() {return m_nFrags;}
	int32	GetDeaths() {return m_nDeaths;}
	//void	IncDeaths() { ++m_nDeaths;}		// ±¤½Ä : Á×Àº È¸¼ö Ç¥½ÃÇÏ±â À§ÇØ Ã·°¡ ÇÔ
	int32	GetTags() {return m_nTags;}

private:
	uint32	m_nClientID;
	// ±¤½Ä
	//int32	m_nScore;
	int16	m_nScore;
	int32	m_nFrags;
	//-----------------------------------------------------
	// ±¤½Ä
	//-----------------------------------------------------
	//int32	m_nDeaths;		// ±¤½Ä : Á×Àº È¸¼ö Ç¥½ÃÇÏ´Â º¯¼ö
	int16	m_nDeaths;		// ±¤½Ä : Á×Àº È¸¼ö Ç¥½ÃÇÏ´Â º¯¼ö
	//-----------------------------------------------------	
	int32	m_nTags;

	bool	m_bInitted;
};

#endif // __SHARED_SCORE_H__