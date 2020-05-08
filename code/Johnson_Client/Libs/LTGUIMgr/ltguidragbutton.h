#ifndef __MURSUM_DRAG_BUTTON_H__
#define __MURSUM_DRAG_BUTTON_H__


#define CMD_PARAM_LDOWN			0x10000000
#define CMD_PARAM_LUP			0x20000000
#define CMD_PARAM_MOUSEMOVE		0x40000000

class CLTGUIDragButton : public CLTGUIButton
{
public:
	CLTGUIDragButton();
	virtual ~CLTGUIDragButton();

	LTBOOL			OnLButtonDown(int x, int y);
    LTBOOL			OnLButtonUp(int x, int y);
	LTBOOL			OnMouseMove(int x, int y);
	LTBOOL			OnWheelUp(int x, int y);
	LTBOOL			OnWheelDown(int x, int y);
	LTBOOL			OnEnter() { return LTFALSE; }

	void			OnSelChange();

	virtual LTBOOL	IsOnMe(int x, int y);

	void			SetLockX( LTBOOL bLock );
	void			SetLockY( LTBOOL bLock );

	void			SetLimitX( int32 nMinX, int32 nMaxX );
	void			SetLimitY( int32 nMinY, int32 nMaxY );

	float			GetPosXRatio();
	float			GetPosYRatio();

	void			SetPosXRatio( float fXRatio );
	void			SetPosYRatio( float fYRatio );
	
	void			SetBasePos( LTIntPt pos );
	void		    SetBasePos( int x, int y );

private:
	LTIntPt			m_ptClickDelta;
	LTBOOL			m_bLButtonDown;
	LTBOOL			m_bLockX;
	LTBOOL			m_bLockY;

	int32			m_nMinLimitX;
	int32			m_nMaxLimitX;
	
	int32			m_nMinLimitY;
	int32			m_nMaxLimitY;
};
#endif //#ifndef __MURSUM_DRAG_BUTTON_H__
