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


// ============================= LOCAL FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// TBrowserOverriddenSettings::TBrowserOverriddenSettings
// constructor sets the values to undefinied
// -----------------------------------------------------------------------------
inline TBrowserOverriddenSettings::TBrowserOverriddenSettings()
    {
         iSmallScreen = KMaxTUint;
         iAutoLoadImage = KMaxTUint;
         iFontSize = EBrowserOverFontSizeLevelUndefined;
         iFullScreen = EBrowserOverFullScreenValueUndefined;
         iCustomAp = 0;
         iContextId = EBrowserContextIdUndefined;
    };

// -----------------------------------------------------------------------------
// TBrowserOverriddenSettings::SetBrowserSetting
// SetBrowserSetting sets the the settings which will be overriden
// Returns: void
// -----------------------------------------------------------------------------
inline void TBrowserOverriddenSettings::SetBrowserSetting(TBrowserOverSettings aBrowserSetting, TUint aValue)
   {
        switch( aBrowserSetting )
            {
            case EBrowserOverSettingsSmallScreen :
                {
                iSmallScreen = aValue;
                break;
                }
            case EBrowserOverSettingsAutoLoadImages :
                {
                iAutoLoadImage = aValue;
                break;
                }
            case EBrowserOverSettingsFontSize :
                {
                iFontSize = aValue;
                break;
                }
            case EBrowserOverSettingsFullScreen :
                {
                iFullScreen = aValue;
                break;
                }
            case EBrowserOverSettingsCustomAp :
                {
                iCustomAp = aValue;
                break;
                }
            case EBrowserOverSettingsContextId :
                {
                iContextId = aValue;
                break;
                }
            	
            default :
                {
                return;
                }
            }
   };

// -----------------------------------------------------------------------------
// TBrowserOverriddenSettings::GetBrowserSetting
// Gets the values of a specified type of setting
// Returns: the value of the specified setting
// -----------------------------------------------------------------------------
inline TUint TBrowserOverriddenSettings::GetBrowserSetting(TBrowserOverSettings aBrowserSetting)
   {
        switch( aBrowserSetting )
            {
            case EBrowserOverSettingsSmallScreen :
                {
                return iSmallScreen;
                break;
                }
            case EBrowserOverSettingsAutoLoadImages :
                {
                return iAutoLoadImage;
                break;
                }
            case EBrowserOverSettingsFontSize :
                {
                return iFontSize;
                break;
                }
            case EBrowserOverSettingsFullScreen :
                {
                return iFullScreen;
                break;
                }
            case EBrowserOverSettingsCustomAp :
                {
                return iCustomAp;
                break;
                }
            case EBrowserOverSettingsContextId :
                {
                return iContextId;
                break;
                }                
            default :
                {
	            return KMaxTUint16;
                }
        }
   } 	
