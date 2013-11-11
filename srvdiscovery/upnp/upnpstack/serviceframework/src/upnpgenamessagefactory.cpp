/** @file
* Copyright (c) 2005-2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Gena message factory
*
*/


// INCLUDE FILES
#include "upnpgenamessagefactory.h"
#include "upnpstring.h"
#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"
// for "Second-"
#include "upnpcommonupnplits.h"

// CONSTANTS
using namespace UpnpGENA;
using namespace UpnpHTTP;


// ================= MEMBER FUNCTIONS =======================

// -----------------------------------------------------------------------------
// RUpnpGenaMessageFactory::SubscribeL
// Subscribe to path and host.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* RUpnpGenaMessageFactory::SubscribeL( const TDesC8& aPath,
                                                                TInetAddr& aHost,
                                                                const TDesC8& aCallback )
{
    LOGS("RUpnpGenaMessageFactory::SubscribeL(TDesC8&, TInetAddr&, TDesC8&)");

    CUpnpGenaMessage* msg = CUpnpGenaMessage::NewLC( aHost );
                
    HBufC8* temp = HBufC8::NewLC( KGenaSubscribe().Length() + 
                                  UpnpString::KSpace().Length() + 
                                  UpnpString::KSlash().Length() + 
                                  aPath.Length() + 
                                  KHttp11().Length() );
        
    temp->Des().Zero();
    temp->Des().Append( KGenaSubscribe );
    temp->Des().Append( UpnpString::KSpace );
    if ( aPath.Find( UpnpString::KSlash() ) != 0 )
    {
        temp->Des().Append( UpnpString::KSlash() );
    }
    temp->Des().Append( aPath );
    temp->Des().Append( KHttp11 );
    msg->AddPairL( temp->Des(), KNullDesC8 );
    CleanupStack::PopAndDestroy( temp );

    HBufC8* host = UpnpString::InetToStringL( aHost );
    CleanupStack::PushL( host );
    msg->AddPairL( UpnpSSDP::KHdrHost(), *host );
    CleanupStack::PopAndDestroy( host );

    msg->AddPairL( KCallback, aCallback );
    msg->AddPairL( UpnpSSDP::KHdrSearchTarget, KDefaultNt );

    CleanupStack::Pop( msg );
    return msg;
}

// -----------------------------------------------------------------------------
// RUpnpGenaMessageFactory::SubscribeL
// Subscribe to path and host using timeout.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* RUpnpGenaMessageFactory::SubscribeL( const TDesC8& aPath,
                                                                TInetAddr& aHost,
                                                                const TDesC8& aCallback,
                                                                const TDesC8& aTimeout )
{
    LOGS("RUpnpGenaMessageFactory::SubscribeL(TDesC8&, TInetAddr&, TDesC8&, TDesC8&)");
    
    CUpnpGenaMessage* msg = SubscribeL( aPath, aHost, aCallback );
    CleanupStack::PushL( msg );
        
    HBufC8* timeout = HBufC8::NewLC( KTimeoutPrefix().Length() + aTimeout.Length() );
    timeout->Des().Append( KTimeoutPrefix() );
    timeout->Des().Append( aTimeout );

    msg->AddPairL( KTimeout, *timeout );
        
    CleanupStack::PopAndDestroy( timeout );

    CleanupStack::Pop( msg );
    return msg;
}

