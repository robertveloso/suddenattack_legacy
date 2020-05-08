//-----------------------------------------------------------------------------
// 파  일 : WeightsetInfo.h
//-----------------------------------------------------------------------------
// 목  적 : 캐릭터애니메이션에 사용되는 weightset 관리 클래스
// 날  자 : [2003, 5]
// 작성자 : [burumal]
// 소  유 : (c) GMOS 
//-----------------------------------------------------------------------------
#ifndef __WEIGHTSET_INFO_H__
#define __WEIGHTSET_INFO_H__

//-----------------------------------------------------------------------------
// 설명 : 상수정의
//-----------------------------------------------------------------------------
#define sANITRACKER_UPPER2_NAME			"__Upper2__"

//-----------------------------------------------------------------------------
// 설명 : 
//-----------------------------------------------------------------------------
struct tagWeightsetName
{
	std::string m_cUpper;
	std::string m_cLower;

	std::string m_cPitchControl;
};
typedef struct tagWeightsetName		WeightsetName_t;
typedef struct tagWeightsetName*	pWeightsetName_t;

typedef std::list<WeightsetName_t*>	WeightsetNameList;

//-----------------------------------------------------------------------------
// 설명 : 
//-----------------------------------------------------------------------------
struct tagWeightsetChange
{
	StringList			m_lstUpperAni;
	StringList			m_lstLowerAni;

	int32				m_nWeightsetID;
	WeightsetName_t		m_sWeightsetName;
};
typedef struct tagWeightsetChange		WeightsetChange_t;
typedef struct tagWeightsetChange*		pWeightsetChange_t;

typedef std::list<WeightsetChange_t*>	WeightsetChangeList;

//-----------------------------------------------------------------------------
// 설명 : CModelWeightsetInfo 클래스 정의
//-----------------------------------------------------------------------------
class CModelWeightsetInfo : public CGameButeMgr
{
	// members...

	protected :

		std::string				m_cTwitch;
		std::string				m_cBlink;		

		WeightsetNameList		m_WeightsetNameList;
		WeightsetChangeList		m_WeightsetChangeList;

	// methods...

	public :

		CModelWeightsetInfo()
		{
		}

		~CModelWeightsetInfo()
		{
			Term();
		}

		LTBOOL Init(const char* pAttributeFile);
		void Term();
	
		const char* GetAttributeFileName()
		{
			return m_strAttributeFile.GetBuffer();
		}

		// accessors
		const WeightsetNameList* GetWeightsetNameList()
		{
			return &m_WeightsetNameList;
		}

		const WeightsetChangeList* GetWeightsetChangeList()
		{
			return &m_WeightsetChangeList;
		}

		const char* GetTwitchName(void)
		{
			if ( m_cTwitch.empty() )
				return LTNULL;

			return m_cTwitch.c_str();
		}

		const char* GetBlinkName(void)
		{
			if ( m_cBlink.empty() )
				return LTNULL;

			return m_cBlink.c_str();
		}

	protected :

		LTBOOL InitWeightsetNameList(const char* pTagName);
		LTBOOL InitWeightsetChangeList(const char* pTagName);
				
		void ReleaseWeightsetNameList();
		void ReleaseWeightsetChangeList();		

		WeightsetName_t* GetWeightsetName(uint32 nWeightsetID);

		LTBOOL IsSameUpper2Tracker(char* pName) const;
};

//-----------------------------------------------------------------------------
// 설명 : CModelWeightsetInfoMgr 클래스 정의
//-----------------------------------------------------------------------------
class CModelWeightsetInfoMgr
{
	// members...

	protected :

	// methods...

	public :
		CModelWeightsetInfoMgr()
		{
		}

		~CModelWeightsetInfoMgr()
		{
			Term();
		}

		static void Init(const char* pAttributeFile);
		static CModelWeightsetInfo* FindExist(const char* pAttributeFile);

	protected :

		void Term();
};

#endif // __WEIGHTSET_INFO_H__