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
#include "senhttpsyncrequester.h"
#include "senhttpchannel.h"
#include "senlayeredhttptransportproperties.h"

// =========================== MEMBER FUNCTIONS ===============================

CSenHttpSyncRequester* CSenHttpSyncRequester::NewL(CSenHttpChannel* aHttpChannel)
    {
    CSenHttpSyncRequester* pNew = CSenHttpSyncRequester::NewLC(aHttpChannel, NULL);
    CleanupStack::Pop();
    return pNew;
    }

CSenHttpSyncRequester* CSenHttpSyncRequester::NewLC(CSenHttpChannel* aHttpChannel)
    {
    CSenHttpSyncRequester* pNew = CSenHttpSyncRequester::NewLC(aHttpChannel, NULL);
    return pNew;
    }


CSenHttpSyncRequester* CSenHttpSyncRequester::NewL(CSenHttpChannel* aHttpChannel,
                                                   MSenResponseObserver* aParent)
    {
    CSenHttpSyncRequester* pNew = CSenHttpSyncRequester::NewLC(aHttpChannel, aParent);
    CleanupStack::Pop();
    return pNew;
    }

CSenHttpSyncRequester* CSenHttpSyncRequester::NewLC(CSenHttpChannel* aHttpChannel,
                                                    MSenResponseObserver* aParent)
    {
    CSenHttpSyncRequester* pNew =
            new (ELeave) CSenHttpSyncRequester(aHttpChannel, aParent);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

MSenProperties& CSenHttpSyncRequester::PropertiesL()
    {
    return iParentObserver->PropertiesL();
    }

CSenHttpSyncRequester::CSenHttpSyncRequester(CSenHttpChannel* aHttpChannel,
                                             MSenResponseObserver* aParent)

:   CTimer(EPriorityStandard),
    iHttpChannel(aHttpChannel),
    iStatusCode(KErrNone),
    iParentObserver(aParent)
    {
    }

void CSenHttpSyncRequester::ConstructL()
    {
    CTimer::ConstructL();
    CActiveScheduler::Add(this);
    isStopped = ETrue;
    }

CSenHttpSyncRequester::~CSenHttpSyncRequester()
    {
    delete iContentType;
    delete iContent;
    if (!isStopped && iSchedulerWait.IsStarted())
        {
        iSchedulerWait.AsyncStop();
        }
    }
void CSenHttpSyncRequester::DoCancel()
    {
    CTimer::DoCancel();
    }
void CSenHttpSyncRequester::RunL()
    {
    if (!isStopped)
        {
        iStatusCode = KErrTimedOut;
	    if (iSchedulerWait.IsStarted())
	        {
	        iSchedulerWait.AsyncStop();
	        }
        }
    isStopped = ETrue;
    }
TInt CSenHttpSyncRequester::RunError(TInt aError)
    {
    if (!isStopped)
        {
    	iStatusCode = aError;
	    if (iSchedulerWait.IsStarted())
	        {
	        iSchedulerWait.AsyncStop();
	        }
        }
    isStopped = ETrue;
    return aError;
    }


/*
// GET
TInt CSenHttpSyncRequester::SubmitL(const TDesC8& aRequestUri,
                                             HBufC8*& aResponseContentType,
                                             HBufC8*& aResponseContent,
                                             TInt& aErrorCode)
    {
    iStatusCode = KErrNone;
    TInt submitId = iHttpChannel->SendL(*this, aRequestUri);
    iSchedulerWait.Start();
    aResponseContentType = iContentType;
    iContentType = NULL;
    aResponseContent = iContent;
    iContent = NULL;
    aErrorCode = iStatusCode;
    return submitId;
    }
*/

// POST
TInt CSenHttpSyncRequester::SubmitL(const TDesC8& aRequestUri,
                                    CSenLayeredHttpTransportProperties& aProps,
                                    const TDesC8& aRequestContent,
                                    HBufC8*& aResponseContentType,
                                    HBufC8*& aResponseContent,
                                    TInt& aErrorCode)
    {
    if (!isStopped)
        {
        iHttpChannel->CancelAllTransactions();
        iSchedulerWait.AsyncStop();
        }
    isStopped = ETrue;
    iStatusCode = KErrNone;
    this->Cancel();
    TInt submitId = iHttpChannel->SendL(*this,
                                        aRequestUri,
                                        aRequestContent,
                                        aProps);
    
    TInt timeOut(0);
    if (aProps.MaxTimeToLiveL(timeOut) == KErrNone)
        {
        this->After(timeOut * 1000000);    
        }
    
    isStopped = EFalse;
    iSchedulerWait.Start();
    this->Cancel();
    aResponseContentType = iContentType;
    iContentType = NULL;
    aResponseContent = iContent;
    iContent = NULL;
    aErrorCode = iStatusCode;
    return submitId;
    }


void CSenHttpSyncRequester::ResponseReceivedL( TInt /*aId*/, const TAny* aContentType, HBufC8* apContent, CSenHttpTransportProperties* /*aHttpProperties*/ )
    {
    iContent = apContent; // transfers ownership of content into _this_ class
    iContentType = ((HBufC8*)aContentType)->AllocL();
    if (!isStopped && iSchedulerWait.IsStarted())
        {
        iSchedulerWait.AsyncStop();
        }
    isStopped = ETrue;
    }

void CSenHttpSyncRequester::ResponseErrorL( TInt aId, TInt aError, HBufC8* apErrorBody, CSenHttpTransportProperties* aHttpProperties  )
    {
    iStatusCode = aError;
    iContentType = NULL;

    if (iParentObserver)
        {
        iContent = apErrorBody;  // To fix EPSK-76XFAN
        iParentObserver->ResponseErrorL( aId, aError, apErrorBody,  aHttpProperties);
        }
    else
        {
        iContent = apErrorBody;
        }    
        
    if (!isStopped)
        {
        iSchedulerWait.AsyncStop();
        }
    isStopped = ETrue;
    }

// Propagate the http status code to remote service consumer
void CSenHttpSyncRequester::StateChanged(TInt aRequestId, TInt aState)
	{
    if (iParentObserver)
        {
        iParentObserver->StateChanged(aRequestId, aState);
        }	
	}

void CSenHttpSyncRequester::SetTrafficDetails(TSenDataTrafficDetails& aDetails) 
	{
    if (iParentObserver)
        {
        iParentObserver->SetTrafficDetails(aDetails);
        }		
	}


//////////////////////////////////////////////////////////////////////////

/*
TInt CSenHttpSyncRequester::SubmitL(const TDesC8& aRequestUri,
                                    const TDesC8* aRequestContentType,
                                    MSenElement& aMessage, //SOAP envelope
                                    HBufC8*& aResponseContentType,
                                    HBufC8*& aResponseContent,
                                    TInt& aErrorCode)
    {
    iStatusCode = KErrNone;


    TInt submitId = iHttpChannel->SubmitL(*this,
                                        aRequestUri,
                                        aRequestContentType,
                                        aMessage);
    iSchedulerWait.Start();
    aResponseContentType = iContentType;
    iContentType = NULL;
    aResponseContent = iContent;
    iContent = NULL;
    aErrorCode = iStatusCode;
    return submitId;
    }
*/

//  End of File


