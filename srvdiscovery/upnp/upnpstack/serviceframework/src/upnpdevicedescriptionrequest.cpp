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
* Description:  Declaration of MUpnpSubscriberLibraryObserver
*
*/

#include "upnpdevicedescriptionrequest.h"

CUpnpDeviceDescriptionRequest::CUpnpDeviceDescriptionRequest( const TInetAddr& aInetAddress ):
    iAddress( aInetAddress )
    {    
    }

CUpnpDeviceDescriptionRequest* CUpnpDeviceDescriptionRequest::NewL( const TDesC8& aUri,
                                                                   const TInetAddr& aInetAddress )
    {
    CUpnpDeviceDescriptionRequest* self = CUpnpDeviceDescriptionRequest::NewLC( aUri,
                                                                                aInetAddress );
    CleanupStack::Pop(); // self;
    return self;
    }

CUpnpDeviceDescriptionRequest* CUpnpDeviceDescriptionRequest::NewLC( const TDesC8& aUri,
                                                                  const TInetAddr& aInetAddress )
    {
    CUpnpDeviceDescriptionRequest* self = new (ELeave) CUpnpDeviceDescriptionRequest( 
                                                                        aInetAddress );
    CleanupStack::PushL(self);
    self->ConstructL( aUri );
    return self;
    }

void CUpnpDeviceDescriptionRequest::ConstructL( const TDesC8& aUri )
    {
    iUri.CreateL( aUri );
    }


CUpnpDeviceDescriptionRequest::~CUpnpDeviceDescriptionRequest()
    {
    iUri.Close();
    }


EXPORT_C void CUpnpDeviceDescriptionRequest::InetAddress( TInetAddr& aInetAddress )
    {
    aInetAddress = iAddress;
    }

EXPORT_C TPtrC8 CUpnpDeviceDescriptionRequest::Uri()
    {
    return iUri;
    }
   

//End of File
