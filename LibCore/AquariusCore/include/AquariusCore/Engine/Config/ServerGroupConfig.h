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
 * FileName: ServerGroupConfig.h
 * 
 * Purpose:  Define Common Server Group Config
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:25
===================================================================+*/
#ifndef _SERVER_GROUP_CONFIG_H_
#define _SERVER_GROUP_CONFIG_H_
#include "AquariusCore/Engine/Config/ServerItemDef.hpp"
#include "boost/property_tree/ptree.hpp"
#include <string>
#include <vector>
#include <set>

// Server group config
class CServerGroupConfig
{
public:
    // Initialize server group config
    static bool Initialize();

    // Get game db config
    static inline const DBConfig* GetDBConfig()
    {
        return &ms_stDBConfig;
    }

    // Get server item by id
    static const ServerItem& GetServerItemByType(AUINT32 nServerType);

    // Get server group list
    static inline const MapServerItem& GetGroupServerItemList() 
    {
        return ms_mapServerItem;
    }

    // Get the flag of in-server-list
    inline static bool IsInServerIPList(const std::string& strIP)
    {
        return (ms_setServerIP.find(strIP) != ms_setServerIP.end());
    }

    // Get server group id
    inline static AUINT32 GetServerGroupId()
    {
        return ms_nServerGroupId;
    }

    // Get server group name
    inline static const std::string& GetServerGroupName()
    {
        return ms_strServerGroupName;
    }

private:
    // Load config, input config file path
    static bool LoadServerConfig( );

    // Read Global config
    static bool ReadGlobalConfig(const boost::property_tree::ptree& ptTree);

    // Read ServerList
    static bool ReadServerList(const boost::property_tree::ptree& ptTree);

    // Read DBConfig
    static bool ReadDBConfig(const boost::property_tree::ptree& ptTree, DBConfig& stDBConfig);

    // Parse connector list
    static bool ParseConnectorSet(const std::string& strConnectorList, std::set<EAquariusServerType>& setConnector);

private:
    // Server Group Id
    static AUINT32 ms_nServerGroupId;
    // Server Group Name
    static std::string ms_strServerGroupName;
    // Game DB config
    static DBConfig ms_stDBConfig;
    // Key: Server Type
    // Value: Server item config
    static MapServerItem ms_mapServerItem;
    // Server ip list
    typedef std::set<std::string> SetServerIP;
    static SetServerIP ms_setServerIP;
};

#endif // _SERVER_GROUP_CONFIG_H_
