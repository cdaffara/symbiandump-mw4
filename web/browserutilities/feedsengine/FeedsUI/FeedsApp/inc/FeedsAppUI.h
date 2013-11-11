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
* Description:  Feeds application UI instance.
*
*/

#ifndef FEEDS_APPUI_H
#define FEEDS_APPUI_H


// INCLUDES
#include <aknViewAppUi.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CEikMenuPane;

// CLASS DECLARATION


/**
*  Feeds application UI instance.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CFeedsAppUi : public CAknViewAppUi
    {
    public: // Constructors and destructor
        /**
        * ConstructL.
        * 2nd phase constructor.
        */
        void ConstructL();

        /**
        * Destructor.
        */        
        virtual ~CFeedsAppUi();


    public: // From CAknAppUi
        /**
        * Takes care of command handling.
        * 
        * @since ??
        * @param aCommand Command to be handled.
        * @return The value.
        */
        void HandleCommandL(TInt aCommand);


    private:
        /**
        * Inits the Engine.
        *
        * @since 3.0
        * @return void
        */
        void InitEngineL();

        /**
        * Inits the UI.
        *
        * @since 3.0
        * @return void
        */
        void InitUIL();


    private:  // Data
        //CTestHarness*  iTestHarness;
    };

#endif      // FEEDS_APPUI_H
            
// End of File
