//------------------------------------------------------------------
//
//	FILE	  : Swapper.h
//
//	PURPOSE	  : Defines the CSwapper templated class.
//
//	CREATED	  : February 5, 1997
//
//	COPYRIGHT : Microsoft 1996 All Rights Reserved
//
//------------------------------------------------------------------

#ifndef __SWAPPER_H__
	#define __SWAPPER_H__


	template<class T>
	class CSwapper
	{
		public:

						CSwapper()
						{
							m_iCur = 0;
						}

						CSwapper( T t1, T t2 )
						{
							m_iCur = 0;
							m_Elements[0] = t1;
							m_Elements[1] = t2;
						}

			void		Init( T t1, T t2 )	{ m_Elements[0]=t1; m_Elements[1]=t2; }

			T&			Cur()		{ return m_Elements[m_iCur]; }
			T&			Opposite()	{ return m_Elements[!m_iCur]; }
			void		Swap()		{ m_iCur = !m_iCur; }

			DWORD		m_iCur;
			T			m_Elements[2];

	};


#endif  // __SWAPPER_H__

