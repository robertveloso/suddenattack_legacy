
#include "stdafx.h"
//#include "CtClient.h"
#include "des.h"
//#include <time.h>

using namespace CasualGame;

BYTE IP[64]
= {	58, 50, 42, 34, 26, 18, 10,  2,
	60, 52, 44, 36, 28, 20, 12,  4,
	62, 54, 46, 38, 30, 22, 14,  6,
	64, 56, 48, 40, 32, 24, 16,  8,
	57, 49, 41, 33, 25, 17,  9,  1,
	59, 51, 43, 35, 27, 19, 11,  3,
	61, 53, 45, 37, 29, 21, 13,  5,
	63, 55, 47, 39, 31, 23, 15,  7	};

BYTE FP[64]
= {	40,  8, 48, 16, 56, 24, 64, 32,
	39,  7, 47, 15, 55, 23, 63, 31,
	38,  6, 46, 14, 54, 22, 62, 30,
	37,  5, 45, 13, 53, 21, 61, 29,
	36,  4, 44, 12, 52, 20, 60, 28,
	35,  3, 43, 11, 51, 19, 59, 27,
	34,  2, 42, 10, 50, 18, 58, 26,
	33,  1, 41,  9, 49, 17, 57, 25	};

BYTE PC1[64]
= {	57, 49, 41, 33, 25, 17,  9,
	 1, 58, 50, 42, 34, 26, 18,
	10,  2, 59, 51, 43, 35, 27,
	19, 11,  3, 60, 52, 44, 36,
	 8, 16, 24, 32,
	63, 55, 47, 39, 31, 23, 15,
	 7, 62, 54, 46, 38, 30, 22,
	14,  6, 61, 53, 45, 37, 29,
	21, 13,  5, 28, 20, 12,  4,
	40, 48, 56, 64	 		};

BYTE keyrot[16]
= {	1, 1, 2, 2, 2, 2, 2, 2, 1, 2, 2, 2, 2, 2, 2, 1	};

BYTE PC2[48]
= {	14, 17, 11, 24,  1,  5,
	 3, 28, 15,  6, 21, 10,
	23, 19, 12,  4, 26,  8,
	16,  7, 27, 20, 13,  2,
	45, 56, 35, 41, 51, 59,
	34, 44, 55, 49, 37, 52,
	48, 53, 43, 60, 38, 57,
	50, 46, 54, 40, 33, 36 	};

