#ifndef _GLOBAL_MODULE_H_
#define _GLOBAL_MODULE_H_

#include "ScreenBase.h"

#define MAX_GRADE_TEXTURE	58
#define MAX_GRADE_TEXT		19
#define MAX_PING_TEXTURE	5

class CGlobalModule
{
public :
	CGlobalModule();
	~CGlobalModule();

	static CGlobalModule& GetSingleton()
	{
		static CGlobalModule GlobalData;
		return GlobalData;
	}

	void			GlobalInit(const char* szMission = MISSION_DEFAULT_FILE);
	HTEXTURE		GetGradeTexture( int nGrade );
	const char*		GetGradeString( int nGrade );
	HTEXTURE		GetPingTexture( int nPing );

public :
	HTEXTURE			m_hGradeTexture[MAX_GRADE_TEXTURE];
	HTEXTURE			m_hPingTexture[MAX_PING_TEXTURE];
	char				m_szGradeText[MAX_GRADE_TEXT][64];
};

#define g_Global_Data		CGlobalModule::GetSingleton()

#endif // _GLOBAL_MODULE_H_