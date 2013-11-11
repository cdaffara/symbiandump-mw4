/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Handles Dialogs Provider events
*
*/

#ifndef BROWSERDIALOGSPROVIDEROBSERVER_H
#define BROWSERDIALOGSPROVIDEROBSERVER_H

#warning The Browser Dialogs Provider API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// INCLUDES
#include <e32def.h>

// DATA TYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*  Provide dialogs needed for browser operation
*
*  @lib BrowserDialogsProvider.lib
*  @since 2.8
*/
class MBrowserDialogsProviderObserver
    {
    public:
        // enums of all types of dialog found in CBrowserDialogsProvider
        enum TDialogType
            {
            ENotifyError,
            ENotifyHttpError,
            EFileSelect,
            ESelectOption,
            EUserAuthentication,
            ENote,
            EAlert,
            EConfirm,
            EPrompt,
            EDownloadObject,
            EDisplayPageImages,
            ECancelAll,          // deprecated, do not use
            EUploadProgress,
            EMimeFileSelect
            };

    public: // New functions

        // Receives events from applications which want to report that a 
        // dialog event has taken place.
        virtual void ReportDialogEventL( TInt aType, TInt aFlags ) = 0 ;
    };


#endif      // BROWSERDIALOGSPROVIDEROBSERVER_H   
            
// End of File
