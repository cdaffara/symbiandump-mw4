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
 * Description:  Declares main application class.
 *
 */

#ifndef C_CUPNPSSDPSERVER_H
#define C_CUPNPSSDPSERVER_H

// INCLUDES
#include <e32base.h>
#include <es_sock.h>
#include <in_sock.h>
#include "upnpudpserver.h"
#include "upnpssdpmessagefactory.h"
#include "upnpssdpserverobserver.h"

// FORWARD DECLARATIONS
class CUpnpMessageHandler;
class CUpnpSsdpMessage;
class CUpnpDeviceLibraryElement;

// CLASS DECLARATION

/**
 *  CUpnpSsdpServer is used to send and receive SSDP messages.
 *
 *  @since Series60 2.0
 */
NONSHARABLE_CLASS(CUpnpSsdpServer) : public CUpnpUdpServer,
        public MUpnpUdpServerObserver
    {
public:
    // Constructors and destructor
    /**
     * Two-phased constructor.
     * @param aSsdpServerObserver Pointer that is used for callbacks.
     * @param aSocketServ Pointer to used Socket Server.
     * @param aListeningPort Port that server is listening to.
     * @return A new CUpnpSsdpServer instance.
     */
    IMPORT_C static CUpnpSsdpServer* NewL(
        MUpnpSsdpServerObserver* aSsdpServerObserver,
        RSocketServ* aSocketServ, TInt aListeningPort );

    /**
     * Two-phased constructor.
     * @param aSsdpServerObserver Pointer that is used for callbacks.
     * @param aSocketServ Pointer to used Socket Server.
     * @param aListeningPort Port that server is listening to.
     * @param aMasterServer Pointer to master server that generates the responses
     for multicast messages.   
     * @return A new CUpnpSsdpServer instance.
     */
    IMPORT_C static CUpnpSsdpServer* NewL(
        MUpnpSsdpServerObserver* aSsdpServerObserver,
        RSocketServ* aSocketServ, TInt aListeningPort,
        CUpnpSsdpServer* aMasterServer );

    /**
     * Destructor.
     */
    IMPORT_C virtual ~CUpnpSsdpServer();

public:
    // New functions

    /**
     * Starts the CUpnpSsdpServer.
     * @since Series60 2.0
     * @return None
     */
    IMPORT_C void StartL();

    /**
     * Stops the CUpnpSsdpServer.
     * @since Series60 2.0
     * @return None
     */
    IMPORT_C void Stop();

public:
    // public functions for SSDP advertising and searching

    /**
     * Advertises element aDev in CUpnpDeviceLibrary.
     * @since Series60 2.0
     * @param aDev Pointer to advertised device.
     * @return None
     */
    /**
     Advertises element aDev in CUpnpDeviceLibrary.
     @param aDev Pointer to advertised device.
     @return None.
     */
    IMPORT_C void AdvertiseAliveL( CUpnpDeviceLibraryElement& aDev );

    /**
     * Advertises ByeBye for element aDev in CUpnpDeviceLibrary.
     * @since Series60 2.0
     * @param aDev Pointer to advertised device.
     * @return None
     */
    /**
     Advertises ByeBye for element aDev in CUpnpDeviceLibrary.
     @param aDev Pointer to advertised device.
     @return None.
     */
    IMPORT_C void AdvertiseByebyeL( CUpnpDeviceLibraryElement& aDev );

    /**
     * Sends a SSDP search, searching for aTarget.
     * @since Series60 2.0
     * @param aTarget Search String.
     * @return None
     */
    IMPORT_C void SearchL( TDesC8& aTarget );

    /**
     * Sends a SSDP search, searching for aTarget.
     * @since Series60 2.0
     * @param aTarget Search String.
     * @param aMaximumWaitTime Value of the MX-header.
     * @return None
     */
    IMPORT_C void SearchL( TDesC8& aTarget, TDesC8& aMaximumWaitTime );

    /**
     * Master server is receiving messages sent to multicast \
        * address via this function and generates the response.
     * @since Series60 2.0
     * @param aMessage Received message
     */
    void MessageFromMulticastL( CUpnpSsdpMessage* aMessage );

private:

    /**
     * C++ default constructor.
     */
    CUpnpSsdpServer( MUpnpSsdpServerObserver* aSsdpServerObserver,
        RSocketServ* aSocketServ, TInt aListeningPort );

    /**
     * C++ default constructor.
     */
    CUpnpSsdpServer( MUpnpSsdpServerObserver* aSsdpServerObserver,
        RSocketServ* aSocketServ, TInt aListeningPort,
        CUpnpSsdpServer* aMasterServer );

    /**
     * By default Symbian 2nd phase constructor is private.
     */
    void ConstructL();

private:
    // private ssdp:alive functions

    /**
     * Sends a single alive message to network using given paramaters.
     * @since Series60 2.0
     * @param aNt Value of NT header
     * @param aUsn Value of USN header
     * @param aLoc Value of LOCATION header
     */
    void SendAdvertiseAliveL( TDesC8& aNt, TDesC8& aUsn, TDesC8& aLoc );

    /**
     * Advertises a device in device library.
     * @since Series60 2.0
     * @param aDev Reference to device that is to be advertised.
     */
    void SingleDeviceAdvertiseAliveL( CUpnpDeviceLibraryElement& aDev );

private:
    // private ssdp:byebye functions

    /**
     * Sends a BYEBYE message to network using given paramaters.
     * @since Series60 2.0
     * @param aNt Value of NT header
     * @param aUsn Value of USN header
     * @return ?description
     */
    void SendAdvertiseByebyeL( TDesC8& aNt, TDesC8& aUsn );

    /**
     * Sends a single device BYEBYE advertisement
     * @since Series60 2.0
     * @param aDev Device to be advertised.
     */
    void SingleDeviceAdvertiseByebyeL( CUpnpDeviceLibraryElement& aDev );

private:
    // private ssdp response generating functions

    /**
     * Function that is used to determine what kind of response is
     * required to be sent. 
     * @since Series60 2.0
     * @param aMsg Message that contains the search request.
     */
    void ResponseL( CUpnpSsdpMessage* aMsg );

    /**
     * Sends a search response with given parameters.
     * @since Series60 2.0
     * @param aDest Address of remote host
     * @param aSt Value of ST header
     * @param aUsn Value of USN header
     * @param aLoc Value of LOCATION header.     
     */
    void SendResponseL( TInetAddr& aDest, TDesC8& aSt, TDesC8& aUsn,
        TDesC8& aLoc );

    /**
     * Creates a search response to given search message.
     * This advertises only the device.
     * @since Series60 2.0
     * @param aDevice Device that needs to be advertised.
     * @param aMsg Search request message.
     * @return ?description
     */
    void SingleDeviceResponseL( CUpnpDeviceLibraryElement* aDevice,
        CUpnpSsdpMessage* aMsg );

    /**
     * Creates a response to search message aMsg from device aDevice.
     * This response concerns about aDevices certain service, aServiceType
     * @since Series60 2.0
     * @param aDevice Device that sends the response.
     * @param aServiceType Service that resides in aDevice.
     * @param aMsg Message that requires the response to be sent (request)
     */
    void SingleServiceResponseL( CUpnpDeviceLibraryElement* aDevice,
        const TDesC8& aServiceType, CUpnpSsdpMessage* aMsg );

private:

    /**
     * Callback function from CUpnpUdpServer.
     * @since Series60 2.0
     * @param aMessage Message that was received thru CUpnpUdpServer.
     * @return None
     */
    void UdpMessageReceivedL( CUdpMessage* aMessage );

private:
    // Data

    // SSDP server observer
    MUpnpSsdpServerObserver* iSsdpServerObserver;

    // SSDP master server
    CUpnpSsdpServer* iMasterServer;

    // value foro SERVER header in SSDP messages
    HBufC8* iServerDescription;

    };

#endif // C_CUPNPSSDPSERVER_H
// End of File
