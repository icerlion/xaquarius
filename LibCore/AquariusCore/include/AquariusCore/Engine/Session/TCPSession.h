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
 * FileName: TCPSession.h
 * 
 * Purpose:  Define TCPSession
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:14
===================================================================+*/
#ifndef _TCP_SESSION_H_
#define _TCP_SESSION_H_
#include "AquariusCore/Engine/Msg/ReceivedDataHandler.h"
#include "AquariusCore/Engine/Msg/MsgStatistic.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "AquariusCore/Util/ObjPool/ObjectPool.hpp"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "boost/asio.hpp"
#include "boost/bind.hpp"
#include "boost/thread.hpp"
#include "boost/timer.hpp"

class CAquariusServer;
class CTCPListener;
class CTCPSession : public IObjInPool
{
public:
    // Constructor
    CTCPSession();

    // Destructor
    virtual ~CTCPSession() override 
    {
    }

    // Get remote end point
    inline const char* GetRemoteEndPointCStr() const
    {
        return m_strRemoteEndPoint.c_str();
    }

    // Get remove ip address
    inline const std::string& GetRemoteIP() const
    {
        return m_strRemoteIP;
    }

    // Set flag of EnableCheckSpeed
    inline void SetEnableLimitReadSpeed(bool bFlag, AUINT32 nMinReadDataInterval)
    {
        m_bEnableLimitReadSpeed = bFlag;
        m_stTimerReadAction.restart();
        m_nMinReadDataInterval = nMinReadDataInterval;
    }

    // Set flag of EnableVerifyRecvData
    inline void SetEnableVerifyRecvData(bool bFlag)
    {
        m_bEnableVerifyRecvData = bFlag;
    }

    // Implement function in IObjInPool
    virtual void ResetPoolObject() override final;

    // Set session error handle function.
    inline void SetFunctionHandleSessionError(std::function< bool(unsigned int) > pfnHandleSessionError)
    {
        m_pfnHandleSessionError = pfnHandleSessionError;
    }

    // Set session id
    inline void SetSessionId(unsigned int nId)
    {
        m_nSessionId = nId;
    }

    // Get session id
    unsigned int GetSessionId() const 
    {
        return m_nSessionId;
    }

    // Destroy this session
    void DestroyThisSession();

    // Get tcp socket
    inline boost::asio::ip::tcp::socket& GetSocket()
    {
        return m_boostTCPSocket;
    }

    // Process session connected event
    void OnSessionConnected();

    // Send type msg
    template<typename MsgStruct>
    bool SendBinaryMsg(const MsgStruct& stMsg)
    {
        if (!m_boostTCPSocket.is_open())
        {
            CAquariusLogger::Logger(LL_WARNING_FUN, "Socket is closed.");
            return false;
        }
        // Make a safe copy
        m_nDataIOActionTime = time(nullptr);
        // Check monitor flag
        if (stMsg.GetMonitorFlag())
        {
            CAquariusLogger::Logger(LL_DEBUG_FUN, "SessionId: [%u], Msg: [%s], Data: [%s]", m_nSessionId, stMsg.GetMsgName(), stMsg.ValueToCStr());
        }
        MsgStruct stCopy = stMsg;
        stCopy.HostToNetStruct();
        boost::asio::async_write(m_boostTCPSocket,
            boost::asio::buffer((char*)(&stCopy), sizeof(stCopy)),
            boost::bind(&CTCPSession::HandleWrite, this,
            boost::asio::placeholders::error,
            boost::asio::placeholders::bytes_transferred));
        // Statistic msg send
        m_hMsgStatistic.OnSendMsg(stMsg.msg_id);
        return true;
    }

