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
* Description: 
*
*/

#ifndef __UPNPAPPFILTERINFO_INL__
#define __UPNPAPPFILTERINFO_INL__


// -----------------------------------------------------------------------------
// CAppFilterInfo::AppName
// @return Returns the application name 
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::AppName()const
    {
    if ( iAppName )
        {
        return *iAppName;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::AppDescription
// @return Returns the app description
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::AppDescription()const
    {
    if ( iAppDesc )
        {
        return *iAppDesc;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::MimeType
// @return Returns the mimetype of the icon
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::MimeType()const
    {
    if ( iMimeType )
        {
        return *iMimeType;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::Width
// @param[out] aFlag True or false
// @return Returns the width value of the icon
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::Width()const
    {
    if ( iWidth )
        {
        return *iWidth;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::Height
// @param[out] aFlag True or false
// @return Returns the height value of the icon
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::Height()const
    {
    if ( iHeight )
        {
        return *iHeight;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::Depth
// @return Returns the depth value of the icon
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::Depth()const
    {
    if ( iDepth )
        {
        return *iDepth;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::AllowedProfileId
// @return Returns the depth value of the icon
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::AllowedProfileIdList()const
    {
    if ( iAllowedIdList )
        {
        return *iAllowedIdList;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::ProtocolId
// @return Returns the protocol name 
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::ProtocolId()const
    {
    if ( iProtocolId )
        {
        return *iProtocolId;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::Format
// @return Returns the format of remoting info element
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::Format()const
    {
    if ( iFormat )
        {
        return *iFormat;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::Direction
// @return Returns the direction of remoting info 
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::Direction()const
    {
    if ( iDirection)
        {
        return *iDirection;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::AppCategory
// @return Returns the category of application for appInfo element
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::AppCategory()const
    {
    if ( iAppCategory )
        {
        return *iAppCategory;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::AppTrustLevel
// @return Returns the trust level of application for appInfo element
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::AppTrustLevel()const
    {
    if ( iAppTrustLevel )
        {
        return *iAppTrustLevel;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::ContentCategory
// @return Returns the category of the content for display info element
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::ContentCategory()const
    {
    if ( iContCategory )
        {
        return *iContCategory;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::ContentCategory
// @return Returns the content rules for display info element
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::ContentRules()const
    {
    if ( iContentRules )
        {
        return *iContentRules;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::ContentTrustLevel
// @return Returns the trust level of the for display info element
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::ContentTrustLevel()const
    {
    if ( iContTrustLevel )
        {
        return *iContTrustLevel;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::AudioType
// @return Returns the type of audio for audio info element
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::AudioType()const
    {
    if ( iAudioType )
        {
        return *iAudioType;
        }
    return KNullDesC8();
    }
// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::ResourceStatus
// @return Returns the protocol name 
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::ResourceStatus()const
    {
    if ( iRscStatus )
        {
        return *iRscStatus;
        }
    return KNullDesC8();
    }

// -----------------------------------------------------------------------------
// CUpnpAppFilterInfo::Signature
// @return Returns the protocol name 
// -----------------------------------------------------------------------------
//
inline const TDesC8& CUpnpAppFilterInfo::Signature()const
    {
    if ( iSignature )
        {
        return *iSignature;
        }
    return KNullDesC8();
    }

#endif   //__UPNPAPPFILTERINFO_INL__
