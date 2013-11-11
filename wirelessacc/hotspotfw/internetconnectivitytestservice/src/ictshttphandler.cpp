/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:   Class for HTTP GET testing
*
*/



#include <implementationproxy.h>
#include <uri8.h> 

#include "ictshttphandler.h"
#include "ictsengine.h"
#include "am_debug.h"

// CONSTANTS
_LIT8( KHeaderName, "X-Nokia-WLAN-Connectivity-Test" );
_LIT8( KHeaderValue, "true" );
_LIT8( KHttpPrefix, "http://" );

const TInt KMovedPermanently = 301;
const TInt KFound = 302;
const TInt KSeeOther = 303;
const TInt KTemporaryRedirect = 307;

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// CIctsHttpHandler::CIctsHttpHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CIctsHttpHandler::CIctsHttpHandler( CIctsEngine& aOwner,
                                    TInt aHttpResponseTime ) :
CTimer( EPriorityLow ), iOwner( aOwner ),
iHttpResponseTime( aHttpResponseTime ), iAttachDone( EFalse )
    {
     CActiveScheduler::Add( this );
    }


// ---------------------------------------------------------------------------
// CIctsHttpHandler::ConstructL
// ---------------------------------------------------------------------------
//
void CIctsHttpHandler::ConstructL()
    {
    DEBUG("CIctsHttpHandler::ConstructL()");
    // Open channel to Socket Server
    User::LeaveIfError( iSocketServ.Connect() );
    // Open connection
    User::LeaveIfError( iConnection.Open(iSocketServ) );
    
    CTimer::ConstructL();
   
    }


