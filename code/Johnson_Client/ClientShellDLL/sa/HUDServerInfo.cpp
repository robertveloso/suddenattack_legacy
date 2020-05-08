// HUDServerInfo.cpp: implementation of the CHUDServerInfo class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "HUDServerInfo.h"

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CHUDServerInfo::CHUDServerInfo()
{

}

CHUDServerInfo::~CHUDServerInfo()
{

}

LTBOOL CHUDServerInfo::Init()
{

	UpdateLayout();

	return LTTRUE;
}

void CHUDServerInfo::Term()
{

}

void CHUDServerInfo::Render()
{

}

void CHUDServerInfo::Update()
{

}

void CHUDServerInfo::UpdateLayout()
{
	char *pTag = "ServerInfo";
	m_BasePos	= g_pLayoutMgr->GetPoint(pTag,"BasePos");

//	m_sRemainTime = 
}



