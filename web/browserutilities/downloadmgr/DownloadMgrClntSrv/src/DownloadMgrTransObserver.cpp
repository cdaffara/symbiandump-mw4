/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*     This file contains the declaration of transaction observer of Download Mgr Server.
*
*/


// INCLUDE FILES
#include "DownloadMgrTransObserver.h"
#include "DownloadMgrClient.h"

// CONSTANTS


// GLOBAL FUNCTIONS

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CTransactionObserver::NewL
// ---------------------------------------------------------
//
CTransactionObserver* CTransactionObserver::NewL( RHttpDownload* aDownload )
    {
    CTransactionObserver* self = new( ELeave ) CTransactionObserver( aDownload );
    
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// ---------------------------------------------------------
// CTransactionObserver::CTransactionObserver
// ---------------------------------------------------------
//
CTransactionObserver::CTransactionObserver( RHttpDownload* aDownload )
    {
    iDownload = aDownload;
    }

// ---------------------------------------------------------
// CTransactionObserver::ConstructL
// ---------------------------------------------------------
//
void CTransactionObserver::ConstructL()
    {
    }

// ---------------------------------------------------------
// CTransactionObserver::CTransactionObserver
// ---------------------------------------------------------
//
CTransactionObserver::~CTransactionObserver()
    {
    }

// ---------------------------------------------------------
// CTransactionObserver::MHFRunL
// ---------------------------------------------------------
//
void CTransactionObserver::MHFRunL(RHTTPTransaction aTransaction, const THTTPEvent& aEvent)
    {
    switch( aEvent.iStatus )
        {
        case THTTPEvent::EGotResponseBodyData:
            {
            MHTTPDataSupplier* respBody = aTransaction.Response().Body();
            TPtrC8 buf;

            respBody->GetNextDataPart( buf );

            iDownload->SetStringAttribute( EDlAttrContinueBody, buf );

            respBody->ReleaseData();
            }
            break;

        case THTTPEvent::ESucceeded:
            {
            iDownload->SetIntAttribute( EDlAttrSucceeded, 0 );
            }
            break;

        case THTTPEvent::EFailed:
        case THTTPEvent::EMoreDataReceivedThanExpected:
        case THTTPEvent::EUnrecoverableError:
        case THTTPEvent::ETooMuchRequestData:
            {
            iDownload->SetIntAttribute( EDlAttrFailed, aEvent.iStatus );
            }
            break;

        case THTTPEvent::ERedirectedPermanently:
            {
            iDownload->SetStringAttribute( EDlAttrRedirectedPermanently, 
                                           aTransaction.Request().URI().UriDes() );
            }
            break;

        case THTTPEvent::ERedirectedTemporarily:
            {
            iDownload->SetStringAttribute( EDlAttrRedirectedTemporary, 
                                           aTransaction.Request().URI().UriDes() );
            }
            break;
            
        default:
            {
            if ( aEvent.iStatus == KErrHttpPartialResponseReceived )
                {
                //Partial response has been received and connection has been disconnected. This error will be 
                //propagated to the client only, if the HTTP:ENotifyOnDisconnect property is set with a value
                //HTTP::EEnableDisconnectNotification
                
                //This error code was cancelling the pausable download. This error shud be ignored to keep the
                //paused download.
                //TSW Err ID : SXUU-77SRWL
                }
                
            else if( aEvent.iStatus < 0 )
                // error occured -> leave will be handled in OnError()
                {
                iDownload->SetIntAttribute( EDlAttrFailed, aEvent.iStatus );
                }
            }
            break;
        }
    }

// ---------------------------------------------------------
// CTransactionObserver::MHFRunError
// ---------------------------------------------------------
//
TInt CTransactionObserver::MHFRunError(TInt aError, RHTTPTransaction /*aTransaction*/, const THTTPEvent& /*aEvent*/)
    {
    iDownload->SetIntAttribute( EDlAttrFailed, aError );
    iDownload->DeleteTransaction();
    
    return KErrNone;
    }

