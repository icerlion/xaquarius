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
 * FileName: ObjectPool.hpp
 * 
 * Purpose:  Implement a template class for allocate object
 *           in memory pool. THIS CLASS IS NOT THEAD-SAFE!
 *           If you want use this class, just as following:
 *           1. Define your concrete class, which must be has member function:
 *               void ResetPoolObject();
 *           2. Define the singleton of ObjectPool:
 *               typedef CObjectPool<CConcreteClass> CConcreteClassPool;
 *           3. Get a new instance from pool:
 *               CConcreteClass* pInstance = CConcreteClassPool::AcquireObject();
 *           4. Recycle instance:
 *               CConcreteClassPool::RecycleObject(pInstance);
 *
 * Author:   Jhon Frank(xaquariuscore@163.com)
 * 
 * Modify:   2017/2/22 15:40
===================================================================+*/
#ifndef _OBJECT_POOL_H_
#define _OBJECT_POOL_H_
#include "AquariusCore/Util/TypeReDef/TypeReDef.hpp"
#include "boost/thread/recursive_mutex.hpp"
#include "AquariusCore/Logger/AquariusLogger.h"

#ifdef WIN32
#pragma warning(push)
#pragma warning(disable: 4127)
#endif // WIN32

// Abstract class of IObjInPool.
class IObjInPool
{
public:
    // Constructor
    IObjInPool()
        :m_bActive(false)
    {
    }
    
    // Destructor
    virtual ~IObjInPool()
    {
    }
    
    // Set active flag
    inline void SetActiveFlag(bool flag)
    {
        m_bActive = flag;
    }

    // Get active flag
    inline bool GetActiveFlag()
    {
        return m_bActive;
    }

    // The derive class should implement this function, 
    // it will be called when acquire/recycle to reset object
    virtual void ResetPoolObject() = 0;

private:
    // active flag
    bool m_bActive;
};

// ElementType: element type
// ElementCount: object count in pool
// ThreadSafe: false as default
template<typename ElementType, AUINT32 ElementCount, bool ThreadSafe = false>
class CObjectPool
{
public:
    // Acquire an object from object pool.
    static inline ElementType* AcquireObject()
    {
        return GetSingleton().ThreadSafeAcquireObject();
    }

    // Recycle an object, the element must be acquired by AcquireObject.
    static inline bool RecycleObject(ElementType* pElement)
    {
        return GetSingleton().ThreadSafeRecycleObject(pElement);
    }

    // Get available object count
    static inline void ShowAvailableCount(const char* pszClassTypeName)
    {
        CAquariusLogger::Logger(LL_DEBUG, "ClassType: [%s], Available: [%d]", pszClassTypeName, GetSingleton().m_nObjCursor);
    }

private:
    // emphasize the following members are private
    CObjectPool(const CObjectPool&) = delete;
    const CObjectPool& operator=(const CObjectPool&) = delete;

private:
    // Thread safe Acquire an object
    inline ElementType* ThreadSafeAcquireObject()
    {
        if (ThreadSafe)
        {
            // Pre-Condition: lock the pool
            boost::unique_lock<boost::recursive_mutex> scoped_lock(m_mutex_pool);
            return AcquireObjectImplement();
        }
        return AcquireObjectImplement();
    }

    // Thread safe Recycle an object
    inline bool ThreadSafeRecycleObject(ElementType* pElement)
    {
        if (ThreadSafe)
        {
            // Pre-Condition: lock the pool
            boost::unique_lock<boost::recursive_mutex> scoped_lock(m_mutex_pool);
            return RecycleObjectImplement(pElement);
        }
        return RecycleObjectImplement(pElement);
    }
    
