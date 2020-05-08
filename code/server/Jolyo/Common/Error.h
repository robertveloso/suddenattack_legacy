/*****************************************************************************************************************
*****************************************************************************************************************/

#pragma once

#include <Windows.h>

namespace Common
{
	namespace Error
	{
		class CError
		{
		public:
			static LPCTSTR GetErrorMessage( DWORD dwErrorCode );

		};// class CError

	}// namespace Error

}// namespace Command
