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
 * FileName: TCPSession.cpp
 * 
 * Purpose:  Implement TCPSession
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:15
===================================================================+*/
#include "AquariusCore/Engine/Session/TCPSession.h"
#include "AquariusCore/Engine/Session/TCPSessionManager.h"
#include "AquariusCore/Engine/Msg/MsgDispatch.h"
#include "AquariusCore/Engine/Server/AquariusServer.h"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"

void CTCPSession::HandleRead( const boost::system::error_code& boostErrorCode, size_t nBytesTransferred )
{
    // 1. Update the flag of m_bWaitForRead
    m_bInAsyncRead = false;
    // 2. Update IO action time and recv data len
    m_nTotalRecvDataLen += (AUINT32)nBytesTransferred;
    m_nDataIOActionTime = time(nullptr);
    // 3. Check error code
    if (boostErrorCode || 0 == nBytesTransferred)
    {
        CAquariusLogger::Logger(LL_DEBUG_FUN, "ErrorCode: [%d], Msg: [%s], SessionId: [%u], RemoteAddress: [%s], byte transfer: [%u]", boostErrorCode.value(), boostErrorCode.message().c_str(), m_nSessionId, GetRemoteEndPointCStr(), nBytesTransferred);
        CTCPSessionManager::DestroyTCPSession(this);
        return;
    }
    // 4. If session id was 0, the session was invalid!
    if (0 == m_nSessionId || m_bSessionInError)
    {
        return;
    }
    // 5. Check speed
    if (!CheckLimitReadSpeed())
    {
        CTCPSessionManager::DestroyTCPSession(this);
        return;
    }
    // 6. Handle received data
    bool bFlag = m_hReceivedData.OnReceiveData((AUINT32)nBytesTransferred, this);
    if (!bFlag && m_bEnableVerifyRecvData )
    {
        CAquariusLogger::Logger(LL_WARNING_FUN, "OnReceiveData failed, Destroy this session, SessionId: [%u], RemoteAddress:[%s]", m_nSessionId, m_strRemoteEndPoint.c_str());
        CTCPSessionManager::DestroyTCPSession(this);
        return;
    }
    // 7. Re-read data from remote client
    StartAsyncRead();
}

void CTCPSession::HandleWrite( const boost::system::error_code& boostErrorCode, size_t nBytesTransferred )
{
    // Update send data len
    m_nTotalSendDataLen += (AUINT32)nBytesTransferred;
    m_nDataIOActionTime = time(nullptr);
    // If failed to write, just handle session error
    if (boostErrorCode || 0 == nBytesTransferred)
    {
        CAquariusLogger::Logger(LL_DEBUG_FUN, "ErrorCode: [%d], SessionId: [%u], RemoteAddress: [%s] as parameter error", boostErrorCode.value(), m_nSessionId, GetRemoteEndPointCStr());
        CTCPSessionManager::DestroyTCPSession(this);
    }
    if (m_bShutDownSessionOnWriteSuccess)
    {
        boost::system::error_code ec;
        m_boostTCPSocket.shutdown(boost::asio::socket_base::shutdown_both, ec);
        if (ec)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "ErrorCode: [%d], SessionId: [%u], RemoteAddress: [%s] as shutdown failed", ec.value(), m_nSessionId, GetRemoteEndPointCStr());
        }
    }
}

void CTCPSession::UpdateRemoteEndPointString()
{
    m_strRemoteEndPoint.clear();
    m_strRemoteIP.clear();
    m_nRemotePort = 0;
    if (!m_boostTCPSocket.is_open())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Socket is not open!");
        return;
    }
    boost::system::error_code boostErrorCode;
    const boost::asio::ip::tcp::endpoint& boostEndPoint = m_boostTCPSocket.remote_endpoint(boostErrorCode);
    if (boostErrorCode)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "ErrorCode: [%d]", boostErrorCode.value());
        return;
    }
    const boost::asio::ip::address& stAddress = boostEndPoint.address();
    m_nRemotePort = boostEndPoint.port();
    m_strRemoteIP = stAddress.to_string(boostErrorCode);
    if (boostErrorCode)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Error code: [%d]", boostErrorCode.value());
    }
    char buffPort[32] = { 0 };
    sprintf_s(buffPort, "%u", m_nRemotePort);
    m_strRemoteEndPoint = m_strRemoteIP;
    m_strRemoteEndPoint.append(":").append(buffPort);
    m_bDestroyTimtOutSession = !CServerGroupConfig::IsInServerIPList(m_strRemoteIP);
}

