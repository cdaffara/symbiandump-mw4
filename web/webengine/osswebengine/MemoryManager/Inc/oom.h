/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
*
*/

#ifndef __OOM_H__
#define __OOM_H__

#ifdef __OOM__

#include "MemoryManager.h"

/*
#include "ObjectBase.h"

// base class with overloaded new/delete operator
#define OOM_NEW_DELETE  public: \
            static TAny* operator new( TUint size )   { return CObjectBase::operator new(size); }\
            static void operator delete( TAny* ptr )  { CObjectBase::operator delete(ptr); }

#define OOM_NEW_DELETE_WITH_ARRAY public: \
            static TAny* operator new( TUint size )   { return CObjectBase::operator new(size); }\
            static void  operator delete( TAny* ptr ) { CObjectBase::operator delete(ptr); } \
            static TAny* operator new[]( TUint size ) { return CObjectBase::operator new[](size); }\
            static void  operator delete[]( TAny* ptr ) { CObjectBase::operator delete[](ptr); }
*/

#ifdef OOM_LOGGING

// logging the function memory usage, the function shouldn't return between the pair
#define OOM_CRITICAL_PATH_BEGIN(func, sz) FunctionLoggerWrapper* __oom_function_logger_wrapper = new FunctionLoggerWrapper(TPtrC8( (const TUint8*)func, strlen(func) ), TPtrC8( (const TUint8*)__FILE__, strlen(__FILE__) ), __LINE__, sz ); {  
#define OOM_CRITICAL_PATH_END         } delete __oom_function_logger_wrapper;

#else // OOM_LOGGING

#define OOM_CRITICAL_PATH_BEGIN(func, sz)
#define OOM_CRITICAL_PATH_END

#endif

// precheck and postcheck macros, the function shouldn't return between precheck and postcheck pair.
// usage:
//      OOM_PRE_CHECK( 100000, "MemoryConsumer::consume" )
//      .... your code without return.
//      OOM_POST_CHECK_FAILED { return_with_oom; }
//      .... your normal return.
#define OOM_PRE_CHECK(s, m, c)  { _LIT8(__temp, c); if( MemoryManager::PreCheck( s, m, __temp ) ) { 
#define OOM_POST_CHECK_FAILED(__r)    } } if( MemoryManager::PostCheck() != ENoOOM ) { __r;}

#else // __OOM__

// empty macros
#define OOM_NEW_DELETE
#define OOM_NEW_DELETE_WITH_ARRAY
#define OOM_CRITICAL_PATH_BEGIN(func, sz)
#define OOM_CRITICAL_PATH_END
#define OOM_PRE_CHECK( s, m, c )
#define OOM_POST_CHECK_FAILED(__r)

#endif

#endif  //__OOM_H__
