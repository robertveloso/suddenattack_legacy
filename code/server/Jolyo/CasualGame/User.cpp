#include "StdAfx.h"

#include "User.h"

/*
using namespace CasualGame;
using namespace Application;
using namespace Session::Message;
using namespace Session::Manage;
using namespace Session::Gate;
*/

using namespace CasualGame;
//using namespace Application;

CUser::CUser()
{
	m_InnerObjectForRoom	.Init( this );
	m_InnerObjectForChannel	.Init( this );
	m_InnerObjectForGame	.Init( this );
}

CUser::~CUser()
{
}
