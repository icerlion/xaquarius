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
 * FileName: ClientTCPSession.cpp
 * 
 * Purpose:  Implememt Client TCPSession
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:32
===================================================================+*/
#include "CPPClient/Net/ClientTCPSession.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Engine/Msg/MsgDispatch.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"
#include "Protocol/CSProtocol.hpp"
#include "boost/bind.hpp"
#include "CPPClient/Net/HandleServerMsg.h"

CClientTCPSession::CClientTCPSession(boost::asio::io_service& boostIOService, int nClientId)
    :m_strServerIP(""),
    m_nServerPort(0),
    m_boostIOService(boostIOService),
    m_boostTCPSocket(m_boostIOService),
    m_bInAysnReadData(false),
    m_nSessionState(Session_OffLine)
{
    m_nSessionId = nClientId;
}

CClientTCPSession::~CClientTCPSession(void)
{
    DestroyTCPSocket();
}

void CClientTCPSession::DestroyTCPSocket()
{
    // 1. Cancel r-w
    boost::system::error_code boostErrorCode;
    m_boostTCPSocket.cancel(boostErrorCode);
    if (boostErrorCode)
    {
        //CAquariusLogger::Logger(LL_DEBUG_FUN, "m_tcp_socket.cancel: msg:[%s], code:[%d]", error_code.message().c_str(), error_code.value());
    }
    // 2. Try to close the socket
    if (m_boostTCPSocket.is_open())
    {
        m_boostTCPSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, boostErrorCode);
        m_boostTCPSocket.close(boostErrorCode);
        if (boostErrorCode)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "m_tcp_socket.close: msg:[%s], code:[%d]", boostErrorCode.message().c_str(), boostErrorCode.value());
        }
        // Only handle read can make socket as off line, just wait!
        int nRunIndex = 0;
        do 
        {
            m_boostIOService.poll_one(boostErrorCode);
            if (boostErrorCode)
            {
                CAquariusLogger::Logger(LL_ERROR_FUN, "m_io_service.poll_one: SessionId:[%u], msg:[%s], code:[%d]", m_nSessionId, boostErrorCode.message().c_str(), boostErrorCode.value());
            }
            if (m_boostTCPSocket.is_open())
            {
                m_boostTCPSocket.close(boostErrorCode);
                if (boostErrorCode)
                {
                    CAquariusLogger::Logger(LL_ERROR_FUN, "m_io_service.poll_one: SessionId:[%u], msg:[%s], code:[%d]", m_nSessionId, boostErrorCode.message().c_str(), boostErrorCode.value());
                }
            }
            else
            {
                if (++nRunIndex > 10)
                {
                    m_nSessionState = Session_OffLine;
                    break;
                }
            }
        } while (m_nSessionState != Session_OffLine);
    }
}

bool CClientTCPSession::CloseThisSession()
{
    CAquariusLogger::Logger(LL_NOTICE_FUN, "For SessionId: [%u]", m_nSessionId);
    DestroyTCPSocket();
    m_bInAysnReadData = false;
    m_hReceivedData.Reset();
    return true;
}

bool CClientTCPSession::OpenThisSession(bool /*bUserMutual*/)
{
    // 1. Reset session status
    CAquariusLogger::Logger(LL_DEBUG_FUN, "For SessionId: [%u]", m_nSessionId);
    m_bInAysnReadData = false;
    m_hReceivedData.Reset();
    // 2. Parse GateIP
    boost::system::error_code error_code;
    boost::asio::ip::address ip_address = boost::asio::ip::address::from_string(m_strServerIP, error_code);
    if (error_code)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Fail to parse ip:[%s], msg:[%s], code:[%d]", m_strServerIP.c_str(), error_code.message().c_str(), error_code.value());
        return false;
    }
    boost::asio::ip::tcp::endpoint stEndPoint;
    stEndPoint.address(ip_address);
    stEndPoint.port(static_cast<unsigned short>(m_nServerPort));
    // 3. Connect to gate server
    m_boostTCPSocket.close();
    m_boostTCPSocket.connect(stEndPoint, error_code);
    if (error_code)
    {
        CAquariusLogger::Logger(LL_WARNING_FUN, "SessionId:[%u] Fail to connect ip:[%s:%d], msg:[%s], code:[%d]", m_nSessionId, m_strServerIP.c_str(), m_nServerPort, error_code.message().c_str(), error_code.value());
        CCommonUtil::SleepThisThread(5000);
        return false;
    }
    std::string strLocalEndPoint = ExportLocalEndPoint(m_boostTCPSocket);
    m_nSessionState = Session_OnLine;
    // 4. StartAsynRead
    StartAsynRead();
    CAquariusLogger::Logger(LL_NOTICE_FUN, "SessionId:[%u] Connect GateServer [%s:%hu] from [%s]", m_nSessionId, m_strServerIP.c_str(), m_nServerPort, strLocalEndPoint.c_str());
    return true;
}