BYTE S[8][64]
= {			// S[1]
	14,  4, 13,  1,  2, 15, 11,  8,  3, 10,  6, 12,  5,  9,  0,  7,
	 0, 15,  7,  4, 14,  2, 13,  1, 10,  6, 12, 11,  9,  5,  3,  8,
	 4,  1, 14,  8, 13,  6,  2, 11, 15, 12,  9,  7,  3, 10,  5,  0,
	15, 12,  8,  2,  4,  9,  1,  7,  5, 11,  3, 14, 10,  0,  6, 13,
			// S[2]
	15,  1,  8, 14,  6, 11,  3,  4,  9,  7,  2, 13, 12,  0,  5, 10,
	 3, 13,  4,  7, 15,  2,  8, 14, 12,  0,  1, 10,  6,  9, 11,  5,
	 0, 14,  7, 11, 10,  4, 13,  1,  5,  8, 12,  6,  9,  3,  2, 15,
	13,  8, 10,  1,  3, 15,  4,  2, 11,  6,  7, 12,  0,  5, 14,  9,
			// S[3]
	10,  0,  9, 14,  6,  3, 15,  5,  1, 13, 12,  7, 11,  4,  2,  8,
	13,  7,  0,  9,  3,  4,  6, 10,  2,  8,  5, 14, 12, 11, 15,  1,
	13,  6,  4,  9,  8, 15,  3,  0, 11,  1,  2, 12,  5, 10, 14,  7,
	 1, 10, 13,  0,  6,  9,  8,  7,  4, 15, 14,  3, 11,  5,  2, 12,
			// S[4]
	 7, 13, 14,  3,  0,  6,  9, 10,  1,  2,  8,  5, 11, 12,  4, 15,
	13,  8, 11,  5,  6, 15,  0,  3,  4,  7,  2, 12,  1, 10, 14,  9,
	10,  6,  9,  0, 12, 11,  7, 13, 15,  1,  3, 14,  5,  2,  8,  4,
	 3, 15,  0,  6, 10,  1, 13,  8,  9,  4,  5, 11, 12,  7,  2, 14,
			// S[5]
	 2, 12,  4,  1,  7, 10, 11,  6,  8,  5,  3, 15, 13,  0, 14,  9,
	14, 11,  2, 12,  4,  7, 13,  1,  5,  0, 15, 10,  3,  9,  8,  6,
	 4,  2,  1, 11, 10, 13,  7,  8, 15,  9, 12,  5,  6,  3,  0, 14,
	11,  8, 12,  7,  1, 14,  2, 13,  6, 15,  0,  9, 10,  4,  5,  3,
			// S[6]
	12,  1, 10, 15,  9,  2,  6,  8,  0, 13,  3,  4, 14,  7,  5, 11,
	10, 15,  4,  2,  7, 12,  9,  5,  6,  1, 13, 14,  0, 11,  3,  8,
	 9, 14, 15,  5,  2,  8, 12,  3,  7,  0,  4, 10,  1, 13, 11,  6,
	 4,  3,  2, 12,  9,  5, 15, 10, 11, 14,  1,  7,  6,  0,  8, 13,
			// S[7]
	 4, 11,  2, 14, 15,  0,  8, 13,  3, 12,  9,  7,  5, 10,  6,  1,
	13,  0, 11,  7,  4,  9,  1, 10, 14,  3,  5, 12,  2, 15,  8,  6,
	 1,  4, 11, 13, 12,  3,  7, 14, 10, 15,  6,  8,  0,  5,  9,  2,
	 6, 11, 13,  8,  1,  4, 10,  7,  9,  5,  0, 15, 14,  2,  3, 12,
			// S[8]
	13,  2,  8,  4,  6, 15, 11,  1, 10,  9,  3, 14,  5,  0, 12,  7,
	 1, 15, 13,  8, 10,  3,  7,  4, 12,  5,  6, 11,  0, 14,  9,  2,
	 7, 11,  4,  1,  9, 12, 14,  2,  0,  6, 10, 13, 15,  3,  5,  8,
	 2,  1, 14,  7,  4, 10,  8, 13, 15, 12,  9,  0,  3,  5,  6, 11   };

BYTE P[32]
= {	16,  7, 20, 21,
	29, 12, 28, 17,
	 1, 15, 23, 26,
	 5, 18, 31, 10,
	 2,  8, 24, 14,
	32, 27,  3,  9,
	19, 13, 30,  6,
	22, 11,  4, 25	};


/////////////////////////////////////////////////////////////////////////////
// CUserSCDApp construction

CUserSCDApp::CUserSCDApp()
{
	// TODO: add construction code here,
	// Place all significant initialization in InitInstance

	// Secret
//	CSecretInit() ;
}


//CUserSCDApp::~CUserSCDApp()
//{
//	// DLL Á¾·á
//}
/////////////////////////////////////////////////////////////////////////////
// The one and only CUserSCDApp object



//CUserSCDApp theApp;

//void CUserSCDApp::CSecretInit()
//{
//	memcpy( IP, imsiIP, sizeof(IP)) ;
//	memcpy( FP, imsiFP, sizeof(FP)) ;
//	memcpy( PC1, imsiPC1, sizeof(PC1)) ;
//	memcpy( keyrot, imsikeyrot, sizeof(keyrot)) ;
//	memcpy( PC2, imsiPC2, sizeof(PC2)) ;
//	memcpy( S, imsiS, sizeof(S)) ;
//	memcpy( P, imsiP, sizeof(P)) ;
//}


void CUserSCDApp::ecbendes(BYTE* b)
{
	BYTE	work[8];
	int	i;

	ecbperm(work, b, IP, 8);

	for (i=0; i<=15; i++)
		ecbround(work, subkey[i]);
	ecbswap(work);

	ecbperm(b, work, FP, 8);
}

