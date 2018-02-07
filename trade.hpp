/**
 * Copyright 2017 ACCELECOM LTD.  All Rights Reserved.
 *  @file trade.cpp
 *  @company  hj
 *  @author liu
 *  @modify
 *  @source encoding UTF-8
 *  @console encoding GB2312
 *  @Formatter GNU Built-in
 *  @brief 一个简单的例子，介绍CXeleTraderApi和CXeleTraderSpi接口的使用。
 *	本例将演示一个报单录入操作的过程,以及以下查询的结构体构建：
 *   	1. 持仓查询(Position)
 *  	2. 报单查询(Order)
 *  	3. 成交单查询(Trade)
 *   	4. 资金查询(Account)
 */
#pragma once
#include "header.h"
#include "istrategy.hpp"
class TdApi:public CXeleTraderSpi
{
	private:
		char g_UserID_str[MEMB_SIZEOF(CXeleFtdcReqUserLoginField, UserID)];
		char g_Password_str[MEMB_SIZEOF(CXeleFtdcReqUserLoginField, Password)];
		char g_ParticipantID_str[MEMB_SIZEOF(CXeleFtdcReqUserLoginField,
					    ParticipantID)];
		char g_ClientID_str[MEMB_SIZEOF(CXeleFtdcInputOrderField, ClientID)];
		char g_AccountID_str[MEMB_SIZEOF(CXeleFtdcQryClientAccountField,
					AccountID)];
		char g_FrontAddress_str[80];
        int requestid;
		int logintimes;
		int orderid;
        CXeleTraderApi *pTraderApi; 
		CXeleMdApi *pMdApi;
		char USERID[MEMB_SIZEOF(CXeleMdFtdcReqUserLoginField, UserID)];
		char PASSWD[MEMB_SIZEOF(CXeleMdFtdcReqUserLoginField, Password)];
		char FRONTADDRESS[40];
		char MCASTADDRESS[40];
		char NIC[9];
		CXeleMdSpi mdspi;
		int g_md_switch;
		pthread_t md_thread;

		pthread_t md_thread_out;

		std::map<std::string,CXeleFtdcRspInstrumentField> instMap;
		std::queue<Tick> qTick;
		pthread_mutex_t mutex;
		std::map<std::string,IStrategy*> strategyMap;
		void loadMdConfigFile(char *iniName) {
			if (iniName == NULL || iniName[0] == 0) {
			}
			memset(USERID, 0, sizeof(USERID));
			memset(PASSWD, 0, sizeof(PASSWD));
			memset(FRONTADDRESS, 0, sizeof(FRONTADDRESS));
			/*
			 * load USERID
			 */
			FILE *ini = popen("sed 's/USERID=\\(.*\\)/\\1/g;tx;d;:x' md_config.ini", "r");
			fscanf(ini, "%s\n", USERID);
			pclose(ini);
			/*
			 * load PASSWD
			 */
			ini = popen("sed 's/PASSWD=\\(.*\\)/\\1/g;tx;d;:x' md_config.ini", "r");
			fscanf(ini, "%s\n", PASSWD);
			pclose(ini);
			/*
			 * load FRONTADDRESS
			 */
			ini = popen("sed 's/FRONTADDRESS=\\(.*\\)/\\1/g;tx;d;:x' md_config.ini", "r");
			fscanf(ini, "%s\n", FRONTADDRESS);
			pclose(ini);
			/*
			 * load MCASTADDRESS
			 */
			ini = popen("sed 's/MCASTADDRESS=\\(.*\\)/\\1/g;tx;d;:x' md_config.ini", "r");
			fscanf(ini, "%s\n", MCASTADDRESS);
			pclose(ini);
			/*
			 * load NIC
			 */
			ini = popen("sed 's/NIC=\\(.*\\)/\\1/g;tx;d;:x' md_config.ini", "r");
			fscanf(ini, "%s\n", NIC);
			pclose(ini);


			fprintf(stderr, "USERID:%s\n", USERID);
			fprintf(stderr, "PASSWD:%s\n", PASSWD);
			fprintf(stderr, "FRONTADDRESS:%s\n", FRONTADDRESS);
			fprintf(stderr, "MCASTADDRESS:%s\n", MCASTADDRESS);
			fprintf(stderr, "NIC:%s\n", NIC);

		}

