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
 * FileName: ISingleton.hpp
 * 
 * Purpose:  Support singleton model, not thread safe
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:41
===================================================================+*/
#ifndef _I_SINGLETON_HPP_
#define _I_SINGLETON_HPP_

template<typename ConcreteType>
class ISingleton
{
public:
    // Static method, get singleton instance
    static ConcreteType& GetSingleton()
    {
        static ConcreteType _instance;
        return _instance;
    }


protected:
    // Constructor
    ISingleton()
    {
    }

    // Destructor
    virtual ~ISingleton()
    {
    }

    // Disable the following function
    ISingleton(const ISingleton& stCopy) = delete;
    void operator=(const ISingleton& stCopy) = delete;
};

#endif // _I_SINGLETON_HPP_