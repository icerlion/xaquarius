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
 * FileName: ClientUser.cpp
 * 
 * Purpose:  Implement client user
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:32
===================================================================+*/
#include "CPPClient/ClientUser/ClientUser.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "CPPClient/Net/HandleServerMsg.h"
#include "AquariusCore/Util/TinyUtil/TimeUtil.h"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"
#include "boost/thread/sync_queue.hpp"
#include "CPPClient/Tester/TestRunner.h"
#include "CPPClient/Net/HandleServerMsg.h"
#include "AquariusCore/Util/TinyUtil/RandUtil.h"


#define CHECK_USER_MUTUAL() if (m_bUserMutual) { CAquariusLogger::Logger(LL_NOTICE, "Please Input Cmd"); return; }

#define TO_STRING(x) #x
#define DISPATCH_CMD(CMD_NAME) \
    s_setCmdName.insert(ToLowerStringCopy(TO_STRING(CMD_NAME)) ); \
    if (0 == strcmp(ToLowerStringCopy(TO_STRING(CMD_NAME)).c_str(), ToLowerStringCopy(strCmdName).c_str())) { Cmd_##CMD_NAME(vCmdToken); return; }
// End of DISPATCH_CMD

void CClientUser::RunUserCmd()
{
    boost::sync_queue<std::string>& sqUserCmd = CTestRunner::GetUserCmd();
    if (sqUserCmd.empty())
    {
        return;
    }
    std::string strUserCmd;
    sqUserCmd.pull(strUserCmd);
    std::vector<std::string> vCmdToken = CStringUtil::SplitStringToToken(strUserCmd, ' ');
    if (vCmdToken.empty())
    {
        return;
    }
    std::string strCmdName = vCmdToken.front();
    static std::set<std::string> s_setCmdName;
    DISPATCH_CMD(Echo);
    DISPATCH_CMD(Close);
    DISPATCH_CMD(Chat);
    CAquariusLogger::Logger(LL_WARNING_FUN, "Invalid CmdName [%s]", strCmdName.c_str());
    char chFirstChar = (char)toupper(strCmdName[0]);
    for each (const std::string& strCurCmdName in s_setCmdName)
    {
        if (toupper(strCurCmdName[0]) == chFirstChar)
        {
            CAquariusLogger::Logger(LL_NOTICE, "CmdName [%s]", strCurCmdName.c_str());
        }
    }
    CAquariusLogger::Logger(LL_NOTICE, "Please Input Cmd");
}

void CClientUser::Cmd_Echo(const std::vector<std::string>& /*vParam*/)
{
    CG_Echo_Request stCGReq;
    strcpy_s(stCGReq.data.protocol_version, AQUARIUS_PROTOCOL_VERSION.c_str());
    strcpy_s(stCGReq.data.char_array, "echo char");
    stCGReq.data.enum_value = EResponseCode::G_Success;
    for (int nIndex = 0; nIndex < A_ARRAY_SIZE(stCGReq.data.int_array); ++nIndex)
    {
        stCGReq.data.int_array[nIndex] = CRandUtil::RandNumber();
    }
    stCGReq.data.int_param = CRandUtil::RandNumber();
    strcpy_s(stCGReq.data.rand_string_field.data, CStringUtil::GenerateRandString(10).c_str());
    SendMsgToGateServer(stCGReq);
}

void CClientUser::Cmd_Close(const std::vector<std::string>& /*vParam*/)
{
    m_pGateServerSession->SetSessionInError();
}

void CClientUser::Cmd_Chat(const std::vector<std::string>& vParam)
{
    if (vParam.size() < 2)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Chat content");
        return;
    }
    CG_Chat_Cmd stCGCmd;
    strcpy_s(stCGCmd.data.content, vParam[1].c_str());
    SendMsgToGateServer(stCGCmd);
}

