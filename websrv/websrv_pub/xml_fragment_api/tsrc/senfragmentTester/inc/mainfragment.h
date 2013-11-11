/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: Header declaration
*
*/






#ifndef MAIN_FRAGMENT_H
#define MAIN_FRAGMENT_H

// INCLUDES
#include "SenFragmentBase.h"
#include "delegatefragment.h"

// CLASS DECLARATIONS
class CMainFragment :  public CSenFragmentBase
    {
    public: // Constructors and destructor

        static CMainFragment* NewL();
        static CMainFragment* NewLC();
        static CMainFragment* NewL(const TDesC8& aNsUri,
                                           const TDesC8& aLocalName,
                                           const TDesC8& aQName);
        static CMainFragment* NewLC(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName);
        virtual ~CMainFragment();
        
        CDelegateFragment& DelegateFragment(); 

    protected:

        /**
        * C++ default constructor.
        */
        CMainFragment();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();
        void BaseConstructL(const TDesC8& aNsUri,
                            const TDesC8& aLocalName,
                            const TDesC8& aQName);

        // Functions from base classes

        // From CSenBaseFragment
        virtual void OnStartElementL(const RTagInfo& aElement,
        							 const RAttributeArray& aAttributes,
        						 	 TInt aErrorCode);

        virtual void OnEndElementL(const RTagInfo& aElement,
        						   TInt aErrorCode);

    private: // Data
        CDelegateFragment* ipDelegateFragment;  // Owned
    };

#endif // MAIN_FRAGMENT_H

// End of File