//
// ecbdedes(b)
//
void CUserSCDApp::ecbdedes(BYTE* b)
{
	BYTE	work[8];
	int	i;

	ecbperm(work, b, IP, 8);

	for (i=15; i>=0; i--)
		ecbround(work, subkey[i]);
	ecbswap(work);

	ecbperm(b, work, FP, 8);
}

//
// ecbround(d, k)
//
void CUserSCDApp::ecbround(BYTE* d,BYTE* k)
{
	BYTE	b[4] = {0}, out[4] = {0};
	BYTE	a[8];
	int	i,j,t,s,rc;

	ecbexpand(a, d+4);

	for (j=0; j<8; j++) {
		rc = a[j] ^ k[j];
		rc =	(rc & 0x20) |		// bit 6,1,2,3,4,5
			((rc << 4) & 0x10) |
			((rc >> 1) & 0x0f);
		s = S[j][rc];
		b[j/2] &= 0xf0;
		b[j/2] |= (s & 0x0f);
		if((j & 1) == 0)
			b[j/2] <<= 4;
	}

	ecbperm(out, b, P, 4);

	for(i=0; i<4; i++) {
		t = d[i] ^ out[i];
		d[i] = d[4+i];
		d[4+i] = t;
	}
}

//
// ecbdesinit(key)
//

void CUserSCDApp::ecbdesinit(BYTE* key) //BYTE	key[8];
{
	BYTE	cd[8];
	int	i;

//	ecbgetkey(key);		 //parity bit setting

	ecbperm(cd, key, PC1, 8);

	cd[3] &= 0xf0;		// mask bit 29,30,31,32
	cd[7] &= 0xf0;		// mask bit 61,62,63,64

	for (i=0; i<16; i++) {
		ecbrotl28(&cd[0], (int)keyrot[i]);
		ecbrotl28(&cd[4], (int)keyrot[i]);
		ecbkeyperm(cd, i);
	}
}

//
// ecbkeyperm(cd, i)
//

#define ECBKEYBIT1	0x20

void CUserSCDApp::ecbkeyperm(BYTE cd[8],int i)
{
	int	j, k;
	BYTE	mask;
	BYTE	*pm = PC2;	// hwjung

	for (j=0; j<8; j++) {
		subkey[i][j] = 0;
		mask = ECBKEYBIT1;
		for (k=0; k<6; k++) {
			if (ecbbit(cd, (int)*pm++) == 1)
				subkey[i][j] |= mask;
			mask >>= 1;
		}
	}
}

//
// ecbrotl28(b, pos)
//

void CUserSCDApp::ecbrotl28(BYTE b[4],int pos)
{
	int	i;
	BYTE	c0,c1,c2,c3;

	for(i=0; i<pos; i++) {
		c0 = b[0] & 0x80;
		c1 = b[1] & 0x80;
		c2 = b[2] & 0x80;
		c3 = b[3] & 0x80;
		b[3] = (b[3] << 1) | (c0 >> 3);
		b[2] = (b[2] << 1) | (c3 >> 7);
		b[1] = (b[1] << 1) | (c2 >> 7);
		b[0] = (b[0] << 1) | (c1 >> 7);
	}
}

//
// ecbperm(out, in, pm, bytes)
//

#define	ECBDESBIT1	0x80

void CUserSCDApp::ecbperm(BYTE* out,BYTE* in,BYTE* pm,int bytes)
{
	BYTE	mask;
	int	i,j;

	for(i=0; i<bytes; i++)
		out[i] = 0x00;
	for(i=0; i<bytes; i++) {
		mask = ECBDESBIT1;
		for(j=0; j<8; j++) {
			if (ecbbit(in, (int)*pm++) == 1)
				out[i] |= mask;
			mask >>= 1;
		}
	}
}

//
// ecbexpand(a, r)
//

#define	MASK6	0x3f

