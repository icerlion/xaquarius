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
 * FileName: ServerGroupConfig.cpp
 * 
 * Purpose:  Implement server group config
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:25
===================================================================+*/
#include "AquariusCore/Engine/Config/ServerGroupConfig.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "boost/property_tree/xml_parser.hpp"
#include "AquariusCore/Util/TinyUtil/StringUtil.h"

const static char CONST_SERVER_GROUP_CONFIG_PATH[] = "../../Config/ServerGroup.xml";

CServerGroupConfig::SetServerIP CServerGroupConfig::ms_setServerIP;
MapServerItem CServerGroupConfig::ms_mapServerItem;
DBConfig CServerGroupConfig::ms_stDBConfig;
std::string CServerGroupConfig::ms_strServerGroupName;
AUINT32 CServerGroupConfig::ms_nServerGroupId;

bool CServerGroupConfig::LoadServerConfig( )
{
    try 
    {
        // 1. Load xml file
        boost::property_tree::ptree ptFile;
        boost::property_tree::read_xml(CONST_SERVER_GROUP_CONFIG_PATH, ptFile);
        boost::property_tree::ptree ptRoot = ptFile.get_child("ServerGroup");
        // 2. Read GlobalConfig
        if (!ReadGlobalConfig(ptRoot.get_child("GlobalConfig")))
        { 
            return false; 
        }
        // 3. Read ServerList
        if (!ReadServerList(ptRoot.get_child("ServerList")))
        { 
            return false; 
        }
        // 4. Read DB config
        if (!ReadDBConfig(ptRoot.get_child("DBConfig"), ms_stDBConfig))
        { 
            return false; 
        }
    }
    catch (const boost::property_tree::ptree_error& ptError)
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "Failed to read xml as [%s]", ptError.what());
        return false;
    }
    return true;
}

// Read global config
bool CServerGroupConfig::ReadGlobalConfig(const boost::property_tree::ptree& ptTree)
{
    ms_nServerGroupId = ptTree.get_child("ServerGroupId").get_value<int>();
    ms_strServerGroupName = ptTree.get_child("ServerGroupName").get_value<std::string>();
    return true;
}

// Read ServerList
bool CServerGroupConfig::ReadServerList(const boost::property_tree::ptree& ptTree)
{
    bool bResult = true;
    for each (auto stNode in ptTree)
    {
        ServerItem stItem;
        auto ptServer = stNode.second;
        stItem.strName = ptServer.get<std::string>("<xmlattr>.Type");
        stItem.nServerType = ServerConfigHelper::ServerNameToType(stItem.strName.c_str());
        stItem.nShutdownDelayTime = ptServer.get<int>("<xmlattr>.ShutDownDelayTime", 60);
        stItem.stPublicListener.strLisetenerName = "PublicListener";
        if (!stItem.stPublicListener.ParseFromString(ptServer.get<std::string>("<xmlattr>.PublicListener", "")))
        {
            bResult = false;
            break;
        }
        stItem.stPrivateListener.strLisetenerName = "PrivateListener";
        if (!stItem.stPrivateListener.ParseFromString(ptServer.get<std::string>("<xmlattr>.PrivateListener")))
        {
            bResult = false;
            break;
        }
        if (!ParseConnectorSet(ptServer.get<std::string>("<xmlattr>.Connector"), stItem.setConnector))
        {
            bResult = false;
            break;
        }
        stItem.nId = (AUINT32)stItem.nServerType; // Server type is id
        if (!ms_mapServerItem.insert(std::make_pair(stItem.nId, stItem)).second)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Re-duplicative server type: [%d]", stItem.nServerType);
            bResult = false;
            break;
        }
        // Update Set server ip
        if (!stItem.stPublicListener.strListenerIP.empty())
        {
            ms_setServerIP.insert(stItem.stPublicListener.strListenerIP);
        }
        if (!stItem.stPrivateListener.strListenerIP.empty())
        {
            ms_setServerIP.insert(stItem.stPrivateListener.strListenerIP);
        }
        if (stItem.stPrivateListener.strListenerConfigString == stItem.stPublicListener.strListenerConfigString)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Server [%d] public listener is the same as private listener", stItem.nServerType);
            bResult = false;
            break;
        }
    }
    return bResult;
}

// Read DBConfig
bool CServerGroupConfig::ReadDBConfig(const boost::property_tree::ptree& ptTree, DBConfig& stDBConfig)
{
    stDBConfig.strHost = ptTree.get_child("Host").get_value<std::string>();
    stDBConfig.nPort = ptTree.get_child("Port").get_value<int>();
    stDBConfig.strUser = ptTree.get_child("User").get_value<std::string>();
    stDBConfig.strPassword = ptTree.get_child("Password").get_value<std::string>();
    stDBConfig.strDBName = ptTree.get_child("DBName").get_value<std::string>();
    return true;
}

bool CServerGroupConfig::Initialize( )
{
    // Load server group config
    if (!LoadServerConfig())
    {
        CAquariusLogger::Logger(LL_ERROR_FUN, "LoadServerConfig failed");
        return false;
    }
    return true;
}

const ServerItem& CServerGroupConfig::GetServerItemByType( AUINT32 nServerType )
{
    // static item, invalid item
    static ServerItem s_stInvalidServerItem;
    // find server item by id
    auto itFind = ms_mapServerItem.find(nServerType);
    if (itFind != ms_mapServerItem.end())
    {
        return itFind->second;
    }
    CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid nServerType: [%d]", nServerType);
    return s_stInvalidServerItem;
}

bool CServerGroupConfig::ParseConnectorSet(const std::string& strConnectorList, std::set<EAquariusServerType>& setConnector)
{
    bool bResult = true;
    std::vector<std::string> vConnector = CStringUtil::SplitStringToToken(strConnectorList, ';');
    for each (const std::string& strConnector in vConnector)
    {
        EAquariusServerType nTargetType = ServerConfigHelper::ServerNameToType(strConnector.c_str());
        if (EAquariusServerType::Invalid == nTargetType)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid target type: [%s]", strConnector.c_str());
            bResult = false;
            break;
        }
        if (!setConnector.insert(nTargetType).second)
        {
            bResult = false;
            CAquariusLogger::Logger(LL_ERROR_FUN, "Invalid connector list: [%s]", strConnectorList.c_str());
            break;
        }
    }
    return bResult;
}

bool ListenerConfig::ParseFromString( const std::string& strAddr )
{
    strListenerConfigString = strAddr;
    if (strAddr.empty())
    {
        return true;
    }
    AUINT32 nPos = (AUINT32)strAddr.find(":");
    if (std::string::npos == nPos)
    {
        return false;
    }
    strListenerIP = strAddr.substr(0, nPos);
    nListenerPort = atoi(strAddr.substr(nPos + 1, strAddr.length()).c_str());
    return true;
}
