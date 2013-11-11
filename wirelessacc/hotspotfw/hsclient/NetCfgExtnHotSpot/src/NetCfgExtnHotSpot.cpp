/*
* Copyright (c) 2002-2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Implements Network Config Extension for HotspotFW
*
*/


// INCLUDE FILES
#ifdef SYMBIAN_ENABLE_SPLIT_HEADERS
#include <es_sock_partner.h>
#include <nifman_internal.h>
#endif
#include "NetCfgExtnHotSpot.h"
#include "implementationproxy.h"
#include "am_debug.h"


// ============================ MEMBER FUNCTIONS ==============================

// ----------------------------------------------------------------------------
// NewL
// ----------------------------------------------------------------------------
//
CNetworkConfigExtensionHotSpot* CNetworkConfigExtensionHotSpot::NewL( 
                                                            TAny* aMNifIfNotify )
	{
	MNifIfNotify* nifIfNotify = reinterpret_cast<MNifIfNotify*>( aMNifIfNotify );
	CNetworkConfigExtensionHotSpot* pDaemon = 
	    new( ELeave )CNetworkConfigExtensionHotSpot( *nifIfNotify );
	CleanupStack::PushL( pDaemon );
	pDaemon->ConstructL();
	CleanupStack::Pop( pDaemon );
	return pDaemon;
	}
	
// ----------------------------------------------------------------------------
// ConstructL
// ----------------------------------------------------------------------------
//    
void CNetworkConfigExtensionHotSpot::ConstructL()
	{
	DEBUG( "CNetworkConfigExtensionHotSpot::ConstructL()" );
	CNetworkConfigExtensionBase::ConstructL();
	iNotAuthenticated = ETrue;
	iNotDeregistered = ETrue;
	iIsStartLoginActive = EFalse;
	iHotspotConnect = KErrNotFound;
	}

// ----------------------------------------------------------------------------
// SendIoctlMessageL 
// Forwards Ioctl request to the daemon. Activates the AO to wait for response.
// ----------------------------------------------------------------------------
//
void CNetworkConfigExtensionHotSpot::SendIoctlMessageL( 
                                    const ESock::RLegacyResponseMsg& aMessage )
	{
  	TInt name = aMessage.Int1();
  	if ( aMessage.Int0() != KCOLConfiguration )
  	    {
  	    User::Leave( KErrNotSupported );
  	    }
  	else 
  		{
  		switch ( name ) 
  			{
  	    	case KConnAddrRelease:
  	    	case KConnAddrRenew:
  	    	case KConnSetDhcpRawOptionData:
		    	if ( aMessage.HasCapability(ECapabilityNetworkControl, "NetCfgExtnDhcp" ) == EFalse )
				    {
				    User::Leave (KErrPermissionDenied );
				}
  			}   
  		}
	CNetworkConfigExtensionBase::SendIoctlMessageL( aMessage );
	}
	
// ----------------------------------------------------------------------------
// ~CNetworkConfigExtensionHotSpot
// ----------------------------------------------------------------------------
//
CNetworkConfigExtensionHotSpot::~CNetworkConfigExtensionHotSpot()
	{
	DEBUG( "CNetworkConfigExtensionHotSpot::~CNetworkConfigExtensionHotSpot" );
	iClient.Close();
	}    

// ----------------------------------------------------------------------------
// ImplementationTable
// ----------------------------------------------------------------------------
//
const TImplementationProxy ImplementationTable[] = 
    {
    IMPLEMENTATION_PROXY_ENTRY( 0x10282ECA, 
                                CNetworkConfigExtensionHotSpot::NewL )
    };

// ----------------------------------------------------------------------------
// ImplementationGroupProxy
// ----------------------------------------------------------------------------
//
EXPORT_C const TImplementationProxy* ImplementationGroupProxy( 
                                                            TInt& aTableCount )
    {
    aTableCount = sizeof( ImplementationTable ) / 
                    sizeof( TImplementationProxy );

    return ImplementationTable;
    }

// ----------------------------------------------------------------------------
// Deregister
// ----------------------------------------------------------------------------
//
void CNetworkConfigExtensionHotSpot::Deregister( TInt aCause )
    {
    DEBUG1( "CNetworkConfigExtensionHotSpot::Deregister() aCause: %d", aCause );
    CNetworkConfigExtensionBase::Deregister( aCause ); // to parent
    
    if ( iIsStartLoginActive )
        {
        DEBUG( "CNetworkConfigExtensionHotSpot::Deregister() LoginComplete" );
        iClient.LoginComplete( iConnectionInfoBuf().iIapId, KErrNone );
        }
    iIsStartLoginActive = EFalse;
    }

// ----------------------------------------------------------------------------
// RunL
// ----------------------------------------------------------------------------
//
void CNetworkConfigExtensionHotSpot::RunL()
    {
    DEBUG( "CNetworkConfigExtensionHotSpot::RunL()" );
    if ( iLastGenericProgressStage == KConfigDaemonStartingRegistration )
        {
        if( iNotAuthenticated )
            {    
            DEBUG( "CNetworkConfigExtensionHotSpot::RunL() StartLogin" );
            iHotspotConnect = iClient.Connect();
            
            if ( KErrNone == iHotspotConnect )
                {
                iIsStartLoginActive = ETrue;
                iClient.StartLogin( iConnectionInfoBuf().iIapId, 
                                    iConnectionInfoBuf().iNetId, 
                                    iStatus );
                SetActive();
                }
            else // to parent
                {
                CNetworkConfigExtensionBase::RunL();
                }
            iNotAuthenticated = EFalse;
            }
        else // to parent
            {
            DEBUG( "CNetworkConfigExtensionHotSpot::RunL() authenticated" );
            iIsStartLoginActive = EFalse;
            CNetworkConfigExtensionBase::RunL();
            }
        
        }
    else if ( iLastGenericProgressStage == 
              KConfigDaemonStartingDeregistration )
        {
        if( iNotDeregistered )
            {
            DEBUG( "CNetworkConfigExtensionHotSpot::RunL() CloseConnection" );
            TUint iapId = iConnectionInfoBuf().iIapId;
            TUint networkId = iConnectionInfoBuf().iNetId;
            
            if ( KErrNone == iHotspotConnect )
                {
                iClient.CloseConnection( iapId, iStatus );
                SetActive();
                }
            else // to parent
                {
                CNetworkConfigExtensionBase::RunL();
                }
            iNotDeregistered = EFalse;
            }
        else // to parent
            {
            DEBUG( "CNetworkConfigExtensionHotSpot::RunL() deregistered" );
            CNetworkConfigExtensionBase::RunL();
            }
        }
    else // to parent
        {
        DEBUG( "CNetworkConfigExtensionHotSpot::RunL() original" );
        CNetworkConfigExtensionBase::RunL();
        }
    }
    
// ----------------------------------------------------------------------------
// DoCancel
// ----------------------------------------------------------------------------
//
void CNetworkConfigExtensionHotSpot::DoCancel()
    {
    DEBUG( "CNetworkConfigExtensionHotSpot::RunL() DoCancel" );
    iClient.CancelLogin();
    }

// end of file