		/*
		 * 辅助函数: 读取配置文件refer.ini
		 */

	    int	iniLoad (char* buf, const char* item, const char* iniName)
		{
		  const char sed_fmt[] = "sed 's/%s=\\(.*\\)/\\1/g;tx;d;:x' %s";
		  char shell_cmd[160];
		  FILE* ini;

		  sprintf (shell_cmd, sed_fmt, item, iniName);
		  ini = popen (shell_cmd, "r");
		  fscanf (ini, "%s\n", buf);
		  pclose (ini);
		  return 0;
		}
	    void loadConfigFile (char* iniName)
		{
		  if (iniName == NULL || iniName[0] == 0)
			{
			  return;
			}

		  memset (g_UserID_str, 0, sizeof(g_UserID_str));
		  iniLoad (g_UserID_str, "USERID", iniName);
		  fprintf (stdout, "USERID:%s\n", g_UserID_str);

		  memset (g_Password_str, 0, sizeof(g_Password_str));
		  iniLoad (g_Password_str, "PASSWD", iniName);
		  fprintf (stdout, "PASSWD:%s\n", g_Password_str);

		  memset (g_ParticipantID_str, 0, sizeof(g_ParticipantID_str));
		  iniLoad (g_ParticipantID_str, "PARTID", iniName);
		  fprintf (stdout, "PARTID:%s\n", g_ParticipantID_str);

		  memset (g_ClientID_str, 0, sizeof(g_ClientID_str));
		  iniLoad (g_ClientID_str, "CIENTID", iniName);
		  fprintf (stdout, "CIENTID:%s\n", g_ClientID_str);

		  memset (g_AccountID_str, 0, sizeof(g_AccountID_str));
		  iniLoad (g_AccountID_str, "INVESTORID", iniName);
		  fprintf (stdout, "INVESTORID:%s\n", g_AccountID_str);

		  memset (g_FrontAddress_str, 0, sizeof(g_FrontAddress_str));
		  iniLoad (g_FrontAddress_str, "FRONTADDRESS", iniName);
		  fprintf (stdout, "FRONTADDRESS:%s\n", g_FrontAddress_str);
		}
		
