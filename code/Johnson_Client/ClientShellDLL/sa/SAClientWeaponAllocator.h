
#ifndef  _SAClientWeaponAllocator_h_INCLUDED_
#define  _SAClientWeaponAllocator_h_INCLUDED_

#include "ClientWeaponAllocator.h"

//
// This class's sole purpose is  to create each 
// instance of the ClientWeapon.  It will
// take a type and match it to a class.
//
// See the ClientWeapon stuff for an example of its
// implementation.  Specifically, CCreator, 
// CStandardCreator, the derived classes of ClientWeaponAllocator
// (CTRONClientWeaponAllocator and CSAClientWeaponAllocator),
// and CPlayerMgr (as well as derived classes 
// CTRONPlayerMgr or CSAPlayerMgr).
//

class CSAClientWeaponAllocator : public CClientWeaponAllocator
{
public:
	virtual IClientWeaponBase *New( int nClientWeaponType ) const;
};

#endif //_SAClientWeaponAllocator_h_INCLUDED_