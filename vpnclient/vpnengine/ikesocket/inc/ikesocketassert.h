/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Special assert macros for IKE socket
*
*/


#ifndef IKESOCKETASSERT_H
#define IKESOCKETASSERT_H

#include "ikedebug.h"

#ifdef _DEBUG

#define IKESOCKET_ASSERT(cond) if(!(cond)){ DEBUG_LOG(_L("ASSERTION FAILED")); DEBUG_LOG1(_L("%s, "), __FILE__); DEBUG_LOG1(_L("%d"), __LINE__); User::Invariant();}
#define IKESOCKET_INVARIANT() DEBUG_LOG(_L("ASSERTION FAILED")); DEBUG_LOG1(_L("%s, "), __FILE__); DEBUG_LOG1(_L("%d"), __LINE__); User::Invariant()

#else

#define IKESOCKET_ASSERT(cond) 
#define IKESOCKET_INVARIANT() 

#endif

#endif // IKESOCKETASSERT_H
