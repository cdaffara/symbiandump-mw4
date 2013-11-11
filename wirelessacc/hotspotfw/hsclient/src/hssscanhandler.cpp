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
* Description:   Implementation of iapsettings
*
*/



// INCLUDE FILES

#include "hssscanhandler.h"
#include "hotspotclientserver.h"
#include <wlanmgmtclient.h> 
#include <wlanscaninfo.h>
#include "hssscaninfo.h"
#include "hssscanlist.h"
#include "am_debug.h"

// CONSTS

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHssScanHandler
// -----------------------------------------------------------------------------
//
CHssScanHandler::CHssScanHandler( )
	{
	iMgmtClient=NULL;
	iScanInfo=NULL;
  DEBUG("CHssScanHandler::CHssScanHandler");
  }

// -----------------------------------------------------------------------------
// ConstructL
// -----------------------------------------------------------------------------
//
void CHssScanHandler::ConstructL()
    {
    DEBUG("CHssScanHandler::ConstructL()");
    iMgmtClient = CWlanMgmtClient::NewL();
    iScanInfo = CWlanScanInfo::NewL();
    }

// -----------------------------------------------------------------------------
// NewL
// -----------------------------------------------------------------------------
//
CHssScanHandler* CHssScanHandler::NewL()
    {
    CHssScanHandler* self = new( ELeave ) CHssScanHandler();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// -----------------------------------------------------------------------------
// ~CHssScanHandler
// -----------------------------------------------------------------------------
//
CHssScanHandler::~CHssScanHandler()
	{
	DEBUG("CHssScanHandler::~CHssScanHandler()");
    delete iMgmtClient;
    delete iScanInfo;
    }

// -----------------------------------------------------------------------------
// ScanRequest
// -----------------------------------------------------------------------------
//
TInt CHssScanHandler::ScanRequest( HssScanList& aResults  )
    {
    DEBUG("CHssScanHandler::ScanRequest()");
    TInt ret( KErrNone );
    ret = iMgmtClient->GetScanResults( *iScanInfo );
    
    TUint count( NULL );

    aResults.ClearAll();
    
   	for ( iScanInfo->First(); !iScanInfo->IsDone(); iScanInfo->Next(), ++count )
    	{
		aResults.Append( iScanInfo->Size(),iScanInfo->Current() );
    	}
    DEBUG1("CHssScanHandler::ScanRequest() DONE ret: %d", ret);	
    return ret;
    }


// -----------------------------------------------------------------------------
// GetConnectionBssid
// -----------------------------------------------------------------------------
//
TInt CHssScanHandler::GetConnectionBssid( THssBssid& aBssId )
    {
    DEBUG("CHssScanHandler::GetConnectionBssid");
    return iMgmtClient->GetConnectionBssid( aBssId );
    }

// -----------------------------------------------------------------------------
// AddIapSsidList
// -----------------------------------------------------------------------------
//
TInt CHssScanHandler::AddIapSsidList(
        TUint aIapId,
        const CArrayFixFlat<THssSsid>& aSsidList )
    {
    DEBUG("CHssScanHandler::AddIapSsidList");
    return iMgmtClient->AddIapSsidList( aIapId, aSsidList );
    }

// End of File
