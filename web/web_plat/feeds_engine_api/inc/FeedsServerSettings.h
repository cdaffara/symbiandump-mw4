/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Encapulate FeedsServer settings.
*
*/


#ifndef FEEDS_SERVER_SETTINGS_H
#define FEEDS_SERVER_SETTINGS_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  Encapsulates Feeds Server related settings.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class TFeedsServerSettings
    {
    public:
        /**
        * Constructor
        *
        * @since 3.1
        */
        inline TFeedsServerSettings() 
                {
                iAutoUpdate = EFalse;
                iAutoUpdateFreq = 0;
                iAutoUpdateAP = 0;
                iAutoUpdateWhileRoaming = EFalse;
                };

        /**
        * Copy constructor
        *
        * @since 3.1
        * @param aSettings The settings to copy.
        */
        inline TFeedsServerSettings(const TFeedsServerSettings& aSettings) 
                {
                iAutoUpdate = aSettings.iAutoUpdate;
                iAutoUpdateFreq = aSettings.iAutoUpdateFreq;
                iAutoUpdateAP = aSettings.iAutoUpdateAP;
                iAutoUpdateWhileRoaming = aSettings.iAutoUpdateWhileRoaming;
                };

        /**
        * Sets whether the Feeds Server should automatically update its feeds.
        *
        * @since 3.1
        * @param aAutoUpdate ETrue to turn auto update on.
        * @return void.
        */
        inline void SetAutoUpdate(TBool aAutoUpdate) {iAutoUpdate = aAutoUpdate;}

        /**
        * Sets whether the Feeds Server should automatically update its feeds.
        *
        * @since 3.1
        * @param aAutoUpdate ETrue to turn auto update on.
        * @return void.
        */
        inline void SetAutoUpdateWhileRoaming(TBool aAutoUpdateWhileRoaming) {iAutoUpdateWhileRoaming = aAutoUpdateWhileRoaming;}
        
        
        /**
        * Returns whether the Feeds Server should automatically update its feeds.
        *
        * @since 3.1
        * @return ETrue if auto update is on.
        */
        inline TBool AutoUpdate() const 
                { 
                return iAutoUpdate;
                }
        /**
        * Returns whether the Feeds Server should automatically update its feeds while roaming.
        *
        * @since 3.1
        * @return ETrue if auto update is on.
        */
        inline TBool AutoUpdateWhileRoaming() const 
                { 
                return iAutoUpdateWhileRoaming;
                }

        /**
        * Sets the number of minutes between updates if auto update is enabled.
        *
        * @since 3.1
        * @param aAutoUpdateFreq The number of hours between updates.
        * @return void.
        */
        inline void SetAutoUpdateFreq(TInt aAutoUpdateFreq) {iAutoUpdateFreq = aAutoUpdateFreq;}

        /**
        * Returns the number of hours between updates if auto update is enabled.
        *
        * @since 3.1
        * @return The number of minutes between updates.
        */
        inline TInt AutoUpdateFreq() const 
                { 
                return iAutoUpdateFreq;
                }

        /**
        * Sets the access point used by the Feeds Server to fetch feeds.
        *
        * @since 3.1
        * @param aAutoUpdateAP The access point used by the Feeds Server to automatic update feeds.
        * @return void.
        */
        inline void SetAutoUpdateAP( TUint aAutoUpdateAP ) 
                {
                iAutoUpdateAP = aAutoUpdateAP;
                }

        /**
        * Returns the access point used by the Feeds Server to fetch feeds.
        *
        * @since 3.1
        * @return Always ask for access point or not.
        */
        inline TUint AutoUpdateAP() const
                { 
                return iAutoUpdateAP;
                }


    private:
        TBool  iAutoUpdate;
        TInt   iAutoUpdateFreq;
        TUint  iAutoUpdateAP;
        TBool  iAutoUpdateWhileRoaming;
    };
    

#endif      // FEEDS_SERVER_SETTINGS_H
            
// End of File
