/*************************************************************************
	> File Name: header.h
	> Author: ma6174
	> Mail: ma6174@163.com 
	> Created Time: Thu 25 Jan 2018 10:18:27 PM EST
 ************************************************************************/

#pragma once
#include <map>
#include<queue>
#include <string>
#include <iostream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cstdlib>
#include <assert.h>

using namespace std;

#include <sched.h>
#include <pthread.h>
#include <signal.h>

#include <XeleFtdcMduserApi.h>
#include <CXeleTraderApi.hpp>
#include "MdUtils.h"
typedef	CXeleShfeMarketDataUnion Tick;
typedef CXeleFtdcOrderField Order;
typedef CXeleFtdcTradeField Trade;
typedef CXeleFtdcRspInstrumentField Instrument;


