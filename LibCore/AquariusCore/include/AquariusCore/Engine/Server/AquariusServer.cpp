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
 * FileName: AquariusServer.cpp
 * 
 * Purpose:  Implement AquariusServer
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:01
===================================================================+*/
#include "AquariusCore/Engine/Server/AquariusServer.h"
#include "AquariusCore/Engine/Session/TCPSessionManager.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Engine/Timer/TimerManager.h"
#include "AquariusCore/Engine/Connector/TCPConnectorManager.h"
#include "AquariusCore/Engine/Msg/MsgHandlerTable.h"
#include "AquariusCore/Util/Dump/Dumper.h"
#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"
#include "AquariusCore/Util/HTTPHelper/HTTPHelper.h"
#include "AquariusCore/Engine/Config/HubServerConfig.h"
#include "AquariusCore/Engine/Config/ServerGroupConfig.h"

#ifdef WIN32
BOOL WINAPI ConsoleCtrlHandler(DWORD dwMsgType)
{
    switch (dwMsgType)
    {
    case CTRL_C_EVENT:
        CAquariusServer::StopThisServer();
        CAquariusLogger::Logger(LL_NOTICE, "[CTRL_C_EVENT:%d]", dwMsgType);
        break;
    case CTRL_CLOSE_EVENT:
        CAquariusServer::StopThisServer();
        CAquariusLogger::Logger(LL_NOTICE, "[CTRL_CLOSE_EVENT:%d]", dwMsgType);
        break;
    default:
        CAquariusLogger::Logger(LL_NOTICE, "[INVALID CODE:%d]", dwMsgType);
        break;
    }
    Sleep(5000);
    return TRUE;
}
#endif // WIN32


bool CAquariusServer::ms_bStartThisServer = false;
ServerItem CAquariusServer::ms_stServerConfig;
boost::asio::io_service CAquariusServer::ms_boostIOService;
boost::asio::signal_set CAquariusServer::ms_boostSignalSet(ms_boostIOService);
AUINT32 CAquariusServer::ms_nShutDownServerTime = 0xFFFFFFFF;
time_t CAquariusServer::ms_nServerStartTime = 0;

bool CAquariusServer::StartThisServer()
{
    // Init
    if (!Initialize())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to init CAquariusServer");
        CloseThisServer();
        return false;
    }
    // Enter main loop
    ms_nServerStartTime = time(nullptr);
    CAquariusLogger::Logger(LL_NOTICE, "****************************");
    if (EAquariusServerType::HubServer == ms_stServerConfig.nServerType)
    {
        CAquariusLogger::Logger(LL_NOTICE, "|%s Run As ServerId: [%d]", ms_stServerConfig.strName.c_str(), ms_stServerConfig.nId);
    }
    else
    {
        CAquariusLogger::Logger(LL_NOTICE, "|%s Run In Group, Id: [%d], Name: [%s]", ms_stServerConfig.strName.c_str(), CServerGroupConfig::GetServerGroupId(), CServerGroupConfig::GetServerGroupName().c_str());
    }
    CAquariusLogger::Logger(LL_NOTICE, "****************************");
    boost::system::error_code error_code;
    AUINT32 nSleepFlag = 0;
    while (ms_bStartThisServer)
    {
        if (++nSleepFlag > 50)
        {
            nSleepFlag = 0;
            CCommonUtil::SleepThisThread(1);
            time_t nCurTime = time(nullptr);
            if (nCurTime > ms_nShutDownServerTime)
            {
                CAquariusLogger::Logger(LL_NOTICE_FUN, "%s in reach shutdown time point", ms_stServerConfig.strName.c_str());
                break;
            }
        }
        // Process Self server io service
        ms_boostIOService.poll_one(error_code);
        if (0 != error_code.value())
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Error occurs in server loop, error code: [%d]", error_code.value());
        }
    }
    CloseThisServer();
    return true;
}

bool CAquariusServer::OpenThisServer()
{
    // 1. Open listener list
    if (!m_hPublicListener.OpenListener(ms_stServerConfig.stPublicListener))
    {
        return false;
    }
    if (!m_hPrivateListener.OpenListener(ms_stServerConfig.stPrivateListener))
    {
        return false;
    }
    // 2. Init CTCPConnectorManager
    if (!CTCPConnectorManager::Initialize(this))
    {
        return false;
    }
    // 3. Init CTCPSessionManager
    if (!CTCPSessionManager::Initialize())
    {
        return false;
    }
    return true;
}

bool CAquariusServer::CloseThisServer()
{
    m_hPublicListener.CloseListener();
    m_hPrivateListener.CloseListener();
    CMsgHandlerTable::Destroy();
    CTCPConnectorManager::Destroy();
    CTCPSessionManager::DestroyAllSession();
    CTimerManager::Destroy();
    if (!OnDestroy())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "OnDestroy failed");
    }
    ms_boostIOService.stop();
    CAquariusLogger::Logger(LL_NOTICE, "CloseThisServer");
    CHTTPHelper::CleanupCURLLib();
    return true;
}

const static char* CONST_LOG_CONFIG_PATH = "../../Config/LogConfig.xml";

