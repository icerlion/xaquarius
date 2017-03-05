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
 * Purpose:  Enter of CPPClient
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:32
===================================================================+*/
#include "AquariusCore/Logger/AquariusLogger.h"
#include "CPPClient/Tester/TestRunner.h"
#include <iostream>
#include "Protocol/CSProtocol.hpp"

int main(int /*argc*/, char** /*argv*/)
{
    // Init log
    if (!CAquariusLogger::Initialize("LogConfig.xml", "CPPClient"))
    {
        std::cout<<"CAquariusLogger::Initialize failed!\n";
        return -1;
    }
    // Init Test Runner
    CTestRunner hTestRunner;
    if (!hTestRunner.Initialize())
    {
        return false;
    }
    hTestRunner.StartRunTest();
    hTestRunner.Destroy();
    return 0;
}