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
* Description:  Declaration of class CSchemeDialog
*
*/


#ifndef SCHEME_DIALOG_H
#define SCHEME_DIALOG_H

// INCLUDES

#include <e32base.h>
#include <eikdialg.h>

// CLASS DECLARATION

/**
* Main dialog for Scheme App
*/
class CSchemeDialog: public CEikDialog
    {

    public: // Constructors and destructor

        /**
        * Destructor.
        */
        ~CSchemeDialog();

    public: // from CEikDialog

        /**
        * Pre-layout dynamic initialization.
        */
        void PreLayoutDynInitL();

        /**
        * Handle button press.
        * @param aButtonId Id of button pressed.
        * @return ETrue if the dialog may exit; EFalse otherwise.
        */
        TBool OkToExitL( TInt aButtonId );

    };

#endif // SCHEME_DIALOG_H

// End of File
