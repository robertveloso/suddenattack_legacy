#if !defined(_USERSCD_)
#define _USERSCD_

//#include <atlbase.h>
namespace CasualGame
{

	class CUserSCDApp
	{
	public:
		CUserSCDApp();

	protected:
		void	ecbperm(BYTE* out,BYTE* in,BYTE* pm,int bytes);
		void	ecbround(BYTE* d,BYTE* k);
		void	ecbswap(BYTE* b);
		void	ecbexpand(BYTE a[8],BYTE r[4]);
		void	ecbrotl28(BYTE b[4],int pos);
		void	ecbkeyperm(BYTE cd[8],int i);
		int		ecbbit(BYTE n[8],int pos);

		void	perm(unsigned long* out,unsigned long* in,BYTE* pm,int n);
		void	round(unsigned long d[2],BYTE* k);
		void	swap(unsigned long b[2]);
		unsigned long f(unsigned long r,BYTE* k);
		void	expand(char* a,unsigned long r);
		void	rotl28(unsigned long* b,int pos);
		void	keyperm(unsigned long cd[2],int i);
		int	    bit(unsigned long n[2],int pos);

	public:
		long    uToDedes (BYTE* data, BYTE SesKey[8], BYTE* OutData, long iLen, long iMulti ) ;
		long    uToEndes (BYTE* data, BYTE SesKey[8], BYTE* OutData, long iLen, long iMulti ) ;
		long	uMemSetN(LPSTR lpStr1, long iAsc, long iLen);
		long	uStrCpyN(LPSTR lpStr1, LPCSTR lpStr2, long iLen);

		static void	Encoder( unsigned char* Input , unsigned char* Result , int len);
		static void	Decoder( unsigned char* Input , unsigned char* Result , int len);

	private:
		BYTE	subkey[16][8];
		void	ecbdesinit(BYTE* deskey);
		void	ecbendes(BYTE* data);
		void	ecbdedes(BYTE* data);
		void	desinit(unsigned long* deskey);
		void	endes(unsigned long* data);
		void	dedes(unsigned long* data);

		//DECLARE_MESSAGE_MAP()
	};

}

#endif // !defined(_USERSCD_)

