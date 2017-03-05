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
 * FileName: CommonUtil.h
 * 
 * Purpose:  Define common util
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:57
===================================================================+*/
#ifndef _COMMON_UTIL_H_
#define _COMMON_UTIL_H_

#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Logger/AquariusLogger.h"

class CCommonUtil
{
public:
    // Get this thread id 
    static unsigned int GetThisThreadId();

    // Sleep by ms
    static void SleepThisThread(AUINT32 nMS);

    // Convert byte order for 16bit int, host to network
    static AUINT16 HostToNet16(AUINT16 nValue)
    {
        return boost::asio::detail::socket_ops::host_to_network_short(nValue);
    }

    // Convert byte order for 32bit int, host to network
    static AUINT32 HostToNet32(AUINT32 nValue)
    {
        return boost::asio::detail::socket_ops::host_to_network_long(nValue);
    }

    // Convert byte order for 32bit int, host to network
    static AINT32 HostToNet32(AINT32 nValue)
    {
        return boost::asio::detail::socket_ops::host_to_network_long(nValue);
    }

    // Convert byte order for 16bit int, network to host
    static AUINT16 NetToHost16(AUINT16 nValue)
    {
        return boost::asio::detail::socket_ops::network_to_host_short(nValue);
    }

    // Convert byte order for 32bit int, network to host
    static AUINT32 NetToHost32(AUINT32 nValue)
    {
        return boost::asio::detail::socket_ops::network_to_host_long(nValue);
    }

    // Reset content of struct as 0
    template<typename TypeStruct>
    static void ZeroStruct(TypeStruct& stTypeStruct)
    {
        memset(&stTypeStruct, 0, sizeof(stTypeStruct));
    }

    // Reset content of char array as 0
    template<typename ElementType, unsigned int ArraySize >
    static void ZeroArray(ElementType (&stCharArray)[ArraySize])
    {
        memset(stCharArray, 0, ArraySize * sizeof(ElementType));
    }
};

#endif // _COMMON_UTIL_H_
