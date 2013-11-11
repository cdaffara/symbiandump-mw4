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
* Description:
*
*/


// INCLUDE FILES
#include "upnpssdphandlerup.h"
#include "upnpssdpserver.h"
#include "upnpmsearchtimer.h"

#define KLogFile _L("UPnPStack.txt")
#include "upnpcustomlog.h"

#include "upnpmessagehandler.pan"

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::CUpnpSsdpHandlerUp
// -----------------------------------------------------------------------------
//
CUpnpSsdpHandlerUp::CUpnpSsdpHandlerUp(
                            MUpnpSsdpServerObserver& aSsdpServerObserver )
                            : CUpnpSsdpHandlerBase( aSsdpServerObserver )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::~CUpnpSsdpHandlerUp
// -----------------------------------------------------------------------------
//
CUpnpSsdpHandlerUp::~CUpnpSsdpHandlerUp()
   {
   ClearTimers();
   delete iSsdpServer;
   delete iMulticastSsdpServer;
   }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::CUpnpSsdpHandlerUp
// -----------------------------------------------------------------------------
//
CUpnpSsdpHandlerUp* CUpnpSsdpHandlerUp::NewL(
                              MUpnpSsdpServerObserver& aSsdpServerObserver )
    {
    CUpnpSsdpHandlerUp* self = new ( ELeave )
                               CUpnpSsdpHandlerUp( aSsdpServerObserver );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpSsdpHandlerUp::ConstructL()
    {
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::StartL
// -----------------------------------------------------------------------------
//
void CUpnpSsdpHandlerUp::StartL( RSocketServ& aSocketServ )
    {
    if ( !iSsdpServer || !iMulticastSsdpServer )
        {
        delete iSsdpServer;
        iSsdpServer = NULL;
        iSsdpServer = CUpnpSsdpServer::NewL( &iSsdpServerObserver,
                                             &aSocketServ,
                                             KDefaultStartPort );

        delete iMulticastSsdpServer;
        iMulticastSsdpServer = NULL;
        iMulticastSsdpServer = CUpnpSsdpServer::NewL( &iSsdpServerObserver,
                                                      &aSocketServ,
                                                      KDefaultMulticastPort,
                                                      iSsdpServer );
        }
    iSsdpServer->StartL();
    iMulticastSsdpServer->StartL();
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::Stop
// -----------------------------------------------------------------------------
//
void CUpnpSsdpHandlerUp::Stop()
    {
    __ASSERT_DEBUG( iSsdpServer && iMulticastSsdpServer,
        User::Panic( KMessageHandler, EMessageHandlerBadState ) );
    ClearTimers();
    iSsdpServer->Stop();
    iMulticastSsdpServer->Stop();
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::AddressChangeL
// -----------------------------------------------------------------------------
//     
void CUpnpSsdpHandlerUp::AddressChangeL( TInetAddr& aAddress )
    {
    iSsdpServer->AddressChangedL( aAddress );
    iMulticastSsdpServer->AddressChangedL( aAddress );  
    }
    
// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::SsdpSearchL
// -----------------------------------------------------------------------------
//
void CUpnpSsdpHandlerUp::SsdpSearchL( const TDesC8& aSearchString, TDesC8& aMX )
    {
    __ASSERT_DEBUG( iSsdpServer && iMulticastSsdpServer,
        User::Panic( KMessageHandler, EMessageHandlerBadState ) );

    if ( iCPCounter == 0 )
        {
        for (TInt i(0); i < iTimers.Count(); i++ )
            {
            CUpnpMSearchTimer* timer = iTimers[i];
            iTimers.Remove( i );
            delete timer;
            }
        return;
        }

    TBuf8<KMXTimeBuffer> time;

    CUpnpMSearchTimer* timer = CUpnpMSearchTimer::NewLC( this, aSearchString, aMX );
    iTimers.AppendL( timer );
    CleanupStack::Pop(timer);

    TInt mx = timer->GetNextValue();
    TInt t = timer->GetNextValue();
    if ( mx > 0 )
        {
        time.Num(mx);
        iSsdpServer->SearchL( const_cast<TDesC8&>( aSearchString ), time );
        }
    if ( t > 0 )
        {
        t = t * KSecond;
        ( iTimers[( iTimers.Count() - 1 )] )->Start( t );
        }
    else
        {
        iTimers.Remove( iTimers.Count() - 1 );
        delete timer;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::AddLocalControlPoint
// -----------------------------------------------------------------------------
//
TInt CUpnpSsdpHandlerUp::AddLocalControlPoint( )
    {
    return iCPCounter++;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::RemoveLocalControlPoint
// -----------------------------------------------------------------------------
//
TInt CUpnpSsdpHandlerUp::RemoveLocalControlPoint( )
    {
    iCPCounter--;
    __ASSERT_ALWAYS( iCPCounter >= 0,
        User::Panic( KMessageHandler, EMessageHandlerRemoveNonExistingCp ) );
    return iCPCounter;
    }


// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::AdvertiseDeviceL
// -----------------------------------------------------------------------------
//
void CUpnpSsdpHandlerUp::AdvertiseDeviceL( TInt aLive,  CUpnpDeviceLibraryElement& aElement )
    {
    LOGSH( 0, "CUpnpMessageHandlerEngine::AdvertiseDeviceL");

    __ASSERT_DEBUG( iSsdpServer && iMulticastSsdpServer,
        User::Panic( KMessageHandler, EMessageHandlerBadState ) );
    // forward advertisement requests to ssdp server
    // depending on the element status, advertise ssdp:alive or ssdp:byebye

    LOGSH( 0, "MESSAGE HANDLER ENGINE ADVERTISING");

    if ( aLive )
        {
        iSsdpServer->AdvertiseAliveL( aElement );
        }
    else
        {
        iSsdpServer->AdvertiseByebyeL( aElement );
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::TimerEventL
// Callback function
// -----------------------------------------------------------------------------
//
void CUpnpSsdpHandlerUp::TimerEventL( CUpnpNotifyTimer* aTimer )
    {
    if ( !(aTimer->iStatus == KErrNone) )
        {
        return;
        }

    TInt i(0);
    if ( !iCPCounter )
        {
        for (; i < iTimers.Count(); i++ )
            {
            CUpnpMSearchTimer* timer = iTimers[i];
            iTimers.Remove( i );
            delete timer;
            }
        return;
        }

    for (; i < iTimers.Count(); i++ )
        {
        if ( iTimers[i] == aTimer )
            {
            break;
            }
        }
    if (i < iTimers.Count() )
        {
        TBuf8<KMXTimeBuffer> time;

        TInt mx = iTimers[i]->GetNextValue();
        TInt t = iTimers[i]->GetNextValue();
        if ( mx > 0 )
            {
            time.Num( mx );
            TPtrC8 ptr = iTimers[i]->GetTarget();
            iSsdpServer->SearchL(ptr, time);
            }
        if ( t > 0 )
            {
            t = t * KSecond;
            ( iTimers[( iTimers.Count() - 1 )])->Start( t );
            }
        else
            {
            CUpnpMSearchTimer* timer = iTimers[i];
            iTimers.Remove(i);
            delete timer;
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::ClearTimers
// -----------------------------------------------------------------------------
//
void CUpnpSsdpHandlerUp::ClearTimers()
    {
    for ( TInt i(0); i < iTimers.Count(); i++ )
        {
        iTimers[i]->Cancel();
        }
    iTimers.ResetAndDestroy();
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpHandlerUp::IsStarted
// -----------------------------------------------------------------------------
//
TBool CUpnpSsdpHandlerUp::IsStarted()
    {
    return ( iSsdpServer && iMulticastSsdpServer );
    }

// End of File
