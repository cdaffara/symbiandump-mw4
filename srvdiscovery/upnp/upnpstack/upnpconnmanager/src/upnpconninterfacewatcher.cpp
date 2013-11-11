/** @file
* Copyright (c) 2007-2008 Nokia Corporation and/or its subsidiary(-ies). 
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

#include "upnpipaddressutils.h"
#include "upnpipeventnotifier.h"
#include "upnpconninterfacewatcher.h"
#include "upnpsettings.h"
#define KLogFile _L("UPnPConnManager.txt")
#include "upnpcustomlog.h"

using namespace UpnpIpAddressUtil;
// ---------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::NewL
//
// ---------------------------------------------------------------------------
//
CUpnpConnInterfaceWatcher* CUpnpConnInterfaceWatcher::NewL(
    MUpnpConnInterfaceWatcherObserver& aIfObserver, 
    RSocketServ& aSockServ,
    RConnection &aConnection )
    {
    CUpnpConnInterfaceWatcher* self = new (ELeave) CUpnpConnInterfaceWatcher( 
                                            aIfObserver, aSockServ, aConnection );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::CUpnpConnInterfaceWatcher
// C++ default constructor can NOT contain any code, that
// might leave.
// ---------------------------------------------------------------------------
//
CUpnpConnInterfaceWatcher::CUpnpConnInterfaceWatcher(
    MUpnpConnInterfaceWatcherObserver& aIfObserver, 
    RSocketServ& aSockServ,
    RConnection &aConnection ) 
    : CActive( CActive::EPriorityStandard ),
      iIfObserver( aIfObserver ),
      iSockServ( aSockServ ),
      iConnection( aConnection )
    {
    }

// ---------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::~CUpnpConnInterfaceWatcher
// Destructor.
// ---------------------------------------------------------------------------
//
CUpnpConnInterfaceWatcher::~CUpnpConnInterfaceWatcher()
    {
    Cancel();
    delete iIPEvent;
    }

// -----------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::ConstructL
// -----------------------------------------------------------------------------
//
void CUpnpConnInterfaceWatcher::ConstructL()
    {
    LOG_FUNC_NAME;
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::SubscribeForConnectionServiceChangeNotifications
// -----------------------------------------------------------------------------
//
void CUpnpConnInterfaceWatcher::SubscribeForConnectionServiceChangeNotifications()
    {
    if ( !IsActive() )
        {
        iConnection.ServiceChangeNotification( iNewISPId, iNewServiceType, iStatus );
        SetActive();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::CancelSubscriptionForConnectionServiceChangeNotifications
// -----------------------------------------------------------------------------
//
void CUpnpConnInterfaceWatcher::CancelSubscriptionForConnectionServiceChangeNotifications()
    {
    Cancel();
    }

// -----------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::ListenAddressChangeL
// Creates and subscribes to CUpnpIPEventNotifier instance
// -----------------------------------------------------------------------------
//  
void CUpnpConnInterfaceWatcher::ListenAddressChangeL( TUint aIapId )
    {
    LOG_FUNC_NAME;
    iIapId = aIapId;
    LOGS1("ListenAddressChangeL: Given IAP: %d",iIapId);
    // get local IP                
    TInt interfaceIndex; // interface index used for subscribing for IP event

    SetLocalAddress( LocalIpAddress( interfaceIndex ) );
    
    delete iIPEvent;
    iIPEvent = NULL;
    
#ifdef _DEBUG
    LOGS1( "ListenAddressChangeL: interfaceIndex: %d", interfaceIndex );
    TBuf<50> buf;
    iLocalIpAddress.Output( buf );
    LOGS1( "ListenAddressChangeL: IP Addr: %S", &buf );
#endif //_DEBUG
    
    iIPEvent = CUpnpIPEventNotifier::NewL( interfaceIndex );
    iIPEvent->Subscribe( this );
    }

// -----------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::LocalAddress
// -----------------------------------------------------------------------------
//  
const TInetAddr& CUpnpConnInterfaceWatcher::LocalAddress()
    {
    return iLocalIpAddress;
    }

// -----------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::SetLocalAddress
// -----------------------------------------------------------------------------
//
void CUpnpConnInterfaceWatcher::SetLocalAddress( const TInetAddr& aAddress )
    {
    iLocalIpAddress.SetAddress( aAddress.Address() );
    }

// -----------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::IPEventOccursL
// -----------------------------------------------------------------------------
//
void CUpnpConnInterfaceWatcher::IPEventOccursL()
    {            
    TInt index;

    TInetAddr ip = LocalIpAddress( index );
#ifdef _DEBUG
    TBuf<KMaxName> address;
    ip.Output(address);       
    LOGS1H( iHandle, "MH IPEvent: %S", &address );
    RDebug::Print(_L("CUpnpConnInterfaceWatcher *** IPEventOccursL *** IP Address: %S"), &address );
#endif //_DEBUG
           
    if ( ip.Address() == 0 )
        {
        return;
        }
           
    if ( ip.Address() != LocalAddress().Address() )
        {       
        SetLocalAddress( ip );
        iIfObserver.InterfaceAddressChange( iLocalIpAddress );
        }    
    }

// -----------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::RunL
// -----------------------------------------------------------------------------
//
void CUpnpConnInterfaceWatcher::RunL()
    {
    if ( iStatus.Int() == KErrCancel )
        {
        iIfObserver.WlanNetworkLost();
        }
    else if ( iStatus.Int() >= KErrNone )
        {
        SubscribeForConnectionServiceChangeNotifications();
        }
    }

// -----------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::DoCancel
// -----------------------------------------------------------------------------
//
void CUpnpConnInterfaceWatcher::DoCancel()
    {
    iConnection.CancelServiceChangeNotification();
    }

// -----------------------------------------------------------------------------
// CUpnpConnInterfaceWatcher::RunError
// -----------------------------------------------------------------------------
//
TInt CUpnpConnInterfaceWatcher::RunError( TInt /*aError*/ )
    {
    LOG_FUNC_NAME;
    return KErrNone;
    }

