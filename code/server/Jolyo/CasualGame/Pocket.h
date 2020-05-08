#pragma once

namespace CasualGame
{
	class CPocket
	{
	private:

	public:
		CPocket(){}
		virtual ~CPocket(){}

	public:
		virtual void	OnAlloc(){}
		virtual void	OnFree(){}
	};// class CPocket

}// namespace CasualGame