    // Process HeartBeat Request
    template<typename HeartBeatResponse>
    bool OnHeartBeatRequest(AUINT16 nInputHeartIndex)
    {
        // Check the heart beat index
        if (m_nHeartBeatIndex != nInputHeartIndex)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "SessionId: [%u], Server HB: [%hu], Client HB: [%hu]", m_nSessionId, m_nHeartBeatIndex, nInputHeartIndex);
            m_nHeartBeatIndex = nInputHeartIndex;
        }
        else
        {
            CAquariusLogger::Logger(LL_DEBUG_FUN, "SessionId: [%u], Server HB: [%hu], Client HB: [%hu]", m_nSessionId, m_nHeartBeatIndex, nInputHeartIndex);
        }
        // Return heart beat
        HeartBeatResponse stGCMsg;
        stGCMsg.heartbeat_index = m_nHeartBeatIndex;
        stGCMsg.server_utc_time = (AUINT32)time(nullptr);
        ++m_nHeartBeatIndex;
        if (m_nHeartBeatIndex % 10 == 0)
        {
            CAquariusLogger::Logger(LL_DEBUG_FUN, "SessionId: [%u], Server HB: [%hu], Client HB: [%hu]", m_nSessionId, m_nHeartBeatIndex, nInputHeartIndex);
        }
        if (!SendBinaryMsg(stGCMsg))
        {
            CAquariusLogger::Logger(LL_WARNING_FUN, "SendBinaryMsg failed, SessionId: [%u], Server HB: [%hu], Client HB: [%hu]", m_nSessionId, m_nHeartBeatIndex, nInputHeartIndex);
        }
        return true;
    }
    // Set the flag of DestroyOnWriteSuccess
    inline void ShutDownSessionOnWriteSuccess()
    {
        m_bShutDownSessionOnWriteSuccess = true;
    }

    // Update session by UTC time
    bool TimerUpdateSession(AUINT32 nCurTime);

    // Statistic recv msg
    void StatRecvMsg(AUINT16 nMsgId);

    // Set bind listener
    void SetBindTCPListener(CTCPListener* pListener);

private:
    // Handle read msg
    void HandleRead(const boost::system::error_code& boostErrorCode, size_t nBytesTransferred);

    // Handle write msg 
    void HandleWrite(const boost::system::error_code& boostErrorCode, size_t nBytesTransferred);

    // Implement function in ISession
    void UpdateRemoteEndPointString() ;

    // Start async read bytes
    void StartAsyncRead();

    // Check the limit of read speed
    bool CheckLimitReadSpeed();
    
    // Destroy tcp socket
    void DestroyTcpSocket();

protected:
    // Heart Beat Index
    AUINT16 m_nHeartBeatIndex;
    // Remote end point string, ip:port
    std::string m_strRemoteEndPoint;
    // Remote ip
    std::string m_strRemoteIP;
    // Remote port
    AUINT32 m_nRemotePort;
    // TCP socket
    boost::asio::ip::tcp::socket m_boostTCPSocket;
    // Received data handler
    CReceivedDataHandler m_hReceivedData;
    // session id
    unsigned int m_nSessionId;
    // error handler function
    std::function< bool(unsigned int) > m_pfnHandleSessionError;
    // Error flag of this session
    bool m_bSessionInError;
    // Flag of enable check receive data speed
    bool m_bEnableLimitReadSpeed;
    // Flag of enable check recv data
    bool m_bEnableVerifyRecvData;
    // Flag of destroy time out session, only for client session, check flag: m_nDataIOActionTime
    bool m_bDestroyTimtOutSession;
    // TimePoint of read/write time, just for verify dead session.
    time_t m_nDataIOActionTime;
    // Time elapsed of read
    boost::timer m_stTimerReadAction;
    // Flag of wait for read
    bool m_bInAsyncRead;
    // Min read data interval time, unit: ms
    AUINT32 m_nMinReadDataInterval; 
    // flag of destroy session after write success
    bool m_bShutDownSessionOnWriteSuccess;
    // m_pBindTCPListener
    CTCPListener* m_pBindTCPListener;
    // Total recv data len
    AUINT32 m_nTotalRecvDataLen;
    // Total send data len
    AUINT32 m_nTotalSendDataLen;
    // Statistic msg detail
    CMsgStatistic m_hMsgStatistic;
};

#define RETURN_ON_NULL_TCP_SESSION(RET_VAL) if (nullptr == pTCPSession) { CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pTCPSession"); return RET_VAL;}

// Define POOL
const static int MAX_SESSION_COUNT = 4096;
typedef CObjectPool<CTCPSession, MAX_SESSION_COUNT> CTCPSessionPool;

#endif // _TCP_SESSION_H_