std::string CClientUser::ToLowerStringCopy(const std::string& strValue)
{
    std::string strResult = strValue;
    std::transform(strResult.begin(), strResult.end(), strResult.begin(), tolower);
    return strResult;
}

CClientUser::CClientUser(boost::asio::io_service& boostIOService, int nClientId)
    :m_pGateServerSession(new CClientTCPSession(boostIOService, nClientId)),
    m_boostAsioStrand(boostIOService),
    m_boostTimerHandler(boostIOService),
    m_nGateHeartBeatIndex(0),
    m_bUserMutual(false)
{
    OpenUserTimer();
    CHandleServerMsg::OnClientUserCreated(m_pGateServerSession->GetSessionId(), this);
}


void CClientUser::ResetUserData()
{
    //////////////////////////////////////////////////////////////////////////
    m_nGateHeartBeatIndex = 0;
}

CClientUser::~CClientUser(void)
{
}

void CClientUser::CloseThisUser()
{
    ResetUserData();
    m_pGateServerSession->CloseThisSession();
}

void CClientUser::OnGC_HeartBeat_Msg(const GC_HeartBeat_Msg& refCGMsg)
{
    CAquariusLogger::Logger(LL_DEBUG_FUN, "ServerUTCTime: [%s]", CTimeUtil::UTCTimeToReadableString(refCGMsg.server_utc_time).c_str());
}

void CClientUser::OnGC_Echo_Response(const GC_Echo_Response& refCGMsg)
{
    CAquariusLogger::Logger(LL_NOTICE_FUN, "data: [%s]", refCGMsg.data.ValueToCStr());
    CAquariusLogger::Logger(LL_NOTICE_FUN, "code: [%s]", EResponseCodeToCStr(refCGMsg.response_code));
}

void CClientUser::OnGC_Chat_Msg(const GC_Chat_Msg& refCGMsg)
{
    CAquariusLogger::Logger(LL_NOTICE_FUN, "data: [%s]", refCGMsg.data.ValueToCStr());
}

void CClientUser::OpenThisUser()
{
    ResetUserData();
    m_pGateServerSession->OpenThisSession(m_bUserMutual);
}

void CClientUser::OpenUserTimer()
{
    m_boostTimerHandler.expires_from_now(boost::posix_time::millisec(5000));
    m_boostTimerHandler.async_wait(m_boostAsioStrand.wrap(boost::bind(&CClientUser::TimerUpdateUser, this)));
}

void CClientUser::SendHeartBeatMsg()
{
    CG_HeartBeat_Msg stCGReq;
    stCGReq.heartbeat_index = m_nGateHeartBeatIndex;
    SendMsgToGateServer(stCGReq);
    CAquariusLogger::Logger(LL_DEBUG_FUN, "HeartBeatIndex: [%hu]", m_nGateHeartBeatIndex);
    ++m_nGateHeartBeatIndex;
    m_boostTimerHeartBeat.restart();
}

void CClientUser::TimerUpdateUser()
{
    // Send HB msg anyway
    SendHeartBeatMsg();
    // ReTimer
    OpenUserTimer();
}

void CClientUser::PollOneTask()
{
    if (m_bUserMutual)
    {
        RunUserCmd();
    }
    switch (m_pGateServerSession->GetSessionState())
    {
    case CClientTCPSession::Session_OffLine:
        OpenThisUser();
        break;
    case CClientTCPSession::Session_InError:
        CloseThisUser();
        break;
    case CClientTCPSession::Session_OnLine:
        m_pGateServerSession->PollOnceIOService();
        break;
    default:
        CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid session state: %d", m_pGateServerSession->GetSessionState());
    }
}

void CClientUser::InitializeUser( const std::string& strGateIP, int nGatePort, bool bUserMutual)
{
    m_pGateServerSession->SetServerAddress(strGateIP, nGatePort);
    m_bUserMutual = bUserMutual;
}

void CClientUser::UpdateGateServerAddr()
{
    m_pGateServerSession->UpdateRemoteEndPointString();
}