    // Implement the function of AcquireObject
    inline ElementType* AcquireObjectImplement()
    {
        // 1. Check the cursor
        if (m_nObjCursor <= 0)
        {
            return nullptr;
        }
        if (m_nObjCursor >= ElementCount)
        {
            // Can't happen!
            CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: m_nObjCursor >= ElementCount, m_nObjCursor: [%d], ElementCount: [%d]", m_nObjCursor, ElementCount);
            return nullptr;
        }
        // 2. Get a pointer
        ElementType* pResult = m_arrayElementPointer[m_nObjCursor];
        if (nullptr == pResult)
        {
            // Can't happen!
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pResult, m_nObjCursor: [%d], ElementCount: [%d]", m_nObjCursor, ElementCount);
            return pResult; 
        }
        // 3. Init the object
        if (pResult->GetActiveFlag())
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Result pointer is active, impossible");
        }
        pResult->ResetPoolObject();
        pResult->SetActiveFlag(true);
        // 4. Dec the cursor
        --m_nObjCursor;
        return pResult;
    }

    // Implement the function of RecycleObject
    inline bool RecycleObjectImplement(ElementType* pElement)
    {
        // 1. Check input parameter
        if (nullptr == pElement)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "nullptr == pElement, m_nObjCursor: [%d], ElementCount: [%d]", m_nObjCursor, ElementCount);
            return false;
        }
        // 2. Avoid re-called
        if (!pElement->GetActiveFlag())
        {
            return false;
        }
        // 3. Check the cursor
        if (m_nObjCursor >= ElementCount)
        {
            // Can't happen!
            CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: m_nObjCursor >= ElementCount, m_nObjCursor: [%d], ElementCount: [%d]", m_nObjCursor, ElementCount);
            return false;
        }
        // 4. Check the address of the input pointer
        void* pCurAddress = (void*)pElement;
        if (pCurAddress < m_pBeginAddress || pCurAddress > m_pEndAddress)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: pCurAddress < m_pBeginAddress || pCurAddress > m_pEndAddress, m_nObjCursor: [%d], ElementCount: [%d]", m_nObjCursor, ElementCount);
            return false;
        }
        // 5. Inc the cursor
        ++m_nObjCursor;
        // 6. Recycle the object pointer
        m_arrayElementPointer[m_nObjCursor] = pElement;
        pElement->SetActiveFlag(false);
        pElement->ResetPoolObject();
        return true;
    }

    // Get the singleton instance
    inline static CObjectPool& GetSingleton()
    {
        static CObjectPool _instance;
        return _instance;
    }

    // Constructor
    CObjectPool(void)
        :m_pBuff(nullptr),
        m_pBeginAddress(nullptr),
        m_pEndAddress(nullptr),
        m_nObjCursor(ElementCount - 1)
    {
        InitPool();
    }

    // Destructor
    virtual ~CObjectPool(void)
    {
        DestroyPool();
    }

    // Init pool
    inline void InitPool()
    {
        // 1.Init element pointer pool
        memset(m_arrayElementPointer, 0, sizeof(m_arrayElementPointer));
        // 2. Allocate enough memory pool for the object
        m_pBuff = new char[sizeof(ElementType) * ElementCount];
        // 3. Calc the boundary of the pool
        m_pBeginAddress = m_pBuff;
        m_pEndAddress = m_pBuff + sizeof(ElementType) * (ElementCount - 1);
        // 4. Constructor object in the memory pool
        AUINT32 nOffset = 0;
        for (AUINT32 i = 0; i < ElementCount; ++i)
        {
            ElementType* pElement = new (m_pBuff + nOffset) ElementType();
            pElement->SetActiveFlag(false);
            m_arrayElementPointer[i] = pElement;
            nOffset += sizeof(ElementType);
        }
    }

    // Destroy pool
    inline void DestroyPool()
    {
        // 1. Check the cursor. If the m_nObjCursor is less then ElementCount, some object has not be recycled yet.
        if (m_nObjCursor != ElementCount - 1)
        {
            CAquariusLogger::Logger(LL_ERROR_FUN, "Impossible: m_nObjCursor != ElementCount - 1, m_nObjCursor: [%d], ElementCount: [%d]", m_nObjCursor, ElementCount);
        }
        // 2. Loop the object list to destroy every object in pool
        for (AUINT32 i = 0; i < ElementCount; ++i)
        {
            ElementType* pElement = m_arrayElementPointer[i];
            pElement->~ElementType();
        }
        // 3. Free memory pool
        delete[] m_pBuff;
        m_pBuff = nullptr;
    }

private:
    // Memory pool buff
    char* m_pBuff;
    // Lower memory boundary
    void* m_pBeginAddress;
    // Upper memory boundary
    void* m_pEndAddress;
    // Object pool cache
    typedef ElementType* PElementType;
    PElementType m_arrayElementPointer[ElementCount];
    // The current available object index in pool
    int m_nObjCursor;
    // pool mutex
    boost::recursive_mutex m_mutex_pool;
};
#ifdef WIN32
#pragma warning(pop)
#endif // WIN32

#endif // _OBJECT_POOL_H_
