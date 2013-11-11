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
* Description:  senfragment header declaration      
*
*/








#ifndef MAIN_DOM_FRAGMENT_H
#define MAIN_DOM_FRAGMENT_H

// INCLUDES
#include "SenDomFragmentBase.h"
#include "delegatedomfragment.h"

// CLASS DECLARATIONS
class CMainDomFragment :  public CSenDomFragmentBase
    {
    public: // Constructors and destructor

        static CMainDomFragment* NewL();
        static CMainDomFragment* NewLC();
        static CMainDomFragment* NewL(const TDesC8& aNsUri,
                                           const TDesC8& aLocalName,
                                           const TDesC8& aQName);
        static CMainDomFragment* NewLC(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName);
        virtual ~CMainDomFragment();

        CDelegateDomFragment& DelegateFragment();        

    protected:

        /**
        * C++ default constructor.
        */
        CMainDomFragment();

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void BaseConstructL();
        void BaseConstructL(const TDesC8& aNsUri,
                            const TDesC8& aLocalName,
                            const TDesC8& aQName);

        virtual void OnStartElementL(const RTagInfo& aElement,
        							 const RAttributeArray& aAttributes,
        						 	 TInt aErrorCode);

        virtual void OnEndElementL(const RTagInfo& aElement,
        						   TInt aErrorCode);

        virtual void OnResumeParsingFromL(const RTagInfo& aElement, TInt aErrorCode);
    private: // Data
        CDelegateDomFragment* ipDelegateFragment;  // Owned
    };

#endif // MAIN_DOM_FRAGMENT_H

// End of File


