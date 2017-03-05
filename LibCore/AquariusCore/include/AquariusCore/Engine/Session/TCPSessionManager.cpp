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
 * FileName: TCPSessionManager.cpp
 * 
 * Purpose:  Implement TCPSession Manager
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:15
===================================================================+*/
#include "AquariusCore/Engine/Session/TCPSessionManager.h"
#include "AquariusCore/Engine/Session/TCPSession.h"
#include "AquariusCore/Engine/Timer/TimerManager.h"

CTCPSessionManager::MapTCPSession CTCPSessionManager::ms_mapTCPSession;

void CTCPSessionManager::DestroyAllSession()
{
    // 1. Make a safe copy if session is not empty
    if (ms_mapTCPSession.empty())
    {
        return;
    }
    // 2. Loop session map
    // Make a safe copy for session map as DestroyTCPSession will erase ms_mapTCPSession
    MapTCPSession mapTCPSessionCopy = ms_mapTCPSession;
    for each (auto& kvp in mapTCPSessionCopy)
    {
        CTCPSession* pTCPSession = kvp.second;
        // Check pointer
        if (nullptr == pTCPSession)
        {   
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == m_pTCPSession");
            continue;
        }
        // Check active flag
        if (!pTCPSession->GetActiveFlag())
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Session is not active, impossible");
        }
        DestroyTCPSession(pTCPSession);
    }
    // 3. Clean copy
    ms_mapTCPSession.clear();
}

void CTCPSessionManager::DestroyTCPSession( CTCPSession* pTCPSession )
{
    // Check pointer
    if (nullptr == pTCPSession)
    {   
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pTCPSession");
        return;
    }
    // Erase session by id
    auto itFind = ms_mapTCPSession.find(pTCPSession->GetSessionId());
    if (itFind != ms_mapTCPSession.end())
    {
        ms_mapTCPSession.erase(pTCPSession->GetSessionId());
        // Make sure that DestroyThisSession will be called only once
        pTCPSession->DestroyThisSession();
    }
}

CTCPSession* CTCPSessionManager::GetTCPSession(AUINT32 nSessionId)
{
    auto itFind = ms_mapTCPSession.find(nSessionId);
    if (itFind == ms_mapTCPSession.end())
    {
        return nullptr;
    }
    return itFind->second;
}

CTCPSession* CTCPSessionManager::CreateTCPSession()
{
    // Create session
    static unsigned int s_nSessionId = 0;
    ++s_nSessionId;
    // Acquire session pointer from object pool
    CTCPSession* pTCPSession = CTCPSessionPool::AcquireObject();
    if (nullptr == pTCPSession)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "TCP session pool is exhausted!");
        return nullptr;
    }
    // Init session parameter
    pTCPSession->SetSessionId(s_nSessionId);
    if (!ms_mapTCPSession.insert(std::make_pair(s_nSessionId, pTCPSession)).second)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "SessionId is existed in m_mapTCPSession: [%u]", s_nSessionId);
        CTCPSessionPool::RecycleObject(pTCPSession);
        return nullptr;
    }
    return pTCPSession;
}

bool CTCPSessionManager::Initialize()
{
    // Unit: ms
    const static AUINT32 TIMER_INTERVAL_UPDATE_SESSION = 30000;
    CTimerManager::RegisterTimer(&CTCPSessionManager::TimerUpdateSession, TIMER_INTERVAL_UPDATE_SESSION);
    return true;
}

void CTCPSessionManager::TimerUpdateSession()
{
    // 1. Get current UTC time
    AUINT32 nCurTime = (AUINT32)time(nullptr);
    // 2. Loop session list
    std::vector<CTCPSession*> vDeadSession;
    vDeadSession.reserve(ms_mapTCPSession.size());
    for each (auto& kvp in ms_mapTCPSession)
    {
        CTCPSession* pTCPSession = kvp.second;
        if (nullptr == pTCPSession)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pTCPSession");
            continue;
        }
        // Process update idle routine
        if (!pTCPSession->TimerUpdateSession(nCurTime))
        {
            vDeadSession.push_back(pTCPSession);
        }
    }
    // 3. Destroy dead session
    for each (auto& pDeadSession in vDeadSession)
    {
        DestroyTCPSession(pDeadSession);
    }
}
