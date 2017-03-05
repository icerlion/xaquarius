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
 * FileName: TimerManager.h
 * 
 * Purpose:  Define TimerManager
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 16:00
===================================================================+*/
#ifndef _TIMER_MANAGER_H_
#define _TIMER_MANAGER_H_

#include "boost/bind.hpp"
#include "boost/function.hpp"
#include "boost/asio.hpp"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"

// Define timer handler
typedef boost::function<void()> TimerHandler;

// Timer wrapper
class CTimerWrapper
{
private:
    // Only CTimerManager can access this class
    friend class CTimerManager;

    /**
     * Description: Constructor
     * Parameter AUINT32 nMilliSec                      [in]: timer interval, unit: ms
     * Parameter boost::asio::strand * pAsioStrand      [in]: strand pointer
     * Parameter TimerHandler pfnTimerHandler           [in]: time handler
     */
    CTimerWrapper(AUINT32 nMilliSec, boost::asio::strand* pAsioStrand, TimerHandler pfnTimerHandler);

    // Destructor
    ~CTimerWrapper();

    // Called after register
    bool OnRegister();

    // Cancel this timer
    bool CancelTimer();

    // Execute Timer Handler
    void ExecuteTimerHandler();

private:
    // Strand handler
    boost::asio::strand* m_pAsioStrand;
    // Asio timer
    boost::asio::deadline_timer* m_pAsioTimer;
    // Interval
    boost::posix_time::millisec m_nMilliSec; // Unit: ms
    // Timer handler
    TimerHandler m_pfnTimerHandler;
};

//////////////////////////////////////////////////////////////////////////
class CTimerManager
{
public:
    // Initialize by io_service
    static bool Initialize(boost::asio::io_service& io_service);

    // Destroy timer list, this function should be called before exit application.
    static void Destroy();
    
    /**
     * Description: Register Timer routine
     * Parameter TimerHandler pfnTimerHandler   [in]: timer handler
     * Parameter unsigned int nMilliSecond      [in]: interval milliseconds
     * Returns void:
     */
    static void RegisterTimer(TimerHandler pfnTimerHandler, unsigned int nMilliSecond);

protected:
    // Timer list
    static std::vector<CTimerWrapper*> ms_vTimer;
    // Asio strand
    static boost::asio::strand* ms_pAsioStrand;
};

#endif // _TIMER_MANAGER_H_
