/**
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: CUpnpTmServerDevice class declaration
*
*/

#ifndef __UPNPTMSERVERDEVICE_H__
#define __UPNPTMSERVERDEVICE_H__

// System Includes
#include <in_sock.h>
#include <upnpdeviceimplementation.h>
#include <upnpdevicedescriptionstore.h>

// User Includes
#include "upnptmappserverservice.h"
#include "upnptmclientprofileservice.h"
#include "upnptmserverdevicexmlparser.h"
#include "upnptmserverdescriptionprovider.h"


// CLASS DECLARATION

// UPnP Terminal Mode Server Device container.

class CUpnpTmServerDevice : public CBase
    {
public:
     // Two-phased constructor.
    static CUpnpTmServerDevice* NewL( CUpnpTmServerDeviceInfo& aDeviceInfo, 
                                            CUpnpTmServerImpl& aTmServerImpl ); 
     // Destructor.     
    ~CUpnpTmServerDevice();

public:
    // Starts TM Server - Device, Service
    void StartTmServerDeviceL();
    // Stops TM Server - Device, Service
    void StopTmServerDeviceL();
    //Event handling methods invoked by CUpnpTmServerImpl object
    void AppStatusUpdateL( const TDesC8& aStatusEventBuffer );
    void AppListUpdateL( const TDesC8& aListEventBuffer );
    void UnUsedProfileIdUpdateL( const TDesC8& aUnusedProfileIdBuffer );

private:
    // Constructor
    CUpnpTmServerDevice();
    // Perform the second phase construction of a CUpnpTmServerDevice object
    void ConstructL( CUpnpTmServerDeviceInfo& aDeviceInfo,CUpnpTmServerImpl& aTmServerImpl );

private:
    // iDevice TerminalMode Server Device 
    CUpnpDeviceImplementation*              iDevice;
    CUpnpTmAppServerService*                iTmAppServerService;
    CUpnpTmClientProfileService*            iTmClientProfileService;
    // Provides files for description part
    CUpnpTmServerDescriptionProvider*       iDescriptionProvider; 
    // Services device description
    CUpnpDeviceDescriptionStore*            iDescriptionStore;
    CUpnpTmServerDeviceXmlParser*           iXmlParser;
    TBool                                   iIsStarted;
    };

#endif //__UPNPTMSERVERDEVICE_H__
