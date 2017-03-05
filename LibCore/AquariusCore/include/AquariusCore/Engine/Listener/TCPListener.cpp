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
 * FileName: TCPListener.cpp
 * 
 * Purpose:  Implement TCPListener
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:58
===================================================================+*/
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Engine/Listener/TCPListener.h"
#include "AquariusCore/Engine/Session/TCPSessionManager.h"
#include "AquariusCore/Engine/Server/AquariusServer.h"

CTCPListener::CTCPListener()
    :m_boostAcceptor(CAquariusServer::GetBoostIOService()),
    m_bInAsyncAccept(false)
{
}


CTCPListener::~CTCPListener(void)
{
}

bool CTCPListener::OpenTCPPort()
{
    // 1. Parse end point
    boost::system::error_code error_code;
    boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(m_stListenerConfig.strListenerIP, error_code);
    if (error_code)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to parse ip: [%s], error code: [%d]", m_stListenerConfig.strListenerIP.c_str(), error_code.value());
        return false;
    }
    boost::asio::ip::tcp::endpoint endpoint(ip_address, static_cast<unsigned short>(m_stListenerConfig.nListenerPort));
    // 2. Open protocol
    m_boostAcceptor.open(endpoint.protocol(), error_code);
    if (error_code)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to open protocol: error code: [%d]", error_code.value());
        return false;
    }
    // 3. Set tcp option
    m_boostAcceptor.set_option(boost::asio::ip::tcp::acceptor::reuse_address(true));
    // 4. bind end point
    m_boostAcceptor.bind(endpoint, error_code);
    if (error_code)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to bind address: [%s:%d], error code: [%d]", m_stListenerConfig.strListenerIP.c_str(), m_stListenerConfig.nListenerPort, error_code.value());
        return false;
    }
    // 5. Listen tcp port
    m_boostAcceptor.listen(0x7fffffff, error_code);
    if (error_code)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to listen tcp: error code: [%d]", error_code.value());
        return false;
    }
    return true;
}

bool CTCPListener::StartAsynAccept()
{
    // 1. Check accept status to avoid re-enter this function
    if (m_bInAsyncAccept)
    {
        return true;
    }
    // 2. Create session
    CTCPSession* pNewSession = CTCPSessionManager::CreateTCPSession();
    if (nullptr == pNewSession)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "TCP session pool is exhausted, this listener won't accept any more session: [%s:%d]", m_stListenerConfig.strListenerIP.c_str(), m_stListenerConfig.nListenerPort);
        return false;
    }
    // 3. Start accept
    pNewSession->SetBindTCPListener(this);
    m_boostAcceptor.async_accept(pNewSession->GetSocket(),
        boost::bind(&CTCPListener::HandleAsynAccept, this, pNewSession,
        boost::asio::placeholders::error));
    m_bInAsyncAccept = true;
    return true;
}


void CTCPListener::HandleAsynAccept(CTCPSession* pNewSession, const boost::system::error_code& error_code)
{
    // 1. Reset m_bInAsyncAccept flag
    m_bInAsyncAccept = false;
    // 2. Check pointer
    if (nullptr == pNewSession)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pNewSession");
        return;
    }
    // 3. Check error
    if (!error_code)
    {
        pNewSession->OnSessionConnected();
    }
    else
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Error occurs, error code: [%d]", error_code.value());
        CTCPSessionManager::DestroyTCPSession(pNewSession);
    }
    // 4. Accept again
    StartAsynAccept();
}

bool CTCPListener::OpenListener(const ListenerConfig& stListenerConfig)
{
    // 1. Check config
    m_stListenerConfig = stListenerConfig;
    if (!m_stListenerConfig.IsAvailable())
    {
        return true;
    }
    // 2. Open TCP port
    if (!OpenTCPPort())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "OpenTCPPort failed!");
        return false;
    }
    // 3. Start accept
    if (!StartAsynAccept())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "StartAsynAccept failed!");
        return false;
    }
    // 4. Write log
    CAquariusLogger::Logger(LL_NOTICE, "%s Start To Open %s: [%s:%d]", CAquariusServer::GetServerName().c_str(), m_stListenerConfig.strLisetenerName.c_str(), m_stListenerConfig.strListenerIP.c_str(), m_stListenerConfig.nListenerPort);
    return true;
}

bool CTCPListener::CloseListener()
{
    // 1. Check listen ip
    if (!m_stListenerConfig.IsAvailable())
    {
        return true;
    }
    // 2. Close acceptor
    boost::system::error_code error_code;
    m_boostAcceptor.close(error_code);
    if (error_code)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed on m_hAcceptor.close, error code: [%d], address: [%s:%d]", error_code.value(), m_stListenerConfig.strListenerIP.c_str(), m_stListenerConfig.nListenerPort);
        return false;
    }
    return true;
}