void CUserSCDApp::ecbexpand(BYTE a[8],BYTE r[4])
{
	BYTE	t;

	t = (r[3]<<5)|(r[0]>>3); a[0] = t & MASK6; // bits 32  1  2  3  4  5
	t = (r[0]<<1)|(r[1]>>7); a[1] = t & MASK6; // bits  4  5  6  7  8  9
	t = (r[0]<<5)|(r[1]>>3); a[2] = t & MASK6; // bits  8  9 10 11 12 13
	t = (r[1]<<1)|(r[2]>>7); a[3] = t & MASK6; // bits 12 13 14 15 16 17
	t = (r[1]<<5)|(r[2]>>3); a[4] = t & MASK6; // bits 16 17 18 19 20 21
	t = (r[2]<<1)|(r[3]>>7); a[5] = t & MASK6; // bits 20 21 22 23 24 25
	t = (r[2]<<5)|(r[3]>>3); a[6] = t & MASK6; // bits 24 25 26 27 28 29
	t = (r[3]<<1)|(r[0]>>7); a[7] = t & MASK6; // bits 28 29 30 31 32  1
}

//
// ecbbit(n, pos)
//
int	CUserSCDApp::ecbbit(BYTE n[8],int pos)
{
	int	b, d, o;

	d = (pos - 1) / 8;
	o = (pos - 1) % 8;
	b = (n[d] >> (7-o)) & 01;
	return (b);
}

//
// ecbswap(b) - exchange the two 32-bit halves of the 64-bit block b
//
void CUserSCDApp::ecbswap(BYTE* b)
{
	BYTE	tmp;
	int	i;

	for(i=0; i<4; i++) {
		tmp = b[i];
		b[i] = b[4+i];
		b[4+i] = tmp;
	}
}

#if 0
ecbgetkey(k)
BYTE	k[8];
{
	int	i, j, p;

	for (i=0; i<8; i++) {
		for (p=0, j=7; j>0; j--)
			if (k[i] & (1 << j))	p++;
		if (p & 1)
			k[i] &= 0xfe;
		else
			k[i] |= 0x01;
	}
}
#endif



//
//	DES Library for SCOS 1.XX (Samsung Chip Operating System)
//
//	Program Name : des64.c
//
//	Date : 1995/8/28 ,  Programed by Jung Hee-Won
//
//	Copyright 1995 Samsung Electronics Co,.Ltd
//

//#if 0
//#include "des64.i"

//
//	Define global data structures used by DES routines
//

//typedef	unsigned char	BYTE;
//typedef	unsigned long	LONG;

//static	BYTE	subkey[16][8];
//#endif


//
// endes(b)
//
void CUserSCDApp::endes(unsigned long* b) // LONG b[2])
{
	unsigned long work[2];
	register int	i;

	perm(work, b, IP, 2);

	for (i=0; i<=15; i++)
		round(work, subkey[i]);
	swap(work);

	perm(b, work, FP, 2);
}

//
// dedes(b)
//
void CUserSCDApp::dedes(unsigned long* b) // LONG b[2])
{
	unsigned long		work[2];
	register int	i;

	perm(work, b, IP, 2);

	for (i=15; i>=0; i--)
		round(work, subkey[i]);
	swap(work);

	perm(b, work, FP, 2);
}

//
// round(d, k)
//
void CUserSCDApp::round(unsigned long* d, BYTE* k ) // LONG d[2],char* k)
{
	unsigned long	t;

	t = d[0] ^ f(d[1], k);

	d[0] = d[1];
	d[1] = t;
}

//
// f(r, k)
//
unsigned long CUserSCDApp::f(unsigned long r,BYTE* k)
{
	unsigned long	b = 0,
		out = 0;
	char	a[8];
	register unsigned long	s;
	register unsigned long	rc;
	register int	j;

	expand(a, r);

	for (j=7; j>=0; j--) {
		rc = a[j] ^ k[j];
		rc =	(rc & 0x20) |
			((rc << 4) & 0x10) |
			((rc >> 1) & 0x0F);
		s = S[j][rc];
		b = (b << 4) | s;
	}

	perm(&out, &b, P, 1);

	return(out);
}

//
// desinit(key)
//

#define MASK28	0x0FFFFFFFL	// Mask DES key bits 1 to 28

void CUserSCDApp::desinit(unsigned long key[2])
{
	unsigned long	cd[2];
	register int	i;

//	getkey(key);

	perm(cd, key, PC1, 2);

	cd[0] &= MASK28;
	cd[1] &= MASK28;

	for (i=0; i<16; i++) {
		rotl28(&cd[0], (int)keyrot[i]);
		rotl28(&cd[1], (int)keyrot[i]);
		keyperm(cd, i);
	}
}

