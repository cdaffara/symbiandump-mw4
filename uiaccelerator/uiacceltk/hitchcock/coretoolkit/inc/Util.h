/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

#ifndef UTIL_H
#define UTIL_H



//#define ENABLE_TRACE
#undef ENABLE_TRACE

#if (defined ENABLE_TRACE) && (!defined __SYMBIAN32__)
	#include <iostream>
#endif

#if (defined ENABLE_TRACE) && (defined __SYMBIAN32__)
#include <e32def.h>
#include <e32debug.h>
#endif

#ifndef SG_ASSERT
	#include <assert.h>
	#define SG_ASSERT assert
#endif

namespace shadergen
{

#ifdef _DEBUG
	#define DEBUG
#endif

/* Compile-time assertion */
#define SG_CT_MAKE_NAME(f, l) sg_ct_assert_failure_##f##l
#define SG_CT_UNIQUE_NAME SG_CT_MAKE_NAME(__FILE__, __LINE__)

#define SG_CT_ASSERT(cond) typedef char SG_CT_UNIQUE_NAME[(cond) ? 1 : -1]

#define SG_LENGTH_OF_ARRAY(arr) (sizeof(arr) / sizeof((arr)[0]))


#if (defined ENABLE_TRACE) && (defined __SYMBIAN32__)

	#define MY_TRACE(str) RDebug::Print(_L(str))
	#define RAW_TRACE(str) RDebug::Printf(str)
	#define TRACE_FUNC_ENTRY() (RAW_TRACE(__FUNCTION__))
#elif (defined ENABLE_TRACE) && (!defined __SYMBIAN32__)
	#define MY_TRACE(str)  std::cout << (str) << '\n'
	#define RAW_TRACE(str) std::cout << (str) << '\n'
	#define TRACE_FUNC_ENTRY() (MY_TRACE(__FUNCTION__))
#elif !defined ENABLE_TRACE
	#define MY_TRACE(str)
	#define RAW_TRACE(str)
	#define TRACE_FUNC_ENTRY()
#endif



template<class T> inline bool SG_IN_RANGE (const T& x, const T& mn, const T& mx)
{
	return x <= mx && x >= mn;
}


typedef struct _Vec4
{
	float x, y, z, w;
} Vec4;

SG_CT_ASSERT(sizeof(Vec4) == 4 * sizeof(float));

} /* namespace shadergen */

#endif /* UTIL_H */
