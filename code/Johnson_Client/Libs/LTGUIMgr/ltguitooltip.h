#ifndef __MURSUM_TOOLTIP_H__
#define __MURSUM_TOOLTIP_H__

class CLTGUITooltip : public CLTGUICtrl
{
public:
	CLTGUITooltip();
	virtual ~CLTGUITooltip();

	virtual LTBOOL	CreateTooltip( uint16 nWidth, uint16 nHeight );
	virtual void	Term() {}
	virtual void	CtrlShow( LTIntPt& Pos );
	virtual void	CtrlHide();

	virtual void	OnPosChanged() {} 
	
	virtual void	Render();
	
	virtual LTBOOL  IsOnMe(int x, int y) { return LTFALSE; }

private:
};

#endif