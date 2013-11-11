/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  enables other applications to launch browser in embedded mode
*
*/


#ifndef BROWSER_OVERRIDDEN_SETTINGS_H
#define BROWSER_OVERRIDDEN_SETTINGS_H

#warning The Browser Launcher API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

//  INCLUDES
#include <e32std.h>
#include <e32def.h>

// DATA TYPES
typedef enum
{
    EBrowserOverSettingsSmallScreen = 0,
    EBrowserOverSettingsAutoLoadImages,
    EBrowserOverSettingsFontSize,
    EBrowserOverSettingsFullScreen,
    EBrowserOverSettingsCustomAp,
    EBrowserOverSettingsContextId,
    EBrowserOverSettingsUndefinied = KMaxTUint16
} TBrowserOverSettings;

typedef enum 
{
  EBrowserOverFontSizeLevelAllSmall = 0,
  EBrowserOverFontSizeLevelSmaller,
  EBrowserOverFontSizeLevelNormal,
  EBrowserOverFontSizeLevelLarger,    
  EBrowserOverFontSizeLevelAllLarge,
  EBrowserOverFontSizeLevelUndefined = KMaxTUint16
} TBrowserOverFontSizeLevel;

typedef enum 
{
  EBrowserOverFullScreenValueNormal = 0,
  EBrowserOverFullScreenValueSoftKeysOnly,
  EBrowserOverFullScreenValueFullScreen,
  EBrowserOverFullScreenValueUndefined = KMaxTUint16
} TBrowserOverFullScreenValue;

typedef enum
{
  EBrowserContextIdNormal = 0,
  EBrowserContextIdFeeds,
  EBrowserContextIdUndefined = KMaxTUint16
} TBrowserOverContextId;

// CLASS DECLARATION

/**
*  This class stores values to pass to the browser in embedded mode
*
*/
class TBrowserOverriddenSettings
{
public :// Constructors and destructor 
   inline TBrowserOverriddenSettings();

public: // New functions
   /**
   * SetBrowserSetting sets the the settings which will be overriden
   * @param aBrowserSetting describes which type of setting to set
   * @param aValue the value to set
   * @return void
   */
   inline void SetBrowserSetting(TBrowserOverSettings aBrowserSetting, TUint aValue);
   
   /**
   * GetBrowserSetting gets the value of the setting
   * @param aBrowserSetting describes which type of setting to get
   * @return the value of the setting
   */
   inline TUint GetBrowserSetting(TBrowserOverSettings aBrowserSetting);
private:// Data
   //stores the value for the small screen setting can be 0 or 1
   //if not set then KMaxUint
   TUint iSmallScreen;
   //stores the value for the auto load image setting can be 0 or 1
   //if not set then KMaxUint
   TUint iAutoLoadImage;
   //stores the value for the font size setting can be 0,1,2,3 or 4
   TUint iFontSize;
   //stores the value for the small screen setting can be 0 or 1
   TUint iFullScreen;
   // Custom access point provided by the operator. 
   // If specified, contains a WAP id of a read only access point,  
   // which is then set as the default and it cannot be changed manually by the user. 
   // If not defined, operator menu works as before.
   // From the operator menu( 0 = not specified, otherwise contains the WAP id of the access point.)
   TUint iCustomAp;
   // Allow the contextId of the call to be passed as a parameter
   TUint iContextId;   
};

#include <BrowserOverriddenSettings.inl>

#endif  //BROWSER_OVERRIDDEN_SETTINGS_H
