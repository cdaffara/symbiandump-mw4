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
* Description: CUpnpTmServerDeviceXmlParser INL file.
*
*/

#ifndef __UPNPTMSERVERDEVICEXMLPARSER_INL__
#define __UPNPTMSERVERDEVICEXMLPARSER_INL__


// -----------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::OnError 
// -----------------------------------------------------------------------------
//
inline void CUpnpTmServerDeviceXmlParser::OnError(TInt /*aErrorCode*/ )
    {
    }

// -----------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::GetExtendedInterface 
// -----------------------------------------------------------------------------
//
inline TAny* CUpnpTmServerDeviceXmlParser::GetExtendedInterface(const TInt32 /*aUid*/)
    {
    return NULL;    
    }

// -----------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::DevicePath 
// @return Returns the device description filepath
// -----------------------------------------------------------------------------
//
inline const TDesC& CUpnpTmServerDeviceXmlParser::DevicePath()
    {
    return iDescFilePath;   
    }

// -----------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::AppServerSrvPath 
// @return Returns TmAppServerService filepath
// -----------------------------------------------------------------------------
//
inline const TDesC& CUpnpTmServerDeviceXmlParser::AppServerSrvPath()
    {
    return iAppServerSrvFilePath;
    }

// -----------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::ClientProfileSrvPath
// @return Returns TmClientProfileService filepath
// -----------------------------------------------------------------------------
//
inline const TDesC& CUpnpTmServerDeviceXmlParser::ClientProfileSrvPath()
    {
    return iClientProfSrvFilePath;
    }

// -----------------------------------------------------------------------------
// CUpnpTmServerDeviceXmlParser::DescriptionUri 
// @return Returns the device description URI
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpTmServerDeviceXmlParser::DescriptionUri()
    {
    return iDescriptionUri;
    }

#endif // __UPNPREMOTEUIDEVICEXMLPARSER_INL__
