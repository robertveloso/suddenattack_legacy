//------------------------------------------------------------------
//
//	FILE	  : BrushToWorld.h
//
//	PURPOSE	  : Defines the class that converts a brush list (CEditRegion)
//              to a nonintersecting polygon list (CPreWorld).
//
//	CREATED	  : February 5, 1997
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

#ifndef __BRUSH_TO_WORLD_H__
	#define __BRUSH_TO_WORLD_H__


	// ----------------------------------------------------------------------- //
	// Includes....
	// ----------------------------------------------------------------------- //

	#include "editpoly.h"
	#include "editregion.h"	
	#include "preworld.h"
	#include "swapper.h"
	#include "btwpoly.h"


	// ----------------------------------------------------------------------- //
	// Functions.
	// ----------------------------------------------------------------------- //

	// Convert a CEditPoly to a CPrePoly.
	CPrePoly* EditPolyToPrePoly(CEditPoly *pEditPoly);

	typedef void (*BrushToWorldCallback)( CReal percent );
	bool BrushToWorld(
		GenList<CEditBrush*> &brushes, 
		CPreWorld *pOut, 
		BrushToWorldCallback callback=NULL);


#endif  // __BRUSH_TO_WORLD_H__

