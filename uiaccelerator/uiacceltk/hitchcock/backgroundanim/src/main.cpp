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

#include <stdio.h>
#include <e32def.h>

#include "bganimhost.h"

int main(int /*argc*/, char** /*argv*/)
    {
    __UHEAP_MARK;
    TBool fail = EFalse;
    CBgAnimHost* host = new CBgAnimHost();
    if (host)
        {
        TRAPD(err, host->ConstructL());
        if (!err)
            {
            TRAP_IGNORE(host->ExecuteL());
            }
        else
            {
            fail = ETrue;
            }
        delete host;
        }
    else
        {
        fail = ETrue;
        }
        
    if (fail)
        {
        // failed to construct the animation properly
        // restore skin state
        // TODO: implement actual restore on failure
        }
     
    __UHEAP_MARKEND;
    return 0;
    }

