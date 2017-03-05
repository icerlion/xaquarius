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
 * FileName: MemoryCache.hpp
 * 
 * Purpose:  Define memory cache, which can hold some data.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:40
===================================================================+*/
#ifndef _MEMORY_CACHE_HPP_
#define _MEMORY_CACHE_HPP_
// Memory Pool when write HTTP data
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "AquariusCore/Logger/AquariusLogger.h"

template<AUINT32 pool_size>
struct MemoryCache 
{
    // Construct
    MemoryCache()
        :offset(0)
    {
        memset(memory, 0, pool_size);
    }

    // Destructor
    ~MemoryCache()
    {
    }
    // Disable
    MemoryCache(MemoryCache& right) = delete;
    // Disable
    MemoryCache& operator=(MemoryCache& right) = delete;

    // Write data to memory pool, return false if failed.
    bool WriteToPool(const char* pData, AUINT32 nLen)
    {
        // 1. Check offset and pool size
        if (offset >= pool_size)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: offset >= pool_size");
            return false;
        }
        // 2. Check overflow pool
        if (offset + nLen >= pool_size)
        {
            CAquariusLogger::Logger(LL_WARNING_FUN, "Impossible: offset + nLen >= pool_size");
            return false;
        }
        // 3. Write data to pool
        memcpy(memory + offset, pData, nLen);
        // 4. Update offset
        offset += nLen;
        return true;
    }

    // Reset the memory pool
    void Reset()
    {
        offset = 0;
        memset(memory, 0, pool_size);
    }

    // the current offset
    unsigned int offset;
    // define memory
    char memory[pool_size];
};

#endif // _MEMORY_CACHE_HPP_
