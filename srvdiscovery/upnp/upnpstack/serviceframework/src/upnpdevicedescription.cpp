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
* Description:  Defines the CUpnpDeviceDescription class
*
*/


#include "upnpdevicedescription.h"


// -----------------------------------------------------------------------------
// CUpnpDeviceDescription::~CUpnpDeviceDescription
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceDescription::~CUpnpDeviceDescription()
{
	
}

// -----------------------------------------------------------------------------
// CUpnpDeviceDescription::CUpnpDeviceDescription
// Destructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CUpnpDeviceDescription::CUpnpDeviceDescription()
{
	
}

EXPORT_C void CUpnpDeviceDescription::SetDescriptionUrlL( const TDesC8& aDescriptionUrl)
{
    HBufC8* tmp = aDescriptionUrl.AllocL();
    delete iDescriptionURL;
    iDescriptionURL = tmp;

}
