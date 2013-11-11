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


#include <http.h>
#include <httpfiltercommonstringsext.h>
#include <httpfilterconnhandlerinterface.h>
#include <InternetConnectionManager.h>
#include <httpfiltercommonstringsaddition.h>

#include "HttpConnection.h"
#include "LeakTracker.h"
#include "Logger.h"


// -----------------------------------------------------------------------------
// CHttpConnection::CHttpConnection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpConnection::CHttpConnection()
    {
    }
        

// -----------------------------------------------------------------------------
// CHttpConnection::BaseConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpConnection::BaseConstructL()
    {
    // Open the session
    iSession.OpenL();

    // Get the connection info
    const TStringTable& stringTable = RHTTPSession::GetTable();

    // Install the necessary http-filters.
    CHttpFilterConnHandlerInterface::InstallFilterL(iSession, this);
    iAutoDelete = CIdle::NewL(CActive::EPriorityIdle);
    }        


// -----------------------------------------------------------------------------
// CHttpConnection::~CHttpConnection
// Deconstructor.
// -----------------------------------------------------------------------------
//
CHttpConnection::~CHttpConnection()
    {
    if(iAutoDelete)
        delete iAutoDelete;
    // move the iSession close to subclass because:
    // the close of iSession should happend before
    // the close of RConnection, RSocketServer, CInternetConnectionManager
    }


// -----------------------------------------------------------------------------
// CHttpConnection::CoverageEvent
//
// This function cancel outstanding transactions and notify a 
// user with the message "Out of coverage" 
// -----------------------------------------------------------------------------
//
void CHttpConnection::CoverageEvent(TInt /*aError*/)
    {
    // Ignore...
    }


// -----------------------------------------------------------------------------
// CHttpConnection::SetObserver
// 
// Sets the observer.  In practice this is only set once during the lifetime 
// of the instance.
// -----------------------------------------------------------------------------
//
void CHttpConnection::SetObserver(MHttpConnectionObserver* aObserver)
    {
    iObserver = aObserver;
    }


// -----------------------------------------------------------------------------
// CHttpConnection::Session
//
// Returns the session.
// -----------------------------------------------------------------------------
//
RHTTPSession& CHttpConnection::Session()
    {
    return iSession;
    }


//--------------------------------------------------------------------------
//CHttpConnection::AutoDelete()
//--------------------------------------------------------------------------
void CHttpConnection::AutoDelete()
    {
    iObserver = NULL;
    iAutoDelete->Start(TCallBack(DelayedDelete, this));
    }

//--------------------------------------------------------------------------
//CHttpConnection::DelayedDelete()
//--------------------------------------------------------------------------
TInt CHttpConnection::DelayedDelete(TAny* aPtr)
    {
    CHttpConnection*  self = static_cast<CHttpConnection*>(aPtr);
    
    delete self;    
    return EFalse;
    }
