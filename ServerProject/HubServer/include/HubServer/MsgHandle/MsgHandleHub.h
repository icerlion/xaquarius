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
 * FileName: MsgHandleHub.h
 * 
 * Purpose:  Register msg handle function in HubServer
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:19
===================================================================+*/
#ifndef _MSGHANDLEHUB_H_
#define _MSGHANDLEHUB_H_

#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Engine/Session//TCPSession.h"


class CMsgHandleHub
{
public:
    // Init msg handle
    static bool InitializeMsgHandle();

private:
    // msg hander list
    static bool OnSS_HeartBeat_Msg(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);
    static bool OnGH_Echo_Request(const char* pData, AUINT32 nDataLen, CTCPSession* pTCPSession);
    
};

#endif // _MSGHANDLEHUB_H_
