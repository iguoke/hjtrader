#pragma once
#include "header.h"
#include <trade.hpp>
class IStrategy
{
	protected:
		IStrategy(std::string name)
		{
			Name=name;
		}
		void RegInst(std::string inst)
		{
			std::vector<std::string>::iterator it=instVec.begin();
			for(;it!=instVec.end();++it)
			{
				if(inst==*it)
					return;
			}
			instVec.push_back(inst);
			//fprintf(stdout,"%d\n",instVec.size());
		}
	protected:
		friend class TdApi;
		std::vector<std::string> instVec;
	public:
		virtual void OnInit()
		{
		
		}
		virtual void OnTick(Tick& p_tick)
		{

		}
		virtual void OnOrder(Order& p_order)
		{
		}
		virtual void OnTrade(Trade& p_trade)
		{
		}
		virtual void OnExit()
		{
		}
	public:
		std::string Name;


};

