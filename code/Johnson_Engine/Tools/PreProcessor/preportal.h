
// Defines the PrePortal stuff.

#ifndef __PREPORTAL_H__
#define __PREPORTAL_H__


	class CPreSurface;


	class PrePortal : public CGLLNode
	{
		public:

			CPreSurface		*m_pSurface;	// Which surface are we piggybacking?
			char			*m_pName;		// Name for this portal (referenced at runtime).
			DWORD			m_Index;		// Index we're referenced by.

	};


#endif  // __PREPORTAL_H__





