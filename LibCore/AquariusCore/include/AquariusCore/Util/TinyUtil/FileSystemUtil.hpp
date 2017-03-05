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
 * FileName: FileSystemUtil.hpp
 * 
 * Purpose:  File system util, depends on boost::filesystem
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:56
===================================================================+*/
#ifndef _FILE_SYSTEM_UTIL_HPP_
#define _FILE_SYSTEM_UTIL_HPP_
#include <string>
#include <sys/stat.h>
#ifdef LINUX
#include <stdio.h>
#include <unistd.h>
#else
#include <io.h>
#endif
#include "boost/filesystem.hpp"
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "boost/algorithm/string.hpp"

class CFileSystemUtil
{
public:
    /**
     * Description: Try to create abs path.
     * Parameter const std::string & strAbsPath [in]: abs path, example: "d:/gas/download", "./log"
     * Returns bool: true if success
     */
    static inline bool TryCreatePath(const std::string& strPath)
    {
        if (IsPathExisted(strPath))
        {
            return true;
        }
        boost::filesystem::path path(strPath.c_str());
        boost::system::error_code stErrorCode;
        boost::filesystem::create_directories(path, stErrorCode);
        return IsPathExisted(strPath);
    }

    // Remove file by path
    static inline bool RemoveFile(const std::string& strPath)
    {
        boost::filesystem::path path(strPath.c_str());
        boost::system::error_code stErrorCode;
        boost::filesystem::remove_all(path, stErrorCode);
        return !stErrorCode;
    }

    // Check file existed
    static inline bool IsPathExisted(const std::string& strAbsPath)
    {
        boost::filesystem::path path(strAbsPath.c_str());
        boost::system::error_code stErrorCode;
        return boost::filesystem::exists(path, stErrorCode);
    }

    // Get file size
    static inline AUINT32 GetFileSize(const std::string& strFilePath)
    {
        boost::filesystem::path path(strFilePath.c_str());
        AUINT32 nSize = static_cast<AUINT32>(boost::filesystem::file_size(path));
        return nSize;
    }

    /**
     * Description: Enum file list for a directory
     * Parameter const std::string & strDirectory       [in]: directory path, for example: "./logs"
     * Parameter std::vector<std::string> & vFileName   [out]: file name
     * Returns void
     */
    static inline void EnumFileList(const std::string& strDirectory, std::vector<std::string>& vFileName)
    {
        vFileName.clear();
        boost::filesystem::path path(strDirectory);
        boost::system::error_code stErrorCode;
        bool result = boost::filesystem::is_directory(path, stErrorCode);
        if (!result)
        {
            return;
        }
        boost::filesystem::directory_iterator itEnd;
        for (boost::filesystem::directory_iterator itDir(path); itDir != itEnd; ++itDir)
        {
            boost::filesystem::path stCurPath = *itDir;
            result = boost::filesystem::is_directory(stCurPath, stErrorCode);
            if (result)
            {
                continue;
            }
            std::string strFileName = stCurPath.string();
            size_t nLength = strFileName.length();
            for (size_t nIndex = 0; nIndex < nLength; ++nIndex)
            {
                if ('\\' == strFileName[nIndex])
                {
                    strFileName[nIndex] = '/';
                }
            }
            size_t nPos = strFileName.rfind('/');
            if (nPos != std::string::npos )
            {
                strFileName = strFileName.substr(nPos + 1, strFileName.size());
                vFileName.push_back(strFileName);
            }
            else
            {
                vFileName.push_back(strFileName);
            }
            
        }
    }

    /**
     * Description: Get last write time of file.
     * Parameter const std::string& strFilePath[in]: file path
     * Returns time_t: last write time, 0 if failed!
     */
    static time_t FileLastWriteTime(const std::string& strFilePath)
    {
        boost::filesystem::path path(strFilePath.c_str());
        boost::system::error_code stErrorCode;
        time_t nLastWriteTime = boost::filesystem::last_write_time(path, stErrorCode);
        return nLastWriteTime;
    }
};

#endif // _FILE_SYSTEM_UTIL_HPP_
