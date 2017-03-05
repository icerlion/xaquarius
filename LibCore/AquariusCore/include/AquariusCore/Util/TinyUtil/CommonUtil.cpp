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
 * FileName: CommonUtil.cpp
 * 
 * Purpose:  Implement common util function
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:57
===================================================================+*/
#include "AquariusCore/Util/TinyUtil/CommonUtil.h"
#include "boost/asio.hpp"
#include "boost/random.hpp"
#include "boost/thread.hpp"
#ifdef WIN32
#include <Windows.h>
#elif defined LINUX
//#endif
//#ifdef LINUX
#include <pthread.h>
#endif

void CCommonUtil::SleepThisThread( AUINT32 nMS )
{
#ifdef WIN32
    Sleep(nMS);
#else // WIN32
    boost::this_thread::sleep_for(boost::chrono::milliseconds(nMS));
#endif // End if
}

unsigned int CCommonUtil::GetThisThreadId()
{
#ifdef WIN32
    return GetCurrentThreadId();
#else // WIN32
    unsigned int result = pthread_self();
    return result;
#endif
}

