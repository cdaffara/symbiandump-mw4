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
* Description:           
*
*/











// INCLUDE FILES
#include <http.h>

#include "sentxnstate.h"
#include "senhttpchannel.h"
#include "sendebug.h"
#include "senlogger.h"
#include "senasynctimeout.h"


// MACROS

// =========================== MEMBER FUNCTIONS ===============================

// ----------------------------------------------------------------------------
// CSenTxnState::CSenTxnState
// C++ default constructor can NOT contain any code, that
// might leave.
// ----------------------------------------------------------------------------
//
CSenTxnState::CSenTxnState(MSenResponseObserver& aObserver)//,
                           //RFileLogger* aLog)
    : iObserver(&aObserver),
      //iLog(aLog),
      iHasRequestBody(EFalse),
      iPostBodyReleased(EFalse)
    {
    }

// ----------------------------------------------------------------------------
// CSenTxnState::NewLC
// ----------------------------------------------------------------------------
//

CSenTxnState* CSenTxnState::NewLC(MSenResponseObserver& aObserver,
                                  //RFileLogger* aLog,
                                  const TDesC8* aUri,
                                  const TDesC8& aContentType,
                                  const TDesC8* aBody)
    {
    CSenTxnState* pNew = new (ELeave) CSenTxnState(aObserver);//, aLog);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aUri, aContentType, aBody);
    return pNew;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::NewL
// ----------------------------------------------------------------------------
//

CSenTxnState* CSenTxnState::NewL(MSenResponseObserver& aObserver,
                                 //RFileLogger* aLog,
                                 const TDesC8* aUri,
                                 const TDesC8& aContentType,
                                 const TDesC8* aBody)
    {
    CSenTxnState* pNew = NewLC( aObserver, /*aLog,*/ aUri,
                                aContentType, aBody );
    CleanupStack::Pop(pNew);
    return pNew;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::ConstructL
// ----------------------------------------------------------------------------
//

void CSenTxnState::ConstructL(const TDesC8* aUri,
                              const TDesC8& aContentType,
                              const TDesC8* aBody)
    {
    __ASSERT_ALWAYS(aUri != NULL,
                    User::Panic(KRequestUriNullPanicText,
                                CSenHttpChannel::ERequestUriNull));
    iRequestUri = aUri->AllocL();

    if(aContentType.Length()>0)
        {
        if(aBody && aBody->Length() > 0)
            {
            iPostBody = aBody->AllocL();
            iPostContentType = aContentType.AllocL();
            iHasRequestBody = ETrue;
            }
        }
    }

// ----------------------------------------------------------------------------
// CSenTxnState::~CSenTxnState
// ----------------------------------------------------------------------------
//
CSenTxnState::~CSenTxnState()
    {
    delete iPostBody;
    delete iRequestUri;
    delete iPostContentType;
    
    // delete allocated body
    delete iResponseBody; 
    delete iTP;
    delete iTimeOut;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::Id
// ----------------------------------------------------------------------------
//
TInt CSenTxnState::Id() const
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenTxnState::Id")));
    return iId;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::SetId
// ----------------------------------------------------------------------------
//
void CSenTxnState::SetId(TInt aId)
    {
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenTxnState::SetId( %d )"),aId));
    iId = aId;
    }

void CSenTxnState::SetTransaction(RHTTPTransaction aTransaction)
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenTxnState::SetTransaction");
    ipTransaction = aTransaction;
    }
    
RHTTPTransaction CSenTxnState::Transaction() const
	{
	TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenTxnState::Transaction")));
    return ipTransaction;
	}

TInt CSenTxnState::Cancel()
    {
    if (iTimeOut)
    	iTimeOut->Cancel();
    ipTransaction.Cancel();
    return KErrNone;
    }
void CSenTxnState::SetSession(RHTTPSession aSession) 
    {
    TLSLOG_L(KSenHttpChannelLogChannelBase , KSenHttpChannelLogLevel,"CSenTxnState::SetSession");
    iSession = aSession;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::CollectResponseBodyL
// ----------------------------------------------------------------------------
//
void CSenTxnState::CollectResponseBodyL(const TDesC8& aDataPart)
    {
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenTxnState(%d)::CollectResponseBodyL"),iId));
    // Allocate memory for the data
    if(!iResponseBody)
        {
        // this is the first call so create the buffer to retain response data
        iResponseBody = HBufC8::NewMaxL(0);
        }

    iResponseBody = iResponseBody->ReAllocL(iResponseBody->Length() +
                                            aDataPart.Length());

    // Note: iResponseContent will be delete when this
    // Transaction (txn) is oved (deleted)

    // Append the new data to the buffer
    TPtr8 ptrResponseBody = iResponseBody->Des();
    ptrResponseBody.Append(aDataPart);
    iDetails.iTotalBytesRecieved += iResponseBody->Size(); 
    }

// ----------------------------------------------------------------------------
// CSenTxnState::RequestUri
// ----------------------------------------------------------------------------
//
const TDesC8& CSenTxnState::RequestUri() const
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KNormalLogLevel,(_L("CSenTxnState::RequestUri")));
    __ASSERT_ALWAYS(iRequestUri != NULL,
                    User::Panic(KRequestUriNullPanicText,
                    CSenHttpChannel::ERequestUriNull));
    return *iRequestUri;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::PostContentType
// ----------------------------------------------------------------------------
//
const TDesC8& CSenTxnState::PostContentType() const
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenTxnState::PostContentType")));
    __ASSERT_ALWAYS(iPostContentType != NULL,
                    User::Panic(KPostContTypeNullPanicText,
                    CSenHttpChannel::EPostContentTypeNull));
    return *iPostContentType;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::SetContentTypeHeaderL
// ----------------------------------------------------------------------------
//
void CSenTxnState::SetContentTypeHeaderL(const RHTTPSession aSession, RHTTPHeaders aHeaders)
	{
    RStringF contTypeStr = aSession.StringPool().OpenFStringL(PostContentType());
    CleanupClosePushL(contTypeStr);
    THTTPHdrVal contType(contTypeStr);
    aHeaders.SetFieldL(aSession.StringPool().StringF(
                       HTTP::EContentType, RHTTPSession::GetTable()), contType);
    // Close stringpool string
    CleanupStack::PopAndDestroy(); // contTypeStr
	}

// ----------------------------------------------------------------------------
// CSenTxnState::ResponseReceivedL
// ----------------------------------------------------------------------------
//
void CSenTxnState::ResponseReceivedL( const TDesC8& aContentType )
    {
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenTxnState(%d)::ResponseReceivedL"), iId));
    // Find the header size for the response aswell
    RHTTPHeaders responseHdr = ipTransaction.Response().GetHeaderCollection(); 
    TInt responseHeaderSize = CalculateHeadersSizeL(responseHdr);
    iDetails.iTotalBytesRecieved += responseHeaderSize ;
	iObserver->SetTrafficDetails(iDetails) ;                

    if( iResponseBody )
        {
        TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMaxLogLevel, _L8("- iResponseBody:")));
        TLSLOG_ALL(KSenHttpChannelLogChannelBase , KMaxLogLevel,(iResponseBody->Des()));
        HBufC8* pResponse = iResponseBody;
        iResponseBody = NULL;
        iObserver->ResponseReceivedL( iId, &aContentType, pResponse  );
        }
    else
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KNormalLogLevel,"CSenTxnState::ResponseReceivedL - NULL response received!");
        iObserver->ResponseReceivedL( iId, &aContentType, NULL );
        }
    }