bool CAquariusServer::Initialize( )
{
    // 1. Init CURL
    CHTTPHelper::InitCURLLib();
    // 2. Init server config
    if (!InitSelfServerConfig())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "InitSelfServerConfig failed");
        return false;
    }
    // 3. Register dump and signal process
#ifdef WIN32
    MiniDumper::Initialize(GetServerName(), (int)GetServerType());
    SetConsoleCtrlHandler(ConsoleCtrlHandler, TRUE);
#endif // WIN32
    ms_boostSignalSet.add(SIGINT);
    ms_boostSignalSet.add(SIGTERM);
    ms_boostSignalSet.add(SIGABRT);
#if defined SIGQUIT
    ms_boostSignalSet.add(SIGQUIT);
#endif
    ms_boostSignalSet.async_wait(boost::bind(&CAquariusServer::HandleSignal, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::signal_number));
    // 4. Init log 
    if (!CAquariusLogger::Initialize(CONST_LOG_CONFIG_PATH, GetServerName()))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Initialize logger failed");
        return false;
    }
    // 5. Init timer manager
    if (!CTimerManager::Initialize(ms_boostIOService))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Initialize timer failed");
        return false;
    }
    // 6. Init concrete server
    if (!OnInit())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "OnInit failed");
        return false;
    }
    // 7. Open this server
    if (!OpenThisServer())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "OpenThisServer failed");
        return false;
    }
    // 8. Can start this server
    ms_bStartThisServer = true;
    return true;
}

bool CAquariusServer::InitSelfServerConfig( )
{
    if (EAquariusServerType::HubServer == ms_stServerConfig.nServerType)
    {
        // Load HubServer config
        if (!CHubServerConfig::Initialize())
        {
            return false;
        }
        ms_stServerConfig = CHubServerConfig::GetHubServerItem();
    }
    else
    {
        // Load GroupServer config
        if (!CServerGroupConfig::Initialize())
        {
            return false;
        }
        // Get the config of this server
        ms_stServerConfig = CServerGroupConfig::GetServerItemByType((AUINT32)ms_stServerConfig.nServerType);
    }
    return true;
}

void CAquariusServer::StopThisServer()
{
    ms_bStartThisServer = false;
}

void CAquariusServer::HandleSignal( const boost::system::error_code& error_code, int signal_number )
{
    if (error_code)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Error code: [%d], signal: [%d]", error_code.value(), signal_number);
    }
    switch (signal_number)
    {
    case SIGINT:
        CAquariusLogger::Logger(LL_NOTICE, "Signal: [SIGINT:%d]", signal_number);
        break;
    case SIGTERM:
        CAquariusLogger::Logger(LL_NOTICE, "Signal: [SIGTERM:%d]", signal_number);
        break;
#if defined SIGQUIT
    case SIGQUIT:
        CAquariusLogger::Logger(LL_NOTICE, "Signal: [SIGQUIT:%d]", signal_number);
        break;
#endif
    default:
        CAquariusLogger::Logger(LL_NOTICE, "Invalid signal: [SIGINT:%d]", signal_number);
        break;
    }
    StopThisServer();
}
CAquariusServer::CAquariusServer( EAquariusServerType nType) 
{
    ms_stServerConfig.nServerType = nType;
    ms_stServerConfig.strName = ServerConfigHelper::ServerTypeToName(nType);
}

bool CAquariusServer::OnRemoteServerUp( const ServerItem& /*stServerItem*/ )
{
    return true;
}

bool CAquariusServer::OnRemoteServerDown( const ServerItem& /*stServerItem*/ )
{
    return true;
}

bool CAquariusServer::SignalShutDownServerTime()
{
    if (0xFFFFFFFF != ms_nShutDownServerTime)
    {
        CAquariusLogger::Logger(LL_CRITICAL_FUN, "The Server [%s] has recv signal shutdown, ignore this signore", ms_stServerConfig.strName.c_str());
        return false;
    }
    ms_nShutDownServerTime = (AUINT32)time(nullptr) + ms_stServerConfig.nShutdownDelayTime;
    CAquariusLogger::Logger(LL_WARNING_FUN, "The Server [%s] recv signal shutdown, set the shutdown server time in: [%s]", ms_stServerConfig.strName.c_str(), CTimeUtil::UTCTimeToReadableString(ms_nShutDownServerTime).c_str());
    return true;
}

AUINT32 CAquariusServer::GetShutDownServerTime()
{
    return ms_nShutDownServerTime;
}

void CAquariusServer::DelayShutDownServerTime( AUINT32 nDelayTime )
{
    ms_nShutDownServerTime += nDelayTime;
    CAquariusLogger::Logger(LL_NOTICE_FUN, "delay shutdown time to: [%s]", CTimeUtil::UTCTimeToReadableString(ms_nShutDownServerTime).c_str());
}

CAquariusServer::~CAquariusServer()
{
}

bool CAquariusServer::IsServerReady()
{
    // delay 5 seconds for startup server
    return (time(nullptr) - ms_nServerStartTime > 5);
}

AUINT32 CAquariusServer::GetServerGroupId()
{
    return CServerGroupConfig::GetServerGroupId();
}
