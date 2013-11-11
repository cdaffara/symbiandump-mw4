/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      Panic function.
*      
*
*/


// INCLUDE FILES

#include <e32std.h>
#include "RoapAppPanic.h"
#include "RoapAppLogger.h"

// ================= LOCAL FUNCTIONS =======================

void RoapAppPanic( TRoapAppPanicReason aReason )
    {
    _LIT( KRoapApp, "RoapApp" );
    CLOG(( 0, _L("*** RoapAppPanic(%d) ***"), aReason ));
    User::Panic( KRoapApp, aReason );
    }
