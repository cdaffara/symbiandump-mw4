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
* Description:  MUpnpSsdpServerObserver
*
*/


#ifndef C_MUPNPSSDPSERVEROBSERVER_H
#define C_MUPNPSSDPSERVEROBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <badesca.h>
#include "upnpdevicelibrary.h"

// FORWARD DECLARATIONS
class CUpnpSsdpMessage;
class CUpnpDeviceLibraryElement;
class CTimeoutElementParent;

// CLASS DECLARATION

/**
*  SSDP server observer.
*
*  @since Series60 2.0
*/
class MUpnpSsdpServerObserver
    {
    public: // New functions

        /**
        * SSDP event.
        * @since Series60 2.0
        * @param aMessage SSDP message
        */
        virtual void SsdpEventL( CUpnpSsdpMessage* aMessage ) = 0;

        /**
        * DeviceList.
        * @since Series60 2.0
        * @return RPointerArray
        */
        virtual RPointerArray<CUpnpDeviceLibraryElement>& DeviceList() = 0;

    };

#endif  // SSDPSERVEROBSERVER_H

// End of File