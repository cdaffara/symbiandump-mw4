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



#include <S32MEM.H>
#include "upnpipfiltering.h"
#include "upnpipfilteringdnsquery.h"

const TInt KGranularity = 1;



// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// CUpnpIPFiltering::CUpnpIPFiltering
// ---------------------------------------------------------------------------
//
CUpnpIPFiltering::CUpnpIPFiltering( RSocketServ& aSocketServer, TInt aIap,
                                    MMessageHandlerEngineObserver& aObserver ) :
                    iAllowedIPsList( KGranularity ),
                    iSockServ( aSocketServer ),
                    iActiveIap( aIap ),
                    iObserver( aObserver )
    {
    }


// ---------------------------------------------------------------------------
// CUpnpIPFiltering::ConstructL
// ---------------------------------------------------------------------------
//
void CUpnpIPFiltering::ConstructL( RSocketServ& aSocketServer )
    {

    iConnectionManagerProxy = CUpnpConnectionManagerProxy::NewL( aSocketServer );
    User::LeaveIfError( iConnectionManagerProxy->EnsureStart() );

    IPList2Des8L();
    }


// ---------------------------------------------------------------------------
// CUpnpIPFiltering::NewL
// ---------------------------------------------------------------------------
//
CUpnpIPFiltering* CUpnpIPFiltering::NewL( RSocketServ &aSocketServer, TInt aIap,
                                          MMessageHandlerEngineObserver& aObserver )
    {
    CUpnpIPFiltering* self = CUpnpIPFiltering::NewLC( aSocketServer, aIap ,aObserver );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpnpIPFiltering::NewLC
// ---------------------------------------------------------------------------
//
CUpnpIPFiltering* CUpnpIPFiltering::NewLC( RSocketServ &aSocketServer, TInt aIap,
                                           MMessageHandlerEngineObserver& aObserver )
    {
    CUpnpIPFiltering* self = new ( ELeave ) CUpnpIPFiltering( aSocketServer, aIap,
                                                              aObserver );
    CleanupStack::PushL( self );
    self->ConstructL( aSocketServer );
    return self;
    }


// ---------------------------------------------------------------------------
// CUpnpIPFiltering::~CUpnpIPFiltering
// ---------------------------------------------------------------------------
//
CUpnpIPFiltering::~CUpnpIPFiltering()
    {
    iDNSQueriesList.ResetAndDestroy();
    iAllowedIPsList.Close();
    delete iConnectionManagerProxy;

    iListBuf.Close();
    }


// ---------------------------------------------------------------------------
// CUpnpIPFiltering::AddAddress
// ---------------------------------------------------------------------------
//
TInt CUpnpIPFiltering::AddAddressL( const TInetAddr& aAddress )
    {
    if ( !( aAddress.IsBroadcast() ||
            aAddress.Address() == 0 ) )
        {
        TInt index( FindInList( aAddress ) );
        if( index == KErrNotFound )
            {
            iAllowedIPsList.AppendL( aAddress );
            IPList2Des8L();
            return KErrNone;
            }
        return index;
        }
    return KErrArgument;
    }

// ---------------------------------------------------------------------------
// CUpnpIPFiltering::AddAddressL
// ---------------------------------------------------------------------------
//
void CUpnpIPFiltering::AddAddressL( CUpnpIPFilteringDNSQuery* aQuery )
    {
    iDNSQueriesList.AppendL( aQuery );
    aQuery->AddAddress();
    aQuery->StartDNSQuery( iSockServ , iConnectionManagerProxy->ConnectionL() , this );
    }

// ---------------------------------------------------------------------------
// CUpnpIPFiltering::RemoveAddress
// ---------------------------------------------------------------------------
//
TInt CUpnpIPFiltering::RemoveAddressL( const TInetAddr& aAddress )
    {
    if ( !( aAddress.IsBroadcast() ||
            aAddress.Address() ==0 ) )
        {
        TInt index( FindInList( aAddress ) );
        if( index != KErrNotFound )
            {
            iAllowedIPsList.Remove( index );
            iAllowedIPsList.Compress();
            IPList2Des8L();
            }
        return index;
        }
    return KErrArgument;
    }

// ---------------------------------------------------------------------------
// CUpnpIPFiltering::RemoveAddressL
// ---------------------------------------------------------------------------
//
void CUpnpIPFiltering::RemoveAddressL( CUpnpIPFilteringDNSQuery* aQuery )
    {
    iDNSQueriesList.AppendL( aQuery );
    aQuery->RemoveAddress();
    aQuery->StartDNSQuery( iSockServ , iConnectionManagerProxy->ConnectionL() , this );
    }

// ---------------------------------------------------------------------------
// CUpnpIPFiltering::RemoveAll
// ---------------------------------------------------------------------------
//
TInt CUpnpIPFiltering::RemoveAll( )
    {
    if( iAllowedIPsList.Count() )
        {
        iAllowedIPsList.Reset();
        iAllowedIPsList.Compress();
        TRAP_IGNORE( IPList2Des8L() );
        return KErrNone;
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CUpnpIPFiltering::IsAllowed
// ---------------------------------------------------------------------------
//
TBool CUpnpIPFiltering::IsAllowed( const TInetAddr& aAddress ) const
  {
  return ( FindInList( aAddress ) != KErrNotFound );
  }

// ---------------------------------------------------------------------------
// CUpnpIPFiltering::Count
// ---------------------------------------------------------------------------
//
TInt CUpnpIPFiltering::Count(  ) const
    {
    return iAllowedIPsList.Count();
    }

const TDesC8& CUpnpIPFiltering::GetIPFilterList(  )
    {
    return iListBuf;
    }

// ---------------------------------------------------------------------------
// CUpnpIPFiltering::FindInList
// ---------------------------------------------------------------------------
//
TInt CUpnpIPFiltering::FindInList( const TInetAddr& aAddress ) const
    {
    for ( TInt index( 0 ) ; index < iAllowedIPsList.Count() ; index++ )
        {
        if ( iAllowedIPsList[index].Address() == aAddress.Address() )
            {
            return index;
            }
        }
    return KErrNotFound;
    }

// ---------------------------------------------------------------------------
// CUpnpIPFiltering::IPList2Des8L
// ---------------------------------------------------------------------------
//
void CUpnpIPFiltering::IPList2Des8L()
    {
    iListBuf.Close();

    const TInt KOne( 10 );

    CBufFlat* buf = CBufFlat::NewL( KOne );
    CleanupStack::PushL( buf );
    RBufWriteStream writer( *buf );
    CleanupClosePushL( writer );

    writer.WriteInt32L( Count() );

    for ( TInt i(0) ; i < iAllowedIPsList.Count() ; i++  )
        {
        TPckg<TInetAddr> temp( iAllowedIPsList[i] );
        writer << temp;
        }

    // create heap descriptor
    iListBuf.CreateL( buf->Size() );
    buf->Read( 0, iListBuf, buf->Size() );

    // clean up;
    CleanupStack::PopAndDestroy( &writer );
    CleanupStack::PopAndDestroy(buf);
    }

// ---------------------------------------------------------------------------
// CUpnpIPFiltering::RequestCompleted
// ---------------------------------------------------------------------------
//
void CUpnpIPFiltering::RequestCompleted( CUpnpIPFilteringDNSQuery* iQuery )
    {
    for( TInt i(0) ; i < iDNSQueriesList.Count() ; i++ )
        {
        if( iDNSQueriesList[i] == iQuery )
            {
            delete iDNSQueriesList[i];
            iDNSQueriesList.Remove( i );
            break;
            }
        }
    }

// ---------------------------------------------------------------------------
// CUpnpIPFiltering::IPListChange
// ---------------------------------------------------------------------------
//
void CUpnpIPFiltering::IPListChange()
    {
    iObserver.IPListChange();
    }
// End of file
