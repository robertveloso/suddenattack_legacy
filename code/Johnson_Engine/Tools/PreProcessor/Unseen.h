//------------------------------------------------------------------
//
//	FILE	  : Unseen.h
//
//	PURPOSE	  : Defines the CUnseen class.  This figures out which
//              parts of the world will never be seen and removes them.
//
//	CREATED	  : February 12 1997
//
//	COPYRIGHT : Microsoft 1997 All Rights Reserved
//
//------------------------------------------------------------------

#ifndef __UNSEEN_H__
	#define __UNSEEN_H__

	#include "editobjects.h"

	class CPreWorld;
	class CPreLeaf;

	class CUnseen
	{
		public:

			// Returns how many polies it removed.
			uint32			RemoveUnseenPortions(CPreWorld *pWorld, BaseEditObjArray &theObjects);

		
		protected:

			void			MarkVisibleLeaves( CPreWorld *pWorld, BaseEditObjArray &theObjects );
			void			GoThruLeaf( CPreLeaf *pLeaf );
			uint32			RemoveInvisibleLeaves( CPreWorld *pWorld );

	};


#endif  // __UNSEEN_H__

