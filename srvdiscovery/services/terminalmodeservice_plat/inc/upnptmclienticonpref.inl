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
* Description: CUpnpTmClientIconPref INL file
*
*/

#ifndef __UPNPTMCLIENTICONPREF_INL__
#define __UPNPTMCLIENTICONPREF_INL__
   

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::ClientProfileId
// Returns the profileID of the client profile.
// ---------------------------------------------------------------------------------
//
inline TUint CUpnpTmClientIconPref::ClientProfileId()const
    {
    return iClientProfileId;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::MimeType
// Returns the mime type of the icon.
// ---------------------------------------------------------------------------------
//
inline const TDesC8& CUpnpTmClientIconPref::MimeType()const
    {
    return *iIconMimeType;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::Width
// Returns the width of the icon.
// ---------------------------------------------------------------------------------
//
inline TInt CUpnpTmClientIconPref::Width()const
   {
   return iIconWidth;
   }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::Height
// Returns the height of the icon.
// ---------------------------------------------------------------------------------
//
inline TInt CUpnpTmClientIconPref::Height()const
    {
    return iIconHeight;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::Depth
// Returns the depth of the icon.
// -------------------------------------------------------------------------------------
//
inline TInt CUpnpTmClientIconPref::Depth()const
    {
    return iIconDepth;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::SetWidthL
// Method is used to set the width of the icon which TM client supports
// @param aWidth   Width of the icon
// ---------------------------------------------------------------------------------
//
inline void CUpnpTmClientIconPref::SetWidth( TInt aWidth ) 
    {
    iIconWidth = aWidth;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::SetHeightL
// Method is used to set the height of the icon which TM client supports
// @param aHeight   Height of the icon
// ---------------------------------------------------------------------------------
//
inline void CUpnpTmClientIconPref::SetHeight( TInt aHeight )
    {
    iIconHeight = aHeight;
    }

// ---------------------------------------------------------------------------------
// CUpnpTmClientIconPref::SetDepthL
// Method is used to set the color depth of the icon which TM client supports
// @param aDepth   Color depth of the icon
// ---------------------------------------------------------------------------------
//
inline void CUpnpTmClientIconPref::SetDepth( TInt aDepth ) 
    {
    iIconDepth = aDepth;
    }

#endif  //__UPNPTMCLIENTICONPREF_INL__