//
// keyperm(cd, i)
//

#define KEYBIT1	0x01

void CUserSCDApp::keyperm(unsigned long cd[2],int i)
{
	register int	j, k;
	register BYTE	mask;
	register BYTE	*pm = PC2;	// hwjung //

	for (j=0; j<8; j++) {
		subkey[i][j] = 0;
		mask = KEYBIT1;
		for (k=0; k<6; k++) {
			if (bit(cd, (int)*pm++) == 1)
				subkey[i][j] |= mask;
			mask <<= 1;
		}
	}
}

//
// rotl28(b, pos)
//

void CUserSCDApp::rotl28(unsigned long* b,int pos)
{
	*b = ((*b >> pos) | (*b << (28-pos)) ) & MASK28;
}

//
// perm(out, in, pm, n)
//

#define DESBIT1	0x00000001L		// 0x80000000L //

void CUserSCDApp::perm(unsigned long* out,unsigned long* in,BYTE* pm,int n)
{
	unsigned long		mask;
	register int	i,j;

	for(i=0; i < n; i++)
		out[i] = 0L;

	for(i=0; i < n; i++) {
		mask = DESBIT1;
		for (j=0; j<32; j++) {
			if (bit(in, (int)*pm++) == 1)
				out[i] |= mask;
			mask <<= 1;
		}
	}
}

//
// expand(a, r)
//

//
//#define	MASK6	0x3f
//
void CUserSCDApp::expand(char* a,unsigned long r)
{
	unsigned long	t;

	t = (r<<1) | (r>>31);
	a[0] = (char)(t & MASK6);							// bits 32  1  2  3  4  5
	t = r >> 3;		a[1] = (char)(t & MASK6);			// bits  4  5  6  7  8  9
	t = r >> 7;		a[2] = (char)(t & MASK6);			// bits  8  9 10 11 12 13
	t = r >> 11;	a[3] = (char)(t & MASK6);			// bits 12 13 14 15 16 17
	t = r >> 15;	a[4] = (char)(t & MASK6);			// bits 16 17 18 19 20 21
	t = r >> 19;	a[5] = (char)(t & MASK6);			// bits 20 21 22 23 24 25
	t = r >> 23;	a[6] =(char)( t & MASK6);			// bits 24 25 26 27 28 29
	t = (r<<5) | (r>>27);	a[7] = (char)(t & MASK6);	// bits 28 29 30 31 32  1
}

//
// bit(n, pos)
//
int CUserSCDApp::bit(unsigned long n[2],int pos)
{
	int	b, d, o;

	d = (pos - 1) / 32;
	o = (pos - 1) % 32;
	b = (int)((n[d] >> o) & 01);
	return (b);
}

//
// swap(b) - exchange the two 32-bit halves of the 64-bit block b
//
void CUserSCDApp::swap(unsigned long b[2])
{
	unsigned long	tmp;
	tmp = b[0];	b[0] = b[1];	b[1] = tmp;
}

#if 0
getkey(k)
char	k[8];
{
	register i, j, p;

	for (i=0; i<8; i++) {
		for (p=0, j=0; j<7; j++)
			if (k[i] & (1 << j))	p++;
		if ((p & 1) == 0)
			k[i] |= 0x80;
		else
			k[i] &= 0x7f;
	}
}
#endif

//void CUserSCDApp::get_random(char *rand_str)
//{
//	int i;
//	int temp_rand;
//
//	srand((unsigned)time(NULL));
//	for(i = 0; i< 2;i++) {
//		temp_rand = rand();
//		_itoa(temp_rand,&rand_str[i * 4],16);
//	}
//	return ;
//}

