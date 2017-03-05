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
 * FileName: TCPSessionManager.h
 * 
 * Purpose:  Manager TCP session, hold all TCP session
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:15
===================================================================+*/
#ifndef _TCP_SESSION_MANAGER_H_
#define _TCP_SESSION_MANAGER_H_

#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"

class CTCPSession;
class CTCPSessionManager
{
public:
    // Initialize Session Manager
    static bool Initialize();

    // Destroy all session
    static void DestroyAllSession();

    /**
     * Description: Create tcp session
     * Returns CTCPSession*: session pointer
     */
    static CTCPSession* CreateTCPSession();
    
    // Destroy tcp session by point
    static void DestroyTCPSession(CTCPSession* pTCPSession);

    // Get TCPSession pointer by id
    static CTCPSession* GetTCPSession(AUINT32 nSessionId);

private:
    // Update session by timer
    static void TimerUpdateSession();

private:
    typedef std::map<AUINT32, CTCPSession*> MapTCPSession;
    // Key: tcp session id
    // Value: session pointer
    static MapTCPSession ms_mapTCPSession;
};

#endif // _TCP_SESSION_MANAGER_H_
