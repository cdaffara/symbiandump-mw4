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
*  CWidgetUiAsyncExit class can exit the WidgetUi in async mode. It calls the CWidgetAppUi`s
*  Exit() method, when the object completes the request.
*
*
*/

#ifndef __WIDGETUIASYNCEXIT_H
#define __WIDGETUIASYNCEXIT_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class MApiProvider;

// CLASS DEFINITION
class CWidgetUiAsyncExit : public CActive
    {
    public:  // constructors

        /**
        * Create a CWidgetUiAsyncExit object, Leaves on failure.
        * @param aApiProvider Api provider. Not owned.
        * @return A pointer to the created instance of CWidgetUiAsyncExit.
        */
        static CWidgetUiAsyncExit* NewL( MApiProvider& aApiProvider );

    private:  // default c++, and 2nd phase constructor

        /**
        * Constructs this object
        * @param aApiProvider Api provider. Not owned.
        */
        CWidgetUiAsyncExit( MApiProvider& aApiProvider );

    public:

        /**
        * Destroy the object and release all memory objects
        */
        ~CWidgetUiAsyncExit();

        /**
        * Complete an asynchronous request.
        */
        void Start();

    protected: // from CActive

        /**
        * Cancel any outstanding requests
        */
        void DoCancel();

        /**
        * Handles object`s request completion event.
        */
        void RunL();

    private:

        /**
        * Callback.
        */
        MApiProvider& iApiProvider;
    };

#endif  // __WIDGETUIASYNCEXIT_H

// End of file
