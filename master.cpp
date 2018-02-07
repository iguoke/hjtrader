/*************************************************************************
	> File Name: master.cpp
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Thu 25 Jan 2018 03:12:30 AM EST
 ************************************************************************/

#include<iostream>
#include "trade.hpp"
using namespace std;
class test:public IStrategy
{
public:
	test():IStrategy("test")
	{
		
	}
	void OnInit() override
	{
		RegInst(std::string("rb1805"));
	}
	void OnTick(Tick& p_tick) override
	{
		fprintf(stdout,"inst=%s,price=%f\n",p_tick.type_high.Instrument,p_tick.type_high.LastPrice);
				//	fprintf(stdout,"%s,%f\n",mdtick.type_high.Instrument,mdtick.type_high.LastPrice);
				//	fprintf(stdout,"inst=%s,price=%d\n",p_tick)

	}
};
int main (int argc, char* argv[])
{
  IStrategy *st=new test();
  TdApi *td=new TdApi();
  td->AddStrategy(st);
  st->OnInit();
  td->Init();
  
  while(true);
  delete td;
  return 0;
}
