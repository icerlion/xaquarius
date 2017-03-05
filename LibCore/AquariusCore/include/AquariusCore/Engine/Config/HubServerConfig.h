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
 * FileName: HubServerConfig.h
 * 
 * Purpose:  Define HubServerConfig,
 *           HubServer is designed to connect different server group.
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:24
===================================================================+*/
#ifndef _HUBSERVERCONFIG_H_
#define _HUBSERVERCONFIG_H_
#include "AquariusCore/Engine/Config/ServerItemDef.hpp"
#include "boost/property_tree/ptree.hpp"

class CHubServerConfig
{
public:
    // Init
    static bool Initialize();

    // Destroy
    static void Destroy();

    // Get HubServer item 
    static inline const ServerItem& GetHubServerItem()
    {
        return ms_stHubServerItem;
    }

    // Get gate server map
    static MapServerItem& GetLinkGateServerMap()
    {
        return ms_mapLinkGateServer;
    }

    // Get ServerItem by group id
    static const ServerItem& GetServerItemByGroupId(AUINT32 nId);

private:
    // Load server item
    static bool LoadServerItem(const boost::property_tree::ptree& ptTree);

    // Load link list
    static bool LoadLinkList(const boost::property_tree::ptree& ptTree);

private:
    static ServerItem ms_stHubServerItem;
    static MapServerItem ms_mapLinkGateServer;
};

#endif // _HUBSERVERCONFIG_H_
