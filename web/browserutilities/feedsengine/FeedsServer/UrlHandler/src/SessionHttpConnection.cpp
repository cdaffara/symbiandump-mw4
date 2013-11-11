/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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


#include "SessionHttpConnection.h"
#include "FeedsServerSession.h"
#include "Logger.h"


// -----------------------------------------------------------------------------
// CSessionHttpConnection::NewL
//
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CSessionHttpConnection* CSessionHttpConnection::NewL( CFeedsServerSession& aFeedsSession )
    {
    CSessionHttpConnection* self = new (ELeave) CSessionHttpConnection( aFeedsSession );
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

        
// -----------------------------------------------------------------------------
// CSessionHttpConnection::CSessionHttpConnection
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CSessionHttpConnection::CSessionHttpConnection( CFeedsServerSession& aFeedsSession ):
        iLeakTracker( CLeakTracker::EHttpConnection ),
        iFeedsSession( aFeedsSession ) 
    {
    }
        

// -----------------------------------------------------------------------------
// CSessionHttpConnection::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CSessionHttpConnection::ConstructL()
    {
    BaseConstructL();

    User::LeaveIfError( iSocketServ.Connect() );
    iConnName = HBufC::NewL( KMaxName );
    Reset();
    }        


// -----------------------------------------------------------------------------
// CSessionHttpConnection::~CSessionHttpConnection
// Deconstructor.
// -----------------------------------------------------------------------------
//
CSessionHttpConnection::~CSessionHttpConnection()
    {
    // close http session
    iSession.Close();

    // close RConnection
    Disconnect();
    delete iConnName;

    // close RSocketServer
    iSocketServ.Close();
    }


// -----------------------------------------------------------------------------
// CSessionHttpConnection::CreateConnection
// 
// It propagates the connection callback to the browser client.
// -----------------------------------------------------------------------------
//
TInt CSessionHttpConnection::CreateConnection(TInt* aConnectionPtr, TInt* aSockSvrHandle, 
        TBool* aNewConn, TApBearerType* aBearerType)
    {
    TInt  err = KErrNone;

    // If need be establish the connection.
    if( !IsConnected() )
        {
        // Prepare connection name & bearer type ready for client response.
        Reset();

		// server session ask client app for connection
        TRAP( err, iFeedsSession.NetworkConnectionNeededL() );

        if  ( err == KErrNone )
            {
            // Start the scheduler to wait for client response. Scheduler will be stopped 
            // when client informs the server which bearer to use.
            //
            // See:
            //
            // [SERVER]
            //      CFeedsServerSession::SetConnectionL
            //      CSessionHttpConnection::SetConnection
            //
            // [CLIENT]
            //      CFolderItemRequestHandler::RequestCompletedL ("EFeedsServerConnectionNeeded")
            //      CFeedRequestHandler::RequestCompletedL ("EFeedsServerConnectionNeeded")
	        //
	        iWait.Start();

            // At this point, assuming that the client was able to provide us
            // with the connection information then we are able to open the specified
            // connection.
            //
            // If not (e.g. client cancelled connection request) then we must clean up
            // and handle the error.
            if  ( ConnectionInformationProvidedByClient() )
                {
                // connection info come back in response from client
                TName connName;
                connName.Copy( *iConnName );
                err = iConnection.Open( iSocketServ, connName );

                if  ( err == KErrNone )
                    {
                    *aNewConn = ETrue;

                    if (iObserver != NULL)
                        {
                        iObserver->ConnectionAvailable();
                        }
                    }
                }
            else
                {
                // Use current connection error value if the client was
                // not able to supply connection parameters.
                err = iConnectionError;
                }
            }
        }       
    else
        {
        // Otherwise the existing connection is valid and no new connection
        // is required.
        *aNewConn = EFalse;
        }

    // End of process - either set up the reference/pointer arguments for
    // a successful connection initiation, or then inform the observer of
    // the error.
    if  ( err == KErrNone )
        {
        *aConnectionPtr = (TInt) &iConnection;
        *aSockSvrHandle = iSocketServ.Handle();
        *aBearerType = iBearerType;
        }
    else
        {
        // Notify the HttpHandler that the establishing the access point failed
        if (iObserver != NULL)
            {
            iObserver->ConnectionFailed(err);
            }
        }

    return err;
    }


// -----------------------------------------------------------------------------
// CSessionHttpConnection::IsConnected
//
// Returns whether or not the connection is active.
// -----------------------------------------------------------------------------
//
TBool CSessionHttpConnection::IsConnected()
    {
    if ( !iConnection.SubSessionHandle() )
        {
        iConnStage = KConnectionUninitialised;
        }
    else
        {
        TNifProgress progress;
        iConnection.Progress( progress );
        iConnStage = progress.iStage;
        }

    return iConnStage == KLinkLayerOpen;
    }
    

// -----------------------------------------------------------------------------
// CSessionHttpConnection::Disconnect
//
// Closes the connection.
// -----------------------------------------------------------------------------
//
void CSessionHttpConnection::Disconnect()
    {
    if (IsConnected())
        {
        iConnection.Close();
        }
    }


// -----------------------------------------------------------------------------
// CSessionHttpConnection::CancelAnyConnectionAttempt
//
// Closes the connection.
// -----------------------------------------------------------------------------
//
void CSessionHttpConnection::CancelAnyConnectionAttempt()
    {
    SetObserver(NULL);
    if  ( iWait.IsStarted() )
        {
        iWait.AsyncStop();
        }
        
    Reset();
    
    iConnectionError = KErrCancel;
    }


// -----------------------------------------------------------------------------
// CSessionHttpConnection::SetConnection
//
// Sets the connection.
// -----------------------------------------------------------------------------
//
TBool CSessionHttpConnection::SetConnection( TDesC& aName, TInt aBearerType  )
    {
    TBool ret = EFalse;

    // unblocking
    if  ( iWait.IsStarted() )
        {
        iConnName->Des().Copy( aName );
        iBearerType = (TApBearerType) aBearerType;
        iConnectionError = KErrNone;

        ret = ETrue;

        iWait.AsyncStop();
        }

    return ret;
    }


//--------------------------------------------------------------------------
//CSessionHttpConnection::Reset()
//--------------------------------------------------------------------------
void CSessionHttpConnection::Reset()
    {
    iConnectionError = KErrNotReady;
    iConnName->Des().Zero();
    iBearerType = EApBearerTypeAllBearers;
    }


//--------------------------------------------------------------------------
//CSessionHttpConnection::ConnectionInformationProvidedByClient()
//--------------------------------------------------------------------------
TBool CSessionHttpConnection::ConnectionInformationProvidedByClient() const
   	{
    return ( iConnName->Length() > 0 );
   	}




