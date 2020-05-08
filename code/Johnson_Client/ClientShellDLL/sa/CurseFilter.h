#ifndef _CurseFilter_h_
#define _CurseFilter_h_

#define CURSE_SUCESS			0
#define CURSE_ERR_ARLEADYINIT	-1
#define CURSE_ERR_DONTEXIST		-2
#define CURSE_ERR_OVERFLOW		-3

#define INIT_NOT_YET		0
#define INIT_CURSE_ONLY		1
#define INIT_CURSE_NOCURSE	2

#ifdef __cplusplus

extern "C" 
{
	extern int __cdecl LoadCurses(const char *filename);
	extern int __cdecl LoadCursesWithNoCurse(const char *curseFileName,const char *nocurseFileName);
	extern int __cdecl IsCurse(const char *str);
	extern char* __cdecl ConvertString(char *str, int max_len);
}

#else

	extern int LoadCurses(const char *filename);
	extern int LoadCursesWithNoCurse(const char *curseFileName,const char *nocurseFileName);
	extern int IsCurse(const char *str);
	extern char *ConvertString(char *str, int max_len);

#endif

#endif