	public:
		TdApi()
		{
			logintimes=0;
			requestid=0;
			orderid=0;
			mutex=PTHREAD_MUTEX_INITIALIZER;
		}
		void Init()
		{
			loadMdConfigFile((char*)"md_config.ini");

			pMdApi=CXeleMdApi::CreateMdApi(&mdspi);
			MdLogin();
			loadConfigFile((char*) "td_config.ini");
			int exchangeID=1;
			pTraderApi=CXeleTraderApi::CreateTraderApi(exchangeID);
			assert(pTraderApi);
		    pTraderApi->RegisterSpi(this);
			pTraderApi->RegisterFront(g_FrontAddress_str);
			pTraderApi->SubscribePrivateTopic(XELE_TERT_RESTART);
			pTraderApi->SubscribePublicTopic(XELE_TERT_RESTART);
			pTraderApi->Init();
		}
		void MdLogin()
		{
			CXeleMdFtdcReqUserLoginField req;
		    S_INPUT(&req, CXeleMdFtdcReqUserLoginField, UserID, USERID);
			S_INPUT(&req, CXeleMdFtdcReqUserLoginField, Password, PASSWD);
			S_INPUT(&req, CXeleMdFtdcReqUserLoginField, ProtocolInfo, "protocol");	
			fprintf(stdout,"%s\n",pMdApi->GetVersion());
			int status=pMdApi->LoginInit(FRONTADDRESS,MCASTADDRESS,NIC,&req);
			if(status==XELEAPI_SUCCESS)
			{
				fprintf(stdout,"XELEAPI_SUCCESS\n");

			}
			else
			{
				pMdApi->Release();
				fprintf(stdout,"LoginInit fail.Exit\n");
				exit(-1);
			}
			g_md_switch=1;
			pthread_create(&md_thread,NULL,job_recv_market_data,this);

			pthread_create(&md_thread_out,NULL,job_back_ontick,this);
		}
		static void *job_back_ontick(void *arg)
		{
			TdApi *api=(TdApi*)arg;
			while(api->g_md_switch)
			{
				pthread_mutex_lock(&api->mutex);
				if(!api->qTick.empty())
				{
					Tick mdtick=api->qTick.front();
					api->qTick.pop();
					std::map<std::string,IStrategy*>::iterator it=api->strategyMap.begin();
					for(;it!=api->strategyMap.end();++it)
					{
						std::vector<std::string>::iterator itvec=it->second->instVec.begin();
						for(;itvec!=it->second->instVec.end();++itvec)
						{
							if(*itvec==std::string(mdtick.type_high.Instrument))
							{
								it->second->OnTick(mdtick);
								break;
							}
						}
					}
				}
				pthread_mutex_unlock(&api->mutex);
			}
		}
		static void *job_recv_market_data(void *arg)
		{
			TdApi *api=(TdApi*)arg;
			int handle = api->pMdApi->GetHandle();
			Tick mdtick;
			//ofstream log("./RecvMarketDataTick.log");
			while (api->g_md_switch) {
				if (RecvShfeMarketDataTick(handle, &mdtick)) {
					if (mdtick.md_type[0] == 'M') {
						//printXeleShfeHighLevelOneMarketData(log, "ShfeHighLevelOneMarketData", &mdtick.type_high);
						pthread_mutex_lock(&api->mutex);
						//fprintf(stdout,"mm\n");
						api->qTick.push(mdtick);
						//fprintf(stdout,"%d\n",api->qTick.size());
						pthread_mutex_unlock(&api->mutex);
					}
					else if (mdtick.md_type[0] == 'S') {
						//printXeleShfeLowLevelOneMarketData(log, "ShfeLowLevelOneMarketData", &mdtick.type_low);
					}
					else if (mdtick.md_type[0] == 'Q') {
						//printXeleShfeDepthMarketData(log, "ShfeDepthMarketData", &mdtick.type_depth);
					}
				}
			}
		}
		void Login()
		{
			CXeleFtdcReqUserLoginField req;

			memset(&req,0,sizeof(req));
			snprintf(req.UserID,sizeof(req.UserID),"%s",g_UserID_str);
			snprintf(req.Password,sizeof(req.Password),"%s",g_Password_str);
			snprintf(req.ParticipantID,sizeof(req.ParticipantID),"%s",g_ParticipantID_str);
			req.DataCenterID=25;
			fprintf(stdout,"{%s}\n",__FUNCTION__);
			pTraderApi->ReqUserLogin(&req,++requestid);
		}
		Instrument GetInstrument(std::string inst)
		{
			Instrument result;
			std::map<std::string,CXeleFtdcRspInstrumentField>::iterator it2=instMap.find(inst);
			if(it2!=instMap.end())
			{
				result=it2->second;
			}
			return result;
		}
		void AddStrategy(IStrategy *stra)
		{
			strategyMap[stra->Name]=stra;
		}
		void InstrumentAll()
		{
			CXeleFtdcQryInstrumentField req;
			memset(&req,0,sizeof(req));
			snprintf(req.InstrumentID,sizeof(req.InstrumentID),"%s","");
			pTraderApi->ReqQryInstrument(&req,++requestid);
		}

