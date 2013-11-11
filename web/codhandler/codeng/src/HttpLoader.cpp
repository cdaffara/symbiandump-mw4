/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: 
*      Loading functions for COD Handler.   
*      
*
*/


// INCLUDE FILES

#include "HttpLoader.h"
#include "CodSaver.h"
#include "HttpTcpSession.h"
#include "HttpWapSession.h"
#include "CodLogger.h"
#include "CodPanic.h"
#include "CodError.h"
#include "CodUtil.h"
#include "Connection.h"
#include "Timeout.h"
#include "CodProgress.h"
#include "CodEngBase.h"

#include <cookiefilterinterface.h>
#include <uaproffilter_interface.h>
#include <deflatefilterinterface.h>
#include <httperr.h>
#include <bldvariant.hrh>
#include <ecom/ecom.h>
#include <es_sock.h>
#include <escapeutils.h>
#include <http/rhttpheaders.h>
#include <Oma2Agent.h>
#include <HttpDownloadMgrCommon.h>
#include    "HeaderField.h"
#include    "FileExt.h"
#include    "CodData.h"



_LIT8( KDRMOldContentType, "x-drm-old-content-type");           // old content type header to be added
_LIT8( KAcceptRangeHeader, "bytes");           

// ================= CONSTANTS =======================

/// Maximum user name length we support.
LOCAL_D const TInt KCodMaxAuthUserName = 50;
/// Maximum password length we support.
LOCAL_D const TInt KCodMaxAuthPassword = 50;
/// WSP protocol name.
_LIT8( KCodWspProtocol, "WSP/WSP" );
/// Accept-header value (application/vnd.wap.mms-message).
_LIT8( KCodAcceptMmsHeaderValue, "application/vnd.wap.mms-message" );
/// Accept-header value (application/vnd.wap.sic).
_LIT8( KCodAcceptSiHeaderValue, "application/vnd.wap.sic" );
/// "No activity" timeout for GET request (in microseconds), 60 sec - Updated to 60 secs as part of error fix JSIN-7JSE6H
/// Let's try to keep the same timeout for WCDMA & GPRS (???)
LOCAL_D const TInt KCodGetTimeout = 60000000;
/// "No activity" timeout for POST request (in microseconds), 10 sec.
LOCAL_D const TInt KCodPostTimeout = 10000000;
/// Install-notify timeout (in microseconds), 50 msec.
LOCAL_D const TInt KCodINTimeout = 50000;
/// Retry count for GET request timeout (1 attempt == no retry).
LOCAL_D const TInt KCodGetRetry = 1;
/// Retry count for POST request timeout (2 attempt == 1 retry).
LOCAL_D const TInt KCodPostRetry = 2;
/// Estimated data transfer for reponse headers. Needed for progress.
LOCAL_D const TInt KCodRespHdrsTraffic = 512;
/// characters which should be escaped from requested URL's
_LIT8( KUriEscChars, " ");

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CHttpLoader::NewL()
// ---------------------------------------------------------
//
CHttpLoader* CHttpLoader::NewL
        (
        CConnection& aConnection,
        MCodLoadObserver* aCodLoadObserver,
        TCodProgress* aProgress ,
        CCodEngBase* aCodEng 
        )
    {
    CHttpLoader* loader =
        new( ELeave ) CHttpLoader( aConnection, aCodLoadObserver, aProgress, aCodEng );
    CleanupStack::PushL( loader );
    loader->ConstructL();
    CleanupStack::Pop( loader );
    return loader;
    }
// ---------------------------------------------------------
// CHttpLoader::~CHttpLoader()
// ---------------------------------------------------------
//
CHttpLoader::~CHttpLoader()
    {
    Cancel();
    iFeatMgr.Close();
    delete iTimeout;
    delete iSess;
    delete iNotifyBody;
    delete iUri;
    delete iINTimeout;
    
     if(iResponseHeaders != NULL)
    {
    iResponseHeaders->ResetAndDestroy();
    delete iResponseHeaders;
    }
    
    delete iDownloadInfo;

    REComSession::FinalClose();
    __ASSERT_DEBUG( !iParentStatus, CodPanic( ECodRequestPending ) );
    CLOG(( EHttpLoad, 2, _L("*** CHttpLoader::~CHttpLoader") ));
    }

// ---------------------------------------------------------
// CHttpLoader::LoadL()
// ---------------------------------------------------------
//
void CHttpLoader::LoadL
        (
        const TDesC8& aUrl,
        MCodSaverFactory& aSaverFactory,
        TRequestStatus* aStatus
        )
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::LoadL") ));
    // Misuse asserts.
    __ASSERT_ALWAYS( aStatus, CodPanic( ECodInvalidArguments ) );
    __ASSERT_ALWAYS( iState == EInit, CodPanic( ECodOffState ) );
    // Internal asserts.
    __ASSERT_DEBUG( !iParentStatus, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iSaverFactory, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iSaver, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iUri, CodPanic( ECodInternal ) );

    iUri = aUrl.AllocL();
    iMethod = HTTP::EGET;
    iRetry = KCodGetRetry;
    iSaverFactory = &aSaverFactory;
    iResult = KErrNone;
    iRedirect = EFalse;
    
    
    iCodEng->LoadSubInfoFileL(iCodEng->Data().ActiveDownload() , iResponseHeaders );
    
    TBuf8<128> contentType;
    TRAPD( err, contentType = GetContentTypeL() );
    if(err == KErrNone)
        {
        iSaver = iSaverFactory->CreateSaverL( contentType );
        }
        
    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;

    if( iProgress && iSaver )
        {
        iSaver->SetSourceUriL(iUri->Des() );
        TInt dlSize( 0 );
        //Current Track Downloaded Size
        dlSize += iCodEng->Data()[iCodEng->Data().ActiveDownload()]->DownloadedSize();
                
        if(dlSize >= 0)
            {
            iProgress->SetAmountL(dlSize);                
            }
        }

    if( iProgress )
    	{
    	iProgress->StartPhaseL(TCodProgress::ELoad);
    	}

    Continue( EStart );

    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::LoadL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::NotifyL()
// ---------------------------------------------------------
//
void CHttpLoader::NotifyL
( const TDesC8& aUrl, const TDesC8& aNotifyBody, TRequestStatus* aStatus )
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::NotifyL") ));
    // Misuse asserts.
    __ASSERT_ALWAYS( aStatus, CodPanic( ECodInvalidArguments ) );
    __ASSERT_ALWAYS( iState == EInit, CodPanic( ECodOffState ) );
    // Internal asserts.
    __ASSERT_DEBUG( !iParentStatus, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iNotifyBody, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( !iUri, CodPanic( ECodInternal ) );

    // Allocation of two members iUri+iNotifyBody is atomic.
    HBufC8* notifyBody = aNotifyBody.AllocLC();
    iUri = aUrl.AllocL();
    iNotifyBody = notifyBody;
    CleanupStack::Pop( notifyBody );    // now member.
    iMethod = HTTP::EPOST;
    iRetry = KCodPostRetry;
    iParentStatus = aStatus;
    *iParentStatus = KRequestPending;
    iResult = KErrNone;

    Continue( EStart );

    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::NotifyL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::DoCancel()
// ---------------------------------------------------------
//
void CHttpLoader::DoCancel()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::DoCancel iState(%d)"), iState ));
    switch ( iState )
        {
        case EStart:
            {
            // Already completed (by self-completion).
            __ASSERT_DEBUG( iStatus != KRequestPending, \
                CodPanic( ECodInternal ) );
            SelfComplete( KErrCancel );
            break;
            }

        case EOpen:
            {
            __ASSERT_DEBUG( iSess, CodPanic( ECodInternal ) );
            // This will complete our status.
            iSess->Disconnect();
            break;
            }

        case ERequest:
            {
            CompleteTransaction( KErrCancel );
            break;
            }

        case EInit:
        default:
            {
            // No requests should be outstanding in these states.
            CLOG(( EHttpLoad, 0, _L("CHttpLoader::DoCancel: unexpected state") ));
            CodPanic( ECodInternal );
            break;
            }
        }

    iResult = KErrCancel;
    Done();

    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::DoCancel") ));
    }



