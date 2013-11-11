/** @file
* Copyright (c) 2005-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  Declares common structs (classes) for UPnP stack:
*                TUpnpAddLocalDevice, TUpnpRemoveLocalDevice, TUpnpGetHTTPMessage
*                TUpnpHTTPMessageSize, TUpnpHTTPMessage, TUpnpGetDeviceListSize
*                TUpnpGetDeviceList, TUpnpCancelRequest, TUpnpDevice, TUpnpService
*                TUpnpSsdpSearch
*
*/


#ifndef C_UPNPCOMMONSTRUCTS_H
#define C_UPNPCOMMONSTRUCTS_H

// INCLUDES
#include <in_sock.h>

// FORWARD DECLARATIONS
class CUpnpDeviceLibraryElement;

// CONSTANTS
static const TInt KUrlMaxLength = 256;//DLNA : 7.2.24.4 Max URL sholdn't exided 256(1024) characters
static const TInt KUdnMaxLength = 68; //DLNA 7.2.19.1  The maximum length is 68 bytes, including the "uuid:" portion
static const TInt KServiceTypeBufferLength = 4 + 255 + 9 + 64 + 3;//urn:domain-name:service:serviceType:v  SEE: UPNP-DeviceArchitecture v1.0
                                                              //4( urn: ) 
                                                              //255 max domain name SEE: http://www.ops.ietf.org/lists/namedroppers/namedroppers.2003/msg00964.html
                                                              //9 ( :service: )
                                                              //64 max service name 
                                                              //3 - ( :xx )
                                                              //sum: 335
static const TInt KDeviceTypeBufferLength = 4 + 255 + 8 + 64 + 3;//urn:domain-name:device:deviceType:v  SEE: UPNP-DeviceArchitecture v1.0
                                                             // 4( urn: ) 
                                                             //255 max domain name SEE: http://www.ops.ietf.org/lists/namedroppers/namedroppers.2003/msg00964.html
                                                             //8 ( :device: )
                                                             //64 max service name 
                                                             //3 - ( :xx )
                                                             //sum: 334
static const TInt KMaxServiceCount = 10;


// CLASS DECLARATIONS

/**
*  TUpnpAddLocalDevice
*
*  @since Series60 2.0
*/
class TUpnpAddLocalDevice
    {
public:
    // Data in buffer

    TInt iBufferLength;

    TInt iUuidLength;
    TInt iDeviceTypeLength;
    TInt iDescriptionUrlLength;
    TInt iDomainLength;
    TInt iPortNumberLength;
    TInt iLocalLength;
    TInt iRootDeviceLength;

    TInt iServiceLength[KMaxServiceCount];

    // External data

    TInt iTimeout;
    };

/**
*  TUpnpRemoveLocalDevice
*
*  @since Series60 2.0
*/
class TUpnpRemoveLocalDevice
    {
public:
    // UID buffer string
    TBuf8<50> iUUID;
    };

/**
*  TUpnpGetHTTPMessage
*
*  @since Series60 2.0
*/
class TUpnpGetHTTPMessage
    {
public:
    // Device path string
    TBuf8<50> iDevicePath;
    };

/**
*  TUpnpHTTPMessageSize
*
*  @since Series60 2.0
*/
class TUpnpHTTPMessageSize
    {
public:
    // Total length
    TInt iTotalLength;
    
    // Buffer length
    TInt iBufferLength;
    
    // Header length
    TInt iHeaderLength;
    // Body length
    TInt iBodyLength;

    // Destintion path length
    TInt iDestinationPathLength;
    // Sender path length
    TInt iSenderPathLength;

    // The incoming file lenght
    TInt iInFileLength;
    // The outgoing file length
    TInt iOutFileLength;
    };

/**
*  TUpnpHTTPMessage
*
*  @since Series60 2.0
*/
class TUpnpHTTPMessage
    {
public:
    // Session ID
    TInt iSessionId;
    // Remote host IP address
    TInetAddr iRemoteHost;

    // The type variable
    TInt iType;
    // Pending request state variable
    void* iPendingRequest;
    };

/**
*  TUpnpGetDeviceListSize
*
*  @since Series60 2.0
*/
class TUpnpGetDeviceListSize
    {
public:
    // The update ID variable
    TInt iUpdateId;
    // The device count variable
    TInt iDeviceCount;
    // The service count variable
    TInt iServiceCount;
    };

/**
*  TUpnpGetDeviceList
*
*  @since Series60 2.0
*/
class TUpnpGetDeviceList
    {
    public:
    
    };

/**
*  TUpnpCancelRequest
*
*  @since Series60 2.0
*/
class TUpnpCancelRequest
    {
    public:

    };

/**
*  TUpnpDevice
*
*  @since Series60 2.0
*/
class TUpnpDevice
    {
    public:
    /**
    * Set device
    * Defined in SSDPServer upnpdevicelibrary.cpp
    * @param elem the device library element to be set
    * @since Series60 2.0
    */
    IMPORT_C void Set( CUpnpDeviceLibraryElement* elem );

    // Description URL string
    TBuf8<KUrlMaxLength> iDescriptionURL;
    // Device type string
    TBuf8<KDeviceTypeBufferLength> iDeviceType;
    // Domain string
    TBuf8<KDeviceTypeBufferLength> iDomain;
    // UID string
    TBuf8<KUdnMaxLength> iUUID;
    // Service count string
    TInt iServiceCount;
    // Is alive variable
    TBool iAlive;
    // Cache-control timeout indicator
    TBool iExpired;
    // Local variable
    TBool iLocal;
    // Remote variable
    TBool iRemote;
    };

/**
*  TUpnpService
*
*  @since Series60 2.0
*/
class TUpnpService
    {
    public:
    // Service type string
    TBuf8<KServiceTypeBufferLength> iServiceType;

    };


/**
*  TUpnpSsdpSearch
*
*  @since Series60 2.0
*/
class TUpnpSsdpSearch
    {
public:
    // The search string
    TBuf8<100> iSearchString;
    };


#endif // C_UPNPCOMMONSTRUCTS_H

// End Of File