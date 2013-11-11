/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* This is a source file for esock library stub functions
*/

#include <es_sock.h>

#ifdef __WINS__

// ----------------------------------------------
// Stub functions for esock library, used in UT
// ----------------------------------------------

#include "wlanqtutilstestcontext.h"

extern WlanQtUtilsTestContext testContext;

/**
 * Returns value from test context.
 * 
 * TConnPref value cannot be verified easily because the real value passed to the function is
 * TCommDbConnPref. Dynamic cast from TConnPref to TCommDbConnPref does not work because
 * TConnPref does not have virtual functions which means that type information for the 
 * class hierarchy is not created by the compiler.
 */
void RConnection::Start(class TConnPref &, class TRequestStatus &aStatus)
{
    User::RequestComplete(&aStatus, testContext.esock_.startRetValue_);
}

#endif // __WINS__
