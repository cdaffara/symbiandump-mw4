/** @file
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies  this distribution, and is available 
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



#include "upnpipfilteringdnsquery.h"


// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CUpnpIPFilteringDNSQuery::CUpnpIPFilteringDNSQuery
// ---------------------------------------------------------------------------
//
CUpnpIPFilteringDNSQuery::CUpnpIPFilteringDNSQuery( const RMessage2& aMessage ) 
                                        : CActive( EPriorityStandard ),
                                          iDNSStatus( KErrNotFound ),
                                          iMessage( aMessage )
                    
    {
    }


// ---------------------------------------------------------------------------
// CUpnpIPFilteringDNSQuery::ConstructL
// ---------------------------------------------------------------------------
//
void CUpnpIPFilteringDNSQuery::ConstructL( const TDesC8& aRemoteName )
    {
    iRemoteName.CreateL( aRemoteName.Length() );
    iRemoteName.Copy( aRemoteName );                                            
    CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CUpnpIPFilteringDNSQuery::NewL(
// ---------------------------------------------------------------------------
//
CUpnpIPFilteringDNSQuery* CUpnpIPFilteringDNSQuery::NewL( 
                                                    const RMessage2& aMessage,
                                                    const TDesC8& aRemoteName
                                                    )
    {
    CUpnpIPFilteringDNSQuery* self = CUpnpIPFilteringDNSQuery::NewLC( 
                                    aMessage, aRemoteName );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpnpIPFilteringDNSQuery::NewLC
// ---------------------------------------------------------------------------
//
CUpnpIPFilteringDNSQuery* CUpnpIPFilteringDNSQuery::NewLC( 
                                                    const RMessage2& aMessage,
                                                    const TDesC8& aRemoteName )
    {
    CUpnpIPFilteringDNSQuery* self = new ( ELeave ) CUpnpIPFilteringDNSQuery( 
                                                            aMessage );
    CleanupStack::PushL( self );
    self->ConstructL( aRemoteName );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpnpIPFilteringDNSQuery::~CUpnpIPFilteringDNSQuery
// ---------------------------------------------------------------------------
//
CUpnpIPFilteringDNSQuery::~CUpnpIPFilteringDNSQuery()
    {
    Cancel();
    iHostResolver.Close();
    iRemoteName.Close();
    }

// ---------------------------------------------------------------------------
// CUpnpIPFilteringDNSQuery::AddAddress
// ---------------------------------------------------------------------------
//
void CUpnpIPFilteringDNSQuery::AddAddress( )
    {
    iAction = EAddAddress;
    }

// ---------------------------------------------------------------------------
// CUpnpIPFilteringDNSQuery::RemoveAddress
// ---------------------------------------------------------------------------
//
void CUpnpIPFilteringDNSQuery::RemoveAddress( )
    {
    iAction = ERemoveAddress;
    }


// ---------------------------------------------------------------------------
// CUpnpIPFilteringDNSQuery::StartDNSQuery
// ---------------------------------------------------------------------------
//
void CUpnpIPFilteringDNSQuery::StartDNSQuery( RSocketServ& aSockServ, 
                                              RConnection& aConnection,
                                              CUpnpIPFiltering* aIPListOwner )
    {
    iIPListOwner = aIPListOwner;
    TInt err( iHostResolver.Open( aSockServ, KAfInet,
                                             KProtocolInetUdp, 
                                             aConnection ) );
    if( err )
        {
        RequestCompleted( err );
        return;
        }
    
    iHostResolver.GetByName( iRemoteName, iAddrDetails, iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// CUpnpIPFilteringDNSQuery::Status
// ---------------------------------------------------------------------------
//
TInt CUpnpIPFilteringDNSQuery::Status( TInt aStatus )
    {
    return ( iDNSStatus == 0 ? iDNSStatus : aStatus );
    }

// ---------------------------------------------------------------------------
// CUpnpIPFilteringDNSQuery::RequestCompleted(
// ---------------------------------------------------------------------------
//
void CUpnpIPFilteringDNSQuery::RequestCompleted( TInt aStatus )
    {
    iMessage.Complete( aStatus );
    iIPListOwner->RequestCompleted( this );
    }

// ---------------------------------------------------------------------------
// From CActive
// CUpnpIPFilteringDNSQuery::RunL
// ---------------------------------------------------------------------------
//
void CUpnpIPFilteringDNSQuery::RunL()
    {
    if( iStatus.Int()== KErrNone )
        {
        iDNSStatus = KErrNone;
        switch( iAction )
            {
            case EAddAddress:
                {
                if( iIPListOwner )
                    {
                    if( iIPListOwner->AddAddressL( 
                        static_cast<TInetAddr>( iAddrDetails().iAddr ) ) >= 0 
                                                 )
                        {
                        iIPListOwner->IPListChange();
                        }
                    }
                break;
                }
            case ERemoveAddress:
                {
                 if( iIPListOwner )
                    {
                    if ( iIPListOwner->RemoveAddressL( 
                         static_cast<TInetAddr>( iAddrDetails().iAddr ) ) >= 0 
                                                     )
                        {
                        iIPListOwner->IPListChange();    
                        }
                    }
                break;
                }
             default:
                {
                break;
                }
            }
        iHostResolver.Next( iAddrDetails , iStatus );
        SetActive();
        }
    else
        {
        RequestCompleted( Status( iStatus.Int() ) );
        }
    }

// ---------------------------------------------------------------------------
// From CActive
// CUpnpIPFilteringDNSQuery::RunError
// ---------------------------------------------------------------------------
//
TInt CUpnpIPFilteringDNSQuery::RunError( TInt aError )
    {
    RequestCompleted( Status( aError ) );
    return KErrNone;
    }
    
// ---------------------------------------------------------------------------
// From CActive
// CUpnpIPFilteringDNSQuery::DoCancel
// ---------------------------------------------------------------------------
//
void CUpnpIPFilteringDNSQuery::DoCancel()
    {
    iHostResolver.Cancel();
    iMessage.Complete( KErrCancel );
    }
// End of file