TInetAddr CUpnpConnInterfaceWatcher::LocalIpAddress(TInt& aInterfaceIndex)
    {
    LOG_FUNC_NAME;
    
    RSocket socket;
    TInt err;
    TInt index; 
    TInt autoIfIndex = 0;
    TBool autoIpFound = EFalse;
    TBool ipFound = EFalse;
    TInetAddr localIpAddress;        
    TInetAddr autoIpAddress;
    
    TInt ret= socket.Open( iSockServ, KAfInet, KSockDatagram, KProtocolInetUdp);
    if ( KErrNone == ret )
          {
          err = socket.SetOpt( KSoInetEnumInterfaces, KSolInetIfCtrl );
           
          if ( err == KErrNone )
                {
                TPckgBuf<TSoInetInterfaceInfo> info;
                err = socket.GetOpt( KSoInetNextInterface, KSolInetIfCtrl, info );
                while ( err == KErrNone )
                    {
                    TPckgBuf<TSoInetIfQuery> query;
                    query().iName = info().iName;
                    err = socket.GetOpt( KSoInetIfQueryByName, KSolInetIfQuery, query );
                    if ( err == KErrNone )
                        {
                        index = query().iIndex;
                        
                          //Address should be IPv4, not 0.0.0.0 & not loopback.
                          if ((info().iAddress.Address() != 0) && !info().iAddress.IsUnspecified() && !info().iAddress.IsLoopback() && query().iZone[1] == iIapId)
                            {
                            
                        #ifdef _DEBUG
                             LOGS1( "LocalIpAddress: IAP is matched with %d",query().iZone[1] );
                             TBuf<50> buf;
                             info().iAddress.Output( buf );
                             LOGS1( "LocalIpAddress: info().iAddress: %S", &buf );
                        #endif
        
                             if ( info().iAddress.IsLinkLocal() )
                                 {
                                 autoIpFound = ETrue;
                                 autoIpAddress = info().iAddress;
                                 autoIpAddress.ConvertToV4();
                                 autoIfIndex = index;
                                 LOGS("LocalIpAddress: IP is Matched with AutoIP");
                            #ifdef _DEBUG
                                 TBuf<50> buf;
                                 autoIpAddress.Output( buf );
                                 LOGS1( "LocalIpAddress: Auto IP Addr: %S", &buf );
                            #endif
                                 }
                             // Other than auto IP
                             else
                                 {
                                 ipFound = ETrue;
                                 localIpAddress = info().iAddress;
                                 // we're using ipv4 addresses
                                 localIpAddress.ConvertToV4();
                                 aInterfaceIndex = index;
                                 LOGS("LocalIpAddress: IP is Matched with DHCP IP");
                            #ifdef _DEBUG
                                 TBuf<50> buf;
                                 localIpAddress.Output( buf );
                                 LOGS1( "LocalIpAddress: DHCP IP Addr: %S", &buf );
                            #endif
                                 }
                
                            }
                        }
                    err = socket.GetOpt( KSoInetNextInterface, KSolInetIfCtrl, info );
                    }
                }
          }
    socket.Close();
    
    if ( !ipFound && autoIpFound )
        {
        localIpAddress = autoIpAddress;
        aInterfaceIndex = autoIfIndex;
        }
    
#ifdef _DEBUG
    TBuf<50> buf;
    localIpAddress.Output( buf );
    LOGS1( "LocalIpAddress: IP Addr: %S", &buf );
#endif
    
    return localIpAddress;  
    
    }




//End of file