// ---------------------------------------------------------------------------
// CIctsHttpHandler::NewL
// ---------------------------------------------------------------------------
//
CIctsHttpHandler* CIctsHttpHandler::NewL( CIctsEngine& aOwner, 
                                            TInt aHttpResponseTime )
    {
    DEBUG("CIctsHttpHandler::NewL()");
    CIctsHttpHandler* self = new( ELeave ) CIctsHttpHandler( aOwner, 
                                                              aHttpResponseTime );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CIctsHttpHandler::~CIctsHttpHandler
// ---------------------------------------------------------------------------
//
CIctsHttpHandler::~CIctsHttpHandler()
    {
    DEBUG("CIctsHttpHandler::~CIctsHttpHandler()");
    RHTTPTransaction notActive;
    if ( iHttpTransaction != notActive )
        {
        iHttpTransaction.Close();
        }
    iHttpSession.Close();
    CTimer::Cancel();
    iConnection.Close();
    iSocketServ.Close();
    DEBUG("CIctsHttpHandler::~CIctsHttpHandler() Done");
    }


// ---------------------------------------------------------------------------
// CIctsHttpHandler::SetHttpConnectionInfoL
// ---------------------------------------------------------------------------
//
void CIctsHttpHandler::SetHttpConnectionInfoL( RConnection& aConnection, 
                                RSocketServ& aSocketServ)
    {
    DEBUG("CIctsHttpHandler::SetHttpConnectionInfoL");
    TInt result;
    TBuf<16> serviceType;
    TUint32 serviceId;
    TBuf<100> query;
    TUint connCount;
    
    iHttpSession.Close();
    iHttpSession.OpenL();
    
    RStringPool strPool = iHttpSession.StringPool();
    
    // Remove first session properties just in case.
    RHTTPConnectionInfo connInfo = iHttpSession.ConnectionInfo();
    
    // Clear RConnection and Socket Server instances
    connInfo.RemoveProperty( strPool.StringF( HTTP::EHttpSocketServ, 
                                               RHTTPSession::GetTable() ) );
    connInfo.RemoveProperty( strPool.StringF( HTTP::EHttpSocketConnection, 
                                               RHTTPSession::GetTable() ) );
    
    // Clear the proxy settings
    THTTPHdrVal proxyUsage( strPool.StringF( HTTP::EUseProxy, 
                                              RHTTPSession::GetTable() ) );
    connInfo.RemoveProperty( strPool.StringF( HTTP::EProxyUsage, 
                                               RHTTPSession::GetTable() ) );
    connInfo.RemoveProperty( strPool.StringF( HTTP::EProxyAddress, 
                                               RHTTPSession::GetTable() ) );
    
    // RConnection has been started, set proxy (if defined) and RConnection and
    // Socket Server session properties.
     
    // Proxy
    result = aConnection.EnumerateConnections( connCount );
    User::LeaveIfError( result) ;
        
    // Get service and service type for this connection
    //
    _LIT(string, "%s\\%s");
    query.Format( string, IAP, IAP_SERVICE );
    result = aConnection.GetIntSetting( query, serviceId );
      
    query.Format( string, IAP, IAP_SERVICE_TYPE );
    result = aConnection.GetDesSetting( query, serviceType );
    User::LeaveIfError( result );
        
        
    // RConnection and Socket Server
    // Now bind the HTTP session with the socket server connection
    connInfo.SetPropertyL ( 
        strPool.StringF( HTTP::EHttpSocketServ, RHTTPSession::GetTable() ), 
        THTTPHdrVal (aSocketServ.Handle() ) );
        
    TInt connPtr1 = reinterpret_cast<TInt>( &aConnection) ;
    connInfo.SetPropertyL ( 
        strPool.StringF( HTTP::EHttpSocketConnection, 
        RHTTPSession::GetTable() ), THTTPHdrVal ( connPtr1 ) );    
   
    }

// ---------------------------------------------------------------------------
// CIctsHttpHandler::SendHttpRequestL
// ---------------------------------------------------------------------------
//    
TInt CIctsHttpHandler::SendHttpRequestL( TDesC8& aIPAddress, 
                                     TUint32 aIapID, 
                                     TUint32 aNetworkId )
    {
    
    DEBUG("CIctsHttpHandler::SendHttpRequestL");
    
    // Cancel possibly outstanding request for polling reasons.
    CTimer::Cancel();
    
    TConnectionInfo info;
    TPckg< TConnectionInfo > pckgInfo( info );
    info.iIapId = aIapID;
    info.iNetId = aNetworkId;
    
    TInt err( KErrNone );
  	
    if ( !iAttachDone )
        {
        err = iConnection.Attach( pckgInfo, RConnection::EAttachTypeMonitor );
        DEBUG1("CIctsHttpHandler::SendHttpRequestL attach: %d", err);
        }
    
    if ( KErrNone == err )
        {
        iAttachDone = ETrue;
        SetHttpConnectionInfoL( iConnection, iSocketServ );
    
        // Remove redirect filter
        RStringPool stringPool = iHttpSession.StringPool();
        RStringF filterName = 
                stringPool.StringF( HTTP::ERedirect, RHTTPSession::GetTable() );
        iHttpSession.FilterCollection().RemoveFilter( filterName );

        RStringPool strPool = iHttpSession.StringPool();
        RStringF method = 
                strPool.StringF( HTTP::EGET, RHTTPSession::GetTable() );
    
        TBuf8<KMaxIpLength> ip;
        
        TBool httpExists = CheckHttp( aIPAddress );
        if ( !httpExists )        
            {
            ip.Copy( KHttpPrefix );
            }
        ip.Append( aIPAddress );
               
        // Parse string to URI
        TUriParser8 uri; 
        uri.Parse(ip);
        iHttpTransaction = iHttpSession.OpenTransactionL( uri, *this, method );
        RHTTPHeaders hdr = iHttpTransaction.Request().GetHeaderCollection();
    
        RStringF headerStrName = strPool.OpenFStringL( KHeaderName() );
        CleanupClosePushL( headerStrName );
        
        RStringF headerStrValue = strPool.OpenFStringL( KHeaderValue() );
        CleanupClosePushL( headerStrValue );
        
        THTTPHdrVal headerValue( headerStrValue );
        hdr.SetFieldL( headerStrName, headerValue );
        
        CleanupStack::PopAndDestroy( &headerStrValue );
        CleanupStack::PopAndDestroy( &headerStrName );

        iHttpTransaction.SubmitL();
        DEBUG("CIctsHttpHandler::SendHttpGetL SubmitL() done");
        CTimer::After( iHttpResponseTime );
        }
    
    return err;
    }  

// ---------------------------------------------------------------------------
// CIctsHttpHandler::CancelHttpRequestL
// ---------------------------------------------------------------------------
//    
void CIctsHttpHandler::CancelHttpRequestL()
    {
    DEBUG("CIctsHttpHandler::CancelHttpRequestL()");
    CTimer::Cancel();
    RHTTPTransaction notActive;
    if ( iHttpTransaction != notActive )
        {
        iHttpTransaction.Close();
        }
    }

// ---------------------------------------------------------------------------
// CIctsHttpHandler::RunL
// ---------------------------------------------------------------------------
//
void CIctsHttpHandler::RunL()
    {
    DEBUG("CIctsHttpHandler::RunL()");
    RHTTPTransaction notActive;
    if ( iHttpTransaction != notActive )
        {
        iHttpTransaction.Close();
        }
    iString = KNullDesC;
    iOwner.HttpEventL( ETimeout, iString );
    }


// ---------------------------------------------------------------------------
// CIctsHttpHandler::MHFRunL
//
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction events.
// ---------------------------------------------------------------------------
//   
void CIctsHttpHandler::MHFRunL( RHTTPTransaction aTransaction, 
                            const THTTPEvent& aEvent )
    {
    DEBUG1("CIctsHttpHandler::MHFRunL aEvent.iStatus: %d", 
            aEvent.iStatus);
    // See HTTP event statuses. Only ESucceeded and EFailed are needed. 
    switch (aEvent.iStatus) 
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            DEBUG("CIctsHttpHandler::THTTPEvent::EGotResponseHeaders");
            if ( CheckStatusCodeL( aTransaction ) )
                {
                // Redirection found. Transaction can be canceled.
                aTransaction.Cancel();
                }
            }
            break;
        case THTTPEvent::ESucceeded:
            {
            DEBUG("CIctsHttpHandler::MHFRunL Succeeded");
            CTimer::Cancel();
            iOwner.HttpEventL( EConnectionOk, iString );
            iString = KNullDesC;
            }
            break;

        case THTTPEvent::EFailed:
            {
            DEBUG("CIctsHttpHandler::MHFRunL Failed");
            CTimer::Cancel();
            iOwner.HttpEventL( EConnectionNotOk, iString );    
            iString = KNullDesC;
            }
            break;
    
        default:
            {
            DEBUG1( "CIctsHttpHandler::MHFRunL default: %d", aEvent.iStatus ); 
            // Do nothing. Timer will take care of error situations.
            } 
            break;
        }
    }