MSenProperties& CSenTxnState::HttpChannelPropertiesL()
    {
    return iObserver->PropertiesL(); 
    }

void CSenTxnState::SetTP(CSenHttpTransportProperties* tp)
    {
    
    delete iTP;
    iTP = tp;
    }
TInt CSenTxnState::CalculateHeadersSizeL(RHTTPHeaders aHeaders) 
    { 
    TInt headersize(0); 
    TBuf8<1024> buf; 
    TPtrC8 fieldValue(buf.Ptr()); 
    _LIT8(KColon,": ");   
    _LIT8(KNewLine,"\r\n"); 

		RStringPool stringPool;
    // Store the string pool for this HTTP session
    stringPool = iSession.StringPool();

    THTTPHdrFieldIter fields = aHeaders.Fields(); 
    fields.First(); 
    while (!fields.AtEnd()) 
        { 
        RStringF str = stringPool.StringF(fields()); 
        
        headersize += str.DesC().Size(); 
 
        headersize += KColon().Size(); 
        
        aHeaders.GetRawField(str, fieldValue);          
          
        headersize += fieldValue.Size(); 
        headersize += KNewLine().Size(); 
          
        ++fields; 
        } 
       
    return headersize; 
    } 
// ----------------------------------------------------------------------------
// CSenTxnState::ResponseErrorL
// ----------------------------------------------------------------------------
//
void CSenTxnState::ResponseErrorL(TInt aError)
    {
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenTxnState(%d)::ResponseErrorL( %d )"), iId, aError));
    RHTTPHeaders ResponseHdr = ipTransaction.Response().GetHeaderCollection(); 
    TInt ResponseHeaderSize = CalculateHeadersSizeL(ResponseHdr);
    iDetails.iTotalBytesRecieved += ResponseHeaderSize ;
	iObserver->SetTrafficDetails(iDetails) ;	                
    if(iResponseBody)
        {
        HBufC8* pError = iResponseBody;
        iResponseBody = NULL;
        iObserver->ResponseErrorL(iId, aError, pError, iTP);
        }
    else
        {
        TLSLOG_L(KSenHttpChannelLogChannelBase , KMinLogLevel,"CSenTxnState::ResponseErrorL - NULL error response received!");
        iObserver->ResponseErrorL(iId, aError, NULL);
        }
    }
