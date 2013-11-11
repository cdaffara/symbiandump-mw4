/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Scan info
*
*/



// INCLUDE FILES
#include <e32std.h>
#include <implementationproxy.h>
#include "am_debug.h"
#include "hssmgmtscaninfoimpl.h"
#include "hotspotclientserver.h"


// ================= MEMBER FUNCTIONS =======================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CHssMgmtScanInfoImpl::CHssMgmtScanInfoImpl()
    {
    iScanInfo=NULL;
    DEBUG("CHssMgmtScanInfoImpl::CHssMgmtScanInfoImpl()");
    }

// Symbian 2nd phase constructor can leave.
void CHssMgmtScanInfoImpl::ConstructL()
    {
    DEBUG( "CHssMgmtScanInfoImpl::ConstructL()" );

    iScanInfo = new(ELeave) HssScanInfo( iScanList );
    }

// Static constructor.
CHssMgmtScanInfoImpl* CHssMgmtScanInfoImpl::NewL()
    {
    DEBUG( "CHssMgmtScanInfoImpl::NewL()" );

    CHssMgmtScanInfoImpl* self = new (ELeave) CHssMgmtScanInfoImpl;
    CleanupStack::PushL( self );    
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }
    
// Destructor
CHssMgmtScanInfoImpl::~CHssMgmtScanInfoImpl()
    {
    DEBUG( "CHssMgmtScanInfoImpl::~CHssMgmtScanInfoImpl()" );
    delete iScanInfo;
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::Size
// ---------------------------------------------------------
//
TUint16 CHssMgmtScanInfoImpl::Size() const
    {
      return iScanInfo->Size();
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::First
// ---------------------------------------------------------
//
const THssMgmtScanInfoFrame* CHssMgmtScanInfoImpl::First()
    {
      return iScanInfo->First();
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::Next
// ---------------------------------------------------------
//
const THssMgmtScanInfoFrame* CHssMgmtScanInfoImpl::Next()
    {
      return iScanInfo->Next();
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::Current
// ---------------------------------------------------------
//
const THssMgmtScanInfoFrame* CHssMgmtScanInfoImpl::Current() const
    {
      return iScanInfo->Current();
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::IsDone
// ---------------------------------------------------------
//
TBool CHssMgmtScanInfoImpl::IsDone() const
    {
      return iScanInfo->IsDone();
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::RXLevel
// ---------------------------------------------------------
//
TUint8 CHssMgmtScanInfoImpl::RXLevel() const
    {
    TUint8 rssiValue = 110 - ( iScanInfo->RXLevel() / 2 );
    DEBUG2( "CHssMgmtScanInfoImpl::RXLevel(): (RCP, RSS) == (%u, %u )", iScanInfo->RXLevel(), rssiValue );
    return rssiValue;
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::Bssid
// ---------------------------------------------------------
//
void CHssMgmtScanInfoImpl::Bssid( THssBssid& aBssid ) const
    {
    TUint8 bssid[KHssMacAddressLength];
    iScanInfo->BSSID( bssid );
    aBssid.Copy( &bssid[0], KHssMacAddressLength );
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::BeaconInterval
// ---------------------------------------------------------
//
TUint16 CHssMgmtScanInfoImpl::BeaconInterval() const
    {
    return iScanInfo->BeaconInterval();
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::Capability
// ---------------------------------------------------------
//
TUint16 CHssMgmtScanInfoImpl::Capability() const
    {
    return iScanInfo->Capability();
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::SecurityMode
// Status : Draft
// ---------------------------------------------------------
//
THssConnectionSecurityMode CHssMgmtScanInfoImpl::SecurityMode() const
    {
    THssConnectionSecurityMode securityMode;
    
    switch( iScanInfo->SecurityMode() )
        {
		case EHssSecurityWep:
			securityMode = EHssConnectionSecurityWep;
			break;
		case EHssSecurity802d1x:
			securityMode = EHssConnectionSecurity802d1x;
			break;
		case EHssSecurityWpa: // Fallthrough on purpose
		case EHssSecurityWpa2:
			securityMode = EHssConnectionSecurityWpa;
			break;
		case EHssSecurityWpaPsk: // Fallthrough on purpose
		case EHssSecurityWpa2Psk:
			securityMode = EHssConnectionSecurityWpaPsk;
			break;
		default:
			securityMode = EHssConnectionSecurityOpen;
			break;
        };
        
    return securityMode;
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::InformationElement
// ---------------------------------------------------------
//
TInt CHssMgmtScanInfoImpl::InformationElement( TUint8 aIE, 
                                            TUint8& aLength, 
                                            const TUint8** aData )
    {
      return iScanInfo->InformationElement( aIE, aLength, aData );
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::WpaIE
// ---------------------------------------------------------
//
TInt CHssMgmtScanInfoImpl::WpaIE( TUint8& aLength, 
                               const TUint8** aData )
    {
      return iScanInfo->WpaIE( aLength, aData );
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::FirstIE
// ---------------------------------------------------------
//
TInt CHssMgmtScanInfoImpl::FirstIE( TUint8& aIE, 
                                 TUint8& aLength, 
                                 const TUint8** aData )
    {
      return iScanInfo->FirstIE( aIE, aLength, aData );
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::NextIE
// ---------------------------------------------------------
//
TInt CHssMgmtScanInfoImpl::NextIE( TUint8& aIE, 
                                TUint8& aLength, 
                                const TUint8** aData )
    {
      return iScanInfo->NextIE( aIE, aLength, aData );
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::GetInternalList
// ---------------------------------------------------------
//
HssScanList& CHssMgmtScanInfoImpl::GetInternalList()
    {
    return iScanList;
    }

// ---------------------------------------------------------
// CHssMgmtScanInfoImpl::UpdateResultsL
// ---------------------------------------------------------
//
void CHssMgmtScanInfoImpl::UpdateResultsL()
    {
    delete iScanInfo;
    iScanInfo = NULL;
    iScanInfo = new(ELeave) HssScanInfo( iScanList );
    }

//  End of File  
