/** @file
* Copyright (c) 2005-2010 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  CUpnpSsdpServer
*
*/


// INCLUDE FILES
#include <in_sock.h>
#include <upnpsettings.h>
#include "upnpssdpserver.h"
#include "upnpcons.h"
#define KLogFile _L("DLNAWebServer.txt")
#include "upnpcustomlog.h"
#include "upnplist.h"


using namespace UpnpSSDP;
using namespace UpnpString;


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::CUpnpSsdpServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpSsdpServer::CUpnpSsdpServer( MUpnpSsdpServerObserver* aSsdpServerObserver,
                          RSocketServ* aSocketServ,
                          TInt aListeningPort,
                          CUpnpSsdpServer* aMasterServer )
   : CUpnpUdpServer( aSocketServ, this, aListeningPort )
    {
    iMasterServer=aMasterServer;

    iSsdpServerObserver = aSsdpServerObserver;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::CUpnpSsdpServer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CUpnpSsdpServer::CUpnpSsdpServer( MUpnpSsdpServerObserver* aSsdpServerObserver,
                          RSocketServ* aSocketServ,
                          TInt aListeningPort )
        : CUpnpUdpServer( aSocketServ, this, aListeningPort )
    {
    iMasterServer = NULL;

    iSsdpServerObserver = aSsdpServerObserver;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::ConstructL()
    {
    UdpConstructL();    
    iServerDescription =  CUpnpSettings::GetServerHeaderL();    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSsdpServer* CUpnpSsdpServer::NewL( MUpnpSsdpServerObserver*
                                                         aSsdpServerObserver,
                                         RSocketServ* aSocketServ,
                                         TInt aListeningPort )
    {
    CUpnpSsdpServer* self = new (ELeave) CUpnpSsdpServer( aSsdpServerObserver,
                                                  aSocketServ,
                                                  aListeningPort );
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpSsdpServer* CUpnpSsdpServer::NewL( MUpnpSsdpServerObserver* 
                                                        aSsdpServerObserver,
                                         RSocketServ* aSocketServ,
                                         TInt aListeningPort,
                                         CUpnpSsdpServer* aMasterServer )
    {
    CUpnpSsdpServer* self = new (ELeave) CUpnpSsdpServer( aSsdpServerObserver,
                                                  aSocketServ,
                                                  aListeningPort,
                                                  aMasterServer );
    
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(self);
    
    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
// 
EXPORT_C CUpnpSsdpServer::~CUpnpSsdpServer()
    {
    delete iServerDescription;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::MessageFromMulticastL
// Message from multicast.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::MessageFromMulticastL( CUpnpSsdpMessage* aMessage )
    {
    ASSERT ( !iSocketDown );
    UdpMessageReceivedL( aMessage );
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::UdpMessageReceivedL
// UDP message received.
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::UdpMessageReceivedL( CUdpMessage* aMessage )
    {
    if ( !aMessage )
        {
        return;
        }
    CUpnpSsdpMessage* message = ( CUpnpSsdpMessage* ) aMessage;
   
       // Unless it's response, check if HOST header has explicitly specified port number - 1900
       // If not, then add this port
       if ( !message->IsSsdpResponse() )
           {
           TPtrC8 host = message->GetHeaderValue( KHdrHost );
           
           // : Not found so port is not specified
           if( host.Find( UpnpString::KColon() ) == KErrNotFound )
            {
            TBuf8<KMaxIntegerLength> defaultPort;
               defaultPort.Num( KMulticastPort );
               
               HBufC8* newHost = HBufC8::NewLC( host.Length() +
                                       UpnpString::KColon().Length() +
                                       defaultPort.Length() );
               
               newHost->Des().Zero();
               newHost->Des().Append( host );
               newHost->Des().Append( UpnpString::KColon() );
               newHost->Des().Append( defaultPort );
                   
               message->AddHostL( *newHost );
               CleanupStack::PopAndDestroy(newHost);
            }
           }
   
    if ( iMasterServer )
        {
        iMasterServer->MessageFromMulticastL( message );
        }
    else
        {
        if ( message->IsSsdpMSearch() )
            {
            LOGS( "SSDP *** Search received");
            ResponseL( message );
            // add additional notification to upper-layer 
            // used for detection IP address change
            iSsdpServerObserver->SsdpEventL( message );
            }
        else if (message->IsSsdpNotify())
            {
            LOGS( "SSDP *** Notify received");
            iSsdpServerObserver->SsdpEventL( message );
            }
        else if (message->IsSsdpResponse())
            {
            LOGS( "SSDP *** Response received");
            iSsdpServerObserver->SsdpEventL( message );
            }
        else
            {
            LOGS( "SSDP *** UNKNOWN MESSAGE RECEIVED");
            }
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::StartL
// Start.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSsdpServer::StartL()
    {
    LOGS( "SSDP *** StartL");
    OpenL();    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::StopL
// Stop.
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSsdpServer::Stop()
    {
    LOGS( "SSDP *** Stop");
    Close();
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::AdvertiseAliveL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSsdpServer::AdvertiseAliveL( CUpnpDeviceLibraryElement& aDev )
    {
    LOGS( "SSDP *** AdvertiseAliveL");
    if ( iSocketDown )
        {
        // Leaves if the UDP socket is destroyed
        User::Leave(iLastSocketError);
        }
    // advertise this device
    SingleDeviceAdvertiseAliveL( aDev );
    SingleDeviceAdvertiseAliveL( aDev );
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::AdvertiseByebyeL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSsdpServer::AdvertiseByebyeL( CUpnpDeviceLibraryElement& aDev )
    {
    if ( iSocketDown )
        {
        // Leaves if the UDP socket is destroyed
        User::Leave(iLastSocketError);
        }
    SingleDeviceAdvertiseByebyeL( aDev );
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::SearchL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSsdpServer::SearchL( TDesC8& aTarget )
    {
    if ( iSocketDown )
        {
        // Leaves if the UDP socket is destroyed
        User::Leave(iLastSocketError);
        }
    SearchL(aTarget, (TDesC8&)UpnpSSDP::KDefaultResponseDelay());
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::SearchL
// 
// -----------------------------------------------------------------------------
//
EXPORT_C void CUpnpSsdpServer::SearchL( TDesC8& aTarget, TDesC8& aMaximumWaitTime )
    {
    if ( iSocketDown )
        {
        // Leaves if the UDP socket is destroyed
        User::Leave(iLastSocketError);
        }
    CUpnpSsdpMessage* msg = RUpnpSsdpMessageFactory::SearchL( aTarget );
    CleanupStack::PushL( msg );
    msg->AddMxL(aMaximumWaitTime);
    
    HBufC8* str = HBufC8::NewLC( KMaxSsdpMessageLength );
    
    HBufC8* msgStr = ( ( CUpnpSsdpMessage* ) msg )->ToStringL();
    CleanupStack::PushL( msgStr );
    
    str->Des().Append( *msgStr );
    
    CleanupStack::PopAndDestroy( msgStr );
    
    TInetAddr addr( KDefaultMulticastAddress, KDefaultMulticastPort );
    
    SendL( *str, addr );
    
    CleanupStack::PopAndDestroy( str );
    CleanupStack::PopAndDestroy( msg );
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::SendAdvertiseAliveL
//
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::SendAdvertiseAliveL( TDesC8& aNt, TDesC8& aUsn, TDesC8& aLoc )
    {
    TInetAddr addr( KDefaultMulticastAddress, KDefaultMulticastPort );
    
    CUpnpSsdpMessage* msg = RUpnpSsdpMessageFactory::AdvertiseAliveL( 
                                            aLoc, aNt, aUsn, *iServerDescription);
    CleanupStack::PushL( msg );
    HBufC8* str = msg->ToStringL();
    CleanupStack::PushL( str );
    
    SendL( *str, addr );
    
    CleanupStack::PopAndDestroy( str );
    str=NULL;
    
    CleanupStack::PopAndDestroy( msg );
    msg=NULL;    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::SendAdvertiseByebyeL
//
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::SendAdvertiseByebyeL( TDesC8& aNt, TDesC8& aUsn )
    {
    TInetAddr addr( KDefaultMulticastAddress, KDefaultMulticastPort );
    
    CUpnpSsdpMessage* msg = RUpnpSsdpMessageFactory::AdvertiseByebyeL( aNt, aUsn );
    CleanupStack::PushL( msg );
    HBufC8* str = msg->ToStringL();
    CleanupStack::PushL( str );    
    
    SendL( *str, addr );
    
    CleanupStack::PopAndDestroy( str );
    str=NULL;
    
    CleanupStack::PopAndDestroy( msg );
    msg=NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::SendResponseL
//
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::SendResponseL( TInetAddr& aDest, 
                                 TDesC8& aSt, 
                                 TDesC8& aUsn, 
                                 TDesC8& aLoc )
    {
    CUpnpSsdpMessage* msg = RUpnpSsdpMessageFactory::ResponseL( 
                                        aSt, aUsn, aLoc, *iServerDescription );
    CleanupStack::PushL( msg );
    HBufC8* str = msg->ToStringL();
    CleanupStack::PushL( str );
    
    SendL( *str, aDest );
    
    CleanupStack::PopAndDestroy( str );
    str=NULL;
    
    CleanupStack::PopAndDestroy( msg );
    msg=NULL;    
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::SingleDeviceAdvertiseAliveL
//
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::SingleDeviceAdvertiseAliveL( CUpnpDeviceLibraryElement& aDev )
    {
    HBufC8* nt=NULL;
    HBufC8* usn=NULL;
    HBufC8* loc=NULL;
    
    loc=HBufC8::NewLC( aDev.DescriptionUrl().Length() );
    loc->Des().Zero();
    loc->Des().Append( aDev.DescriptionUrl() );
    
    CDesC8Array* services = &aDev.ServiceList();
    
    if ( aDev.IsRootDevice() )
        {
        
        nt = HBufC8::NewLC( KUPnPRootDevice().Length() );
        nt->Des().Zero();
        nt->Des().Append( KUPnPRootDevice() );
        
        usn = HBufC8::NewLC( KUPnPUuid().Length()
            + aDev.UUID().Length()
            + KDblColon().Length()
            + KUPnPRootDevice().Length() );

        usn->Des().Zero();
        usn->Des().Append( KUPnPUuid() );
        usn->Des().Append( aDev.UUID() );
        usn->Des().Append( KDblColon() );
        usn->Des().Append( KUPnPRootDevice() );
        
        SendAdvertiseAliveL( *nt, *usn, *loc );
        
        CleanupStack::PopAndDestroy( usn );
        usn = NULL;
        
        CleanupStack::PopAndDestroy( nt );
        nt = NULL;
        }
    
    nt=HBufC8::NewLC( KUPnPUuid().Length() + aDev.UUID().Length() );
    nt->Des().Zero();
    nt->Des().Append( KUPnPUuid() );
    nt->Des().Append( aDev.UUID() );
    
    usn=HBufC8::NewLC( KUPnPUuid().Length() + aDev.UUID().Length() );
    usn->Des().Zero();
    usn->Des().Append( KUPnPUuid() );
    usn->Des().Append( aDev.UUID() );
    
    SendAdvertiseAliveL( *nt, *usn, *loc );
    
    CleanupStack::PopAndDestroy( usn );
    usn = NULL;
    
    CleanupStack::PopAndDestroy( nt );
    nt = NULL;
    
    nt = HBufC8::NewLC( aDev.DeviceType().Length() );

    nt->Des().Zero();
    nt->Des().Append( aDev.DeviceType() );
    
    usn = HBufC8::NewLC( KUPnPUuid().Length()
        + aDev.UUID().Length()
        + KDblColon().Length()
        + aDev.DeviceType().Length() );

    usn->Des().Zero();
    usn->Des().Append( KUPnPUuid() );
    usn->Des().Append( aDev.UUID() );
    usn->Des().Append( KDblColon() );
    usn->Des().Append( aDev.DeviceType() );
    
    SendAdvertiseAliveL( *nt, *usn, *loc );
    
    CleanupStack::PopAndDestroy( usn );
    usn = NULL;
    
    CleanupStack::PopAndDestroy( nt );
    nt = NULL;
    
    if(!services)
        {
        return;
        }
    
    TInt NumOfServices = services->Count();
    
    TInt i = 0;
    
    for( i = 0; i < NumOfServices; i++ )
        {
        
        nt = HBufC8::NewLC( KUPnPServiceSchema().Length()
            + (*services)[i].Length() );

        nt->Des().Zero();
        nt->Des().Append( KUPnPServiceSchema() );
        nt->Des().Append( (*services)[i] );
        
        usn=HBufC8::NewLC( KUPnPUuid().Length()
            + aDev.UUID().Length()
            + KDblColon().Length()
            + KUPnPServiceSchema().Length()
            + (*services)[i].Length() );

        usn->Des().Zero();
        usn->Des().Append( KUPnPUuid() );
        usn->Des().Append( aDev.UUID() );
        usn->Des().Append( KDblColon() );
        usn->Des().Append( KUPnPServiceSchema() );
        usn->Des().Append( (*services)[i] );
        
        SendAdvertiseAliveL( *nt, *usn, *loc );
        
        CleanupStack::PopAndDestroy( usn );
        usn = NULL;
        
        CleanupStack::PopAndDestroy( nt );
        nt = NULL;
        }
    
    CleanupStack::PopAndDestroy( loc );
    loc = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::SingleDeviceAdvertiseByebyeL
//
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::SingleDeviceAdvertiseByebyeL(CUpnpDeviceLibraryElement& aDev)
    {
    HBufC8* nt=NULL;
    HBufC8* usn=NULL;
    
    CDesC8Array* services = &aDev.ServiceList();
    
    if (aDev.IsRootDevice())
        {
        nt=HBufC8::NewLC(KUPnPRootDevice().Length());
        nt->Des().Copy(KUPnPRootDevice());
        
        usn=HBufC8::NewLC(KUPnPUuid().Length()
            +aDev.UUID().Length()
            +UpnpString::KDblColon().Length()
            +KUPnPRootDevice().Length());

        usn->Des().Zero();
        usn->Des().Append(KUPnPUuid());
        usn->Des().Append(aDev.UUID());
        usn->Des().Append(UpnpString::KDblColon());
        usn->Des().Append(KUPnPRootDevice());
        
        SendAdvertiseByebyeL(*nt, *usn);
        
        CleanupStack::PopAndDestroy(usn); 
        usn=NULL;
        
        CleanupStack::PopAndDestroy(nt);
        nt=NULL;
        }
    
    nt=HBufC8::NewLC(KUPnPUuid().Length()+aDev.UUID().Length());
    nt->Des().Zero();
    nt->Des().Append(KUPnPUuid());
    nt->Des().Append(aDev.UUID());
    
    usn=HBufC8::NewLC(KUPnPUuid().Length()+aDev.UUID().Length());
    usn->Des().Zero();
    usn->Des().Append(KUPnPUuid());
    usn->Des().Append(aDev.UUID());
    
    
    SendAdvertiseByebyeL(*nt, *usn);
    
    
    CleanupStack::PopAndDestroy(usn);
    usn=NULL;
    
    CleanupStack::PopAndDestroy(nt);
    nt=NULL;
    
    nt=HBufC8::NewLC(aDev.DeviceType().Length());

    nt->Des().Zero();
    nt->Des().Append(aDev.DeviceType());    
    
    usn=HBufC8::NewLC(KUPnPUuid().Length()
        +aDev.UUID().Length()
        +UpnpString::KDblColon().Length()
        +aDev.DeviceType().Length());

    usn->Des().Zero();
    usn->Des().Append(KUPnPUuid());
    usn->Des().Append(aDev.UUID());
    usn->Des().Append(UpnpString::KDblColon());
    usn->Des().Append(aDev.DeviceType());
    
    
    SendAdvertiseByebyeL(*nt, *usn);
    
    
    CleanupStack::PopAndDestroy(usn);
    usn=NULL;
    
    CleanupStack::PopAndDestroy(nt); 
    nt=NULL;
    
    TInt i=0;
    
    if(!services)
        {
        return;
        }
    
    TInt NumOfServices=services->Count();
    
    for(i=0; i<NumOfServices; i++)
        {
        
        nt=HBufC8::NewLC(KUPnPServiceSchema().Length()
            +(*services)[i].Length());

        nt->Des().Zero();
        nt->Des().Append(KUPnPServiceSchema());
        nt->Des().Append((*services)[i]);
        
        usn=HBufC8::NewLC(KUPnPUuid().Length()
            +aDev.UUID().Length()
            +UpnpString::KDblColon().Length()
            +KUPnPServiceSchema().Length()
            +(*services)[i].Length());

        usn->Des().Zero();
        usn->Des().Append(KUPnPUuid());
        usn->Des().Append(aDev.UUID());
        usn->Des().Append(UpnpString::KDblColon());
        usn->Des().Append(KUPnPServiceSchema());
        usn->Des().Append((*services)[i]);
        
        
        SendAdvertiseByebyeL(*nt, *usn);
        
        
        CleanupStack::PopAndDestroy(usn);
        usn=NULL;
        
        CleanupStack::PopAndDestroy(nt);
        nt=NULL;
        }
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::SingleServiceResponseL
//
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::SingleServiceResponseL( CUpnpDeviceLibraryElement* aDevice, 
                                          const TDesC8& aServiceType, 
                                          CUpnpSsdpMessage* aMsg )
    {
    if ( !aDevice || !aMsg  )
        {
        return;
        }
    // name buffers
    HBufC8* nt=NULL;
    HBufC8* usn=NULL;
    HBufC8* loc=NULL;
    
    // set location buffer to point to the 
    // service description file of this service
    // e.g. loc = 192.168.100.2/mediaserver/desc.xml
    
    loc=HBufC8::NewLC(aDevice->DescriptionUrl().Length());
    loc->Des().Zero();
    loc->Des().Append(aDevice->DescriptionUrl());
    

    // separate different search targets
    switch(aMsg->SearchTarget())
        {
        case CUpnpSsdpMessage::ESsdpNone:
            
            break;
            
        case CUpnpSsdpMessage::ESsdpAll:
            // In case of Ssdp:all search, 
            // every service should send a device uuid response
            
            // 1. service type response
            /*
            NT:     urn:schemas-upnp-org:service:serviceType:v
            USN:     uuid:device-UUID::urn:schemas-upnp-org:service:serviceType:v
            */
            
            // nt = urn:schemas-upnp-org:service:serviceType:v
             nt = HBufC8::NewLC( KUPnPServiceSchema().Length() +
                                 aServiceType.Length() );
            nt->Des().Zero();
            nt->Des().Append(KUPnPServiceSchema());
            nt->Des().Append(aServiceType);
            
            // usn = uuid:device-UUID::urn:schemas-upnp
            // -org:service:serviceType:v
            
            usn=HBufC8::NewLC(KUPnPUuid().Length()
                +aDevice->UUID().Length()
                +UpnpString::KDblColon().Length()
                +KUPnPServiceSchema().Length()
                +aServiceType.Length());

            usn->Des().Zero();
            usn->Des().Append(KUPnPUuid());
            usn->Des().Append(aDevice->UUID());
            usn->Des().Append(UpnpString::KDblColon());
            usn->Des().Append(KUPnPServiceSchema());
            usn->Des().Append(aServiceType);    
            
            // send response to sender
            SendResponseL(aMsg->Sender() ,*nt, *usn, *loc);
            
            CleanupStack::PopAndDestroy(usn);
            usn=NULL;
            
            CleanupStack::PopAndDestroy(nt);
            nt=NULL;
            
            break;
            
        case CUpnpSsdpMessage::EUPnPRootDevice:
            
            break;
            
        case CUpnpSsdpMessage::EDeviceUid:
            
            break;
            
        case CUpnpSsdpMessage::EDeviceType:
            
            break;
            
        case CUpnpSsdpMessage::EServiceType:
            {
            /*
            NT:     urn:schemas-upnp-org:service:serviceType:v
            USN:     uuid:device-UUID::urn:schemas-upnp-
                         org:service:serviceType:v
            */
                
             // nt = urn:schemas-upnp-org:service:serviceType:v
             nt = HBufC8::NewLC( KUPnPServiceSchema().Length() + 
                                    aServiceType.Length());

             nt->Des().Zero();
             nt->Des().Append(KUPnPServiceSchema());
             nt->Des().Append(aServiceType);
            
            if (aMsg->St().Compare(*nt) == 0)
                {
                
                // usn = uuid:device-UUID::urn:schemas-upnp
                //       -org:service:serviceType:v
                usn=HBufC8::NewLC(KUPnPUuid().Length()
                    +aDevice->UUID().Length()
                    +UpnpString::KDblColon().Length()
                    +KUPnPServiceSchema().Length()
                    +aServiceType.Length());

                usn->Des().Zero();
                usn->Des().Append(KUPnPUuid());
                usn->Des().Append(aDevice->UUID());
                usn->Des().Append(UpnpString::KDblColon());
                usn->Des().Append(KUPnPServiceSchema());
                usn->Des().Append(aServiceType);    
                
                
                // send response to sender
                SendResponseL(aMsg->Sender() ,*nt, *usn, *loc);
                
                
                CleanupStack::PopAndDestroy(usn);
                usn=NULL;
                
                
                }        
            
            CleanupStack::PopAndDestroy(nt);
            nt=NULL;
            }
            break;
        default:
            
            break;
        }
        
        CleanupStack::PopAndDestroy(loc);
        loc=NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::SingleDeviceResponseL
//
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::SingleDeviceResponseL( CUpnpDeviceLibraryElement* aDevice, 
                                         CUpnpSsdpMessage* aMsg )
    {
    if ( !aDevice || !aMsg )
        {
        return;
        }
    
    // name buffers
    HBufC8* nt=NULL;
    HBufC8* usn=NULL;
    HBufC8* loc=NULL;
    
    // format location buffer to point 
    // to the device description file of this device
    
    loc=HBufC8::NewLC(aDevice->DescriptionUrl().Length());
    loc->Des().Zero();
    loc->Des().Append(aDevice->DescriptionUrl());
    
    // separate different search targets
    switch( aMsg->SearchTarget() )
        {
        case CUpnpSsdpMessage::ESsdpNone:
            
            
            break;
            
        case CUpnpSsdpMessage::ESsdpAll:
            // In case of Ssdp:all search, every device should 
            // send following responses:
            // 1. if a root device, a root device response
            // 2. a device uuid response
            // 3. a device type response
            
            //************************
            // 1. root device response        
            /*
            NT:     upnp:rootdevice
            USN:     uuid:device-UUID::upnp:rootdevice
            */
            // if a root device
            if (aDevice->IsRootDevice())
                {
                
                
                nt=HBufC8::NewLC(KUPnPRootDevice().Length());
                nt->Des().Zero();
                nt->Des().Append(KUPnPRootDevice());
                
                // nt = upnp:rootdevice
                
                // usn = uuid:device-UUID::upnp:rootdevice
                
                usn=HBufC8::NewLC(KUPnPUuid().Length()
                    +aDevice->UUID().Length()
                    +UpnpString::KDblColon().Length()
                    +KUPnPRootDevice().Length());

                usn->Des().Zero();
                usn->Des().Append(KUPnPUuid());
                usn->Des().Append(aDevice->UUID());
                usn->Des().Append(UpnpString::KDblColon());
                usn->Des().Append(KUPnPRootDevice());
                
                // send response to sender
                SendResponseL(aMsg->Sender(), *nt, *usn, *loc);
                
                CleanupStack::PopAndDestroy(usn);
                usn=NULL;
                
                CleanupStack::PopAndDestroy(nt);
                nt=NULL;
                }
            
            
            //************************
            // 2. device uuid response
            /*
            NT:     uuid:device-UUID
            USN:     uuid:device-UUID
            */
            
            nt=HBufC8::NewLC(KUPnPUuid().Length()+aDevice->UUID().Length());
            nt->Des().Zero();
            nt->Des().Append(KUPnPUuid());
            nt->Des().Append(aDevice->UUID());
            
            // nt = uuid:device-UUID
            
            // usn = uuid:device-UUID
            
            usn=HBufC8::NewLC(KUPnPUuid().Length()+aDevice->UUID().Length());
            usn->Des().Zero();
            usn->Des().Append(KUPnPUuid());
            usn->Des().Append(aDevice->UUID());
            
            // send response to sender
            SendResponseL(aMsg->Sender(), *nt, *usn, *loc);
            
            CleanupStack::PopAndDestroy(usn);
            usn=NULL;
            
            CleanupStack::PopAndDestroy(nt);
            nt=NULL;
            
            
            //************************
            // 3. device type response
            /*
            NT:     urn:schemas-upnp-org:device:deviceType:v
            USN:     uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v
            */
            
            nt=HBufC8::NewLC(aDevice->DeviceType().Length());

            nt->Des().Zero();
            nt->Des().Append(aDevice->DeviceType());
            
            // nt = urn:schemas-upnp-org:device:deviceType:v
            
            // usn = uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v
            
            usn=HBufC8::NewLC(KUPnPUuid().Length()
                +aDevice->UUID().Length()
                +UpnpString::KDblColon().Length()
                +aDevice->DeviceType().Length());

            usn->Des().Zero();
            usn->Des().Append(KUPnPUuid());
            usn->Des().Append(aDevice->UUID());
            usn->Des().Append(UpnpString::KDblColon());
            usn->Des().Append(aDevice->DeviceType());
            
            // send response to sender
            SendResponseL(aMsg->Sender(), *nt, *usn, *loc);
            
            
            CleanupStack::PopAndDestroy(usn);
            usn=NULL;
            
            CleanupStack::PopAndDestroy(nt);
            nt=NULL;
            
            
            break;
            
        case CUpnpSsdpMessage::EUPnPRootDevice:
            // if the search is done for only root devices, 
            // every root device should answer with a root device response
            
            //************************
            // 1. root device response        
            /*
            NT:     upnp:rootdevice
            USN:     uuid:device-UUID::upnp:rootdevice
            */
            // if device is a root device
            if (aDevice->IsRootDevice())
                {
                
                nt=HBufC8::NewLC(KUPnPRootDevice().Length());
                nt->Des().Zero();
                nt->Des().Append(KUPnPRootDevice());
                
                usn=HBufC8::NewLC(KUPnPUuid().Length()
                    +aDevice->UUID().Length()
                    +UpnpString::KDblColon().Length()
                    +KUPnPRootDevice().Length());

                usn->Des().Zero();
                usn->Des().Append(KUPnPUuid());
                usn->Des().Append(aDevice->UUID());
                usn->Des().Append(UpnpString::KDblColon());
                usn->Des().Append(KUPnPRootDevice());
                
                // send response to sender
                SendResponseL(aMsg->Sender(), *nt, *usn, *loc);
                
                CleanupStack::PopAndDestroy(usn);
                usn=NULL;
                
                CleanupStack::PopAndDestroy(nt); 
                nt=NULL;
                }
            
            
            break;
            
        case CUpnpSsdpMessage::EDeviceUid:
            {
            // if the search is done for only devices with specific uuid,
            // response if the uuids match
            
            //************************
            // 2. device uuid response
            /*
            NT:     uuid:device-UUID
            USN:     uuid:device-UUID
            */
            
            // nt = uuid:device-UUID
            nt=HBufC8::NewLC(KUPnPUuid().Length()
                    +aDevice->UUID().Length());

            nt->Des().Zero();
            nt->Des().Append(KUPnPUuid());
            nt->Des().Append(aDevice->UUID());
            
            
            // if device uuids match
            if (aMsg->St().Compare(*nt) == 0)
                {

                usn=HBufC8::NewLC(KUPnPUuid().Length()
                    +aDevice->UUID().Length());

                usn->Des().Zero();
                usn->Des().Append(KUPnPUuid());
                usn->Des().Append(aDevice->UUID());
                
                // send response to sender
                SendResponseL(aMsg->Sender(), *nt, *usn, *loc);
                
                CleanupStack::PopAndDestroy(usn);
                usn=NULL;
                
                
                }
                CleanupStack::PopAndDestroy(nt);
                nt=NULL;
            
            }
            break;
            
        case CUpnpSsdpMessage::EDeviceType:
            {
            // if the search is done for only devices of specific type,
            // response if types match
            
            //************************
            // 3. device type response
            /*
            NT:     urn:schemas-upnp-org:device:deviceType:v
            USN:     uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v
            */
                        
            // nt = urn:schemas-upnp-org:device:deviceType:v                
             nt=HBufC8::NewLC(aDevice->DeviceType().Length());

             nt->Des().Zero();
             nt->Des().Append(aDevice->DeviceType());
            
            // if device types match
            if (aMsg->St().Compare(*nt) == 0)
                {         
                
                // usn = uuid:device-UUID::urn:schemas-upnp-org:device:deviceType:v                
                usn=HBufC8::NewLC(KUPnPUuid().Length()
                    +aDevice->UUID().Length()
                    +KDblColon().Length()
                    +aDevice->DeviceType().Length());

                usn->Des().Zero();
                usn->Des().Append(KUPnPUuid());
                usn->Des().Append(aDevice->UUID());
                usn->Des().Append(KDblColon());
                usn->Des().Append(aDevice->DeviceType());
                
                // send response to sender
                SendResponseL(aMsg->Sender(), *nt, *usn, *loc);
                
                CleanupStack::PopAndDestroy(usn);
                usn=NULL;
                
                
                }
                CleanupStack::PopAndDestroy(nt);
                nt=NULL;

            
            }
            break;
            
        case CUpnpSsdpMessage::EServiceType:
            // devices do not answer if searching services
            
            break;
        default:
            
            break;
        }
        
    CleanupStack::PopAndDestroy( loc );
    loc = NULL;
    }

// -----------------------------------------------------------------------------
// CUpnpSsdpServer::ResponseL
//
// -----------------------------------------------------------------------------
//
void CUpnpSsdpServer::ResponseL(CUpnpSsdpMessage* aMsg)
    {
    if ( !aMsg )
        {
        return;
        }
    
    // a search message is received
    // seek for devices and services matching to the search
    
    // get device list from message handler
    RPointerArray<CUpnpDeviceLibraryElement>& devs 
        = iSsdpServerObserver->DeviceList();
   

    TInt devCount=0;
     // get number of devices
    devCount=devs.Count();
    
    TInt j=0;
    
    for(j=0;j<devCount;j++)
        {
        CUpnpDeviceLibraryElement* dev = (CUpnpDeviceLibraryElement*) devs[j];
        
        if (dev->Local())
            {
            // for every device
            // make this device respond if needed
            SingleDeviceResponseL(dev, aMsg);
            // get the service list of this device
            CDesC8Array* services = &dev->ServiceList();
            // get the first service
            
            for(TInt i=0; i<services->Count(); i++)
                {
                TPtrC8 serviceType = (*services)[i];
                SingleServiceResponseL(dev, serviceType, aMsg);
                }
            }
        }
    }

//  End of File
