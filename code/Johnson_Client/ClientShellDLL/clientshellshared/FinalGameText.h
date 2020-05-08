#ifndef __FINAL_GAME_TEXT_HEADER__
#define __FINAL_GAME_TEXT_HEADER__

#define NF_FINAL_GAME_TEXT()		CFinalGameText::GetInstance()

//////////////////////////////////////////////////////////////////////////
typedef struct _FinalGameTextHeader
{
	DWORD dwIdx;
	DWORD dwOffset;
} FinalGameTextHeader, *LPFinalGameTextHeader;
//////////////////////////////////////////////////////////////////////////

class CFinalGameText
{
public:
	static CFinalGameText* GetInstance();
	BOOL	Destroy();
	
	LPCTSTR GetMsg( DWORD dwIdx );

	BOOL	SetHeaderWithParsing( LPVOID lpHeaderBuf );
	BOOL	SetContextWithParsing( LPVOID lpHeaderInfoBuf, LPVOID lpContextBuf );
	
protected:
	CFinalGameText();
	~CFinalGameText();

	static CFinalGameText*	m_pInstance;
	
	DWORD					m_dwHeaderCount;
	LPFinalGameTextHeader	m_pArrFinalGameTextHeader;
	TCHAR*					m_pWholeContext;
	DWORD					m_dwWholeContextSize;
};


#endif //__FINAL_GAME_TEXT_HEADER__