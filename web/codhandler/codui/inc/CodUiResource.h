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
* Description:  Declaration of class CCodUiResource.
*
*/


#ifndef COD_UI_RESOURCE_H
#define COD_UI_RESOURCE_H

// INCLUDES

#include <e32base.h>

// FORWARD DECLARATIONS

class CEikonEnv;

// CLASS DECLARATION

/**
* Instantiate this class to have CodUi resources loaded.
*/
NONSHARABLE_CLASS( CCodUiResource ): public CBase
    {

    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aEikEnv Eikon environment.
        * @return The constructed object.
        */
        static CCodUiResource* NewL( CEikonEnv& aEikEnv );

        /**
        * Destructor.
        */
        virtual ~CCodUiResource();

    protected:  // Constructors

        /**
        * Constructor.
        * @param aEikEnv Eikon environment.
        */
        CCodUiResource( CEikonEnv& aEikEnv );

        /**
        * Second-phase constructor.
        */
        void ConstructL();

    private:    // Data 

        CEikonEnv* iEikEnv;     ///< Eikon environment. Not owned.
        TInt iResourceOffset;   ///< Resource file offset.

    };

#endif 
