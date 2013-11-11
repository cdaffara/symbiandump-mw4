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
* Description: CUpnpTerminalModeIcon INL file
*
*/

#ifndef __UPNTERMINALMODEICON_INL__
#define __UPNTERMINALMODEICON_INL__
   
// ---------------------------------------------------------------------------------
// CUpnpTerminalModeIcon::IconFilename
// Returns the icon filename along with the complete filepath.
// ---------------------------------------------------------------------------------
//
inline const TDesC& CUpnpTerminalModeIcon::IconFilename()const
   {
   if ( iFilename )
       {
       return *iFilename;
       }
   return KNullDesC();
   }

// ---------------------------------------------------------------------------------
// CUpnpTerminalModeIcon::Width
// Returns the width of the icon.
// ---------------------------------------------------------------------------------
//
inline TInt CUpnpTerminalModeIcon::Width()const
   {
   return iIconWidth;
   }

// ---------------------------------------------------------------------------------
// CUpnpTerminalModeIcon::Height
// Returns the height of the icon.
// ---------------------------------------------------------------------------------
//
inline TInt CUpnpTerminalModeIcon::Height()const
    {
    return iIconHeight;
    }

// ---------------------------------------------------------------------------------
// CUpnpTerminalModeIcon::Depth
// Returns the depth of the icon.
// -------------------------------------------------------------------------------------
//
inline TInt CUpnpTerminalModeIcon::Depth()const
    {
    return iIconDepth;
    }

// ---------------------------------------------------------------------------------
// CUpnpTerminalModeIcon::MimeType
// Returns the mime type of the icon.
// ---------------------------------------------------------------------------------
//
inline const TDesC8& CUpnpTerminalModeIcon::MimeType()const
    {
    if ( iIconMimeType )
       {
       return *iIconMimeType;
       }
    return KNullDesC8();
    }


#endif  //__UPNTERMINALMODEICON_INL__