// ---------------------------------------------------------------------------
// CIctsHttpHandler::MHFRunError
//
// Inherited from MHTTPTransactionCallback
// Called by framework to pass transaction error.
// ---------------------------------------------------------------------------
//   
TInt CIctsHttpHandler::MHFRunError( TInt /*aError*/, 
                                RHTTPTransaction /*aTransaction*/, 
                                const THTTPEvent& /*aEvent*/)
    {
    DEBUG("CIctsHttpHandler::MHFRunError");
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CIctsHttpHandler::CheckHttp
// ----------------------------------------------------------------------------    
TBool CIctsHttpHandler::CheckHttp( TDesC8& aIPAddress )
    {
    // The "http://" prefix is expected to be at the beginning of the URI.
    return ( 0 == aIPAddress.Find( KHttpPrefix ) );
    }

// ----------------------------------------------------------------------------
// CIctsHttpHandler::CheckStatusCodeL
// ----------------------------------------------------------------------------    
TBool CIctsHttpHandler::CheckStatusCodeL( RHTTPTransaction aTransaction )
    {
    DEBUG1("CIctsHttpHandler::CheckStatusCodeL response statuscode: %d", 
                aTransaction.Response().StatusCode());
          
    TBool ret ( EFalse );
    
    switch ( aTransaction.Response().StatusCode() )
        {
        case KMovedPermanently:
        case KFound:
        case KSeeOther:
        case KTemporaryRedirect:
            {
            DEBUG("CIctsHttpHandler::CheckStatusCodeL Redirect");
            CTimer::Cancel();
            ret = ETrue;
            
            //parse the headers and look for location header
            RHTTPHeaders hdr =aTransaction.Response().GetHeaderCollection();
            RStringPool strP = aTransaction.Session().StringPool();
            RStringF location = 
                    strP.StringF( HTTP::ELocation,RHTTPSession::GetTable() );
            THTTPHdrVal hVal;
            if( hdr.GetField( location, 0, hVal ) == KErrNone ) 
                {
                DEBUG("CIctsHttpHandler::CheckStatusCodeL location header");
                // Location header is present
                RStringF fieldValStr = strP.StringF(hVal.StrF());
                const TDesC8& fieldValDesC = fieldValStr.DesC();
                iString.Copy( fieldValDesC );
                iOwner.HttpEventL( EHttpAuthenticationNeeded, iString );
                }
            else
                {
                DEBUG("CIctsHttpHandler::CheckStatusCodeL no location header");
                // No location header. Can't redirect.
                iOwner.HttpEventL( EConnectionNotOk, iString );
                iString = KNullDesC;
                }
            }
            break;
        
        default:
            // Do nothing
            break;
        }
    return ret;
    }

//  End of File
