/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  active object, that monitors activating
*
*/


#include "ikedebug.h"
#include "ikepolparser.h"
#include "internaladdress.h"
#include "vpnconnection.h"

// CLASS HEADER
#include "activationstarter.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Two-phased constructor.
// ---------------------------------------------------------------------------
//
CActivationStarter* CActivationStarter::NewL( CVpnConnection& aConnection,
                                              MActivationStarterCallback& aCallback,
                                              MIkeDebug& aDebug )
    {
    CActivationStarter* self = new ( ELeave ) CActivationStarter( aConnection,
                                                                    aCallback,
                                                                    aDebug );
    return self;
    }

// ---------------------------------------------------------------------------
// Destructor.
// ---------------------------------------------------------------------------
//
CActivationStarter::~CActivationStarter()
    {
    Cancel();
    }

// ---------------------------------------------------------------------------
// Constructor.
// ---------------------------------------------------------------------------
//
CActivationStarter::CActivationStarter( CVpnConnection& aConnection,
                                        MActivationStarterCallback& aCallback,
                                        MIkeDebug& aDebug )
 : CActive( EPriorityStandard ),
   iConnection( aConnection ),
   iCallback( aCallback ),
   iDebug( aDebug )
    {
    CActiveScheduler::Add( this );
    }

// ---------------------------------------------------------------------------
// Starts asynchronous activating. 
// ---------------------------------------------------------------------------
//
void CActivationStarter::Activate( CIkeData& aIkeData,
                                   const TDesC& aVpnIfName )
    {
    TUint32 vpnInterfaceIndex( 0 );
    TInt err = GetVpnInterfaceIndex( aVpnIfName,
                                     vpnInterfaceIndex );
    
    if ( err != KErrNone )
        {
        TRequestStatus* status = &iStatus;
        *status = KRequestPending;
        SetActive();
        
        User::RequestComplete( status, err );
        return;
        }
    
    TInetAddr remoteAddr = aIkeData.iAddr;    
    iIpVersion = IkeSocket::EIPv4;
    if ( remoteAddr.Family() == KAfInet6 &&
         !remoteAddr.IsV4Mapped() )
        {
        iIpVersion = IkeSocket::EIPv6;
        }        
    iDnsServerAddr = aIkeData.iDnsServer;
    
    iConnection.NegotiateWithHost( aIkeData,
                                   vpnInterfaceIndex,
                                   iIpVersion,
                                   iInternalAddress,
                                   iStatus );
    SetActive();
    }

// ---------------------------------------------------------------------------
// Gets VPN interface index. 
// ---------------------------------------------------------------------------
//
TInt CActivationStarter::GetVpnInterfaceIndex( const TDesC& aVpnIfName,
                                               TUint32& aVpnInterfaceIndex )
    {
    TInt err( KErrNone );
    RSocketServ ss;        
    err = ss.Connect();

    if ( err == KErrNone )
        {
        RSocket socket;
        err = socket.Open( ss,
                           KAfInet,
                           KSockDatagram,
                           KProtocolInetIp );    

        if ( err == KErrNone )
            {
            TPckgBuf<TSoInetIfQuery> opt;
            opt().iName = aVpnIfName;
        
            err = socket.GetOpt( KSoInetIfQueryByName,
                                 KSolInetIfQuery,
                                 opt );
            if ( err == KErrNone )
                {
                aVpnInterfaceIndex = opt().iZone[0]; // VPN Interface index
                }
            }
        socket.Close();
        }

    ss.Close();    
    return err;
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles completion of asynchronous activating. 
// ---------------------------------------------------------------------------
//
void CActivationStarter::RunL()
    {
    TInt err( iStatus.Int() );
    
    if ( err == KErrNone )
        {
        if ( iInternalAddress.iVPNIfAddr.IsUnspecified() )
            {
            TInetAddr localAddr;
            err = iConnection.GetLocalAddress( iIpVersion, localAddr );

            if ( err == KErrNone )
                {
                iInternalAddress.iVPNIfAddr = localAddr;
                }
            }
        }
    
    if ( err == KErrNone &&
         iInternalAddress.iVPNIfDNS1.IsUnspecified() )
        {    
        if ( iDnsServerAddr.Address() != KAFUnspec )
            {    
#ifdef _DEBUG        
            TBuf<39> addrBuf;
            iDnsServerAddr.OutputWithScope( addrBuf );
            DEBUG_LOG1(_L("DNS Server Address in IKE data %S"), &addrBuf);
#endif //_DEBUG                             
            iInternalAddress.iVPNIfDNS1 = iDnsServerAddr;
            }
        else
            {
            DEBUG_LOG(_L("DNS server not defined in policy"));
            }
        }
    
    iCallback.ActivationCompleted( err, iInternalAddress );
    }

// ---------------------------------------------------------------------------
// From class CActive
// Handles cancellation of asynchronous activating. 
// ---------------------------------------------------------------------------
//
void CActivationStarter::DoCancel()
    {
    iConnection.CancelNegotiateWithHost();
    }