		void OrderIn(TXeleFtdcInstrumentIDType inst,char dire,char offset,double price,int vol)
		{
	
			CXeleFtdcInputOrderField order;
			memset(&order,0,sizeof(order));
			snprintf(order.OrderLocalID,sizeof(order.OrderLocalID),"%s",orderid);
			snprintf (order.ParticipantID, sizeof(order.ParticipantID), "%s",
				g_ParticipantID_str);
			snprintf (order.ClientID, sizeof(order.ClientID), "%s", g_ClientID_str);
			snprintf (order.UserID, sizeof(order.UserID), "%s", g_UserID_str);
			strcpy (order.InstrumentID, inst);
			order.OrderPriceType = XELE_FTDC_OPT_LimitPrice;
			order.Direction = dire;//XELE_FTDC_D_Buy;
			order.CombOffsetFlag[0] =offset;// XELE_FTDC_OF_Open;
			order.CombHedgeFlag[0] = XELE_FTDC_HF_Speculation;
			order.LimitPrice = price;
			order.VolumeTotalOriginal = vol;
			order.TimeCondition = XELE_FTDC_TC_GFD;
			order.VolumeCondition = XELE_FTDC_VC_AV;
			order.MinVolume = 0;
			order.ContingentCondition = XELE_FTDC_CC_Immediately;
			order.StopPrice = 0;
			order.ForceCloseReason = XELE_FTDC_FCC_NotForceClose;
			order.IsAutoSuspend = 0;
			pTraderApi->ReqOrderInsert(&order,++requestid);
						
		}
		void ordercancel(TXeleFtdcOrderSysIDType sysid)
		{
			CXeleFtdcOrderActionField req;
			memset(&req,0,sizeof(req));
			snprintf (req.ParticipantID, sizeof(req.ParticipantID), "%s",
				g_ParticipantID_str);
			snprintf (req.ClientID, sizeof(req.ClientID), "%s", g_ClientID_str);
			snprintf (req.UserID, sizeof(req.UserID), "%s", g_UserID_str);
			strcpy(req.OrderSysID,sysid);
			pTraderApi->ReqOrderAction(&req,++requestid);

		}
	public:
		virtual void OnFrontConnected()
		{
			if(logintimes<3)
			{
				Login();
			}
		}

		virtual void OnFrontDisconnected(int nReason)
		{
		}
		;
		virtual void OnPackageStart(int nTopicID, int nSequenceNo)
		{
		}
		;

		virtual void OnPackageEnd(int nTopicID, int nSequenceNo)
		{
		}
		;

		virtual void OnRspError(CXeleFtdcRspInfoField *pRspInfo,
								int nRequestID,
								bool bIsLast)
		{
		}
		;
		virtual void OnRspUserLogin(CXeleFtdcRspUserLoginField *pRspUserLogin,
									CXeleFtdcRspInfoField *pRspInfo,
									int nRequestID,
									bool bIsLast)
		{
			InstrumentAll();
			fprintf(stdout,"{maxlocalid=%s,%s}\n",pRspUserLogin->MaxOrderLocalID,__FUNCTION__);
		}
		;

		virtual void OnRspUserLogout(CXeleFtdcRspUserLogoutField *pRspUserLogout,
									 CXeleFtdcRspInfoField *pRspInfo,
									 int nRequestID,
									 bool bIsLast)
		{
		}
		;

		virtual void OnRspOrderInsert(CXeleFtdcInputOrderField *pInputOrder,
									  CXeleFtdcRspInfoField *pRspInfo,
									  int nRequestID,
									  bool bIsLast)
		{
		}
		;

		virtual void OnRspOrderAction(CXeleFtdcOrderActionField *pOrderAction,
									  CXeleFtdcRspInfoField *pRspInfo,
									  int nRequestID,
									  bool bIsLast)
		{
		}
		;

		virtual void OnRspQryClientPosition(CXeleFtdcRspClientPositionField *pRspClientPosition,
											CXeleFtdcRspInfoField *pRspInfo,
											int nRequestID,
											bool bIsLast)
		{
		}
		;