void CHttpLoader::Pause()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::Pause iState(%d)"), iState ));
    switch ( iState )
        {
        case ERequest:
            {
            CompleteTransaction( KErrCodHttpDownloadPaused );
            break;
            }
        }
    //Done();

    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::Pause") ));
    }

// ---------------------------------------------------------
// CHttpLoader::RunL()
// ---------------------------------------------------------
//
void CHttpLoader::RunL()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::RunL iState(%d) iStatus(%d)"), \
        iState, iStatus.Int() ));

    User::LeaveIfError( iStatus.Int() );    // Handle errors in RunError.
    switch ( iState )
        {
        case EStart:
            {
            // Operation initiated - Open a session.
            OpenSessionL();
            break;
            }

        case EOpen:
            {
            // Session opened. Make the HTTP request.
            RequestL();
            break;
            }

        case ERequest:
            {
            // Request completed, we are done.
            Done();
            break;
            }

        case EInit:
        default:
            {
            // No requests should be outstanding in these states.
            CLOG(( ECodEng, 0, _L("CHttpLoader::RunL: unexpected state") ));
            CodPanic( ECodInternal );
            break;
            }
        }
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::RunL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::RunError()
// ---------------------------------------------------------
//
TInt CHttpLoader::RunError( TInt aError )
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::RunError aError(%d)"), aError ));
    iResult = aError;
    Done();
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::RunError") ));
    return KErrNone;
    }

