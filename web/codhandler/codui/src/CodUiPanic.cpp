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
* Description:  COD Viewer Panic.   
*
*/


// INCLUDE FILES

#include <e32std.h>
#include "CodUiPanic.h"

// ================= LOCAL FUNCTIONS =======================

void CodUiPanic( TCodUiPanicReason aReason )
    {
    _LIT( KCodUi, "CodUi" );
    User::Panic( KCodUi, aReason );
    }