		///合约查询应答
		virtual void OnRspQryInstrument(CXeleFtdcRspInstrumentField *pRspInstrument,
										CXeleFtdcRspInfoField *pRspInfo,
										int nRequestID,
										bool bIsLast)
		{
			std::map<std::string,CXeleFtdcRspInstrumentField>::iterator it;
			it=instMap.find(pRspInstrument->InstrumentID);
			
			//fprintf(stdout,"%d\n",bIsLast);
			if(it==instMap.end())
			{
				//fprintf(stdout,"%s\n",pRspInstrument->InstrumentID);
				instMap.insert(std::map<std::string,CXeleFtdcRspInstrumentField>::value_type(pRspInstrument->InstrumentID,*pRspInstrument));
			}
			//fprintf(stdout,"{inst=%s,%s}\n",pRspInstrument->InstrumentID,__FUNCTION__);
			if(bIsLast)
			{
				
				std::map<std::string,CXeleFtdcRspInstrumentField>::iterator it2=instMap.begin();
				for(;it2!=instMap.end();++it2)
				{
		//			fprintf(stdout,"%s\n",it2->first.c_str());
				}
			}
		}
		virtual void OnRspQryClientAccount(CXeleFtdcRspClientAccountField *pClientAccount,
										   CXeleFtdcRspInfoField *pRspInfo,
										   int nRequestID,
										   bool bIsLast)
		{
		}
		;
		///合约保证金率查询应答
		virtual void OnRspQryInstrumentMarginRate(CXeleFtdcRspInstrumentMarginRateField *pRspInstrumentMarginRate,
												  CXeleFtdcRspInfoField *pRspInfo,
												  int nRequestID,
												  bool bIsLast)
		{

		}
		;
		///合约手续费率查询应答
		virtual void OnRspQryInstrumentCommissionRate(CXeleFtdcRspInstrumentCommissionRateField *pRspInstrumentCommissionRate,
													  CXeleFtdcRspInfoField *pRspInfo,
													  int nRequestID,
													  bool bIsLast)
		{
		}
		;

		virtual void OnRspUserPasswordUpdate(CXeleFtdcUserPasswordUpdateField *pUserPasswordUpdate,
											 CXeleFtdcRspInfoField *pRspInfo,
											 int nRequestID,
											 bool bIsLast)
		{
		}
		;

		///成交回报
		virtual void OnRtnTrade(CXeleFtdcTradeField *pTrade)
		{
		}
		;

		///报单回报
		virtual void OnRtnOrder(CXeleFtdcOrderField *pOrder)
		{
		}
		;

		///合约交易状态通知
		virtual void OnRtnInstrumentStatus(CXeleFtdcInstrumentStatusField *pInstrumentStatus)
		{
		}
		;

		///增加合约通知
		virtual void OnRtnInsInstrument(CXeleFtdcInstrumentField *pInstrument)
		{
		}
		;

		///报单录入错误回报
		virtual void OnErrRtnOrderInsert(CXeleFtdcInputOrderField *pInputOrder,
										 CXeleFtdcRspInfoField *pRspInfo)
		{
		}
		;

		///报单操作错误回报
		virtual void OnErrRtnOrderAction(CXeleFtdcOrderActionField *pOrderAction,
										 CXeleFtdcRspInfoField *pRspInfo)
		{
		}
		;
		///报单查询应答
		virtual void OnRspQryOrder(CXeleFtdcOrderField* pOrderField, CXeleFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast){};
		///成交单查询应答
		virtual void OnRspQryTrade(CXeleFtdcTradeField* pTradeField, CXeleFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast){};

		virtual void OnRspInstrumentPrice(CXeleFtdcRspInstrumentPriceField* pTradeField, CXeleFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast){};
		///交易所前置查询应答
		virtual void OnRspQryExchangeFront(CXeleFtdcRspExchangeFrontField *pRspExchangeFront, CXeleFtdcRspInfoField* pRspInfo, int nRequestID, bool bIsLast){};

};
//
//int main (int argc, char* argv[])
//{
//  TdApi *td=new TdApi();
//  td->Init();
//  while(true);
//  delete td;
//  return 0;
//}