// ---------------------------------------------------------
// CHttpLoader::MHFRunL()
// ---------------------------------------------------------
//
void CHttpLoader::MHFRunL
( RHTTPTransaction DEBUG_ONLY( aTransaction ), const THTTPEvent& aEvent )
    {
    CLOG(( EHttpLoad, 0, _L("-> CHttpLoader::MHFRunL event(%d)"), \
                                                            aEvent.iStatus ));
    __ASSERT_DEBUG( aTransaction == iTrans, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iState == ERequest, CodPanic( ECodOffState ) );

    StartTimeout();    // There was activity -> restart timeout.

    switch ( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseHeaders:
            {
            // Now we know that the request was processed by the server.
            iSuppressErrors = EFalse;
            if (iMethod == HTTP::EGET)
            {
                StoreResponseHeaderL();
                
                //If DD contain TYPE defined more than once, 
            	//TYPE defined in the ClntServ should match the Content-Type that we obtain in the header while content downloading 
            
           	    const TDesC8& ContentType = GetContentTypeL( iTrans.Response().GetHeaderCollection() );
                const CCodData& CodData = iCodEng->Data();
          
                for ( TInt i = 1; i < CodData[CodData.ActiveDownload()]->Types().MdcaCount(); i++ )
            		{
          			const TDataType& type( CodData[CodData.ActiveDownload()]->Types().MdcaPoint( i ) );
            		if(ContentType.Find (type.Des8()) != KErrNotFound )             
                		{
                		CodData[CodData.ActiveDownload()]->ReArrangeTypesL(i);
                		iCodEng->ContentTypeChanged();
                		break;
               			}                
            		}

                iCodEng->StoreSubInfoFileL(iResponseHeaders, iCodEng->Data().ActiveDownload() );
            }
            HandleResponseHeadersL( iTrans.Response() );
            
            if (iResult == KErrCodHttpBadUrl ) 
            {
            	User::Leave( KErrCodHttpBadUrl );
            } 
            else if ( iResult == KErrCodHttpBadResponse ) 
            {
            	User::Leave( KErrCodHttpBadResponse );
            }
            else 
            {
                // Increment progress a bit to make update frequent.
                IncProgressL( KCodRespHdrsTraffic );	
            }
            break;
            }

        case THTTPEvent::EGotResponseBodyData:
            {
            // Get body data and save (GET) or ignore (POST)
            TInt err( KErrNone );
            TInt size;
            MHTTPDataSupplier* body = iTrans.Response().Body();
            TPtrC8 bodyP;
            // Caution: no leaving between body->GetNextDataPart and
            // body->ReleaseData calls! Data must always be released.
#ifdef __TEST_COD_LOG
            TBool lastChunk = // ('Log-only' variable.)
#endif /* def __TEST_COD_LOG */
            body->GetNextDataPart( bodyP );     // No leave...
            size = bodyP.Size();
#ifdef __TEST_COD_LOG
            CDUMP(( EHttpLoad, 5, _S("Data: "), _S("      "), \
                bodyP.Ptr(), bodyP.Size() ));
            if ( lastChunk )
                {
                CLOG(( EHttpLoad, 5, _L("      (EOF)") ));
                }
            else
                {
                CLOG(( EHttpLoad, 5, _L("      (more data)") ));
                }
#endif /* def __TEST_COD_LOG */
            if ( iMethod == HTTP::EGET )
                {
//TODO check  if iSaver == NULL occures                __ASSERT_DEBUG( iSaver, CodPanic( ECodInternal ) );
                if (iSaver)
				{
                  err = iSaver->AppendData( bodyP );
                  iCodEng->UpdateDownloadedSize( bodyP.Size());  
				}
			}
            body->ReleaseData();                // ...until here.
            User::LeaveIfError( err );
            IncProgressL( size );
            break;
            }
            
        case THTTPEvent::EResponseComplete:
            {
             if(iSaver)
                 {
                 iSaver->OnComplete();
                 }
             
        	 break;
            }    

        case THTTPEvent::EFailed:
            {
            // Safety code: we should already have an error code.
            if ( iResult == KErrNone )
                {
                iResult = KErrCodWapConnectionDropped;
                }
            // Fall through.
            }

        case THTTPEvent::ESucceeded:
            {
            CompleteTransaction( iResult );
            break;
            }

        case THTTPEvent::ERedirectRequiresConfirmation:
            {
            // 3xx redirect response received for POST. Redirect filter is
            // cautious to redirect POST request and asks for confirmation.
            // Confirmation means that we must resubmit the transaction.
            iTrans.SubmitL();
            break;
            }
            
        case THTTPEvent::ERedirectedPermanently:
        	{
        	RedirectedPermanentlyL(iTrans.Request().URI().UriDes());
        	break;
        	}
        	
        case THTTPEvent::ERedirectedTemporarily:
        	{
        	RedirectedTemporaryL(iTrans.Request().URI().UriDes());
        	break;
        	}

        default:
            {
            if( aEvent.iStatus == KErrHttpRedirectUseProxy && UseProxyL() )
                {
                // KErrHttpRedirectUseProxy is sent by redirect filter if the
                // response is 305 Use Proxy. This is not really an error;
                // error code is returned only to indicate that the redirect
                // filter cannot handle this kind of redirect automatically;
                // client action is required.
                //
                // If UseProxyL can successfully set the new proxy address,
                // this is not an error. Otherwise it is.
                ;
                }
            else if( aEvent.iStatus == KErrConnectionTerminated )
                {
                // if we get KErrConnectionTerminated during period of time
                // when install-notify timeout is active, that means that
                // connection has closed prior to timer firing and we should
                // not make content available to user, then handle the error
                // normally after that
                CLOG(( EHttpLoad, 2, _L("CHttpLoader::MHFRunL: aEvent.iStatus = KErrConnectionTerminated") ));
                if( iINTimeout && iINTimeout->IsActive() )
                    {
                    CLOG(( EHttpLoad, 2, _L(" :install-notify timeout active: set iSuppressError=EFalse") ));
                    iINTimeout->Cancel();
                    iSuppressErrors = EFalse;
                    }
                // finish handling error in MHFRunError
                User::Leave( KErrConnectionTerminated ); 
                }
            else if( aEvent.iStatus ==  KErrHttpPartialResponseReceived )
                {
                //Partial response has been received and connection has been disconnected. This error will be 
                //propagated to the client only, if the HTTP:ENotifyOnDisconnect property is set with a value
                //HTTP::EEnableDisconnectNotification
                
                //This error code was cancelling the pausable download. This error shud be ignored to keep the
                //paused download.
                //TSW Err ID : SXUU-77SRWL
                
                CLOG(( EHttpLoad, 2, _L("CHttpLoader::MHFRunL: aEvent.iStatus = KErrHttpPartialResponseReceived") ));
                }                
            else
                {
                // Handle errors in MHFRunError.
                User::LeaveIfError( aEvent.iStatus );
                }
            break;
            }
        }
    CLOG(( EHttpLoad, 0, _L("<- CHttpLoader::MHFRunL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::MHFRunError()
// ---------------------------------------------------------
//
TInt CHttpLoader::MHFRunError
        (
        TInt aError,
        RHTTPTransaction DEBUG_ONLY( aTransaction ),
        const THTTPEvent& /*aEvent*/
        )
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::MHFRunError (%d)"), aError ));
    __ASSERT_DEBUG( aTransaction == iTrans, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iState == ERequest, CodPanic( ECodOffState ) );
    CompleteTransaction( aError );
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::MHFRunError") ));
    return KErrNone;
    }

// ---------------------------------------------------------
// CHttpLoader::GetNextDataPart()
// ---------------------------------------------------------
//
TBool CHttpLoader::GetNextDataPart( TPtrC8& aDataPart )
    {
    __ASSERT_DEBUG( iNotifyBody, CodPanic( ECodInternal ) );
    aDataPart.Set( iNotifyBody->Des() );
    return ETrue;
    }

// ---------------------------------------------------------
// CHttpLoader::ReleaseData()
// ---------------------------------------------------------
//
void CHttpLoader::ReleaseData()
    {
    // Do not delete iNotifyBody here. Reset() may be called and since we
    // do support it, we may need to provide the data again.
    // iNotifyBody will be deleted in Done(); that's a safe place.
    }

// ---------------------------------------------------------
// CHttpLoader::OverallDataSize()
// ---------------------------------------------------------
//
TInt CHttpLoader::OverallDataSize()
    {
    __ASSERT_DEBUG( iNotifyBody, CodPanic( ECodInternal ) );
    return iNotifyBody->Des().Size();
    }

// ---------------------------------------------------------
// CHttpLoader::Reset()
// ---------------------------------------------------------
//
TInt CHttpLoader::Reset()
    {
    return KErrNone;
    }

// ---------------------------------------------------------
// CHttpLoader::GetCredentialsL()
// ---------------------------------------------------------
//
TBool CHttpLoader::GetCredentialsL
        (
        const TUriC8& aURI,
        RString aRealm, 
        RStringF /*aAuthenticationType*/,
        RString& aUsername, 
        RString& aPassword
        )
    {
    TBool ret( EFalse );
    if ( iCodLoadObserver )
        {
        // Unfortunately, everything has to be converted from 8 to 16 bit
        // and vice versa.
        HBufC* host = CodUtil::ConvertLC( aURI.UriDes() );
        HBufC* realm = CodUtil::ConvertLC( aRealm.DesC() );
        TBuf<KCodMaxAuthUserName> userName;
        TBuf<KCodMaxAuthPassword> password;
        ret = iCodLoadObserver->UserAuthL
            ( *host, *realm, EFalse, userName, password );
        CleanupStack::PopAndDestroy( 2 );   // realm, host
        HBufC8* userName8 = CodUtil::ConvertLC( userName );
        aUsername = iSess->Sess().StringPool().OpenStringL( *userName8 );
        CleanupStack::PopAndDestroy( userName8 );
        CleanupClosePushL<RString>( aUsername );
        HBufC8* password8 = CodUtil::ConvertLC( password );
        aPassword = iSess->Sess().StringPool().OpenStringL( *password8 );
        CleanupStack::PopAndDestroy( password8 );
        CleanupStack::Pop();    // closing aUsername
        }
    return ret;
    }

// ---------------------------------------------------------
// CHttpLoader::CHttpLoader()
// ---------------------------------------------------------
//
CHttpLoader::CHttpLoader
        (
        CConnection& aConnection,
        MCodLoadObserver* aCodLoadObserver,
        TCodProgress* aProgress,
        CCodEngBase* aCodEng
        )
: CActive( CActive::EPriorityStandard ),
  iMethod( HTTP::EGET ),
  iHttpVersion( HTTP::EHttp11 ),
  iConn( aConnection ),
  iCodLoadObserver( aCodLoadObserver ),
  iState( EInit ),
  iResult( KErrNone ),
  iSuppressErrors( EFalse ),
  iProxySet( EFalse ),
  iProgress( aProgress ),
  iRetry( KCodGetRetry ),
  iCodEng(aCodEng),
  iPausableDRM( ETrue )
    {
    CActiveScheduler::Add( this );
	if (iCodLoadObserver)
	{
	iCodLoadObserver->SetConnError( KErrNone);
	}
    CLOG(( EHttpLoad, 2, _L("*** CHttpLoader::CHttpLoader") ));
    }
// ---------------------------------------------------------
// CHttpLoader::ConstructL()
// ---------------------------------------------------------
//
void CHttpLoader::ConstructL() 
    {
    iFeatMgr.OpenL();
    
	iResponseHeaders = new (ELeave) CArrayPtrFlat<CHeaderField>(2);	
	
    }

// ---------------------------------------------------------
// CHttpLoader::OpenSessionL
// ---------------------------------------------------------
//
void CHttpLoader::OpenSessionL()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::OpenSessionL") ));

    if( iSess )
        {
        // Already opened.
        // Synchronous state change - we are already under RunL.
        CLOG(( EHttpLoad, 3, _L("  already open") ));
        iState = EOpen;
        RequestL();
        }
    else
        {
        if ( iFeatMgr.FeatureSupported( KFeatureIdWsp ) )
            {
            CLOG(( EHttpLoad, 4, _L("  KFeatureIdWsp supported") ));
            TUint32 iap;
            if ( !iConn.IsConnected( iap ) )
                {
                CLOG(( EHttpLoad, 3, _L("  not connected, leaving") ));
                User::Leave( KErrCodCannotConnect );
                }
            HBufC8* gateway = CodUtil::WapGatewayL( iap );  // NULL if not WAP.
            CleanupStack::PushL( gateway );                 // Push NULL is OK.
            if ( gateway )
                {
                CLOG(( EHttpLoad, 4, _L("  AP has WAP gateway") ));
                // We have a valid WAP gateway. Check WSP protocol
                // availability.
                CLOG(( EHttpLoad, 3, _L("  Protocols available:") ));
                TInt i;
                TPtrC8 protocol;
                RPointerArray<HBufC8> protocols;
                RHTTPSession::ListAvailableProtocolsL( protocols );
                for( i = 0; i < protocols.Count(); i++ )
                    {
                    protocol.Set( protocols[i]->Des() );
                    CLOG(( EWapConn | ETcpConn, 3, _L8("  <%S>"), &protocol ));
                    if( !protocol.Compare( KCodWspProtocol ) )
                        {
                        // WSP available. Use a WAP session.
                        iSess = CHttpWapSession::NewL( *gateway );
                        break;
                        }
                    }
                }
            CleanupStack::PopAndDestroy( gateway ); // NULL is OK.
            }
        if( !iSess )
            {
            // No valid WAP gateway or WSP not available - use TCP session.
            iSess = CHttpTcpSession::NewL();
            }

        SetupSessionL();

        iSess->ConnectL( &iStatus );
        iState = EOpen;
        SetActive();
        }

    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::OpenSessionL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::RequestL()
// ---------------------------------------------------------
//
void CHttpLoader::RequestL()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::RequestL") ));
    __ASSERT_DEBUG( iState == EOpen, CodPanic( ECodOffState ) );
    __ASSERT_DEBUG( iSess, CodPanic( ECodInternal ) );
    if( !iTimeout )
        {
        iTimeout = CTimeout::NewL
            ( CActive::EPriorityStandard, TCallBack( StaticTimeout, this ) );
        }
    iHttpVersion = HTTP::EHttp11;
    CreateTransactionL();
    SubmitTransactionL();
    // Wait it out.
    // In ERequest state there is no other object that takes a request status
    // to complete. Instead, events come via MHF... callbacks, and there is
    // always a final event, when we complete our status manually.
    iState = ERequest;
    iStatus = KRequestPending;
    SetActive();
    StartTimeout(); // After state change! Timeout unexpected in other states.
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::RequestL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::Done()
// ---------------------------------------------------------
//
void CHttpLoader::Done()
    {
    CLOG(( EHttpLoad, 2, \
        _L("-> CHttpLoader::Done iResult(%d) iSuppressErrors(%d)"), \
        iResult, iSuppressErrors ));
    if( iTimeout )
        {
        iTimeout->Cancel();
        }
    iTrans.Close();
    if( iProxySet && iSess )
        {
        // Remove proxy address property from session.
        RHTTPConnectionInfo connInfo = iSess->Sess().ConnectionInfo();
        connInfo.RemoveProperty( StringF( HTTP::EUseProxy ) );
        connInfo.RemoveProperty( StringF( HTTP::EProxyAddress ) );
        iProxySet = EFalse;
        }
    if ( iSaver )
        {
        iSaver->CloseStore();
        }
    if( iINTimeout )
        {
        iINTimeout->Cancel();
        }
    iSaverFactory = NULL;
    iSaver = NULL;
    delete iNotifyBody;
    iNotifyBody = NULL;
    delete iUri;
    iUri = NULL;
    iMethod = HTTP::EGET;
    if ( iSuppressErrors )
        {
        iResult = KErrNone;
        iSuppressErrors = EFalse;
        }
    // Notify parent.
    __ASSERT_DEBUG( iParentStatus, CodPanic( ECodInternal ) );
    User::RequestComplete( iParentStatus, iResult );
    iParentStatus = NULL;
    iState = EInit;
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::Done") ));
    }

// ---------------------------------------------------------
// CHttpLoader::Continue
// ---------------------------------------------------------
//
void CHttpLoader::Continue( TState aNextState )
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::Continue nextState(%d)"), \
        aNextState ));
    __ASSERT_DEBUG( !IsActive(), CodPanic( ECodInternal ) );

    iState = aNextState;
    TRequestStatus* ownStatus = &iStatus;
    *ownStatus = KRequestPending;
    SetActive();
    User::RequestComplete( ownStatus, KErrNone );
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::Continue") ));
    }

// ---------------------------------------------------------
// CHttpLoader::SelfComplete
// ---------------------------------------------------------
//
void CHttpLoader::SelfComplete( TInt aError )
    {
    // This method safely handles the case when a request may be completed
    // from more then one place.
    // The main use is for loading (ERequest state), where
    // there is no real external request made and the request can complete
    // the following ways:
    // - Transaction finished
    // - Error (MHFRunError())
    // - Cancel
    // - Timeout
    CLOG(( EHttpLoad, 2, _L("CHttpLoader::SelfComplete(%d)"), aError ));
    if ( iStatus == KRequestPending )
        {
        // Request is pending, complete now.
        CLOG(( EHttpLoad, 4, _L("  completing now") ));
        TRequestStatus* ownStatus = &iStatus;
        User::RequestComplete( ownStatus, aError );
        }
    else
        {
        // Request already completed.
        // - If this second completion is error, override status.
        // - If this second completion is success, don't override - existing
        //   result may be error and we can't undo that.
        CLOG(( EHttpLoad, 4, _L("  already completed") ));
        if ( aError != KErrNone )
            {
            iStatus = aError;
            }
        }
    }
// -----------------------------------------------------------------------------
// CHttpLoader::SetRequestHeaderL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpLoader::SetRequestHeaderL( RStringPool& aStringPool,
                                       RHTTPHeaders& aHeaders)
    {
    // Set default Accept header
    SetHeaderL( aHeaders, HTTP::EAccept, HTTP::EAnyAny );

    // Some wap gateways update their database of MMS capable devices
    // *each time* the phone accesses the gateway. If these accept header is
    // only */* in content download, then the database will be updated so that
    // the terminal cannot receive MMS messages. As a result, MMS notifications
    // will not be sent to the phone and the user never receives MMS messages.
    // Overcome: add MMS and SI content types explicitly.
    SetHeaderL( aHeaders, HTTP::EAccept, KCodAcceptMmsHeaderValue );
    SetHeaderL( aHeaders, HTTP::EAccept, KCodAcceptSiHeaderValue );
    
    if ( iMethod == HTTP::EPOST )
        {
        // Content type header and body for POST.
        SetHeaderL( aHeaders, HTTP::EContentType, HTTP::ETextPlain );
        iTrans.Request().SetBody( *this );
        }

    // Find ETag in response header
    RStringF etag = aStringPool.StringF(HTTP::EETag, RHTTPSession::GetTable());
    TInt fieldInd = FindHeaderField( iResponseHeaders, etag.DesC() );
    
    if( fieldInd != KErrNotFound )
        // ETag is known. ETag identifies the content. Set If-Match to see
        // that if it's changed, or a redirection goes to another url.
        // Server will respond with 412 on error.
        {
        RStringF ifMatch = aStringPool.StringF(HTTP::EIfMatch, RHTTPSession::GetTable());
        aHeaders.RemoveField( ifMatch );
        aHeaders.SetRawFieldL( ifMatch, 
                               *(*iResponseHeaders)[fieldInd]->FieldRawData(), 
                               KHttpFieldSeparator );
        }

    SetRangeFieldL( aStringPool, aHeaders );
    }
// ---------------------------------------------------------
// CHttpLoader::CreateTransactionL()
// ---------------------------------------------------------
//
void CHttpLoader::CreateTransactionL()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::CreateTransactionL") ));
    __ASSERT_DEBUG( iUri, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iMethod == HTTP::EGET || iMethod == HTTP::EPOST, \
        CodPanic( ECodInternal ) );

#ifdef __TEST_COD_LOG
    TPtrC8 uriDes( iUri->Des() );
    CLOG(( EHttpLoad, 4, _L8("  method<%S> URI<%S>"), \
        &StringF( iMethod ).DesC(), &uriDes ));
#endif /* def __TEST_COD_LOG */

    // Create the transaction.
    TUriParser8 uri; 
    User::LeaveIfError( uri.Parse( *iUri ) );

    // escape the uri for characters defined in KUriEscChars
    HBufC8* escUri = NULL;
    TRAPD( err, escUri = EscapeUtils::SpecificEscapeEncodeL(*iUri, KUriEscChars) );
    if( err == KErrNone )
        {
        // switch buffers to use the the escape-encoded version
        delete iUri;
        iUri = escUri;
        User::LeaveIfError( uri.Parse( *iUri ) );
        }
    else
        {
        // if encoding function leaves, perform cleanup for safety
        // and attempt to continue with request
        delete escUri;  
        }
    iTrans = iSess->Sess().OpenTransactionL( uri, *this, StringF( iMethod ) );

    // Set request headers.
    RHTTPHeaders hdr = iTrans.Request().GetHeaderCollection();
    
    RStringPool strPool = iSess->Sess().StringPool();
    SetRequestHeaderL(strPool, hdr);
        
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::CreateTransactionL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::SubmitTransactionL()
// ---------------------------------------------------------
//
void CHttpLoader::SubmitTransactionL()
    {
#ifdef __TEST_COD_LOG
    CLOG(( EHttpLoad, 0, _L("-> CHttpLoader::SubmitTransactionL") ));
    RHTTPRequest req = iTrans.Request();
    TPtrC8 method( req.Method().DesC() );
    TPtrC8 uri( req.URI().UriDes() );
    CLOG(( EHttpLoad, 0, _L8("  method<%S> URI<%S>"), &method, &uri ));
    LogHeaders( req.GetHeaderCollection() );
#endif /* def __TEST_COD_LOG */
#ifdef _DEBUG
    HBufC* method16 = CodUtil::ConvertLC( iTrans.Request().Method().DesC() );
    User::InfoPrint( *method16 );
    CleanupStack::PopAndDestroy( method16 );
#endif

    if( !IsConnectionActive() )
        {
        User::Leave( KErrCodHttpCommsFailed );
        }
    CLOG(( EHttpLoad, 0, _L(" :calling RHttpTransaction.SubmitL()") ));
    iTrans.SubmitL();
    if( iMethod == HTTP::EPOST ) // ie, install-notify
        {
        if( !iINTimeout )
            {
            iINTimeout = CTimeout::NewL
                         ( CActive::EPriorityStandard, TCallBack( INStaticTimeout, this ) );
            }
        CLOG(( EHttpLoad, 1, _L(" :starting iINTimeout with timeout of %d microsecs"), KCodINTimeout ));
        TTimeIntervalMicroSeconds32 inTimeout = KCodINTimeout;
        iINTimeout->After( inTimeout );
        }
    CLOG(( EHttpLoad, 0, _L("<- CHttpLoader::SubmitTransactionL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::CompleteTransaction()
// ---------------------------------------------------------
//
void CHttpLoader::CompleteTransaction( TInt aError )
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::CompleteTransaction(%d)"), \
        aError ));
    __ASSERT_DEBUG( iState == ERequest, CodPanic( ECodOffState ) );
    iTrans.Close();
    iResult = aError;
    SelfComplete( iResult );
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::CompleteTransaction") ));
    }

// ---------------------------------------------------------
// CHttpLoader::RestartTransaction()
// ---------------------------------------------------------
//
void CHttpLoader::RestartTransaction()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::RestartTransaction")));
    __ASSERT_DEBUG( iState == ERequest, CodPanic( ECodOffState ) );
    
    iState = EStart;
    CompleteTransaction(KErrNone);
    
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::CompleteTransaction") ));
    }


// ---------------------------------------------------------
// CHttpLoader::AcceptRangesSupported()
// ---------------------------------------------------------
//
TBool CHttpLoader::AcceptRangesSupported()
	{
    RStringF range = iSess->Sess().StringPool().StringF(HTTP::EAcceptRanges, RHTTPSession::GetTable());
    THTTPHdrVal value;
    
    TInt index = FindHeaderField(iResponseHeaders, range.DesC());
    
    if( index  != KErrNotFound ) 
        {
        if( !(*iResponseHeaders)[index]->FieldRawData()->Compare( KAcceptRangeHeader() ) )
            {
            return ETrue;
            }
        }
        
    return EFalse;    
	}

// ---------------------------------------------------------
// CHttpLoader::HandleResponseHeadersL()
// ---------------------------------------------------------
//
void CHttpLoader::HandleResponseHeadersL( RHTTPResponse aResponse )
    {
    CLOG(( EHttpLoad, 0, _L("-> CHttpLoader::HandleResponseHeadersL") ));
    TInt httpCode = aResponse.StatusCode();
#ifdef __TEST_COD_LOG
    CLOG(( EHttpLoad, 0, _L8("  HttpStatus(%d) <%S>"), \
        httpCode, &(aResponse.StatusText().DesC()) ));
    LogHeaders( aResponse.GetHeaderCollection() );
#endif /* def __TEST_COD_LOG */

    if ( HTTPStatus::IsInformational( httpCode ) )
        {
        // 1xx
        // Informational messages. Do nothing.
        }
    else if ( httpCode == HTTPStatus::EOk ||
              httpCode == HTTPStatus::ENonAuthoritativeInfo ||
              httpCode == HTTPStatus::EPartialContent
            )
        {
        // 200 OK
        // 203 Non-Authoritative Information
        iResult = KErrNone;
        if ( iMethod == HTTP::EGET )
            {
            // Successful GET. Get a saver.
            __ASSERT_DEBUG( iSaverFactory, CodPanic( ECodInternal ) );

            if(!iSaver)
                {
                iSaver = iSaverFactory->CreateSaverL( GetContentTypeL( aResponse.GetHeaderCollection() ) );
                iSaver->SetSourceUriL( GetSourceUriL( iTrans ) );
                }
            if(httpCode != HTTPStatus::EPartialContent)
                {
                if( !iProgress->CurrentValue() )
                    {
                    iSaver->ResetL();
                    iProgress->SetAmountL(1024);    
                    }                
                }
            CheckRealDRMContentTypeL();
           	iCodEng->SetPausable(AcceptRangesSupported() && iCodEng->Pausable());

            // If we know the size, check it is valid and preallocate buffer
            // for content.
            if ( aResponse.HasBody() )
                {
                TInt dataSize = aResponse.Body()->OverallDataSize();
                if ( dataSize >= 0 )
                    {
                    // Content size is known. Check if fits.
                    User::LeaveIfError( iSaver->CheckMaxSize( dataSize ) );
                    }
                }
                iCodEng->SetResumePDAvailable();
            }
        else
            {
            // Successful POST.
            // Do nothing.
            }
        }
    else if ( HTTPStatus::IsSuccessful( httpCode ) )
        {
        // 2xx
        // Success codes without an usable body.
        // For GET, it is a failure; for POST it's OK
        iResult = (iMethod == HTTP::EGET) ? KErrCodHttpNoResponse : KErrNone;
        }
    // 3xx codes handled by redirect filter.
    else if ( httpCode == HTTPStatus::EUnauthorized ||
              httpCode == HTTPStatus::EProxyAuthenticationRequired )
        {
        // 401 Unauthorized
        // 407 Proxy authentication required
        iResult = KErrCodHttpAuthFailed;
        }
    else if ( httpCode == HTTPStatus::ENotFound ||
              httpCode == HTTPStatus::EGone )
        {
        // 404 Not found
        // 410 Gone
        iResult = KErrCodHttpBadUrl;
        
        // Cancel the download if bad url.
        // Set pausable status to false.
        iCodEng->SetPausable( EFalse );
        }
        
    else if ( httpCode == HTTPStatus::ERequestedRangeNotSatisfiable )
        {
        //416 Requested Range Not Satisfiable. Download has to be cancelled.
        iResult = KErrCodHttpRequestedRangeNotSatisfiable;
        }
    else if( httpCode == HTTPStatus::EPreconditionFailed )        
        {
        // Re init the download
        if(iSaver)
            {
            iSaver->ResetL();
            iProgress->SetAmountL(0);
            }
        CompleteTransaction( KErrCodHttpPreconditionFailed );
        }
    else if ( HTTPStatus::IsClientError( httpCode ) )
        {
        // 4xx
        iResult = KErrCodHttpUnavailable;
        }
    else if ( httpCode == HTTPStatus::EHTTPVersionNotSupported )
        {
        // 505 HTTP Version Not Supported
        // Retry with lower HTTP version if we can.
        iResult = VersionRetryL() ? KErrNone : KErrCodHttpUnavailable;
        }
    else if ( HTTPStatus::IsServerError( httpCode ) )
        {
        // 5xx
        // HTTP/1.0 servers may return other 5xx error codes for HTTP/1.1
        // requests. So the same treatment  is given for all 5xx errors
        // (version retry) - it's worth a try.
        iResult = VersionRetryL() ? KErrNone : KErrCodHttpServerError;
        }
    else
        {
        // Everything else.
        iResult = KErrCodHttpBadResponse;
        }
    CLOG(( EHttpLoad, 0, _L("<- CHttpLoader::HandleResponseHeadersL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::VersionRetryL()
// ---------------------------------------------------------
//
TBool CHttpLoader::VersionRetryL()
    {
    CLOG(( EHttpLoad, 2, \
        _L8("-> CHttpLoader::VersionRetryL iHttpVersion <%S>"), \
        &(StringF( iHttpVersion ).DesC() ) ));

    // We should be in ERequest state, with the request outstanding.
    __ASSERT_DEBUG( iState == ERequest, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iStatus == KRequestPending, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( IsActive(), CodPanic( ECodInternal ) );

    TBool retryDone( EFalse );
    if ( iHttpVersion == HTTP::EHttp11 )
        {
        // Currently using HTTP/1.1. Cancel transaction and resubmit it using
        // HTTP/1.0.
        iTrans.Cancel();
        iSess->Sess().ConnectionInfo().SetPropertyL
            (
            StringF( HTTP::EHTTPVersion ),
            THTTPHdrVal( StringF( HTTP::EHttp10 ) )
            );
        iHttpVersion = HTTP::EHttp10;
        SubmitTransactionL();
        retryDone = ETrue;
        }
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::VersionRetryL return(0x%x)"), \
        retryDone ));
    return retryDone;
    }

// ---------------------------------------------------------
// CHttpLoader::UseProxyL()
// ---------------------------------------------------------
//
TBool CHttpLoader::UseProxyL()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::UseProxyL iProxySet(0x%x)"), \
        iProxySet ));

    // We should be in ERequest state, with the request outstanding.
    __ASSERT_DEBUG( iState == ERequest, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iStatus == KRequestPending, CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( IsActive(), CodPanic( ECodInternal ) );
    __ASSERT_DEBUG( iTrans.Response().StatusCode() == HTTPStatus::EUseProxy, \
        CodPanic( ECodInvalidArguments ) );

    // First check if we already have a proxy address set. If yes, do not set
    // another.
    if( !iProxySet )
        {
        TBool proxySet( EFalse );   // Set by somebody else (?).
        RStringF proxyUsage = StringF( HTTP::EProxyUsage );
        RStringF useProxy = StringF( HTTP::EUseProxy );
        RHTTPConnectionInfo connInfo = iSess->Sess().ConnectionInfo();
        THTTPHdrVal val;
        if( connInfo.Property( proxyUsage, val ) )
            {
            if( val.Type() != THTTPHdrVal::KStrFVal )
                {
                User::Leave( KErrCodHttpBadResponse );
                }
            if( val.StrF() == useProxy )
                {
                proxySet = ETrue;
                CLOG(( EHttpLoad, 2, _L("  proxy already set") ));
                }
            }
        if( !proxySet )
            {
            // No proxy is currently set. Get the proxy address to set, from
            // the Location header field.
            RHTTPHeaders headers = iTrans.Response().GetHeaderCollection();
            if( !headers.GetField( StringF( HTTP::ELocation ), 0, val ) )
                {
                if( val.Type() != THTTPHdrVal::KStrFVal )
                    {
                    User::Leave( KErrCodHttpBadResponse );
                    }
                // Cancel the transaction and resubmit after the proxy was set.
                CLOG(( EHttpLoad, 2, _L8("  setting proxy <%S>"), \
                    &(val.StrF().DesC()) ));
                iTrans.Cancel();
                connInfo.SetPropertyL( StringF( HTTP::EProxyAddress ), val );
                connInfo.SetPropertyL( proxyUsage, THTTPHdrVal( useProxy ) );
                SubmitTransactionL();
                iProxySet = ETrue;
                }
            }
        }
    CLOG(( EHttpLoad, 2, \
        _L("<- CHttpLoader::UseProxyL return iProxySet(0x%x)"), iProxySet ));
    return iProxySet;
    }

// ---------------------------------------------------------
// CHttpLoader::SetupSessionL()
// ---------------------------------------------------------
//
void CHttpLoader::SetupSessionL()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::SetupSessionL") ));
    // Set our Socket Server handle and Connection as session properties.
    iSess->Sess().ConnectionInfo().SetPropertyL
        (
        StringF( HTTP::EHttpSocketServ ),
        THTTPHdrVal( iConn.SockServ().Handle() )
        );
    iSess->Sess().ConnectionInfo().SetPropertyL
        (
        StringF( HTTP::EHttpSocketConnection ),
        THTTPHdrVal( REINTERPRET_CAST( TInt, &iConn.Conn() ) )
        );

    // Set the disconnect notification 
    iSess->Sess().ConnectionInfo().SetPropertyL
        (
        iSess->Sess().StringPool().StringF( HTTP::ENotifyOnDisconnect, RHTTPSession::GetTable() ), 
        iSess->Sess().StringPool().StringF( HTTP::EEnableDisconnectNotification, RHTTPSession::GetTable() )
        ); 

    CLOG(( EHttpLoad, 4, _L("  Install UAProf filter...") ));
    // Install UAProf filter.
    CHttpUAProfFilterInterface::InstallFilterL( iSess->Sess() );

    CLOG(( EHttpLoad, 4, _L("  Install Cookie filter...") ));
    // Install cookie filter.
    CHttpCookieFilter::InstallFilterL( iSess->Sess() );

    CLOG(( EHttpLoad, 4, _L("  Install Auth filter...") ));
    // Install authentication filter.
    InstallAuthenticationL( iSess->Sess() );

    CLOG(( EHttpLoad, 4, _L("  Install Deflate filter...") ));
    // Install deflate Filter.
    CHttpDeflateFilter::InstallFilterL( iSess->Sess() );

    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::SetupSessionL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::StartTimeout()
// ---------------------------------------------------------
//
void CHttpLoader::StartTimeout()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::StartTimeout") ));
    __ASSERT_DEBUG( iState == ERequest, CodPanic( ECodOffState ) );
    __ASSERT_DEBUG( iTimeout, CodPanic( ECodInternal ) );
    TTimeIntervalMicroSeconds32 timeout = 
        iMethod == HTTP::EGET ? KCodGetTimeout : KCodPostTimeout;
    iTimeout->Cancel();         // Cancel pending (if any).
    iTimeout->After( timeout ); // Start over.
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::StartTimeout (%d microsecs)"), \
        timeout.Int() ));
    }

// ---------------------------------------------------------
// CHttpLoader::Timeout()
// ---------------------------------------------------------
//
void CHttpLoader::Timeout()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::Timeout iRetry(%d)"), iRetry ));
    __ASSERT_DEBUG( iState == ERequest, CodPanic( ECodOffState ) );
    if ( --iRetry > 0 )
        {
        CompleteTransaction( KErrNone );    // Close (abandon) transaction.
        iState = EOpen;                     // To RequestL() by self-complete.
        }
    else
        {
		if (iCodLoadObserver)
			{
			iCodLoadObserver->SetConnError( KErrTimedOut);
			}
        CompleteTransaction( KErrTimedOut );
        }
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::Timeout") ));
    }

// ---------------------------------------------------------
// CHttpLoader::INTimeout()
// ---------------------------------------------------------
//
void CHttpLoader::INTimeout()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::INTimeout") ));
    __ASSERT_DEBUG( iState == ERequest, CodPanic( ECodOffState ) );
    iSuppressErrors = ETrue;    // release content to user 
    if( !IsConnectionActive() )
        {
        iSuppressErrors = EFalse;         // do not release content
        iResult = KErrCodHttpCommsFailed; // set error code
        }
    CLOG(( EHttpLoad, 2, _L(" :iSuppressErrors = %d"), iSuppressErrors ));
    CLOG(( EHttpLoad, 2, _L(" :iResult = %d"), iResult ));
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::INTimeout") ));
    }

// ---------------------------------------------------------
// CHttpLoader::IncProgressL()
// ---------------------------------------------------------
//
void CHttpLoader::IncProgressL( TInt aSize )
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::IncProgressL(%d)"), aSize ));
    if ( iProgress )
        {
        iProgress->IncrementL( aSize );
        }
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::IncProgressL") ));
    }

// ---------------------------------------------------------
// CHttpLoader::SetHeaderL()
// ---------------------------------------------------------
//
void CHttpLoader::SetHeaderL
( RHTTPHeaders aHeaders, HTTP::TStrings aHdrField, const TDesC8& aHdrValue )
    {
    RStringF valStr = iSess->Sess().StringPool().OpenFStringL( aHdrValue );
    CleanupClosePushL<RStringF>( valStr );
    SetHeaderL( aHeaders, aHdrField, valStr );
    CleanupStack::PopAndDestroy();  // close valStr
    }

// ---------------------------------------------------------
// CHttpLoader::SetHeaderL()
// ---------------------------------------------------------
//
void CHttpLoader::SetHeaderL
( RHTTPHeaders aHeaders, HTTP::TStrings aHdrField, HTTP::TStrings aHdrValue )
    {
    SetHeaderL( aHeaders, aHdrField, StringF( aHdrValue ) );
    }

// ---------------------------------------------------------
// CHttpLoader::SetHeaderL()
// ---------------------------------------------------------
//
void CHttpLoader::SetHeaderL
( RHTTPHeaders aHeaders, HTTP::TStrings aHdrField, const RStringF aHdrValue )
    {
    CLOG(( EHttpLoad, 2, _L8("CHttpLoader::SetHeaderL <%S> <%S>"), \
        &StringF( aHdrField ).DesC(), &aHdrValue.DesC() ));
    THTTPHdrVal val( aHdrValue );
    aHeaders.SetFieldL( StringF( aHdrField ), val );
    }

// ---------------------------------------------------------
// CHttpLoader::GetContentTypeL()
// ---------------------------------------------------------
//
const TDesC8& CHttpLoader::GetContentTypeL( RHTTPHeaders aHeaders )
    {
    THTTPHdrVal hdrVal;
    User::LeaveIfError
        ( aHeaders.GetField( StringF( HTTP::EContentType ), 0, hdrVal ) );
    if( hdrVal.Type() != THTTPHdrVal::KStrFVal )
        {
        User::Leave( KErrCodHttpBadResponse );
        }
    return hdrVal.StrF().DesC();
    }

// ---------------------------------------------------------
// CHttpLoader::GetContentTypeL()
// ---------------------------------------------------------
//
const TDesC8& CHttpLoader::GetContentTypeL()
    {
    _LIT8(KContentType, "Content-Type");

    THTTPHdrVal value;
    
    TInt index = FindHeaderField(iResponseHeaders, KContentType);
    
    if( index  != KErrNotFound ) 
        {
        HBufC8 *ptr = (*iResponseHeaders)[index]->FieldRawData();
        return *ptr;
        }

    return KNullDesC8;
    }

// ---------------------------------------------------------
// CHttpLoader::GetSourceUriL()
// ---------------------------------------------------------
//
const TDesC8& CHttpLoader::GetSourceUriL( RHTTPTransaction aTransaction )
    {
    // TODO if redirected URI exists, how do we get it?
    // For the moment request URI is used.
    return aTransaction.Request().URI().UriDes();
    }

// ---------------------------------------------------------
// CHttpLoader::StaticTimeout()
// ---------------------------------------------------------
//
TInt CHttpLoader::StaticTimeout( TAny* aPtr )
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::StaticTimeout") ));
    STATIC_CAST( CHttpLoader*, aPtr )->Timeout();
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::StaticTimeout") ));
    return EFalse;
    }

// ---------------------------------------------------------
// CHttpLoader::INStaticTimeout()
// ---------------------------------------------------------
//
TInt CHttpLoader::INStaticTimeout( TAny* aPtr )
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::INStaticTimeout") ));
    STATIC_CAST( CHttpLoader*, aPtr )->INTimeout();
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::INStaticTimeout") ));
    return EFalse;
    }


// ---------------------------------------------------------
// CHttpLoader::IsConnectionActive()
// ---------------------------------------------------------
//
TBool CHttpLoader::IsConnectionActive()
    {
    CLOG(( EHttpLoad, 2, _L("-> CHttpLoader::IsConnectionActive") ));

    THTTPHdrVal val;
    TBool active( ETrue );
    TBool hasProp = iSess->Sess().ConnectionInfo().Property
        ( StringF( HTTP::EHttpSocketConnection ), val );
    if( hasProp && val.Type() == THTTPHdrVal::KTIntVal)
        {
        active = ( val.Int() != NULL );
        CLOG(( EHttpLoad, 0, _L(" :RConnection found = %d"), active ));
        }    

    if( active ) // confirm with second level check
        {
        TUint32 iap;
        active = iConn.IsConnected( iap );
        CLOG(( EHttpLoad, 3, _L(" :CConnection.IsConnection() = %d"), active ));
        }
    CLOG(( EHttpLoad, 2, _L("<- CHttpLoader::IsConnectionActive") ));
    return active;
    }
    
// -----------------------------------------------------------------------------
// CHttpDownload::StoreResponseHeaderL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpLoader::StoreResponseHeaderL()
	{

    TPtrC8 rawData;
    RStringPool strPool = iSess->Sess().StringPool();

    RHTTPHeaders headers( iTrans.Response().GetHeaderCollection() );
    THTTPHdrFieldIter it = headers.Fields();

    // forget the previous headers
    iResponseHeaders->ResetAndDestroy();
    
    while ( !it.AtEnd() )
        {
        RStringTokenF fieldName = it();
        RStringF fieldNameStr = strPool.StringF (fieldName );

        headers.GetRawField( fieldNameStr, rawData );

        CHeaderField* field = CHeaderField::NewL( &fieldNameStr.DesC(), &rawData );
        CleanupStack::PushL( field );

        iResponseHeaders->AppendL( field );

        CleanupStack::Pop( field );

        ++it;
        }    

    //ParseContentTypeL( strPool );

    RStringF length = strPool.StringF(HTTP::EContentLength,RHTTPSession::GetTable());
    RStringF date = strPool.StringF(HTTP::EDate,RHTTPSession::GetTable());
    RStringF expires = strPool.StringF(HTTP::EExpires,RHTTPSession::GetTable());
    RStringF maxAge = strPool.StringF(HTTP::EMaxAge,RHTTPSession::GetTable());
    RStringF cacheControl = strPool.StringF(HTTP::ECacheControl,RHTTPSession::GetTable());
    RStringF acceptRanges = strPool.StringF(HTTP::EAcceptRanges,RHTTPSession::GetTable());
    
    THTTPHdrVal value;
    /*
    if( !headers.GetField( length, 0, value ) )
        {
        if( iStorage->Length() == KDefaultContentLength )
            // content size is
            {
            iStorage->SetLength( value );
            }
        }
             
    CheckRealDRMContentTypeL();
    if( !iDrmContentLengthValid )
        // Content was original encoded -> we don't know the actual content size.
        {
        iStorage->SetLength( KDefaultContentLength );
        }*/ 
        
		CheckRealDRMContentTypeL();
		
    iMaxAge = 0;
    TInt parts( 0 );
    // this leave is trapped because we can still go on
    TRAPD( err, parts = headers.FieldPartsL( cacheControl ) );

    if( !err )
        // try to find max-age in Cache-control field
        {
        for( TInt i = 0; i < parts; ++i )
            {
            RStringF directive;
            THTTPHdrVal hdrVal;
            TInt err;

            // Get the cache-control from the headers
            // initialise the fieldname
            headers.GetField( cacheControl, i, hdrVal );

            if((hdrVal.Type() == THTTPHdrVal::KStrVal) || (hdrVal.Type() == THTTPHdrVal::KStrFVal))
                {
                RStringF cacheDir = hdrVal.StrF();

                TInt endPos;
                _LIT8(KFind, "=");

                endPos = cacheDir.DesC().Find( KFind );
                if( endPos != -1 )
                    {
                    TRAP(err, directive = strPool.OpenFStringL(cacheDir.DesC().Left(endPos)));
                    if( !err )
                        {
                        if( directive == maxAge )
                            {
                            TInt valueInt( 0 );
                            TLex8 value( cacheDir.DesC().Right(cacheDir.DesC().Length() - endPos - 1) );

                            value.Val( valueInt );
                            iMaxAge = valueInt;
                            }
                        }
                    }
                }
            }
        }

    if( !headers.GetField( expires, 0, value ) )
        {
        iExpires = value;
        }
    else
        {
        iExpires.SetYear( 0 );
        }

    if( !headers.GetField( date, 0, value ) )
        {
        iDate = value;
        }
    else
        {
        iDate.SetYear( 0 );
        }
	}


	
void CHttpLoader::LoadHeadersL( RFile& aFile)
	{
    TInt headers;
    READ_INT_L( aFile, headers );

    iResponseHeaders->ResetAndDestroy();

    for( TInt i = 0; i < headers; ++i )
        {
        CHeaderField* field = CHeaderField::NewL();
        CleanupStack::PushL( field );

        field->LoadHeaderInfoL( aFile );
        iResponseHeaders->AppendL( field );

        CleanupStack::Pop( field );
        }	
	}
	
void CHttpLoader::SetRangeFieldL( RStringPool& aStringPool,
                                    RHTTPHeaders& aHeaders )
    {
    
    if(!iSaver)
    {
        return;
    }
    
    TInt size (iSaver->DownloadedFileSize());
    if( iSaver->DataType().Des8().Find( KFotaPackageDataType ) != KErrNotFound ) 
        {
        iCodEng->UpdateDownloadedSize( size );
        IncProgressL(size);
        }
   
    if( size <= 0 )
        {
		// no bytes have been downloaded yet
        return;
        }

    RStringF range = aStringPool.StringF(HTTP::ERange, RHTTPSession::GetTable());

    aHeaders.RemoveField( range );

    TBuf8<48> rawData;

    rawData.Format( _L8("bytes=%d-"), size);

    aHeaders.SetRawFieldL( range, rawData, KHttpFieldSeparator );
    }
    
// -----------------------------------------------------------------------------
// CHttpLoader::FindHeaderField
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
TInt CHttpLoader::FindHeaderField( CArrayPtrFlat<CHeaderField>* aHeaders,
                                     const TDesC8& aFieldName ) const
    {
    for( TInt index = 0; index < aHeaders->Count(); ++index )
        {
        if( *(*aHeaders)[index]->FieldName() == aFieldName )
            {
            return index;
            }
        }

    return KErrNotFound;
    }
	
void CHttpLoader::CheckRealDRMContentTypeL()
    {
    iPausableDRM = ETrue;
    iDrmContentLengthValid = ETrue;
    
    TInt index = FindHeaderField( iResponseHeaders, KDRMOldContentType );
    if( index != KErrNotFound )
        // this is an old DRM protected content
        // This transaction cannot be paused.
        {
        if( !(*iResponseHeaders)[index]->FieldRawData()->Compare( KDrmMessageMimeType() ) )
            {
            iPausableDRM = EFalse;
            }
        }
        
    UpdatePausableL();
    }	

// -----------------------------------------------------------------------------
// CHttpLoader::UpdatePausableL
// ?implementation_description
// (other items were commented in a header).
// -----------------------------------------------------------------------------
//
void CHttpLoader::UpdatePausableL()
    {
    TBool pausable( ETrue );

    if( !iPausableDRM )
        {
        pausable = EFalse;
        }
        
    if( iMethod == EMethodPOST )
        {
        pausable = EFalse;
        }

    if( pausable != iCodEng->Pausable() )
        {
        iCodEng->SetPausable( pausable );
        
        //TODO : Shud the change in pause behavior shud be notified. ??
        // inform client about change
        //TriggerEvent( iPausable ? EHttpDlPausable : EHttpDlNonPausable );

        //TRAP_IGNORE(StoreDownloadInfoL() );
        iCodEng->StoreSubInfoFileL(iResponseHeaders, iCodEng->Data().ActiveDownload() );
        }
    }    

// ---------------------------------------------------------
// CHttpLoader::LogHeaders()
// ---------------------------------------------------------
//
void CHttpLoader::LogHeaders( RHTTPHeaders LOG_ONLY( aHeaders ) )
    {
#ifdef __TEST_COD_LOG
    _LIT(KDateFormat,"%D%M%Y%/0%1%/1%2%/2%3%/3 %:0%H%:1%T%:2%S.%C%:3");

    CLOG(( EHttpLoad, 1, _L("Headers:") ));

    TInt i;
    TInt fieldParts;
    RStringPool strP = iTrans.Session().StringPool();
    THTTPHdrFieldIter it = aHeaders.Fields();

    while ( !it.AtEnd() )
        {
        RStringTokenF fieldName = it();
        RStringF fieldNameStr = strP.StringF (fieldName );
        THTTPHdrVal fieldVal;
        fieldParts = 0; // For the case if next the call fails.
        TRAP_IGNORE( fieldParts =  aHeaders.FieldPartsL( fieldNameStr ) );
        for ( i = 0; i < fieldParts; i++ )
            {
            if ( aHeaders.GetField( fieldNameStr, i, fieldVal ) == KErrNone )
                {
                const TDesC8& fieldNameDesC = fieldNameStr.DesC();
                switch ( fieldVal.Type() )
                    {
                    case THTTPHdrVal::KTIntVal:
                        {
                        CLOG(( EHttpLoad, 1, _L8("  <%S> (%d)"), \
                            &fieldNameDesC, fieldVal.Int() ));
                        break;
                        }

                    case THTTPHdrVal::KStrFVal:
                        {
                        RStringF fieldValStr = strP.StringF( fieldVal.StrF() );
                        const TDesC8& fieldValDesC = fieldValStr.DesC();
                        CLOG(( EHttpLoad, 1, _L8("  <%S> <%S>"), \
                            &fieldNameDesC, &fieldValDesC ));
                        }
                        break;

                    case THTTPHdrVal::KStrVal:
                        {
                        RString fieldValStr = strP.String( fieldVal.Str() );
                        const TDesC8& fieldValDesC = fieldValStr.DesC();
                        CLOG(( EHttpLoad, 1, _L8("  <%S> <%S>"), \
                            &fieldNameDesC, &fieldValDesC ));
                        }
                        break;

                    case THTTPHdrVal::KDateVal:
                        {
                        TDateTime date = fieldVal.DateTime();
                        TBuf<40> dateTimeString;
                        TTime t( date );
                        TRAP_IGNORE\
                            ( t.FormatL( dateTimeString, KDateFormat ) );
                        TBuf8<40> dateTimeString8;
                        dateTimeString8.Copy( dateTimeString );
                        CLOG(( EHttpLoad, 1, _L8("  <%S> <%S>"), \
                            &fieldNameDesC, &dateTimeString8 ));
                        } 
                        break;

                    default:
                        CLOG(( EHttpLoad, 1, \
                            _L8("  <%S> unrecognised value type(%d)"), \
                            &fieldNameDesC, fieldVal.Type() ));
                        break;
                    }

                // Display realm for WWW-Authenticate header.
                RStringF wwwAuth = strP.StringF
                    ( HTTP::EWWWAuthenticate, RHTTPSession::GetTable() );
                if ( fieldNameStr == wwwAuth )
                    {
                    // check the auth scheme is 'basic'
                    RStringF basic = strP.StringF
                        ( HTTP::EBasic, RHTTPSession::GetTable() );
                    RStringF realm = strP.StringF
                        ( HTTP::ERealm, RHTTPSession::GetTable() );
                    THTTPHdrVal realmVal;
                    if ( ( fieldVal.StrF() == basic ) && 
                        ( !aHeaders.GetParam( wwwAuth, realm, realmVal ) ) )
                        {
                        RString realmValStr = strP.String( realmVal.Str() );
                        const TDesC8& realmValDesC = realmValStr.DesC();
                        CLOG(( EHttpLoad, 1, _L8("    Realm<%S>"), \
                            &realmValDesC ));
                        }
                    }
                }
            }
        ++it;
        }
#endif /* def __TEST_COD_LOG */
    }


// -----------------------------------------------------------------------------
// CHttpLoader::RedirectedPermanentlyL
// -----------------------------------------------------------------------------
//
void CHttpLoader::RedirectedPermanentlyL( const TDesC8& aNewUrl )
    {
    CLOG(( EHttpLoad, 0, _L("CHttpLoader::RedirectedPermanentlyL => NewUrl(%S)"), \
                                                            &aNewUrl ));

	// Use the redirected Url
	ReallocateStringL( iUri, aNewUrl, KMaxUrlLength );
	
	// There has already been a temporary redirection
    // This permanent is not used on next submitted request 
	if (!iRedirect)
		{
		HBufC* newUrl = CodUtil::ConvertLC( aNewUrl );
		iCodEng->SetUrlL( *newUrl );
		
		CleanupStack::PopAndDestroy( newUrl );
		}
    }

// -----------------------------------------------------------------------------
// CHttpLoader::RedirectedTemporaryL
// -----------------------------------------------------------------------------
//
void CHttpLoader::RedirectedTemporaryL( const TDesC8& aNewUrl )
    {
    CLOG(( EHttpLoad, 0, _L("CHttpLoader::RedirectedTemporaryL => NewUrl(%S)"), \
                                                            &aNewUrl ));

	// Make it TRUE so that any permanent redirections after
	// this are not saved
	iRedirect = ETrue;
	ReallocateStringL( iUri, aNewUrl, KMaxUrlLength );
    }

// -----------------------------------------------------------------------------
// CHttpLoader::ResponseHeaders
// -----------------------------------------------------------------------------
//
CArrayPtrFlat<CHeaderField>* CHttpLoader::ResponseHeaders()
	{
	return iResponseHeaders;
	}
