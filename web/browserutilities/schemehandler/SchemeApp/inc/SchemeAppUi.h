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
* Description:  Declaration of class CSchemeAppUi
*
*/


#ifndef SCHEME_APP_UI_H
#define SCHEME_APP_UI_H

// INCLUDES

#include <e32base.h>
#include <aknappui.h>

// FORWARD DECLARATIONS

class CSchemeDocument;
class CSchemeDialog;
class CSchemeHandler;

// CLASS DECLARATION

/**
* Application UI for the Scheme App.
*/
class CSchemeAppUi: public CAknAppUi
    {

    public:     // Constructors and destructor

        /**
        * Constructor.
        */      
        CSchemeAppUi( CSchemeDocument* aSchemeDocument );
        
        /**
        * Second phase constructor. Leaves on failure.
        */      
        void ConstructL();

        /**
        * Destructor.
        */      
        virtual ~CSchemeAppUi();

    private:    // Functions from base classes

        /**
        * from EikAppUi.
        */
		TBool ProcessCommandParametersL( TApaCommand aCommand, 
                                         TFileName& aDocumentName, 
                                         const TDesC8& aTail );

    public:    // from CAknAppUi (CEikAppUi)

        /**
        * Handle command.
        * @param aCommand Command id.
        */
        void HandleCommandL( TInt aCommand );
    protected:

       /**
        * Handles screen resolution changes
        */
        void HandleScreenDeviceChangedL();
    
    private:    // data

        CSchemeDialog*   iAppDialog;    ///< App's main dialog.

		CSchemeHandler*  iSchemeHandler; ///< Owned.
        CSchemeDocument* iSchemeDocument;

    };

#endif // SCHEME_APP_UI_H

// End of File
