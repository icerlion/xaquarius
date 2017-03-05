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
 * FileName: TypeReDef.hpp
 * 
 * Purpose:  Re-Define basic type
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:42
===================================================================+*/
#ifndef _TYPE_RE_DEF_H_
#define _TYPE_RE_DEF_H_

#include <map>
#include <set>
#include <list>
#include <string>
#include "boost/asio.hpp"

// typedef for basic type
typedef unsigned char AINT8;
typedef unsigned char AUINT8;
typedef short AINT16;
typedef unsigned short AUINT16;
typedef int AINT32;
typedef unsigned int AUINT32;
typedef int64_t AINT64;
typedef uint64_t AUINT64;
typedef std::map<std::string, std::string> MapKeyValue;

// Define Max msg length, unit: byte
const static AUINT32 CONST_MAX_MSG_LENGTH = 512;

const static unsigned int INVALID_AUINT32 = 0xffffffff;
const static AUINT16 INVALID_AUINT16 = 0xffff;

// Make var to string
#define A_STRINGIFY(x) #x

// Define Array size 
#define A_ARRAY_SIZE(_array) ((int)(sizeof(_array)) / (int)(sizeof(*_array)))

#ifdef LINUX
#define a_sprintf sprintf
#else
#define a_sprintf sprintf_s
#endif

#endif // _TYPE_RE_DEF_H_
