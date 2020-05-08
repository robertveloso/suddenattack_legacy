#pragma once

#undef NDEBUG

#ifndef MACRO
#define MACRO
	#undef NULL
	#undef TRUE
	#undef FALSE
	#undef BLANK
	#undef OPERATION
	#undef LOCATION
	#undef EXECUTE
	#undef DEBUG_BREAK
	#undef RELEASE_BREAK
	#undef DEBUGGER
	#undef aSsErT
	#undef assert
	#undef Assert
	#undef ASSERT
	#undef SAFE_DELETE
	#undef SAFE_DELETE_ARRAY
	#undef SAFE_RELEASE

	#define NULL	0

	#define BLANK
	#define OPERATION														\
			static_cast<void>( 0 )
	#define LOCATION														\
			"File: %s, Line: %d\n", __FILE__, __LINE__
	#define EXECUTE(statement)												\
			do {															\
				DEBUGGER( OUTPUT( #statement "\n" ) );						\
				statement;													\
			} while( 0 )

	#define DEBUG_BREAK														\
			DEBUGGER( do { __asm { INT 3 } } while ( 0 ) )
//			DEBUGGER( RELEASE_BREAK )
	#define RELEASE_BREAK													\
			do {															\
				OUTPUT( STRING().Printf( LOCATION ) );						\
				if ( System::Available() == FALSE ) ABORT();			\
				else System()->Leave( "User breakpoint reached.\n" );	\
			} while( 0 )

	#ifdef NDEBUG
		#define DEBUGGER(statement)											\
				OPERATION
	#else
		#define DEBUGGER(statement)											\
				statement
		#ifdef WIN32
			#undef RELEASE_BREAK
			#define RELEASE_BREAK											\
					do { __asm { INT 3 } } while ( 0 )
		#endif
	#endif

	#define aSsErT(expression)												\
			typedef char AsSeRt[ (expression) ? 1 : -1 ];
	#define assert(expression)												\
			DEBUGGER( if ( ! (expression) ) do { __asm { INT 3 } } while ( 0 ) )
	#define Assert(expression)												\
			DEBUGGER( if ( ! (expression) ) do { __asm { INT 3 } } while ( 0 ) )
	#define ASSERT(expression)												\
			if ( ! (expression) ) RELEASE_BREAK

	#define SAFE_DELETE(pointer)											\
			do { delete (pointer); (pointer) = NULL; } while ( 0 )
	#define SAFE_DELETE_ARRAY(pointer)										\
			do { delete [] (pointer); (pointer) = NULL; } while ( 0 )
	#define SAFE_RELEASE(pointer)											\
			RELEASE( pointer ), (pointer) = NULL
#endif