// -----------------------------------------------------------------------------
// RUpnpGenaMessageFactory::UnsubscribeL
// Unsubscribe from path and host.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* RUpnpGenaMessageFactory::UnsubscribeL( const TDesC8 &aPath,
                                                                  TInetAddr &aHost,
                                                                  const TDesC8 &aSid )
{
    LOGS("RUpnpGenaMessageFactory::UnsubscribeL(TDesC8&, TInetAddr&, TDesC8&)");
    
    CUpnpGenaMessage* msg = CUpnpGenaMessage::NewLC( aHost );
                
    HBufC8* temp = HBufC8::NewLC( KGenaUnSubscribe().Length() + 
                                  UpnpString::KSpace().Length() + 
                                  UpnpString::KSlash().Length() + 
                                  aPath.Length() + 
                                  KHttp11().Length() );
        
    temp->Des().Zero();
    temp->Des().Append( KGenaUnSubscribe );
    temp->Des().Append( UpnpString::KSpace );
    if ( aPath.Find( UpnpString::KSlash() ) != 0 )
    {
        temp->Des().Append( UpnpString::KSlash() );     
    }
    temp->Des().Append( aPath );
    temp->Des().Append( KHttp11 );
    msg->AddPairL( temp->Des(), KNullDesC8 );
    CleanupStack::PopAndDestroy( temp );

    HBufC8* host = UpnpString::InetToStringL( aHost );
    CleanupStack::PushL( host );
    msg->AddPairL( UpnpSSDP::KHdrHost(), *host );
    CleanupStack::PopAndDestroy( host );

    msg->AddPairL( UpnpGENA::KSid(), aSid );

    CleanupStack::Pop( msg );
        
    return msg;
}

// -----------------------------------------------------------------------------
// RUpnpGenaMessageFactory::ResubscribeL
// Resubscribe to path and host with SID.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* RUpnpGenaMessageFactory::ResubscribeL( const TDesC8& aPath,
                                                                  TInetAddr& aHost,
                                                                  const TDesC8& aSid )
    {
    LOGS("RUpnpGenaMessageFactory::ResubscribeL(TDesC8&, TInetAddr&, TDesC8&)");
    CUpnpGenaMessage* msg = CUpnpGenaMessage::NewLC( aHost );
        
    HBufC8* temp = HBufC8::NewLC( KGenaUnSubscribe().Length() + 
                                  UpnpString::KSpace().Length() + 
                                  UpnpString::KSlash().Length() + 
                                  aPath.Length() + 
                                  KHttp11().Length() );
        
    temp->Des().Zero();
    temp->Des().Append( KGenaSubscribe );
    temp->Des().Append( UpnpString::KSpace );
    if ( aPath.Find( UpnpString::KSlash() ) != 0 )
    {
        temp->Des().Append( UpnpString::KSlash() );
    }
    temp->Des().Append( aPath );
    temp->Des().Append( KHttp11 );
    msg->AddPairL( temp->Des(), KNullDesC8 );
    CleanupStack::PopAndDestroy( temp );

    HBufC8* host = UpnpString::InetToStringL( aHost );
    CleanupStack::PushL( host );
    msg->AddPairL( UpnpSSDP::KHdrHost(), *host );
    CleanupStack::PopAndDestroy( host );

    msg->AddPairL( UpnpGENA::KSid, aSid );

    CleanupStack::Pop( msg );

    return msg;
}

// -----------------------------------------------------------------------------
// RUpnpGenaMessageFactory::ResubscribeL
// Resubscribe to path and host with SID using timeout.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* RUpnpGenaMessageFactory::ResubscribeL( const TDesC8& aPath,
                                                                  TInetAddr& aHost,
                                                                  const TDesC8& aSid,
                                                                  const TDesC8& aTimeout )
{
    LOGS("RUpnpGenaMessageFactory::ResubscribeL(TDesC8&, TInetAddr&, TDesC8&, TDesC8&)");

    CUpnpGenaMessage* msg = ResubscribeL( aPath, aHost, aSid );
    CleanupStack::PushL( msg );

    HBufC8* timeout = HBufC8::NewLC( KTimeoutPrefix().Length() + aTimeout.Length() );
    timeout->Des().Append( KTimeoutPrefix() );
    timeout->Des().Append( aTimeout );

    msg->AddPairL( KTimeout, *timeout );
        
    CleanupStack::PopAndDestroy( timeout );

    CleanupStack::Pop( msg );

return msg;
}

// -----------------------------------------------------------------------------
// RUpnpGenaMessageFactory::SubscribeResponseL
// Subscribe response.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* RUpnpGenaMessageFactory::SubscribeResponseL( 
                                                       TInt aSessionId,
                                                       const TInetAddr& aDestination,
                                                       const TDesC8& aSid,
                                                       const TDesC8& aTimeout )
{
    LOGS("RUpnpGenaMessageFactory::SubscribeResponseL(TInt, TInetAddr&, TDesC8&, TDesC8&, TDesC8&)");
    
    CUpnpGenaMessage* msg = SubscribeResponseL( aSessionId, aDestination,
                                                (TBool)ETrue, aSid, aTimeout );

    return msg;
}

