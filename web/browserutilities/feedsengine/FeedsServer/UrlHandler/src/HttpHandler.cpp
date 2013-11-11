/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class that fetches resources via HTTP 1.1.
*
*/


#include <escapeutils.h>
#include <http.h>
#include <http/mhttpdatasupplier.h>
#include <httpfiltercommonstringsaddition.h>

#include "HttpConnection.h"
#include "HttpHandler.h"
#include "LeakTracker.h"
#include "Logger.h"

#include "CUserAgent.h"

// -----------------------------------------------------------------------------
// CHttpHandler::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CHttpHandler* CHttpHandler::NewL(CHttpConnection& aHttpConnection)
    {
    CHttpHandler* self = new (ELeave) CHttpHandler(aHttpConnection);
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CHttpHandler::CHttpHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpHandler::CHttpHandler(CHttpConnection& aHttpConnection):
        KTimerPeriod(40000000), iLeakTracker(CLeakTracker::EHttpHandler),
        iHttpConnection(&aHttpConnection), 
        iSession(aHttpConnection.Session()), iLastActivity(0), iStatusCode(KErrNone)
    {
    iTimerState.iFunction = TimerCallback;
    iTimerState.iPtr = static_cast<TAny*>(this);
    }
        

// -----------------------------------------------------------------------------
// CHttpHandler::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpHandler::ConstructL()
    {
    // Get the http connection.
    if (!iHttpConnection->IsConnected())
        {
        // Set this instance as the connection observer.
        
        // TODO: In 3.1 when multiple tasks are supported this needs to be
        //      changed to AddObserver as multiple tasks may need to be waken.
        iHttpConnection->SetObserver(this);
        iIsConnectionObserver = ETrue;
        }

    iTimer = CPeriodic::NewL(EPriorityHigh);
    
    iStringPool = iSession.StringPool();
    }        


// -----------------------------------------------------------------------------
// CHttpHandler::~CHttpHandler
// Deconstructor.
// -----------------------------------------------------------------------------
//
CHttpHandler::~CHttpHandler()
    {
    // If this happened to be the instances that is observing the connection 
    // then clear the connection's observer...
    if (iIsConnectionObserver)
        {
	        if(iHttpConnection)
	        {
	        	iHttpConnection->SetObserver(NULL);	
	        }
        
        }
	RHTTPTransaction temp;
	if (iTransaction != temp)
	{
		// Close the transaction.
	    iTransaction.Close();	
	}
    delete iResponseBuffer;    
    delete iTimer;
    }


// -----------------------------------------------------------------------------
// CHttpHandler::LoadUrl
// 
// Loads the given url -- asynchronously
// -----------------------------------------------------------------------------
//
void CHttpHandler::LoadUrlL(const TDesC& aUrl, MLoadObserver& aObserver)
    {
    TUriParser8  uriParser;
    HBufC8*      url;

    // Set the observer.
    iObserver = &aObserver;

    // Parse the url.
    url = HBufC8::NewLC(aUrl.Length());
    url->Des().Copy(aUrl);
    User::LeaveIfError(uriParser.Parse(*url));

    // Create the transaction.
    iTransaction = iSession.OpenTransactionL(uriParser, *this,
        iSession.StringPool().StringF(HTTP::EGET, RHTTPSession::GetTable()));

    // TODO: Set the headers if any.
    //++PK Add UA header code

    iUserAgentHeader = iTransaction.Request().GetHeaderCollection();

    CUserAgent* tWebUtilsStandardUA = CUserAgent::NewL();
    CleanupStack::PushL(tWebUtilsStandardUA);

    HBufC8* tWebUtilsStandardUAHeaderValue = tWebUtilsStandardUA->UserAgentL();
    CleanupStack::PushL(tWebUtilsStandardUAHeaderValue);

    RStringF tStringValue = iSession.StringPool().OpenFStringL(*tWebUtilsStandardUAHeaderValue);

    THTTPHdrVal tHeaderValue( tStringValue );

    RStringF tStringUA = iSession.StringPool().StringF(HTTP::EUserAgent, RHTTPSession::GetTable());

    iUserAgentHeader.SetFieldL( tStringUA, tStringValue );

    //++PK
    // Submit the request.
    iTransaction.SubmitL();

    // If the connection is available it is safe to tell to the observer
    // to display progress indicators (i.e. a wait-dialog).
    if (iHttpConnection->IsConnected())
        {
        iObserver->StartWait();

        // Start the inactive connection timer as well.
        iTimer->Start(KTimerPeriod, KTimerPeriod, iTimerState);
        iLastActivity.HomeTime();
        }

    tStringValue.Close();
    CleanupStack::PopAndDestroy(tWebUtilsStandardUAHeaderValue);
    CleanupStack::PopAndDestroy(tWebUtilsStandardUA);
    CleanupStack::PopAndDestroy(url);
    }


// -----------------------------------------------------------------------------
// CHttpHandler::ConnectionAvailable
// 
// Notifies the observer that the connection is available.
// -----------------------------------------------------------------------------
//
void CHttpHandler::ConnectionAvailable()
    {
    // Now that the connection is available it is safe to tell to the observer
    // to display progress indicators (i.e. a wait-dialog).
    iObserver->StartWait();

    // Start the inactive connection timer.
    iTimer->Start(KTimerPeriod, KTimerPeriod, iTimerState);
    iLastActivity.HomeTime();
    }


// -----------------------------------------------------------------------------
// CHttpHandler::ConnectionFailed
// 
// Notifies the observer that the establishment of the connection failed.
// -----------------------------------------------------------------------------
//
void CHttpHandler::ConnectionFailed(TInt aStatus)
    {
    // Cancel the transaction.
    iTransaction.Cancel();
		aStatus = -aStatus;
    // Notify the observer.
    LoadCompleted(aStatus, NULL, KNullDesC, KNullDesC);  
    }


// -----------------------------------------------------------------------------
// CHttpHandler::MHFRunL
// 
// Called when the filter's registration conditions are satisfied for events that
// occur on a transaction.
// -----------------------------------------------------------------------------
//
void CHttpHandler::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
	{
    // Some kind of connection activity occurred so update iLastActivity.
    iLastActivity.HomeTime();

	switch (aEvent.iStatus)
		{
		case THTTPEvent::EGotResponseHeaders:
			{
			RHTTPResponse  resp = aTransaction.Response();

            // Get the headers
            iRespHeaders = resp.GetHeaderCollection();
			}
			break;

		case THTTPEvent::EGotResponseBodyData:
			{
			MHTTPDataSupplier*  dataSupplier;
			TPtrC8              ptr;

			// Get the data.
            dataSupplier = aTransaction.Response().Body();
			dataSupplier->GetNextDataPart(ptr);
            CleanupStack::PushL(TCleanupItem(&CleanupDataSupplier, dataSupplier));

			// Append to iResponseBuffer
			if (iResponseBuffer == NULL)
				{
				iResponseBuffer = ptr.AllocL();
				}
			else
				{
				iResponseBuffer = iResponseBuffer->ReAllocL(iResponseBuffer->Length() + ptr.Length());
				iResponseBuffer->Des().Append(ptr);
				}

			// Release the body data.
            CleanupStack::PopAndDestroy(/*dataSupplier*/);
			}
			break;

		case THTTPEvent::ESucceeded:
			{
			TDesC*   contentType = NULL;
            TDesC*   charSet = NULL;
            
            // Get the content-type and char-set.
            GetContentTypeL(contentType, charSet);

            // Pass the buffer to the observer.
			LoadCompleted(KErrNone, iResponseBuffer, *contentType, *charSet);
            iResponseBuffer = NULL;

            delete contentType;
            contentType = NULL;
            delete charSet;
            charSet = NULL;
			}
			break;

        case THTTPEvent::EFailed:
        case THTTPEvent::EUnrecoverableError:
        	{
        	// Notify the observer.
        	RHTTPResponse  resp = aTransaction.Response();
        	TInt statusCode = resp.StatusCode();
        	if(iStatusCode == KErrNone)
        		{
        		iStatusCode = (statusCode + 20000); // Web server HTTP status code are lesser than -20000
        		}
        	if(iStatusCode == -(KErrNoMemory)) //KErrNoMemory is not HTTP error so it should be -ve
        		{
        		iStatusCode = KErrNoMemory;
        		}
            LoadCompleted( iStatusCode, NULL, KNullDesC, KNullDesC );  

            // Clean up the response buffer.
            delete iResponseBuffer;
            iResponseBuffer = NULL;
        	}
            break;

        default:
            FEED_LOG1(_L("Feeds"), _L("Feeds_Errors.log"), 
                    EFileLoggingModeAppend, _L("CHttpHandler::MHFRunL - Failure: default %d."), aEvent.iStatus);
            if(aEvent.iStatus < KErrNone)
            	{
            	// HTTP errors should always be +ve
            	iStatusCode = -(aEvent.iStatus); 
            	}
			break;
		}
	}


// -----------------------------------------------------------------------------
// CHttpHandler::MHFRunError
// 
// Called when RunL leaves from a transaction event. This works in the same
// way as CActve::RunError; return KErrNone if you have handled the error.
// -----------------------------------------------------------------------------
//
TInt CHttpHandler::MHFRunError(TInt aError, RHTTPTransaction /*aTransaction*/, 
        const THTTPEvent& /*aEvent*/)
	{
    FEED_LOG1(_L("Feeds"), _L("Feeds_Errors.log"), 
            EFileLoggingModeAppend, _L("CHttpHandler::MHFRunError: %d."), aError);

    // Notify the observer.
	LoadCompleted(aError, NULL, KNullDesC, KNullDesC);

    // Clean up the response buffer.
    delete iResponseBuffer;
    iResponseBuffer = NULL;

    return KErrNone;
	}


// -----------------------------------------------------------------------------
// CHttpHandler::CleanupDataSupplier
// 
// Cleanup stack callback method to release a DataSupplier.
// -----------------------------------------------------------------------------
//
void CHttpHandler::CleanupDataSupplier(TAny *aPtr)
    {
    MHTTPDataSupplier*  supplier = static_cast<MHTTPDataSupplier*>(aPtr);

    // Release it
	supplier->ReleaseData();
    }


// -----------------------------------------------------------------------------
// CHttpHandler::GetContentTypeL
// 
// Get the content-type and char-encoding from the response header.
// -----------------------------------------------------------------------------
//
void CHttpHandler::GetContentTypeL(TDesC*& aContentType, TDesC*& aCharSet)
    {
    RStringF     fieldName;
    RStringF     fieldParam;
    THTTPHdrVal  value;

    // Get the content-type.
    fieldName = iStringPool.StringF(HTTP::EContentType, RHTTPSession::GetTable());
    if (iRespHeaders.GetField(fieldName, 0, value) == KErrNone)
        {
        aContentType = EscapeUtils::ConvertToUnicodeFromUtf8L(value.StrF().DesC());
        CleanupStack::PushL(aContentType);
        }

    // Get the char-encoding.
    fieldParam = iStringPool.StringF(HTTP::ECharset, RHTTPSession::GetTable());
    if (iRespHeaders.GetParam(fieldName, fieldParam, value) == KErrNone)
        {
        aCharSet = EscapeUtils::ConvertToUnicodeFromUtf8L(value.StrF().DesC());
        CleanupStack::PushL(aCharSet);
        }

    if (aContentType == NULL)
        {
        aContentType = KNullDesC().AllocL();
        CleanupStack::PushL(aContentType);
        }

    if (aCharSet == NULL)
        {
        aCharSet = KNullDesC().AllocL();
        CleanupStack::PushL(aCharSet);
        }

    CleanupStack::Pop(2);
    }


// -----------------------------------------------------------------------------
// CHttpHandler::LoadComplete
// 
// Passes the status code and responseBody to the observer.  The observer
// adopts aResponseBody.
// -----------------------------------------------------------------------------
//
void CHttpHandler::LoadCompleted(TInt aStatusCode, TDesC8* aResponseBody,
        const TDesC& aContentType, const TDesC& aCharSet)
    {
    delete iTimer;
    iTimer = NULL;

    RHTTPTransaction temp;
    if (iTransaction != temp)
        {
		    // Close the transaction.
        iTransaction.Close();	
        }
    if (iIsConnectionObserver)
        {
        if(iHttpConnection)
            {
       	    iHttpConnection->SetObserver(NULL);	
            }
        iIsConnectionObserver = EFalse;
        }

    // Pass the buffer to the observer.
    iObserver->LoadCompleted(aStatusCode, aResponseBody, aContentType, aCharSet);

    if (aStatusCode != KErrNone)
        {
        FEED_LOG1(_L("Feeds"), _L("Feeds_Errors.log"), 
                EFileLoggingModeAppend, _L("CHttpHandler::LoadCompleted: %d."), aStatusCode);
        }
    }


// -----------------------------------------------------------------------------
// CHttpHandler::TimerCallback
// 
// The timer's callback used to abort connections that stop responding.
// -----------------------------------------------------------------------------
//
TInt CHttpHandler::TimerCallback(TAny* aPtr)
    {
    CHttpHandler*  self = static_cast<CHttpHandler*>(aPtr);
    TTime          now;

    // Get the current time.
    now.HomeTime();

    // If the connection is inactive then delete the timer and cancel the load.
    if ((self->iLastActivity + self->KTimerPeriod) < now)
        {
        delete self->iTimer;
        self->iTimer = NULL;

        FEED_LOG(_L("Feeds"), _L("Feeds_Errors.log"), 
                EFileLoggingModeAppend, _L("CHttpHandler::TimerCallback. TIMEDOUT"));

        // Cancel the transaction.
        self->iTransaction.Cancel();

        // Notify the observer.
        self->LoadCompleted(KErrTimedOut, NULL, KNullDesC, KNullDesC);  
        }

    return KErrNone;
    }
