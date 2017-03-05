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
 * FileName: TimeUtil.h
 * 
 * Purpose:  Define TimeUtil function
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:42
===================================================================+*/
#ifndef _TIME_UTIL_HPP_
#define _TIME_UTIL_HPP_

#include <string>
#include <time.h>
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"

class CTimeUtil
{
public:
    // Called only in AnyThread, thread safe, output format: YYYY-MM-DD hh:mm:ss 
    static std::string UTCTimeToReadableString( time_t nTimePoint);

    // Convert string to UTC time, time format: 2015-07-01 11:12:00 20150701111200, 2015-07-01T11:12:00
    static time_t StringToUTCTime(std::string strUTCTime);

    // Convert utc time to local time
    static struct tm UTCTimeToLocalTime(time_t nTime);

};

#endif // _TIME_UTIL_HPP_
