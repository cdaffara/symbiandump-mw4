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


#include <ApUtils.h> 
#include <InternetConnectionManager.h>

#include "ServerHttpConnection.h"
#include "Logger.h"


// -----------------------------------------------------------------------------
// CServerHttpConnection::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CServerHttpConnection* CServerHttpConnection::NewL( TUint32 iDefaultAccessPoint )
    {
    CServerHttpConnection* self = new (ELeave) CServerHttpConnection( iDefaultAccessPoint );
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CServerHttpConnection::CServerHttpConnection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CServerHttpConnection::CServerHttpConnection( TUint32 aDefaultAccessPoint ):
        iLeakTracker( CLeakTracker::EHttpConnection ),
        iDefaultAccessPoint( aDefaultAccessPoint )
    {
    }
        

// -----------------------------------------------------------------------------
// CServerHttpConnection::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CServerHttpConnection::ConstructL()
    {
    BaseConstructL();

    // Create the connection mgr.
    iConMgr = CInternetConnectionManager::NewL(ETrue);
    }        


// -----------------------------------------------------------------------------
// CServerHttpConnection::~CServerHttpConnection
// Deconstructor.
// -----------------------------------------------------------------------------
//
CServerHttpConnection::~CServerHttpConnection()
    {
    // close http session
    iSession.Close();

    // delete ConnectionManager
    Disconnect();
    delete iConMgr;
    }


// -----------------------------------------------------------------------------
// CServerHttpConnection::CreateConnection
// 
// It propagates the connection callback to the browser client.
// -----------------------------------------------------------------------------
//
TInt CServerHttpConnection::CreateConnection(TInt* aConnectionPtr, TInt* aSockSvrHandle, 
        TBool* aNewConn, TApBearerType* aBearerType)
    {
    TInt  err = KErrNone;

    // If need be establish the connection.
    if(!IsConnected())
        {
        // Set the default access point.
        iConMgr->SetRequestedAP( iDefaultAccessPoint );
            
        // Open a connection.
        TRAP(err, err = iConMgr->StartConnectionL(ETrue));                
        if (err != KErrNone)
            {
            // Notify the HttpHandler that the establishing the access point failed
            if (iObserver != NULL)
                {
                iObserver->ConnectionFailed(err);
                }
                
            return err;
            }

        *aNewConn = ETrue;

        if (iObserver != NULL)
            {
            iObserver->ConnectionAvailable();
            }
        }
        
    // Otherwise the connection is valid.
    else
        {
        *aNewConn = EFalse;
        }

    // Set the connection, server handler, and bearer type.
    *aConnectionPtr = (TInt) &iConMgr->Connection();
    *aSockSvrHandle = (TInt) iConMgr->SocketServer().Handle();

    TRAP(err, *aBearerType = iConMgr->CurrentBearerTypeL());
    if (err != KErrNone)
        {
        // Notify the HttpHandler that the establishing the access point failed
        if (iObserver != NULL)
            {
            iObserver->ConnectionFailed(err);
            }
                
        return err;
        }
    
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CServerHttpConnection::IsConnected
//
// Returns whether or not the connection is active.
// -----------------------------------------------------------------------------
//
TBool CServerHttpConnection::IsConnected()
    {
    return iConMgr->Connected();
    }
    

// -----------------------------------------------------------------------------
// CServerHttpConnection::Disconnect
//
// Closes the connection.
// -----------------------------------------------------------------------------
//
void CServerHttpConnection::Disconnect()
    {
    if (IsConnected())
        {
        iConMgr->Disconnect();
        }
    }


// -----------------------------------------------------------------------------
// CServerHttpConnection::SetAccessPointL
// 
// Set the new access point.
// -----------------------------------------------------------------------------
//
void CServerHttpConnection::SetAccessPointL( TUint32 aAccessPoint )
    {
    iDefaultAccessPoint = aAccessPoint;
    }
