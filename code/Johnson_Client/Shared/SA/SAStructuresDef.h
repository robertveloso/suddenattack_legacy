
#ifndef __SASTRUCTURESDEF_H__
#define __SASTRUCTURESDEF_H__

typedef struct _UIText
{
	char szText[512];
	uint16 x;
	uint16 y;
	DWORD dwColor;
	LPD3DXFONT pFont;
} UITEXT, *LPUITEXT;


#endif