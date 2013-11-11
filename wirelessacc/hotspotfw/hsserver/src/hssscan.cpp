/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implementation of Active Object
*
*/



// INCLUDE FILES
#include <hssscan.h>
#include <hotspotsession.h>

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHssScan
// -----------------------------------------------------------------------------
CHssScan::CHssScan( CWlanMgmtClient* aWlanMgmtClient, CHotSpotSession& aCallback) :
    CActive( CActive::EPriorityStandard ),
    iCallback( aCallback ),
    iMgmtClient( aWlanMgmtClient ),
    iScanInfo( NULL )
    {
    TRAP_IGNORE( iScanInfo = CWlanScanInfo::NewL() );
    // Possible leave error neglected!?
    CActiveScheduler::Add( this );        
    }
    
// -----------------------------------------------------------------------------
// ~CHssScan
// -----------------------------------------------------------------------------
CHssScan::~CHssScan()
    {
    Cancel();
    delete iScanInfo;
    }

// -----------------------------------------------------------------------------
// RunL
// -----------------------------------------------------------------------------
void CHssScan::RunL()
    {
        iCallback.ScanCompleteL( iStatus , *iScanInfo );
    }

// -----------------------------------------------------------------------------
// RunError 
// -----------------------------------------------------------------------------
TInt CHssScan::RunError( TInt /* aError */ )
    {

    return KErrNone;
    }

// -----------------------------------------------------------------------------
// DoCancel
// -----------------------------------------------------------------------------
void CHssScan::DoCancel()
    {
    
    }

// -----------------------------------------------------------------------------
// IssueRequest
// -----------------------------------------------------------------------------
void CHssScan::IssueRequest()
    {
        iMgmtClient->GetScanResults( iStatus, *iScanInfo );
	    SetActive();
    }
    
    
// end of file
