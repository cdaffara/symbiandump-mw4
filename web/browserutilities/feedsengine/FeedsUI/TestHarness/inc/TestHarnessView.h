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
* Description:  A view to set options.
*
*/


#ifndef SETTINGS_VIEW_H
#define SETTINGS_VIEW_H


// INCLUDES
#include <aknview.h>

#include "TestHarness.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CTestHarnessContainer;

// CLASS DECLARATION


/**
*  A view to set options.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CTestHarnessView: public CAknView, public MTestHarnessObserver
    {
    public:
        /**
        * Two-phased constructor.
        */
        static CTestHarnessView* NewL();

        /**
        * Destructor.
        */        
        ~CTestHarnessView();

        /**
        * Shows the settings view.
        *
        * @since 3.0
        * @return void.
        */
        static void ShowL(TUid aRestoreToId);


    public: // From CAknView
        /**
        * Returns the id of the view.
        *
        * @since ?
        * @return the id.
        */
        virtual TUid Id() const;

        /**
        * Processes commands.
        *
        * @since ?
        * @param aCommand the command to process.
        * @return void.
        */
        virtual void HandleCommandL(TInt aCommand);


    private: // From CAknView
        /**
        * Called when the view is activated.
        *
        * @since ?
        * @param aPrevViewId the id of the prev view.
        * @param aCustomMessageId The activation message id.
        * @param aCustomMessage Activation params.
        * @return void.
        */
        virtual void DoActivateL(const TVwsViewId& aPrevViewId,TUid aCustomMessageId, 
                const TDesC8& aCustomMessage);

        /**
        * Called when the view is deactivated.
        *
        * @since ?
        * @return void.
        */
        virtual void DoDeactivate();


    public:  // From MTestHarnessObserver
        /**
        * Notifies the observer that the test harness is completed.
        *
        * @since 3.0
        * @return void.
        */
        virtual void Completed();


    private:
        /**
        * C++ default constructor.
        */
        CTestHarnessView();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    
    private:
        CTestHarnessContainer* iContainer;
        TBool                  iContainerOnStack;
        
        CTestHarness*          iHarness;
    };

#endif      // SETTINGS_VIEW_H
            
// End of File

