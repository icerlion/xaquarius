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
 * FileName: RandUtil.cpp
 * 
 * Purpose:  Implement RandUtil
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:49
===================================================================+*/ 
#include "AquariusCore/Util/TinyUtil/RandUtil.h"
#include "AquariusCore/Logger/AquariusLogger.h"

CRandUtil::BoostRandGenerator CRandUtil::ms_hBoostRandGenerator(boost::mt19937(), boost::uniform_int<>(0, 10000));
boost::mutex CRandUtil::ms_mutex_random;

AINT32 CRandUtil::RandNumber( AINT32 nMinValue, AINT32 nMaxValue )
{
    if (nMaxValue == nMinValue)
    {
        return nMinValue;
    }
    if (nMaxValue < nMinValue)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: nMaxValue [%u] < nMinValue [%u]", nMaxValue, nMinValue);
        return 0;
    }
    AINT32 nDistance = nMaxValue - nMinValue;
    AINT32 nResult = RandNumber(nDistance);
    nResult += nMinValue;
    if (nResult > nMaxValue)
    {
        CAquariusLogger::Logger(LL_CRITICAL_FUN, "Impossible: nResult > nMaxValue, Fix it to nMaxValue, Param: [%d-%d]", nMinValue, nMaxValue);
        nResult = nMaxValue;
    }
    if (nResult < nMinValue)
    {
        CAquariusLogger::Logger(LL_CRITICAL_FUN, "Impossible: nResult < nMinValue , Fix it to nMinValue, Param: [%d-%d]", nMinValue, nMaxValue);
        nResult = nMinValue;
    }
    return nResult;
}


int CRandUtil::HashInt32( int nValue )
{
    nValue = (nValue + 0x7ed55d16) + (nValue << 12);
    nValue = (nValue ^ 0xc761c23c) ^ (nValue >> 19);
    nValue = (nValue + 0x165667b1) + (nValue << 5);
    nValue = (nValue + 0xd3a2646c) ^ (nValue << 9);
    nValue = (nValue + 0xfd7046c5) + (nValue << 3);
    nValue = (nValue ^ 0xb55a4f09) ^ (nValue >> 16);
    return nValue;
}

AUINT32 CRandUtil::RandNumber(AUINT32 nMaxValue /*= 0*/)
{
    // Begin lock
    boost::unique_lock<boost::mutex> scoped_lock(ms_mutex_random);
    AUINT32 nIntValue = ms_hBoostRandGenerator();
    // End lock
    if (0 != nMaxValue)
    {
        nIntValue = nIntValue % (nMaxValue + 1);
    }
    return nIntValue;
}
