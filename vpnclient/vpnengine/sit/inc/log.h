/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Logging utility.
*
*/



#if !defined(__LOG_H__)
#define __LOG_H__

_LIT(KLogFile,"sit.txt");

#include "logcommon.h"

// Ádditional debugging macros

#ifdef _DEBUG
#define DEB(a)      a
#else
#define DEB(a)
#endif

#endif // __LOG_H__
