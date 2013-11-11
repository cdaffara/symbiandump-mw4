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
* Description:  Enables the destruction of the Select Element Popup Dialog
*               asynchronously
*
*
*/

#ifndef __BROWSERASYNCEXIT_H
#define __BROWSERASYNCEXIT_H

// INCLUDES
#include "BrowserSelectElementDlg.h"
#include <e32base.h>

// FORWARD DECLARATIONS

// CLASS DEFINITION
class CBrowserDialogsProviderAsyncExit : public CActive
    {
    public:  // constructors

        /**
        * Create a CBrowserDialogsProviderAsyncExit object, Leaves on failure.
        * @param aDlg. Not owned.
        * @return A pointer to the created instance of 
        * CBrowserDialogsProviderAsyncExit.
        */
        static CBrowserDialogsProviderAsyncExit* NewL( 
                                            CBrowserSelectElementDlg& aDlg );

        /**
        * Create a CBrowserDialogsProviderAsyncExit object. Leaves on failure.
        * @param aDlg . Not owned.
        * @return A reference to the created instance of 
        * CBrowserDialogsProviderAsyncExit
        */
        static CBrowserDialogsProviderAsyncExit* NewLC( 
                                            CBrowserSelectElementDlg& aDlg );

    private:  // default c++, and 2nd phase constructor

        /**
        * Constructs this object
        * @param aDlg. Not owned.
        */
        CBrowserDialogsProviderAsyncExit( CBrowserSelectElementDlg& aDlg );

        /**
        * Performs second phase construction of this object
        */
        void ConstructL();

    public:

        /**
        * Destroy the object and release all memory objects
        */
        ~CBrowserDialogsProviderAsyncExit();

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
        * Indicates the completion status of a request made to a 
        * service provider.
        */
        //TRequestStatus iStatus;

        /**
        * To access CBrowserSelectElementDlg`s DestroyPopupL() method.
        */
        CBrowserSelectElementDlg& iDlg;  // not owned
    };

#endif  // __BROWSERASYNCEXIT_H

// End of file
