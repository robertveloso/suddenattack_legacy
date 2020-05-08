//------------------------------------------------------------------
//
//	FILE	  : GameMath.h
//
//	PURPOSE	  : Defines the Math class, which contains tons of
//              useful math routines!
//
//	CREATED	  : 1st May 1996
//
//	COPYRIGHT : Monolith Productions Inc. 1996-99
//
//------------------------------------------------------------------

#ifndef __GAMEMATH_H__
	#define __GAMEMATH_H__
	


	// Includes....

	class Math
	{
		public:

			// Constructor
								Math();

			// Destructor
								~Math();


			// Member functions

			// Square root.
			static LTFLOAT		Sqrt( LTFLOAT x )		{ return (LTFLOAT)sqrt(x); }

			static LTFLOAT		Cos( LTFLOAT angle )	{ return (LTFLOAT)cos(angle); }
			static LTFLOAT		Sin( LTFLOAT angle )	{ return (LTFLOAT)sin(angle); }

			static LTFLOAT		ACos( LTFLOAT x )		{ return (LTFLOAT)acos(x); }

			// Returns from 0 to CIRCLE (NOT -PI to PI).
			static LTFLOAT		ATan2( LTFLOAT x, LTFLOAT y );

	};


#endif




