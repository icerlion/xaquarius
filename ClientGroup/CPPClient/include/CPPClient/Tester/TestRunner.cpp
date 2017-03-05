/*+==================================================================
 * Copyright (C) 2017 XAQUARIUS. All Rights Reserved.
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 *
 * FileName: TestRunner.cpp
 * 
 * Purpose:  Test runner
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:30
===================================================================+*/
#include "CPPClient/Tester/TestRunner.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "CPPClient/Net/HandleServerMsg.h"
#include "AquariusCore/Util/Dump/Dumper.h"
#include "CPPClient/ClientUser/ClientUser.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"
#include "AquariusCore/Engine/Msg/MsgHandlerTable.h"
#include "boost/property_tree/ini_parser.hpp"

bool CTestRunner::ms_stop_all_player = false;

#ifdef WIN32
BOOL WINAPI ClientHandleCtrlPlusC(DWORD key)
{
    if (CTRL_C_EVENT == key)
    {
        CTestRunner::StopAllUser();
    }
    return true;
}
#endif // WIN32

boost::sync_queue<std::string> CTestRunner::ms_sqUserCmd;

CTestRunner::CTestRunner(void)
{
}


CTestRunner::~CTestRunner(void)
{
    
}

bool CTestRunner::Initialize()
{
#ifdef WIN32
    MiniDumper::Initialize("Client", 0);
    SetConsoleCtrlHandler(ClientHandleCtrlPlusC, true);
#endif // WIN32
    boost::property_tree::read_ini("client.ini", m_ptInitFile);
    // [EXTEND] Load your game config
    if (!CHandleServerMsg::InitializeMsgHandle())
    {
        return false;
    }
    return true;
}

void CTestRunner::Destroy()
{
    CMsgHandlerTable::Destroy();
}

void CTestRunner::ClientThread( CClientUser* pUser, boost::asio::io_service* pIOService)
{
    // 1. Check player count
    pUser->UpdateGateServerAddr();
    // 2. Mark the begin time
    AUINT32 nTotalRunTime = m_ptInitFile.get<AUINT32>("total-run-time");
    time_t nBeginTime = time(0);
    do 
    {
        pUser->PollOneTask();
        CCommonUtil::SleepThisThread(1);
    } while (!ms_stop_all_player && (time(0) - nBeginTime < nTotalRunTime));
    // 3. Destroy all player
    while (!pIOService->stopped())
    {
        CCommonUtil::SleepThisThread(10);
        pIOService->stop();
    }
    delete pUser;
    // 5. Destroy io_service
    delete pIOService;
    pIOService = nullptr;
    CAquariusLogger::Logger(LL_NOTICE_FUN, "Exit this thread!");
}

void CTestRunner::StartRunTest()
{
    // Get gate ip
    std::string strGateIP = m_ptInitFile.get<std::string>("gate-ip");
    AUINT16 nGatePort = m_ptInitFile.get<AUINT16>("gate-port");
    // Start thread
    boost::thread_group hThreadGroup;
    int nBeginClientId = m_ptInitFile.get<int>("begin-client-id");
    int nEndClientId = m_ptInitFile.get<int>("end-client-id");
    bool bUserMutual = m_ptInitFile.get<bool>("user-mutual");
    // Update parameter for UserMutual
    if (bUserMutual)
    {
        nEndClientId = nBeginClientId;
    }
    CAquariusLogger::Logger(LL_NOTICE, "Param: begin-client-id:  %d", nBeginClientId);
    CAquariusLogger::Logger(LL_NOTICE, "Param: end-client-id:    %d", nEndClientId);
    CAquariusLogger::Logger(LL_NOTICE, "Param: user-mutual:      %s", bUserMutual ? "true" : "false");
    for (int nClientId = nBeginClientId; nClientId <= nEndClientId; ++nClientId)
    {
        // Thread will be deleted in ClientThread
        boost::asio::io_service* pIOService = new boost::asio::io_service();
        CClientUser* pUser = new CClientUser(*pIOService, nClientId);
        pUser->InitializeUser(strGateIP, nGatePort, bUserMutual);
        try
        {
            hThreadGroup.create_thread(boost::bind(&CTestRunner::ClientThread, this, pUser, pIOService));
        }
        catch (const boost::thread_resource_error& e)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to create thread: msg: [%s], code:[%d], cur size:[%d], except size:[%d]", e.what(), e.code().value(), hThreadGroup.size(), nEndClientId);
            break;
        }
    }
    // Start input thread;
    if (bUserMutual)
    {
        hThreadGroup.create_thread(boost::bind(&CTestRunner::CmdInputThread, this));
    }
    // Wait all thread
    hThreadGroup.join_all();
}

void CTestRunner::CmdInputThread()
{
    // 2. Mark the begin time
    time_t nBeginTime = time(0);
    AUINT32 nTotalRunTime = m_ptInitFile.get<int>("total-run-time");
    do 
    {
        std::string strLine;
        std::getline(std::cin, strLine);
        ms_sqUserCmd.push(strLine);
        while (!ms_sqUserCmd.empty() && ms_stop_all_player)
        {
            CCommonUtil::SleepThisThread(10);
            continue;
        }
        if (ms_stop_all_player)
        {
            break;
        }
    } while (!ms_stop_all_player && (time(0) - nBeginTime < nTotalRunTime));
}

