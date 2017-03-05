
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
 * FileName: RandUtil.h
 * 
 * Purpose:  Define RandUtil
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:48
===================================================================+*/
#ifndef _RAND_UTIL_H_
#define _RAND_UTIL_H_

#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "boost/random.hpp"
#include "boost/thread.hpp"

class CRandUtil
{
public:
    // Get a rand number
    // If 0 != nMaxValue, return [0, nMaxValue]
    // If 0 == nMaxValue, return [0, 10000]
    static AUINT32 RandNumber(AUINT32 nMaxValue = 0);

    /**
     * Description: Generate rand number between nMinValue and nMaxValue
     * Parameter AUINT32 nMinValue [in]: min value
     * Parameter AUINT32 nMaxValue [in]: max value
     * Returns AUINT32: rand number, [nMinValue, nMaxValue]
     */
    static AINT32 RandNumber(AINT32 nMinValue, AINT32 nMaxValue);


    // Robert Jenkins' 32 bit integer hash function (http://www.concentric.net/~ttwang/tech/inthash.htm)
    static int HashInt32(int nValue);

private:
    static boost::mutex ms_mutex_random;
    typedef boost::variate_generator<boost::mt19937, boost::uniform_int<> > BoostRandGenerator;
    static BoostRandGenerator ms_hBoostRandGenerator;
};

#endif // _RAND_UTIL_H_
