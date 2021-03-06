/**
 *	@file md_configence.cpp
 *  @author shuaiw
 */
#include <iostream>
#include <fstream>
#include <iomanip>
#include <cstring>
#include <cstdio>
#include <cstdlib>

using namespace std;

#include <XeleFtdcMduserApi.h>
#include "DemoUtils.h"
#include <pthread.h>
#include <signal.h>

/*
 * 辅助函数: 读取配置文件md_config.ini
 */

static char USERID[MEMB_SIZEOF(CXeleMdFtdcReqUserLoginField, UserID)];
static char PASSWD[MEMB_SIZEOF(CXeleMdFtdcReqUserLoginField, Password)];
static char FRONTADDRESS[40];
static char MCASTADDRESS[40];
static char NIC[9];


static void loadConfigFile(char *iniName) {
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
 *
 * 辅助函数: 填写login域
 */
static void fill_userlogin(CXeleMdFtdcReqUserLoginField *req) {
    S_INPUT(req, CXeleMdFtdcReqUserLoginField, UserID, USERID);
    S_INPUT(req, CXeleMdFtdcReqUserLoginField, Password, PASSWD);
    S_INPUT(req, CXeleMdFtdcReqUserLoginField, ProtocolInfo, "protocol");
}

/*
 * 示例Spi
 */
struct ExampleSpi : public CXeleMdSpi {
public:

    virtual void OnFrontDisconnected(int nReason) {
        (cout) << "<" << __FUNCTION__ << ">" << " Errcode:" << nReason << endl;
    }

};

/*
 * 主函数:
 */

int g_md_switch = 1;

void *job_recv_market_data(void *arg) {
    CXeleMdApi *api = (CXeleMdApi *) arg;
    int handle = api->GetHandle();
    CXeleShfeMarketDataUnion mdtick;
    ofstream log("./RecvMarketDataTick.log");
    while (g_md_switch) {
        if (RecvShfeMarketDataTick(handle, &mdtick)) {
            if (mdtick.md_type[0] == 'M') {
                printXeleShfeHighLevelOneMarketData(log, "ShfeHighLevelOneMarketData", &mdtick.type_high);
            }
            else if (mdtick.md_type[0] == 'S') {
                printXeleShfeLowLevelOneMarketData(log, "ShfeLowLevelOneMarketData", &mdtick.type_low);
            }
            else if (mdtick.md_type[0] == 'Q') {
                printXeleShfeDepthMarketData(log, "ShfeDepthMarketData", &mdtick.type_depth);
            }
        }
    }
}

int main(int argc, char *argv[]) {
    /*
     * 读取md_config.ini
     */
    loadConfigFile(NULL);

    std::string msg;

    /*
     * 创建对象
     */
    ExampleSpi spi;
    CXeleMdApi *api = CXeleMdApi::CreateMdApi(&spi);

    /*
     * 准备login的结构体
     */
    CXeleMdFtdcReqUserLoginField login_info;
    fill_userlogin(&login_info);

    /*
     * 开始登录
     */
    fprintf(stdout, "%s\n", api->GetVersion());

    int status = api->LoginInit(FRONTADDRESS, MCASTADDRESS, NIC, &login_info);
    if (status == XELEAPI_SUCCESS) {
        cout << "XELEAPI_SUCCESS" << endl;
    }
    else {
        api->Release();
        cout << "LoginInit fail. Exit." << endl;
        return 1;
    }
    /*
     * 创建线程, 获取数据
     */

    pthread_t md_thread;
    g_md_switch = 1;
    pthread_create(&md_thread, NULL, job_recv_market_data, api);

    do {
        cout << "Input 'q' to disconnect API:";
        getline(cin, msg);
    } while (msg != "q");
    g_md_switch = 0;
    pthread_join(md_thread, NULL);
    api->Release();
    cerr << "API release done. Exit Demo." << endl;
}
