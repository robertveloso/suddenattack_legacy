// ----------------------------------------------------------------------- //
//
// MODULE  : Globals.h
//
// PURPOSE : Global data
//
// CREATED : 4/26/99
//
// (c) 1999 Monolith Productions, Inc.  All Rights Reserved
//
// ----------------------------------------------------------------------- //

#ifndef _GLOBALS_H_
#define _GLOBALS_H_

//#define _TRON_E3_DEMO

// global constants
const int MAX_OBJECT_ARRAY_SIZE = 32;

#define	COORD_CENTER	0x80000000

// Version to use with dinput.
#define DIRECTINPUT_VERSION  0x0800

inline BOOL PtInRect(RECT* pRect, int x, int y)
{
	if((x >= pRect->right) || (x < pRect->left) ||
	   (y >= pRect->bottom) || (y < pRect->top))
	   return FALSE;

	return TRUE;
}

// Helper classes

template<class TYPE>
class CRange
{
	public:

		CRange() { }
		CRange(TYPE fMin, TYPE fMax) { m_fMin = fMin; m_fMax = fMax; }

		void Set(TYPE fMin, TYPE fMax) { m_fMin = fMin; m_fMax = fMax; }

		TYPE GetMin() { return m_fMin; }
		TYPE GetMax() { return m_fMax; }

	protected:

		TYPE m_fMin;
		TYPE m_fMax;
};

// Miscellaneous functions...

template< class T >
inline T MinAbs( T a, T b)
{
    return (T)fabs(a) < (T)fabs(b) ? a : b;
}

template< class T >
inline T MaxAbs( T a, T b)
{
    return (T)fabs(a) > (T)fabs(b) ? a : b;
}

template< class T >
inline T Clamp( T val, T min, T max )
{
	return Min( max, Max( val, min ));
}


#define UTC_REAL_IP					0x00000000
#define UTC_FULL_CONE				0x00000001
#define UTC_RESTRICTED_CONE			0x00000002
#define UTC_PORT_RESTRICTED_CONE	0x00000004
#define UTC_SYMMETRIC_CONE			0x00000008

#define NODE_HEAD					4
#define NODE_NECK					5

#endif  // _GLOBALS_H_