void CSenTxnState::StateChanged(TInt aState) // Propagate http status code to remote service consumer
	{
	TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KMinLogLevel, _L8("CSenTxnState(%d)::StateChanged( %d )"), iId, aState));
    iObserver->StateChanged(iId,aState);	
	}

// ----------------------------------------------------------------------------
// CSenTxnState::GetNextDataPart
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
TBool CSenTxnState::GetNextDataPart(TPtrC8& aDataPart)
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenTxnState::GetNextDataPart")));
    if(iPostBody && (!iPostBodyReleased))
        {
        aDataPart.Set(*iPostBody);
        }
    else
        {
        aDataPart.Set(NULL, 0);
        }
    iDetails.iTotalBytesSent += aDataPart.Size();

    return ETrue;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::ReleaseData
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
void CSenTxnState::ReleaseData()
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenTxnState::ReleaseData")));
    // Don't delete the submit buffer here because call to
    // Reset always needs to start from the beginning of the buffer.
    iPostBodyReleased = ETrue;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::OverallDataSize
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
TInt CSenTxnState::OverallDataSize()
    {
    TInt size = 0;
    if(iPostBody)
        {
        size = iPostBody->Length();
        }
    TLSLOG_FORMAT((KSenHttpChannelLogChannelBase , KNormalLogLevel, _L8("CSenTxnState::OverallDataSize() [%d]"), size));
    return size;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::Reset
// Implementation of the pure virtual method from MHTTPDataSupplier
// ----------------------------------------------------------------------------
//
TInt CSenTxnState::Reset()
    {
    TLSLOG(KSenHttpChannelLogChannelBase , KMinLogLevel,(_L("CSenTxnState::Reset")));
    iPostBodyReleased = EFalse;
    return KErrNone;
    }

// ----------------------------------------------------------------------------
// CSenTxnState::Body
// Returns reference to txn body, or KNullDesC8, if
// body has not been set.
// ----------------------------------------------------------------------------
//
TPtrC8 CSenTxnState::Body()
    {
    if(iPostBody)
        {
        return *iPostBody;
        }
    else
        {
        return KNullDesC8();
        }
    }

void CSenTxnState::TransformBodyToUriL()
    {
    if(!iPostBody || iPostBody->Length()==0)
        {
        return; // nothing to do
        }

    TPtrC8 uri(KNullDesC8);
    if(iRequestUri)
        {
        uri.Set(*iRequestUri);
        }

    HBufC8* pUri = HBufC8::NewLC(uri.Length()+iPostBody->Length());
    TPtr8 ptrUri = pUri->Des();
    ptrUri.Append(uri);
    ptrUri.Append(*iPostBody);
    
    // replace current uri with the new one:
    delete iRequestUri;
    iRequestUri = pUri;
    CleanupStack::Pop();

    delete iPostBody;
    iPostBody = NULL;
    iHasRequestBody = EFalse;
    TLSLOG_L(KSenHttpChannelLogChannelBase , KMaxLogLevel,"CSenTxnState::TransformBodyToUriL():");

    TLSLOG_ALL(KSenHttpChannelLogChannelBase , KMaxLogLevel,(*iRequestUri));
    }

void CSenTxnState::EnableTimeOutL(TInt aTimeOutSec)
    {
    delete iTimeOut;
    iTimeOut = NULL;
    iTimeOut = CSenAsyncTimeOut::NewL(this);
    iTimeOut->StartL(aTimeOutSec);
    }

void CSenTxnState::DisableTimeOutL()
    {
    delete iTimeOut;
    iTimeOut = NULL;
    }
//  End of File
