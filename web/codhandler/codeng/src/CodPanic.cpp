/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*      COD Handler Panic.   
*      
*
*/


// INCLUDE FILES

#include <e32std.h>
#include "CodPanic.h"
#include "CodLogger.h"

// ================= LOCAL FUNCTIONS =======================

// ---------------------------------------------------------
// CodPanic()
// ---------------------------------------------------------
//
void CodPanic( TCodHandlerPanicReason aReason )
    {
    _LIT( KCodHandler, "CodHandler" );
    CLOG(( ELogAll, 0, _L("*** CodPanic(%d) ***"), aReason ));
    User::Panic( KCodHandler, aReason );
    }