void CTCPSession::ResetPoolObject()
{
    // Close socket
    m_nHeartBeatIndex = 0;
    m_strRemoteEndPoint.clear();
    m_strRemoteIP.clear();
    m_nRemotePort = 0;
    DestroyTcpSocket();
    m_strRemoteEndPoint.clear();
    m_hReceivedData.Reset();
    m_nSessionId = 0;
    m_pfnHandleSessionError = nullptr;
    m_bSessionInError = false;
    m_bEnableLimitReadSpeed = false;
    m_bEnableVerifyRecvData = false;
    m_bDestroyTimtOutSession = false;
    m_nDataIOActionTime = 0;
    m_bInAsyncRead = false;
    m_nMinReadDataInterval = 0;
    m_bShutDownSessionOnWriteSuccess = false;
    m_pBindTCPListener = nullptr;
    m_nTotalRecvDataLen = 0;
    m_nTotalSendDataLen = 0;
    m_hMsgStatistic.Reset();
}

void CTCPSession::DestroyTcpSocket()
{
    boost::system::error_code boostErrorCode;
    m_boostTCPSocket.cancel(boostErrorCode);
    if (boostErrorCode)
    {
        CAquariusLogger::Logger(LL_DEBUG_FUN, "Failed on m_tcp_socket.cancel: ErrorCode: [%d]", boostErrorCode.value());
    }
    // Close socket
    if (m_boostTCPSocket.is_open())
    {
        m_boostTCPSocket.shutdown(boost::asio::ip::tcp::socket::shutdown_both, boostErrorCode);
        if (boostErrorCode)
        {
            CAquariusLogger::Logger(LL_DEBUG_FUN, "Failed on m_tcp_socket.shutdown: ErrorCode: [%d]", boostErrorCode.value());
        }
        m_boostTCPSocket.close(boostErrorCode);
        if (boostErrorCode)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Failed on m_tcp_socket.close: ErrorCode: [%d]", boostErrorCode.value());
        }
    }
}

void CTCPSession::DestroyThisSession()
{
    // 1. Check session error flag, avoid re-enter this function
    if (m_bSessionInError)
    {
        return;
    }
    m_bSessionInError = true;
    // 2. Check active flag
    if (!GetActiveFlag())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Session is not active, SessionId: [%u]", m_nSessionId);
    }
    // 3. Process session error hander
    if (nullptr != m_pfnHandleSessionError)
    {
        m_pfnHandleSessionError(m_nSessionId);
    }
    // 4. Check session open flag
    DestroyTcpSocket();
    // 5. Recycle session pointer
    CAquariusLogger::Logger(LL_DEBUG_FUN, "Session Is Down, SessionId: [%u], RemoteAddress: [%s]", m_nSessionId, m_strRemoteEndPoint.c_str());
    // 6. Output session statistic
    m_hMsgStatistic.ShowStatisticResult(m_nSessionId);
    // Save bind TCP listener before recycle this session
    CTCPListener* pBindTCPListener = m_pBindTCPListener;
    // Make a copy for session id
    CTCPSessionPool::RecycleObject(this);
    // 7. Maybe the listener can be accept again after session is full
    if (nullptr != pBindTCPListener)
    {
        pBindTCPListener->StartAsynAccept();
    }
}

void CTCPSession::OnSessionConnected()
{
    // Update recv data timer after session connected
    m_nDataIOActionTime = time(nullptr);
    UpdateRemoteEndPointString();
    StartAsyncRead();
    CAquariusLogger::Logger(LL_DEBUG_FUN, "Session Is Up, SessionId: [%u], RemoteAddress: [%s]", m_nSessionId, m_strRemoteEndPoint.c_str());
}

