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
* Description:  Declares HttpServer class.
*
*/


// INCLUDE FILES
#include <e32std.h>
#include <upnpsettings.h>

#include "upnphttpserver.h"
#include "upnphttpsession.h"
#include "upnphttpfileaccess.h"
#include "upnpstring.h"
#include "upnpcommonupnplits.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
#include "upnpcons.h"
#include "upnphttpmessagefactory.h"
#include "upnphttpservertransactioncreator.h"
#ifdef RD_UPNP_REMOTE_ACCESS
#include "upnpipfiltermanager.h"
#endif

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpHttpServer::CUpnpHttpServer
// C++ default constructor
// -----------------------------------------------------------------------------
//
CUpnpHttpServer::CUpnpHttpServer( MUpnpHttpServerObserver* aObserver,
                                  MUpnpHttpServerTransactionCreator* aCreator,
                                  RSocketServ* aSocketServ,
                                  TInt aActiveIap )
    : CUpnpTcpServer( aSocketServ, KDefaultHttpPort, aActiveIap )
    {
    iObserver = aObserver;
    iTransactionCreator = aCreator;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::~CUpnpHttpServer
// C++ default destructor
// -----------------------------------------------------------------------------
//
CUpnpHttpServer::~CUpnpHttpServer()
    {
    delete iServerDescription;
    iFs.Close();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::NewL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
CUpnpHttpServer* CUpnpHttpServer::NewL( MUpnpHttpServerObserver* aObserver,
                                                 RSocketServ* aSocketServ,
                                                 TInt aActiveIap )
    {
    LOG_FUNC_NAME;
       
    CUpnpHttpServer* self = new (ELeave) CUpnpHttpServer( aObserver, NULL,           
            aSocketServ, aActiveIap );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );

    return self;
    }


// -----------------------------------------------------------------------------
// CUpnpHttpServer::ConstructL
// Two-phased constructor
// -----------------------------------------------------------------------------
//
void CUpnpHttpServer::ConstructL()
    {
    BaseConstructL();
    User::LeaveIfError(iFs.Connect());
    iServerDescription =  CUpnpSettings::GetServerHeaderL();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::StartL
// Start tcp server
// -----------------------------------------------------------------------------
//
void CUpnpHttpServer::StartL( const TInt aPort )
    {
    LOG_FUNC_NAME;
    OpenSocketL();
    if ( aPort )
        {
        BindL( aPort );
        }
        else
        {
        BindRandomPortL();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::Stop
// Stop tcp server
// -----------------------------------------------------------------------------
//
void CUpnpHttpServer::Stop()
    {
    LOG_FUNC_NAME;
    TRAP_IGNORE( StopTcpServerL() );
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::ToReceiveStackD
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpServer::ToReceiveStackD( CUpnpHttpMessage* aMsg )
    {
    LOG_FUNC_NAME;
    //to indicate that the upper layer can give a response for it is acting as a host in this session
    if ( !aMsg )
        {
        return;
        }
	aMsg->SetClientRequest(ETrue);
	//for redirection codes

    #ifdef _DEBUG
        TInt err;
        TRAP( err, iObserver->HttpEventLD( aMsg ) );
        LOGS1H( iHandle, "CUpnpHttpServer::ToReceiveStack() Error %i", err );

    #else
        TRAP_IGNORE(  iObserver->HttpEventLD( aMsg ) );
    #endif
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::SendMessageL
// Send HTTP message
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpServer::SendMessageL( CUpnpHttpMessage* aMessage )
    {
    LOG_FUNC_NAME;

    if (!aMessage)
		{
		LOGS("CUpnpHttpServer::SendMessageL - Tried to send Null");
		return KErrNotFound;
		}

    TInt trapError( KErrNone );
    TInt sessId( KErrNone );

    TRAP(trapError, sessId = TrapSendMessageL( aMessage ) );

    if( trapError < KErrNone )
        {
        LOGS1( "HTTP *** Sending of message failed. Error: %i", trapError );
        // HttpSession errors that cannot be forwarded to upper layers
        if( -trapError >= EHttpBadRequest && -trapError <= EHttpExpectationFailed )
            {
            CUpnpHttpMessage* notify = NULL;
            notify = RUpnpHttpMessageFactory::HttpResponseErrorL( aMessage, -trapError );
            CUpnpHttpSession* sess = GetSession( aMessage->SessionId() );
            sess->DeleteThisSessionL( sess );

            ToReceiveStackD( notify ); //Upper layer takes care about cleanup
            }
        else
            {
            User::Leave(trapError);
            }
        }
    // nobody use the value
    return sessId;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::TrapSendMessageL
// Send HTTP message
// -----------------------------------------------------------------------------
//
TInt CUpnpHttpServer::TrapSendMessageL( CUpnpHttpMessage* aMessage )
    {
    CUpnpHttpSession* sess = GetSession( aMessage->SessionId() );
    if( sess )
        {
        sess->SendL( aMessage );
        return sess->Id();
        }
    else
        {
        // If running out of memory, there is nothing else to do but leave
        if ( KErrNoMemory == aMessage->SessionId() )
            {
            LOGS("CUpnpHttpServer::TrapSendMessageL - OUT OF MEMORY");
            User::Leave( KErrNoMemory );
            }        
        }
    return KErrNotFound;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::GetSession
//
// -----------------------------------------------------------------------------
//
CUpnpHttpSession* CUpnpHttpServer::GetSession( TInt aSessionId )
    {
    CUpnpHttpSession* sess = NULL;

    for ( TInt i=0; i < iSessionList.Count(); i++ )
        {
        if ( ( (CUpnpHttpSession*) iSessionList[i] )->Id() == aSessionId )
            {
            sess = (CUpnpHttpSession*) iSessionList[i];
            return sess;
            }
        }

    return sess;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::ServerDescription
// Returns server's description
// -----------------------------------------------------------------------------
//
TDesC8& CUpnpHttpServer::ServerDescription()
    {
    return *iServerDescription;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::ConnectionAcceptedL
//
// -----------------------------------------------------------------------------
//
CUpnpTcpSession* CUpnpHttpServer::ConnectionAcceptedL( RSocket aSocket )
    {
    LOG_FUNC_NAME;

    #ifdef _DEBUG
    TInetAddr tempAddr;
    aSocket.RemoteName( tempAddr );
    tempAddr.ConvertToV4();

    const TInt KMaxAdressLength = 20;
    TBuf<KMaxAdressLength> addrBuf;
    tempAddr.Output( addrBuf );

    HBufC8* addrBuf8 = UpnpString::FromUnicodeL( addrBuf );
    CleanupStack::PushL( addrBuf8 );

    LOGS( "CUpnpHttpServer::ConnectionAcceptedL - Remote socket connected" );
    LOGT( addrBuf8->Des() );

    LOGS1("CUpnpHttpServer::ConnectionAcceptedL - Creating a new Http session. Session count: %i", iSessionList.Count());

    CleanupStack::PopAndDestroy(addrBuf8);
    #endif //_DEBUG
    CUpnpHttpSession* sess = CUpnpHttpSession::NewL( aSocket, this,
        CUpnpHttpMessage::NewSessionIdL(), EPriorityNormal );

    return sess;
    }


// -----------------------------------------------------------------------------
// CUpnpHttpServer::SetCdDataFinder
//
// -----------------------------------------------------------------------------
//
void CUpnpHttpServer::SetTransactionCreator( 
                                 MUpnpHttpServerTransactionCreator* aTransactionCreator )
    {
    iTransactionCreator = aTransactionCreator;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::HttpFilter
//
// -----------------------------------------------------------------------------
//
MUpnpHttpServerTransactionCreator* CUpnpHttpServer::TransactionCreator()
    {
    //ASSERT( iHttpFilter ) set in constructor defaultHttpFilter to ensure not null
    return iTransactionCreator;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::FileSession
// -----------------------------------------------------------------------------
//
RFs& CUpnpHttpServer::FileSession()
    {
    return iFs;
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::ConnectionL
// -----------------------------------------------------------------------------
//
RConnection& CUpnpHttpServer::ConnectionL()
    {
    return iConnectionManagerProxy->ConnectionL();
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::StartIPFilteringL
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpServer::StartIPFilteringL()
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    if( !iIPFilteringManager )
    {
    iIPFilteringManager = CUpnpIPFilterManager::NewL();
    }
    #endif
    }

// -----------------------------------------------------------------------------
// CUpnpHttpServer::StopIPFiltering
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpHttpServer::StopIPFiltering()
    {
    #ifdef RD_UPNP_REMOTE_ACCESS
    delete iIPFilteringManager;
    iIPFilteringManager = NULL;
    #endif
    }

//  End of File
