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
* Description:  Declares the CUpnpDeviceDescription class
*
*/


#ifndef C_CUPNPDEVICEDESCRIPTION_H
#define C_CUPNPDEVICEDESCRIPTION_H

// INCLUDES


#include "upnpdevice.h"

/**
 *  Device
 *
 *  Prior using the CUpnpDeviceDescription class, client application has to create the network
 *  connection. If connection is not up and running, the
 *  CUpnpDispatcherEngine class construction will fail.
 *  See UPnP Interface Design document for instuctions.
 *
 *  @since Series60 2.6
 */
class CUpnpDeviceDescription : public CUpnpDevice
    {

public:
    /**
     * Destructor.
     */
    IMPORT_C virtual ~CUpnpDeviceDescription();

public: // New functions

    IMPORT_C void SetDescriptionUrlL( const TDesC8& aDescriptionUrl);
    
    /**
     * C++ default constructor.
     */
    IMPORT_C CUpnpDeviceDescription();

    };

#endif  // C_CUPNPDEVICEDESCRIPTION_H

