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
 * FileName: HandleServerMsg.h
 * 
 * Purpose:  Handle msg from GateServer
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:31
===================================================================+*/
#ifndef _HANDLE_SERVER_MSG_H_
#define _HANDLE_SERVER_MSG_H_

#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Engine/Session/TCPSession.h"

class CClientUser;
class CHandleServerMsg
{
public:
    // Init msg handler
    static bool InitializeMsgHandle();

private:
    static bool OnGC_HeartBeat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);
    static bool OnGC_Echo_Response(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);
    static bool OnGC_Chat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);

public:
    // Called after user created
    static bool OnClientUserCreated(AUINT32 nGateSessionId, CClientUser* pClientUser)
    {
        return ms_mapClientUser.insert(std::make_pair(nGateSessionId, pClientUser)).second;
    }

private:
    typedef std::map<AUINT32, CClientUser*> MapClientUser;
    static MapClientUser ms_mapClientUser;
};

#endif // _HANDLE_SERVER_MSG_H_
