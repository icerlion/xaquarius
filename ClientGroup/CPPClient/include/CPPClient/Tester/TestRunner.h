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
 * FileName: TestRunner.h
 * 
 * Purpose:  Define TestRunner
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/3/5 16:31
===================================================================+*/
#ifndef _TEST_RUNNER_H_
#define _TEST_RUNNER_H_
#include <string>
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "boost/thread/sync_queue.hpp"
#include "boost/property_tree/ptree.hpp"

class CClientUser;
class CTestRunner
{
public:
    // Constructor
    CTestRunner(void);

    // Destructor
    ~CTestRunner(void);

    // Flag of stop all player
    static inline void StopAllUser()
    {
        ms_stop_all_player = true;
    }

    // Start run test
    void StartRunTest();

    // Init by cmd line
    bool Initialize();

    // Client thread function
    void ClientThread(CClientUser* pUser, boost::asio::io_service* pIOService);

    // Get user cmd list
    static boost::sync_queue<std::string>& GetUserCmd()
    {
        return ms_sqUserCmd;
    }

    // Destroy the runner
    void Destroy();

private:
    // Only if the player count is 1
    void CmdInputThread();

private:
    // stop all player
    static bool ms_stop_all_player;
    // cmd line
    boost::property_tree::ptree m_ptInitFile;
    static boost::sync_queue<std::string> ms_sqUserCmd;
};

#endif // _TEST_RUNNER_H_
