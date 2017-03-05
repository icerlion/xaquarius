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
 * FileName: HubServerConfig.cpp
 * 
 * Purpose:  Load HubServer config file
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:24
===================================================================+*/
#include "AquariusCore/Engine/Config/HubServerConfig.h"
#include "AquariusCore/Logger/AquariusLogger.h"
#include "boost/property_tree/xml_parser.hpp"

ServerItem CHubServerConfig::ms_stHubServerItem;
std::map<AUINT32, ServerItem> CHubServerConfig::ms_mapLinkGateServer;

void CHubServerConfig::Destroy()
{
    ms_mapLinkGateServer.clear();
}

const static char CONST_HUB_SERVER_CONFIG_PATH[] = "../../Config/HubServer.xml";

bool CHubServerConfig::LoadServerItem(const boost::property_tree::ptree& ptTree)
{
    ms_stHubServerItem.nServerType = EAquariusServerType::HubServer;
    ms_stHubServerItem.strName = ServerConfigHelper::ServerTypeToName(ms_stHubServerItem.nServerType);
    ms_stHubServerItem.nId = ptTree.get_child("Id").get_value<int>();
    std::string strListener = ptTree.get_child("Listener").get_value<std::string>();
    ms_stHubServerItem.stPublicListener.ParseFromString(strListener);
    ms_stHubServerItem.setConnector.insert(EAquariusServerType::GateServer);
    return true;
}

bool CHubServerConfig::LoadLinkList(const boost::property_tree::ptree& ptTree)
{
    bool bResult = true;
    for each (auto stNode in ptTree)
    {
        ServerItem stItem;
        auto ptGateServer = stNode.second;
        stItem.nServerType = EAquariusServerType::GateServer;
        stItem.strName = ServerConfigHelper::ServerTypeToName(stItem.nServerType);
        stItem.nId = ptGateServer.get<int>("<xmlattr>.ServerGroupId");
        stItem.stPrivateListener.strLisetenerName = "PublicListener";
        stItem.stPrivateListener.ParseFromString(ptGateServer.get<std::string>("<xmlattr>.Listener"));
        stItem.setConnector.insert(EAquariusServerType::HubServer);
        if (!ms_mapLinkGateServer.insert(std::make_pair(stItem.nId, stItem)).second)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Reduplicate GateServerId: [%d]", stItem.nId);
            bResult = false;
            break;
        }
    }
    return bResult;
}

bool CHubServerConfig::Initialize()
{
    try
    {
        // Load xml file
        boost::property_tree::ptree ptFile;
        boost::property_tree::read_xml(CONST_HUB_SERVER_CONFIG_PATH, ptFile);
        boost::property_tree::ptree ptRoot = ptFile.get_child("HubServerConfig");
        // Load config data
        if (!LoadServerItem(ptRoot.get_child("ServerItem")))
        {
            return false;
        }
        if (!LoadLinkList(ptRoot.get_child("LinkList")))
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

const ServerItem& CHubServerConfig::GetServerItemByGroupId( AUINT32 nId )
{
    auto itFind = ms_mapLinkGateServer.find(nId);
    if (itFind == ms_mapLinkGateServer.end())
    {
        static ServerItem s_stServerItemEmpty;
        return s_stServerItemEmpty;
    }
    return itFind->second;
}
