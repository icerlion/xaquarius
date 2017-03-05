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
 * FileName: ClientUser.h
 * 
 * Purpose:  Define clinet user
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:32
===================================================================+*/
#ifndef _CLIENT_USER_H_
#define _CLIENT_USER_H_
#include "CPPClient/Net/ClientTCPSession.h"
#include "boost/chrono.hpp"
#include "boost/timer.hpp"

class CClientUser
{
public:
    // Constructor
    CClientUser(boost::asio::io_service& boostIOService, int nClientId);

    // Destructor
    ~CClientUser(void);

    // Init player
    void InitializeUser( const std::string& strGateIP, int nGatePort, bool bUserMutual);

    void UpdateGateServerAddr();

    // Open this player
    void OpenThisUser();

    // Close this player
    void CloseThisUser();

    // Handle msg for this user
    void OnGC_HeartBeat_Msg(const GC_HeartBeat_Msg& refCGMsg);
    void OnGC_Echo_Response(const GC_Echo_Response& refCGMsg);
    void OnGC_Chat_Msg(const GC_Chat_Msg& refCGMsg);
    

    // Poll one task of this player
    void PollOneTask();

    // Send msg to server
    template<typename MsgStruct>
    bool SendMsgToGateServer(const MsgStruct& msg)
    {
        return m_pGateServerSession->SendTypeMsg(msg);
    }

private:
    // Send msg function
    void SendHeartBeatMsg();

    // Timer function, send HB
    void TimerUpdateUser();

    // Reset player data
    void ResetUserData();

    // Open heart beat timer
    void OpenUserTimer();

    // Try run user cmd
    void RunUserCmd();

    // Execute use cmd
    void Cmd_Echo(const std::vector<std::string>& vParam);
    void Cmd_Close(const std::vector<std::string>& vParam);
    void Cmd_Chat(const std::vector<std::string>& vParam);

    static std::string ToLowerStringCopy(const std::string& strValue);

private:
    CClientTCPSession* m_pGateServerSession;
    //////////////////////////////////////////////////////////////////////////
    // Init player data
    bool m_bUserMutual;
    //////////////////////////////////////////////////////////////////////////
    AUINT16 m_nGateHeartBeatIndex;
    //////////////////////////////////////////////////////////////////////////
    boost::asio::strand m_boostAsioStrand;
    boost::asio::deadline_timer m_boostTimerHandler;
    boost::timer m_boostTimerHeartBeat;
};

#endif // _CLIENT_USER_H_