void CClientTCPSession::PollOnceIOService()
{
    boost::system::error_code boostErrorCode;
    m_boostIOService.poll_one(boostErrorCode);
    if (boostErrorCode)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "SessionId:[%u], msg:[%s], code:[%d]", m_nSessionId, boostErrorCode.message().c_str(), boostErrorCode.value());
        SetSessionInError();
    }
}

void CClientTCPSession::StatRecvMsg(AUINT16)
{

}

void CClientTCPSession::HandlerRead(const boost::system::error_code& boostErrorCode, std::size_t bBytesTransferred)
{
    m_bInAysnReadData = false;
    // Error happened
    if (boostErrorCode)
    {
        if (m_boostTCPSocket.is_open())
        {
            CAquariusLogger::Logger(LL_DEBUG_FUN, "msg:[%s], code:[%d], SessionId:[%u]", boostErrorCode.message().c_str(), boostErrorCode.value(), m_nSessionId);
            SetSessionInError();
        }
        else
        {
            m_nSessionState = Session_OffLine;
        }
        return;
    }
    // Socket is closed
    if (!m_boostTCPSocket.is_open())
    {
        m_nSessionState = Session_InError;
        CAquariusLogger::Logger(LL_ERROR_FUN, "m_tcp_socket.is_open failed, SessionId:[%u]", m_nSessionId);
        return;
    }
    // Process received data
    if (!m_hReceivedData.OnReceiveData((AUINT32)bBytesTransferred, this))
    {
        SetSessionInError();
    }
    // Re-read
    StartAsynRead();
}

void CClientTCPSession::UpdateRemoteEndPointString()
{
    m_strRemoteEndPoint = m_strServerIP;
    m_strRemoteEndPoint.append(":").append(CStringUtil::Int32ToDecString(m_nServerPort));
}

void CClientTCPSession::StartAsynRead()
{
    if (m_bInAysnReadData)
    {
        return;
    }
    m_boostTCPSocket.async_read_some(boost::asio::buffer(m_hReceivedData.GetThisTimeReadDataBuffPointer(), m_hReceivedData.GetThisTimeReadDataBuffLength()), 
        boost::bind(&CClientTCPSession::HandlerRead, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
    m_bInAysnReadData = true;
}

void CClientTCPSession::HandleWrite( const boost::system::error_code& boostErrorCode, size_t nBytesTransferred )
{
    // If failed to write, just handle session error
    if (boostErrorCode || 0 == nBytesTransferred)
    {
        CAquariusLogger::Logger(LL_DEBUG_FUN, "error msg:[%s], code:[%d], SessionId:[%u]", boostErrorCode.message().c_str(), boostErrorCode.value(), m_nSessionId);
        SetSessionInError();
    }
}

std::string CClientTCPSession::ExportLocalEndPoint( boost::asio::ip::tcp::socket& boostTCPSocket )
{
    boost::system::error_code boostErrorCode;
    boost::asio::ip::tcp::endpoint stLocalEndPoint = boostTCPSocket.local_endpoint(boostErrorCode);
    if (boostErrorCode)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "msg:[%s], code:[%d]", boostErrorCode.message().c_str(), boostErrorCode.value());
        return "";
    }
    const boost::asio::ip::address& address = stLocalEndPoint.address();
    int nPort = stLocalEndPoint.port();
    std::string strResult = address.to_string(boostErrorCode);
    strResult.append(":").append(CStringUtil::Int32ToDecString(nPort));
    return strResult;
}

void CClientTCPSession::SetSessionInError()
{
    m_nSessionState = Session_InError;
}

CClientTCPSession::ESessionState CClientTCPSession::GetSessionState() const
{
    return m_nSessionState;
}

void CClientTCPSession::SetServerAddress( const std::string& strIP, int nPort)
{
    m_strServerIP = strIP;
    m_nServerPort = (AUINT16)nPort;
}
