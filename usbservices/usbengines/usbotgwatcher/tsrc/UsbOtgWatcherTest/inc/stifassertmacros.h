/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#ifndef STIFASSERT_MACRO_H
#define STIFASSERT_MACRO_H

#ifdef __STIF_ASSERT_SHARED
#undef __STIF_ASSERT_SHARED
/*********************************************************************************
 * Assert Macros
 *********************************************************************************/
#define __STIF_ASSERT_SHARED( aFunction, aMessage ) \
	if(!aFunction) \
		{ \
		iLog->Log( aMessage, __STIF_DBG_FILE__, __LINE__ );\
		iResult.SetResult( KErrGeneral, _L("Testcase failed"));\
		return;\
		}
#endif

#ifdef __STIF_ASSERT_SHARED_DESC
#undef __STIF_ASSERT_SHARED_DESC
#define __STIF_ASSERT_SHARED_DESC( aFunction, aMessage, aDesc ) \
	if(!aFunction) \
		{ \
		iLog->Log( aMessage, __STIF_DBG_FILE__, __LINE__ );\
		iResult.SetResult( KErrGeneral, aDesc );\
        return;\
		} \
	else \
		{ \
		iResult.SetResult( KErrNone, aDesc ); \
		}
#endif		

#ifdef STIF_ASSERT_PANIC_DESC
#undef STIF_ASSERT_PANIC_DESC

#define STIF_ASSERT_PANIC_DESC( aPanicCode, aStatement, aDescription ) \
	{ \
	TestModuleIf().SetExitReason( CTestModuleIf::EPanic, aPanicCode ); \
	iResult.SetResult(KErrNone, aDescription); \
	aStatement; \
	}
#endif

#endif
