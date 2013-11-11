/*
 * Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description:  Methods for USB UI notifiers
 *
 */

// INCLUDE FILES
#include <ecom/ecom.h>
#include <ecom/implementationproxy.h>
#include <eiknotapi.h>

#include "usbuincableconnectednotifier.h"
#include "usbuinqueriesnotifiermdrv.h"
#include "usbuinotifdebug.h"
#include "usbuinotifotgwarning.h"    
#include "usbuinotifotgerror.h"    
#include "usbuinotifmsmmerror.h" 

// CONSTANTS
const TInt KUSBUINotifierArrayIncrement = 5;

// ================= EXPORTED FUNCTIONS =======================================
// ----------------------------------------------------------------------------
//
// Instantiate notifiers
//
// ----------------------------------------------------------------------------

void CreateUSBUINotifiersL(CArrayPtrFlat<MEikSrvNotifierBase2>* aNotifiers)
    {
    FLOG(_L("[USBUINOTIF]\t CreateUSBUINotifiersL"));

    CUSBUICableConnectedNotifier* cableConnectedNotifier =
        CUSBUICableConnectedNotifier::NewL();
    CleanupStack::PushL( cableConnectedNotifier );
    aNotifiers->AppendL( cableConnectedNotifier );
    CleanupStack::Pop( cableConnectedNotifier );

    CUSBUIQueriesNotifier* queriesNotifier = CUSBUIQueriesNotifier::NewL();
    CleanupStack::PushL( queriesNotifier );
    aNotifiers->AppendL( queriesNotifier );
    CleanupStack::Pop( queriesNotifier );

    CUsbUiNotifOtgWarning* generalNote = CUsbUiNotifOtgWarning::NewL();
    CleanupStack::PushL( generalNote );
    aNotifiers->AppendL( generalNote );
    CleanupStack::Pop( generalNote );

    CUsbUiNotifOtgError* generalQuery = CUsbUiNotifOtgError::NewL();
    CleanupStack::PushL( generalQuery );
    aNotifiers->AppendL( generalQuery );
    CleanupStack::Pop( generalQuery );
    
    CUsbUiNotifMSMMError* msmmerrorQuery = CUsbUiNotifMSMMError::NewL();
    CleanupStack::PushL( msmmerrorQuery );
    aNotifiers->AppendL( msmmerrorQuery );
    CleanupStack::Pop( msmmerrorQuery );

    FLOG(_L("[USBUINOTIF]\t CreateUSBUINotifiersL completed"));
    }

// ----------------------------------------------------------------------------
// Creates a notifiers array.
// ----------------------------------------------------------------------------
CArrayPtr<MEikSrvNotifierBase2>* NotifierArray()
    {
    FLOG(_L("[USBUINOTIF]\t NotifierArray"));

    CArrayPtrFlat<MEikSrvNotifierBase2>* notifiers = NULL;
    TRAPD( err, notifiers =
            new (ELeave)CArrayPtrFlat<MEikSrvNotifierBase2>(
                    KUSBUINotifierArrayIncrement ));

    if (err == KErrNone)
        {
        if (notifiers)
            {
            TRAPD( errCreate, CreateUSBUINotifiersL( notifiers ));
            if (errCreate)
                {
                TInt count = notifiers->Count();
                while (count--)
                    {
                    (*notifiers)[count]->Release();
                    }
                delete notifiers;
                notifiers = NULL;
                }

            FLOG(_L("[USBUINOTIF]\t NotifierArray completed"));
            }
        }
    else
        {
        FLOG(_L("[USBUINOTIF]\t NotifierArray Failed!!!"));
        }

    return notifiers;
    }

// ----------------------------------------------------------------------------
// ECOM 
// ----------------------------------------------------------------------------

const TImplementationProxy ImplementationTable[] =
{
    IMPLEMENTATION_PROXY_ENTRY( 0x10281F23, NotifierArray )
};

EXPORT_C const TImplementationProxy* ImplementationGroupProxy(
        TInt& aTableCount)
    {
    FLOG(_L("[USBUINOTIF]\t ImplementationGroupProxy"));
    aTableCount = sizeof(ImplementationTable) / sizeof(TImplementationProxy);
    return ImplementationTable;
    }

//  End of File
