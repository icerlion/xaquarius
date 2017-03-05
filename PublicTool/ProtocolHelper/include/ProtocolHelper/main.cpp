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
 * FileName: main.cpp
 * 
 * Purpose:  Defines the entry point for the console application.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 14:18
===================================================================+*/
#include "ProtocolHelper/ProtocolParse/ProtocolParse.h"
#include "AquariusCore/Logger/AquariusLogger.h"

int main()
{
    // 1. Init log.
    if (!CAquariusLogger::Initialize(nullptr, "ParseProtocol"))
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Init Logger Failed");
        return -1;
    }
    // 2. Parse protocol.
    CProtocolParse::Initialize();
    CProtocolParse::ParseProtocolGroup();
    CProtocolParse::Destroy();
}

