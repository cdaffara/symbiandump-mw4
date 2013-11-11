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
* Description:  Declares ServiceInfi class.
*
*/


// INCLUDE FILES

#include <e32base.h>
#include <e32cons.h>
#include "upnpdevice.h"
#include "upnpserviceinfo.h"
#include "upnpcontrolpoint.h"
#include "upnpcommonupnplits.h"
#include "upnpgenamessagefactory.h"

#undef  KLogFile
#define KLogFile _L("UPnPCP.txt")
#include "upnpcustomlog.h"
using namespace UpnpHTTP;

static const TInt KSubscriptionRenewalTimeWindow = 3;
static const TInt KSubscriptionRenewalMinimalTime = 6;
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpAction::NewLC
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CUpnpServiceInfo* CUpnpServiceInfo::NewLC(
                            CUpnpControlPoint* aControlPoint,
                            CUpnpService* aService )
    {
    CUpnpServiceInfo* self = new (ELeave) CUpnpServiceInfo(
                                aControlPoint,
                                aService );
    CleanupStack::PushL( self );
    self->ConstructL();
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpServiceInfo::ConstructL()
    {
    iResubscribeTimer = CUpnpNotifyTimer::NewL( this );
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::CUpnpServiceInfo
// -----------------------------------------------------------------------------
//
CUpnpServiceInfo::CUpnpServiceInfo( CUpnpControlPoint* aControlPoint,
                                    CUpnpService* aService ):
        iService( aService ),
        iControlPoint( aControlPoint ),
        iTimeOut( CUpnpControlPoint::KDefaultSubscriptionTimeout )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::~CUpnpServiceInfo
// -----------------------------------------------------------------------------
//
CUpnpServiceInfo::~CUpnpServiceInfo()
    {
    delete iResubscribeTimer;
    delete iSid;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::TimerEventL
// Callback function
// -----------------------------------------------------------------------------
//
void CUpnpServiceInfo::TimerEventL( CUpnpNotifyTimer* /*aTimer*/ )
    {
    ResubscribeL();
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::StartTimerL
// -----------------------------------------------------------------------------
//
void CUpnpServiceInfo::StartTimerL( const TDesC8& aTimeout )
    {
    if ( aTimeout.FindC( KSeconds() ) != KErrNotFound )
        {
        TLex8 lex( aTimeout );
        static const TInt KLengthOfWordSecondMinus = 7;
        lex.Inc( KLengthOfWordSecondMinus );
        TInt tOutValue;
        TInt err( lex.Val( tOutValue ) );
        User::LeaveIfError( err );
        iTimeOut = tOutValue;
        }
    else
        {
        iTimeOut =
             CUpnpControlPoint::KDefaultSubscriptionTimeout ;
        }
//prevention from too frequent subscriptions
    if ( iTimeOut < KSubscriptionRenewalMinimalTime )
        {
        iTimeOut = KSubscriptionRenewalMinimalTime;
        }
    iResubscribeTimer->AfterSeconds(
        iTimeOut - KSubscriptionRenewalTimeWindow);
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::SessionId
// -----------------------------------------------------------------------------
//
TInt CUpnpServiceInfo::SessionId()
    {
    return iSessionId;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::Service
// -----------------------------------------------------------------------------
//
CUpnpService* CUpnpServiceInfo::Service()
    {
    return iService;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::Sid
// -----------------------------------------------------------------------------
//
const TDesC8& CUpnpServiceInfo::Sid() const
    {
    if ( iSid )
        {
        return *iSid;
        }
    else
        {
        return KNullDesC8;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::SetSidL
// -----------------------------------------------------------------------------
//
void CUpnpServiceInfo::SetSidL( const TDesC8& aSid )
    {
    delete iSid;
    iSid = NULL;
    iSid = aSid.AllocL();
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::SetSeq
// Sets sequence
// -----------------------------------------------------------------------------
//
void CUpnpServiceInfo::SetSeq( TUint32 aSeq )
    {
    iSeq = aSeq;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::Seq
//
// -----------------------------------------------------------------------------
//
TUint32 CUpnpServiceInfo::Seq() const
    {
    return iSeq;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::CreateSubscribtionMessageL
// -----------------------------------------------------------------------------
//
CUpnpGenaMessage* CUpnpServiceInfo::CreateSubscriptionMessageL(
                                                     THTTPMsgType aType)
    {
    return CreateSubscriptionMessageL( *iSid, aType );
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::CreateSubscribtionMessageL
// -----------------------------------------------------------------------------
//
CUpnpGenaMessage* CUpnpServiceInfo::CreateSubscriptionMessageL(
                                        const TDesC8& aPath, THTTPMsgType aType)
    {
    CUpnpGenaMessage* result = NULL;

    TBuf8<UpnpString::KMaxIntLength> timeoutBuf;
    timeoutBuf.Num( iTimeOut );

    CUpnpDevice& device = iService->Device();

    TInetAddr servAddress = iService->Address();
    TPtrC8 subscribe = iService->SubscriptionUrl();

    // check if path is not absolute
    if( subscribe.Find( KHttp() ) == 0 )
        {
        subscribe.Set( subscribe.Mid( KHttp().Length() ) );
        TInt slashPos = subscribe.Find( UpnpString::KSlash() );
        if( slashPos != KErrNotFound )
            {
            servAddress    = iControlPoint->GiveAddressL( subscribe.Left( slashPos ));
            subscribe.Set( subscribe.Mid( slashPos ) );
            }
        }

    switch( aType )
        {
        case ESubscription:
            {
            result = RUpnpGenaMessageFactory::SubscribeL(
                            subscribe, servAddress, aPath, timeoutBuf );
            break;
            }
        case EResubscription:
            {
            result = RUpnpGenaMessageFactory::ResubscribeL(
                            subscribe, servAddress, aPath, timeoutBuf );
            break;
            }
        case EUnSubscription:
            {
            result = RUpnpGenaMessageFactory::UnsubscribeL(
                            subscribe, servAddress, aPath);
            break;
            }
        default:
            {
            // Undefined. Should not come here at all!
            User::Leave(KErrArgument);
            break;
            }

        }
    if(result == NULL)
        {
        // To avoid Coverity FORWARD_NULL errors
        User::Leave(KErrArgument);
        }
    result->SetType( aType );
    iSessionId = result->SessionId();
    return result;
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::ResubscribeL
// -----------------------------------------------------------------------------
//
void CUpnpServiceInfo::ResubscribeL()
    {
    LOGS( "CUpnpServiceInfo::ResubscribeL - start" );
    iControlPoint->ResubscribeL( this );
    LOGS( "CUpnpServiceInfo::ResubscribeL - end" );
    }

// -----------------------------------------------------------------------------
// CUpnpServiceInfo::ConstructSubscribtionPathLC
//
// -----------------------------------------------------------------------------
//
HBufC8* CUpnpServiceInfo::ConstructSubscribtionPathLC( const TDesC8& aPath,
                                     TInetAddr& aAddr )
    {
    TBuf8<UpnpString::KMaxAddrOutputLength> addrBuf;
    CUpnpHttpMessage::AddrOutput( aAddr, addrBuf );

    TBuf<UpnpString::KMaxIntLength> port;
    port.Num( aAddr.Port() );

    HBufC8* longPath = HBufC8::NewLC(
        1 + KHTTPUrl().Length() +   // "<"
        addrBuf.Length() + 1 +      // ":"
        port.Length() + 1 +         // "\"
        aPath.Length() + 1 +        // "\"
        aPath.Length() + 1 );       // ">"

    TPtr8 longPathPtr( longPath->Des() );
    longPathPtr.Append( KLessThan8() );
    longPathPtr.Append( KHTTPUrl() );
    longPathPtr.Append( addrBuf );
    longPathPtr.Append( UpnpString::KColon() );
    longPathPtr.Append( port );
    longPathPtr.Append( UpnpString::KSlash() );
    longPathPtr.Append( aPath );
    longPathPtr.Append( UpnpString::KSlash() );
    longPathPtr.Append( aPath );
    longPathPtr.Append( KGreaterThan8() );

    return longPath;
    }

//end of file
