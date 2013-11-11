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
* Description: CUpnpAppFilterInfo class declaration
*
*/

#ifndef __UPNPAPPFILTERINFO_H__
#define __UPNPAPPFILTERINFO_H__

// Include Files
#include <e32base.h>
#include "terminalmodeconsts.h"

// Literals
_LIT8(KProtocolId,          "protocolID");
_LIT8(KFormat,              "format");
_LIT8(KRemotingDirection,   "direction");
_LIT8(KAppCategory,         "appCategory");
_LIT8(KTrustLevel,          "trustLevel");
_LIT8(KContCategory,        "contentCategory");
_LIT8(KContentRules,        "contentRules");
_LIT8(KAudioType,           "audioType");

/**
 *  This class stores the parsed information provided in the AppListingFilter
 *  input argument of the GetApplicationList action.
 *  Provides internal setter and getter methods.
 *  Maintains a structure which holds the information such as which all optional 
 *  elements are needed in the response and which all applications should go
 *  in the response.
 */
class CUpnpAppFilterInfo : public CBase
      {
public: 
      static  CUpnpAppFilterInfo* NewL();
      ~CUpnpAppFilterInfo();
      
      /*
       * Method is used to set the value for each key provided in the filter string as an input
       * Returns error if the there is a mismatch of the key or key is not as per the GetApplicationList schema
       * The default parameter( aDisplayInfoFlag ) is provided only to distinguish the child element "trustLevel" ( child element)
       * of displayInfo element from "trustLevel" of appInfo element.
       */
      void SetFilterInfoL( const TDesC8& aKey, const TDesC8& aValue,
                                  TTerminalModeErrorCode& aErr, TBool aDisplayInfoFlag = EFalse );
      // inlined Getter methods
      inline const TDesC8& AppName()const;
      inline const TDesC8& AppDescription()const;
      inline const TDesC8& MimeType()const;
      inline const TDesC8& Width()const;
      inline const TDesC8& Height()const;
      inline const TDesC8& Depth()const;
      inline const TDesC8& AllowedProfileIdList()const;
      inline const TDesC8& ProtocolId()const;
      inline const TDesC8& Format()const;
      inline const TDesC8& Direction()const;
      inline const TDesC8& AppCategory()const;
      inline const TDesC8& AppTrustLevel()const;
      inline const TDesC8& ContentCategory()const;
      inline const TDesC8& ContentRules()const;
      inline const TDesC8& ContentTrustLevel()const;
      inline const TDesC8& AudioType()const;
      inline const TDesC8& ResourceStatus()const;
      inline const TDesC8& Signature()const;
      
private:
      CUpnpAppFilterInfo( );
      
private:
      HBufC8*                     iAppName;
      HBufC8*                     iAppDesc;
      HBufC8*                     iMimeType;
      HBufC8*                     iWidth;
      HBufC8*                     iHeight;
      HBufC8*                     iDepth;
      HBufC8*                     iAllowedIdList;
      HBufC8*                     iProtocolId;
      HBufC8*                     iFormat;
      HBufC8*                     iDirection;
      HBufC8*                     iAppCategory;
      HBufC8*                     iAppTrustLevel;
      HBufC8*                     iContCategory;
      HBufC8*                     iContentRules;
      HBufC8*                     iContTrustLevel;
      HBufC8*                     iAudioType;
      HBufC8*                     iRscStatus;
      HBufC8*                     iSignature;
      };

#include "upnpappfilterinfo.inl"

#endif  //__UPNPAPPFILTERINFO_H__
