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
* Description: 
*
*
*/


#ifndef BROWSERSCRIPTPROMPTDIALOG_H
#define BROWSERSCRIPTPROMPTDIALOG_H

//  INCLUDES
#include <AknQueryDialog.h>

// CLASS DECLARATION

/**
*  Class definition
*/
class CBrowserScriptPromptDialog : public CAknTextQueryDialog
    {
    public:  // Constructors and destructor
    
        /**
        * C++ default constructor.
        * @param aMsg User name (it may contains default value).
        * @param aResp Password.
        */
        CBrowserScriptPromptDialog( TDes& aDefInput, HBufC*& aRetBuf );

        /**
        * Destructor.
        */
        virtual ~CBrowserScriptPromptDialog(){};

    private:    // from CEikDialog

        /**
        * Fills the edwins with default values.
        */
        void  PreLayoutDynInitL();

        /**
        * Set the password if the user pressed Ok.
        * @param aKeyCode The ID of the pushed button.
        * @return May the dialog exit?
        */
        TBool OkToExitL( TInt aKeycode );

        // MAknQueryControlObserver
        TBool HandleQueryEditorStateEventL( CAknQueryControl* aQueryControl,
                                            TQueryControlEvent aEventType,
                                            TQueryValidationStatus aStatus);

    private:    // Data

        HBufC*& iRetBuf;// not owned; left on the cleanup stack as a return type
    };

#endif

// End of File
