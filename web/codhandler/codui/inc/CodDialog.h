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
* Description:  Declaration of class CodDialog.   
*
*/


#ifndef COD_DIALOG_H
#define COD_DIALOG_H

// INCLUDES

#include <e32base.h>
#include <centralrepository.h>
#include <BrowserUiSDKCRKeys.h>


// FORWARD DECLARATION

class CCoeEnv;

// CLASS DECLARATION

/**
* Wrapper class for miscellaneous dialogs.
*/
NONSHARABLE_CLASS( CodDialog )
    {
    public:     // new methods

        /**
        * Get download root path.
        * Leave with KErrCancel if query is cancelled.
        * @param aRootPath Root path returned here.
        */
        static void GetRootPathL( TDes& aRootPath );

        /**
        * Get confirmation for creating network connection.
        * @return ETrue if connection can be created.
        */
        static TBool ConfirmConnectL();
        
        /**
        * Show an information note.
        * @param aPromptResourceId Resource id of prompt text (RESOURCE TBUF).
        * @param aCoeEnv Control environment.
        */
        static void InfoNoteL( TInt aPromptResourceId, CCoeEnv& aCoeEnv );

        /**
        * Show an information note.
        * @param aPromptPrompt text.
        */
        static void InfoNoteL( const TDesC& aPrompt );

        /**
        * Show an confirmation note.
        * @param aPromptResourceId Resource id of prompt text (RESOURCE TBUF).
        * @param aCoeEnv Control environment.
        */
        static void ConfNoteL( TInt aPromptResourceId, CCoeEnv& aCoeEnv );

        /**
        * Show an confirmation note.
        * @param aPrompt Prompt text.
        */
        static void ConfNoteL( const TDesC& aPrompt );

    };

#endif /* def COD_DIALOG_H */
