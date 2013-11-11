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
* Description:  Declaration of class CCodViewDialog.   
*
*/


#ifndef COD_VIEW_DIALOG_H
#define COD_VIEW_DIALOG_H

// INCLUDES

#include <AknDialog.h>

// FORWARD DECLARATIONS

class CCodData;

// CLASS DECLARATION

/**
* COD details view dialog.
* TODO implement it.
*/
NONSHARABLE_CLASS( CCodViewDialog ): public CAknDialog
    {

    public:     // Constructors

        /**
        * Constructor.
        * @param aCod COD flag (show Price).
        * @param aData COD data to display. Ownership not taken.
        */
        CCodViewDialog( const CCodData& aData, TBool aCod );

    public:     // New methods

        /**
        * Construct and execute the dialog.
        * @return Command that dismissed the dialog.
        */
        TInt ExecuteLD();

	private:    // From CAknDialog & its base classes

        /**
        * Create custom control.
        * @param aControlType Control type.
        * @return Control info.
        */
        SEikControlInfo CreateCustomControlL( TInt aControlType );

        /**
        * Process command.
        * @param aCommandId Command id.
        */
        void ProcessCommandL( TInt aCommandId );

        /**
        * Handle key event.
        * @param aKeyEvent The key event.
        * @param aType Key event type.
        * @return Response (was the key event consumed?).
        */
        TKeyResponse OfferKeyEventL
            ( const TKeyEvent& aKeyEvent, TEventCode aType );

        /**
        * Post-layout initialization.
        */
        void PostLayoutDynInitL();

        /**
        * Activate.
        */
        void ActivateL();
        
    private:    // Data

        const CCodData& iData;  ///< Data.
        TBool iCod;             ///< COD or DD?

        };

#endif /* def COD_VIEW_DIALOG_H */
