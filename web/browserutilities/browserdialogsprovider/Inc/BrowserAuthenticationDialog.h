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
* Description: 
*      Password dialog
*
*
*/


#ifndef BROWSERAUTHENTICATIONDIALOG_H
#define BROWSERAUTHENTICATIONDIALOG_H


// INCLUDE FILES
#include <AknQueryDialog.h>

// CLASS DECLARATION

/**
*  Undocumented dialog class from Nokia.
*  @lib Browser.app
*  @since Series 60 1.2
*/
class CBrowserAuthenticationDialog : public CAknMultiLineDataQueryDialog
    {
    public:  // Constructors and destructor

        /**
        * C++ default constructor.
        * @since Series 60 1.2
        * @param aMsg User name (it may contains default value)
        * @param aPassword Password
        */
        static CBrowserAuthenticationDialog* NewL( TDes& aUsername,
                                                            TDes& aPassword );  

    protected:

        /**
        * CBrowserAuthenticationDialog default constructor.
        */
        CBrowserAuthenticationDialog();

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL( TDes* aUsername, 
                         TDes* aPassword );

    private:    // from CEikDialog

        /**
        * Fills the edwins with default values.
        * @since Series 60 1.2
        */
        void  PreLayoutDynInitL();

        /**
        * Set the password if the user pressed Ok.
        * @since Series 60 1.2
        * @param aKeyCode The ID of the pushed button
        * @return May the dialog exit?
        */
        TBool OkToExitL( TInt aKeycode );

        /**
        * Setting focus to second line + see CEikDialog
        * @since Series 60 1.2
        */
        virtual void SetInitialCurrentLineL();

        /**
        * Enables/disables left softkey
        * @since Series 60 1.2
        * @param aQueryControl  Pointer to query control
        *                           which sent the event
        * @param aEventType     Type of the event
        * @param aStatus        Editor validation status
        */
        TBool HandleQueryEditorStateEventL(CAknQueryControl* aQueryControl, 
                                           TQueryControlEvent aEventType, 
                                           TQueryValidationStatus aStatus);

    private:    // Data

        TBool               iEmptyName;
        TBool               iEmptyPassword;
        TDes*               iUsername;
        TDes*               iPassword;
    };

#endif

// End of File
