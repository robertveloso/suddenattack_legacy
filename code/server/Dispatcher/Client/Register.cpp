#include "StdAfx.h"

#include "./Register.h"

namespace Dispatcher {		namespace Client {

Register::Register()
{
	m_pList					= new LIST;

	this->install();
}

Register::~Register()
{
	this->uninstall();

	SAFE_DELETE( m_pList );
}

void
Register::Regist( IBUILDER * pBuilder )
{
	m_pList->PushHead( pBuilder );
}

Register::IBUILDER *
Register::First()
{
	m_stIterator			= m_pList->GetHead();
	return m_stIterator.GetItem();
}

Register::IBUILDER *
Register::Next()
{
	m_stIterator.MoveNext();
	return m_stIterator.GetItem();
}

} /* Client */				} /* Dispatcher */