long CUserSCDApp::uToDedes (BYTE* data, BYTE SesKey[8], BYTE* OutData, long iLen, long iMulti )
{
	BYTE temp_ses[8];
	BYTE temp_src[8];
	BYTE temp_multi[8];
	long  i, k, im = 0 ;

	memcpy( temp_ses, SesKey,8);
	//iLen = _mbslen(data) ;

	for ( i=0 ; i<iLen ; i=i+8 )
	{
		memcpy(temp_src,data+i,8);

		if ( im > iMulti )
			im = 0 ;
		if ( im == 0 )
		{
			ecbdesinit(temp_ses);
			ecbdedes(temp_src);
		}
		else
		{
			for ( k=0 ; k<8 ; k++ )
				temp_src[k] = temp_src[k]^temp_multi[7-k] ;
		}
		im++;
		memcpy(temp_multi,data+i,8);

	    memcpy(OutData+i,temp_src,8);
	}
	*(OutData+i) = 0;
	return i ;
}

long CUserSCDApp::uToEndes (BYTE* data, BYTE SesKey[8], BYTE* OutData, long iLen, long iMulti  )
{
	BYTE temp_ses[8];
	BYTE temp_src[8];
	BYTE temp_multi[8];
	long  i, k, im = 0 ;

	memcpy( temp_ses, SesKey,8);
	//iLen = _mbslen(data) ;

	for ( i=0 ; i<iLen ; i=i+8 )
	{
		if ( i+7 < iLen )
			memcpy(temp_src,data+i,8);
		else
		{
			memset(temp_src,0,8);
			memcpy(temp_src,data+i, iLen - i);
		}

		if ( im > iMulti )
			im = 0 ;
		if ( im == 0 )
		{
			ecbdesinit(temp_ses);
			ecbendes(temp_src);
		}
		else
		{
			for ( k=0 ; k<8 ; k++ )
				temp_src[k] = temp_src[k]^temp_multi[7-k] ;
		}
		im++;
		memcpy(temp_multi,temp_src,8);
	    memcpy(OutData+i,temp_src,8);
	}
	*(OutData+i) = 0;
	//memset(OutData+i, 0, 8 ) ;
	return i ;
}

//TCHAR CUserSCDApp::Get_Bcc( TCHAR * string, long iLen )
//{
//	int i ;
//	uCHAR  bcc = 0x00 ;
//
//	for (i=0; i<iLen; i++)
//	{
//		bcc = bcc ^ *string ;
//		string++ ;
//	}
//	return bcc ;
//}
//////////////////////////////////////////////////////////////////////////////


long CUserSCDApp::uMemSetN(LPSTR lpStr1, long iAsc, long iLen)
{
	memset(lpStr1,(int)iAsc, (size_t)iLen) ;
	return iLen ;
}
long CUserSCDApp::uStrCpyN(LPSTR lpStr1, LPCSTR lpStr2, long iLen)
{
	int i=0 ;
	for (i=0; i < iLen ; i++)
	{
		*(lpStr1+i) = *(lpStr2+i) ;
	}
	return i ;
}



void CUserSCDApp::Encoder( unsigned char* Input , unsigned char* Result , int len)
{
	int i;
	int MixCount = 0;

	unsigned char hib, lob, sb;

	for( i = 0 ; i<len ; i++ )
	{		
		sb = (unsigned char)Input[i];
		hib = sb >> 4;
		lob = sb & 0x0f;
		
		if ( hib < 10 )
			Result[MixCount] = hib + '0';
		else
			Result[MixCount] = hib + 87;

		MixCount ++;

		if ( lob < 10 )
			Result[MixCount] = lob + '0';
		else
			Result[MixCount] = lob + 87;

		MixCount ++;	
	}

	Result[MixCount] = 0;

	return;
}

void CUserSCDApp::Decoder( unsigned char* Input , unsigned char* Result , int len)
{
	int i;
	int MixCount = 0;

	unsigned char hib, lob;

	for( i = 0 ; i<len ; i+=2 )
	{	
		if ( Input[i] < 97 )
			hib = Input[i] - '0';
		else
			hib = Input[i] - 87;

		if ( Input[i+1] < 97 )
			lob = Input[i+1] - '0';
		else
			lob = Input[i+1] - 87;
		
		Result[MixCount] = hib << 4 | lob;

	//	hib = sb >> 4;
	//	lob = sb & 0x0f;

		MixCount++;
	}

	Result[MixCount] = 0;

	return;
}
