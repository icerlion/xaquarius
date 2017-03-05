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
 * FileName: TimerManager.cpp
 * 
 * Purpose:  Implement TimerManager
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:00
===================================================================+*/
#include "AquariusCore/Engine/Timer/TimerManager.h"
#include "AquariusCore/Logger/AquariusLogger.h"

//////////////////////////////////////////////////////////////////////////
CTimerWrapper::CTimerWrapper( AUINT32 nMilliSec, boost::asio::strand* pAsioStrand, TimerHandler pfnTimerHandler ) 
    :m_pAsioStrand(pAsioStrand),
    m_pAsioTimer(nullptr),
    m_nMilliSec(boost::posix_time::millisec(nMilliSec)),
    m_pfnTimerHandler(pfnTimerHandler)
{
}

CTimerWrapper::~CTimerWrapper()
{
    if (nullptr == m_pAsioTimer)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == m_pAsioTimer, impossible");
    }
    else
    {
        delete m_pAsioTimer;
        m_pAsioTimer = nullptr;
    }
}

bool CTimerWrapper::OnRegister()
{
    if (nullptr == m_pAsioStrand)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == m_pAsioStrand");
        return false;
    }
    m_pAsioTimer = new boost::asio::deadline_timer(m_pAsioStrand->get_io_service(), m_nMilliSec);
    m_pAsioTimer->async_wait(m_pAsioStrand->wrap(boost::bind(&CTimerWrapper::ExecuteTimerHandler, this)));
    return true;
}

bool CTimerWrapper::CancelTimer()
{
    // Cancel timer
    if (nullptr == m_pAsioTimer)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == m_pAsioTimer");
        return false;
    }
    boost::system::error_code stErrorCode; 
    m_pAsioTimer->cancel(stErrorCode);
    if (stErrorCode)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed on m_pAsioTimer->cancel, error code: [%d]", stErrorCode.value());
        return false;
    }
    return true;
}

void CTimerWrapper::ExecuteTimerHandler()
{
    m_pfnTimerHandler();
    if (nullptr == m_pAsioTimer)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == m_pAsioTimer");
        return;
    }
    // Re-wait the timer
    m_pAsioTimer->expires_from_now(m_nMilliSec);
    m_pAsioTimer->async_wait(m_pAsioStrand->wrap(boost::bind(&CTimerWrapper::ExecuteTimerHandler, this)));
}

//////////////////////////////////////////////////////////////////////////
std::vector<CTimerWrapper*> CTimerManager::ms_vTimer;
boost::asio::strand* CTimerManager::ms_pAsioStrand = nullptr;

void CTimerManager::RegisterTimer(TimerHandler pfnTimerHandler, unsigned int nMilliSecond )
{
    // Register timer
    CTimerWrapper* pTimer = new CTimerWrapper(nMilliSecond, ms_pAsioStrand, pfnTimerHandler);
    pTimer->OnRegister();
    ms_vTimer.push_back(pTimer);
}

bool CTimerManager::Initialize( boost::asio::io_service& io_service )
{
    // The Initialize function will be called only on startup, so we can use new.
    ms_pAsioStrand = new boost::asio::strand(io_service);
    return (nullptr != ms_pAsioStrand);
}

void CTimerManager::Destroy()
{
    // Loop every timer instance and delete it.
    for each (CTimerWrapper* pTimer in ms_vTimer)
    {
        delete pTimer;
        pTimer = nullptr;
    }
    // Clean timer list
    ms_vTimer.clear();
    delete ms_pAsioStrand;
    ms_pAsioStrand = nullptr;
}