// -----------------------------------------------------------------------------
// RUpnpGenaMessageFactory::SubscribeResponseL
// Subscribe response.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* RUpnpGenaMessageFactory::SubscribeResponseL( 
                                                TInt aSessionId,
                                                const TInetAddr& aDestination, 
                                                TBool /*aTimeStamp*/,                                           
                                                const TDesC8& aSid,
                                                const TDesC8& aTimeout )
    {
    LOGS("RUpnpGenaMessageFactory::SubscribeResponseL(TInt, TInetAddr&, TBool, TDesC8&, TDesC8&, TDesC8&)");

    CUpnpGenaMessage* msg = CUpnpGenaMessage::NewLC( aDestination );

    msg->SetSessionId( aSessionId );
    msg->AddPairL( KHttp11Ok, KNullDesC8 );
        
    msg->AddPairL( UpnpGENA::KSid, aSid );
    msg->AddPairL( KTimeout, aTimeout );

    CleanupStack::Pop( msg );
    return msg;
}

// -----------------------------------------------------------------------------
// RUpnpGenaMessageFactory::UnsubscribeResponseL
// Unsubscribe response.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* RUpnpGenaMessageFactory::UnsubscribeResponseL( 
                                                 TInt aSessionId, 
                                                 const TInetAddr& aDestination )
{
    LOGS("RUpnpGenaMessageFactory::UnsubscribeResponseL(TInt, TInetAddr&)");

    CUpnpGenaMessage* msg = CUpnpGenaMessage::NewLC( aDestination );
    msg->SetSessionId( aSessionId );
    msg->AddPairL( KHttp11Ok, KNullDesC8 );

    CleanupStack::Pop( msg );
    return msg; 
}

// -----------------------------------------------------------------------------
// RUpnpGenaMessageFactory::EventingLC
// Eventing.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpGenaMessage* RUpnpGenaMessageFactory::EventingLC( const TDesC8& aPath,
                                                                TInetAddr& aHost,
                                                                const TDesC8& aSid,
                                                                TUint32 aSeq )
{
    LOGS("RUpnpGenaMessageFactory::EventingLC(TDesC8&, TInetAddr&, TDesC8&, TUint32)");

    CUpnpGenaMessage* msg = CUpnpGenaMessage::NewLC( aHost );
                
    HBufC8* temp = HBufC8::NewLC( KMaxHeaderLength );
        
    temp->Des().Zero();
    temp->Des().Append( KGenaNotify );
    temp->Des().Append( UpnpString::KSpace );
    
    if ( aPath.Find( UpnpString::KSlash() ) != 0 )
    {
        temp->Des().Append( UpnpString::KSlash() );
    }

    temp->Des().Append( aPath );
    temp->Des().Append( UpnpString::KSpace );
    temp->Des().Append( KHttp11WithoutSpace );
    msg->AddPairL( temp->Des(), KNullDesC8 );
    CleanupStack::PopAndDestroy( temp );
    temp = NULL;
        
    HBufC8* host = UpnpString::InetToStringL( aHost );
    CleanupStack::PushL( host );
    msg->AddPairL( UpnpSSDP::KHdrHost(), *host );
    CleanupStack::PopAndDestroy( host );

    msg->AddPairL( UpnpGENA::KContentType, KTextXmlUtf8 );
    // length automatic by httpmessage
    msg->AddPairL( UpnpHTTP::KHdrContentLength, KNullDesC8 );
    msg->AddPairL( UpnpSSDP::KHdrSearchTarget, KDefaultNt );
    msg->AddPairL( UpnpSSDP::KHdrNotificationType, KDefaultNts );
    msg->AddPairL( UpnpGENA::KSid, aSid );

    temp = HBufC8::NewLC( UpnpString::KMaxTUintLength );
    temp->Des().NumUC( aSeq );
    msg->AddPairL( KSeq, *temp );
    CleanupStack::PopAndDestroy( temp );


    return msg;
}

// End of File
