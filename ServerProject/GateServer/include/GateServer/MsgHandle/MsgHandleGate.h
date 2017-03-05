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
 * FileName: MsgHandleGate.h
 * 
 * Purpose:  Register msg handle function in GateServer
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:25
===================================================================+*/
 
#ifndef _MSGHANDLEGATE_H_
#define _MSGHANDLEGATE_H_

#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Engine/Session/TCPSession.h"


class CMsgHandleGate
{
public:
    // Init msg handle
    static bool InitializeMsgHandle();

    // Timer function
    static void TimerUpdate();

private:
    // msg hander list
    static bool OnCG_HeartBeat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);
    static bool OnSS_HeartBeat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);
    static bool OnCG_Echo_Request(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);
    static bool OnFG_Echo_Response(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);
    static bool OnHG_Echo_Response(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);
    static bool OnCG_Chat_Cmd(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);

    // Called on Client Session Error
    static bool OnClientSessionError(AUINT32 nSessionId);

private:
    // Client Session id
    static std::set<AUINT32> ms_setClientSessionId;
};

#endif // _MSGHANDLEGATE_H_