bool CTCPSession::CheckLimitReadSpeed()
{
    if (!m_bEnableLimitReadSpeed)
    {
        return true;
    }
    // Unit: ms
    AUINT32 nReadIntervalMS = (AUINT32)(m_stTimerReadAction.elapsed() * 1000.0f);
    m_stTimerReadAction.restart();
    //CAquariusLogger::Logger(LL_DEBUG_FUN, "Interval: [%d] ms", nReadIntervalMS);
    if (nReadIntervalMS < m_nMinReadDataInterval )
    {
        CAquariusLogger::Logger(LL_DEBUG_FUN, "[%s] send data too fast, SessionId: [%u], interval: [%dms]!", GetRemoteEndPointCStr(), m_nSessionId, nReadIntervalMS);
    }
    return true;
}

CTCPSession::CTCPSession() 
    :m_nHeartBeatIndex(0),
    m_nRemotePort(0),
    m_boostTCPSocket(CAquariusServer::GetBoostIOService()),
    m_nSessionId(0),
    m_pfnHandleSessionError(nullptr),
    m_bSessionInError(true),
    m_bEnableLimitReadSpeed(false),
    m_bEnableVerifyRecvData(false),
    m_bDestroyTimtOutSession(false),
    m_nDataIOActionTime(0),
    m_bInAsyncRead(false),
    m_nMinReadDataInterval(0),
    m_bShutDownSessionOnWriteSuccess(false),
    m_pBindTCPListener(nullptr),
    m_nTotalRecvDataLen(0),
    m_nTotalSendDataLen(0)
{
}

void CTCPSession::StartAsyncRead()
{
    // Only be called once!
    if (m_bInAsyncRead)
    {
        return;
    }
    m_boostTCPSocket.async_read_some(boost::asio::buffer(m_hReceivedData.GetThisTimeReadDataBuffPointer(), m_hReceivedData.GetThisTimeReadDataBuffLength()),
        boost::bind(&CTCPSession::HandleRead, this,
        boost::asio::placeholders::error,
        boost::asio::placeholders::bytes_transferred));
    m_bInAsyncRead = true;
}

bool CTCPSession::TimerUpdateSession( AUINT32 nCurTime )
{
    // 1. If m_nActionIOTime is 0, the session is not connected
    if (0 == m_nDataIOActionTime)
    {
        return true;
    }
    // 2. Check current time point
    if (m_nDataIOActionTime > nCurTime)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Session Is Error: DataIOActionTime:[%s], CurTime:[%s],SessionId:[%u],RemoteAddress:[%s]", CTimeUtil::UTCTimeToReadableString(m_nDataIOActionTime).c_str(), CTimeUtil::UTCTimeToReadableString(nCurTime).c_str(), m_nSessionId, m_strRemoteEndPoint.c_str());
        return false;
    }
    // 3. Check read time interval
    const static AUINT32 MAX_ACTION_IO_INTERVAL = 60 * 5;
    if (nCurTime - m_nDataIOActionTime > MAX_ACTION_IO_INTERVAL)
    {
        CAquariusLogger::Logger(LL_WARNING_FUN, "Session Is Dead: DataIOActionTime:[%s], CurTime:[%s], SessionId:[%u],RemoteAddress:[%s], WillDestroySession:[%s]", CTimeUtil::UTCTimeToReadableString(m_nDataIOActionTime).c_str(), CTimeUtil::UTCTimeToReadableString(nCurTime).c_str(), m_nSessionId, m_strRemoteEndPoint.c_str(), m_bDestroyTimtOutSession ? "true" : "false");
        // If the remote client was game client, we should destroy it.
        return !m_bDestroyTimtOutSession;
    }
    return true;
}

void CTCPSession::StatRecvMsg( AUINT16 nMsgId )
{
    m_hMsgStatistic.OnRecvMsg(nMsgId);
}

void CTCPSession::SetBindTCPListener( CTCPListener* pListener )
{
    m_pBindTCPListener = pListener;
}
