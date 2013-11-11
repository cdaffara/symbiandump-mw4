/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CHttpCacheObserver
*
*/

// INCLUDE FILES
#include "HttpCacheObserver.h"
#include "HttpCacheHandler.h"


// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheObserver::CHttpCacheObserver
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheObserver::CHttpCacheObserver(CHttpCacheHandler* aHttpCacheHandler)
    : CActive( CActive::EPriorityIdle ),
      iHttpCacheHandler(aHttpCacheHandler)
    {
    CActiveScheduler::Add(this);
    }

// -----------------------------------------------------------------------------
// CHttpCacheObserver::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheObserver::ConstructL(const HBufC* aDirectory, const HBufC* aIndexFile)
    {
    User::LeaveIfError(iFsSession.Connect());
    iFileName = HBufC::NewL(aDirectory->Length() + aIndexFile->Length());
    iFileName->Des().Copy(*aDirectory);
    iFileName->Des().Append(*aIndexFile);
    }

// -----------------------------------------------------------------------------
// CHttpCacheObserver::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpCacheObserver* CHttpCacheObserver::NewL( const HBufC* aDirectory, const HBufC* aIndexFile,
                                                         CHttpCacheHandler* aHttpCacheHandler)
    {
    CHttpCacheObserver* self = new( ELeave ) CHttpCacheObserver(aHttpCacheHandler);
    CleanupStack::PushL( self );
    self->ConstructL(aDirectory, aIndexFile);
    CleanupStack::Pop();
    return self;
    }

// Destructor
CHttpCacheObserver::~CHttpCacheObserver()
    {
    Cancel();
    delete iFileName;
    iFsSession.Close();
    }

void CHttpCacheObserver::Queue()
    {
    if(IsActive())
        {
        User::WaitForAnyRequest();  // consume signal
        }
    else
        {
        SetActive();
        }

    // queue next notification
    iFsSession.NotifyChange(ENotifyWrite, iStatus, *iFileName);
    }



// -----------------------------------------------------------------------------
// CHttpCacheObserver::RunL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheObserver::RunL()
    {
    Queue();
    iHttpCacheHandler->UpdateLookupTable();
    }

// -----------------------------------------------------------------------------
// CHttpCacheObserver::RunError
//
// -----------------------------------------------------------------------------
//
TInt CHttpCacheObserver::RunError(TInt /*aError*/)
    {
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CHttpCacheObserver::DoCancel
//
// -----------------------------------------------------------------------------
//
void CHttpCacheObserver::DoCancel()
    {
    iFsSession.NotifyChangeCancel(iStatus);
    }

// -----------------------------------------------------------------------------
// CHttpCacheObserver::StartObserver
//
// -----------------------------------------------------------------------------
//
void CHttpCacheObserver::StartObserver()
    {
    if (!IsActive())
        {
        iFsSession.NotifyChange(ENotifyWrite, iStatus, *iFileName);
        SetActive();
        }
    }


//  End of File
