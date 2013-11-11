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






#ifndef DELEGATE_FRAGMENT_H
#define DELEGATE_FRAGMENT_H

//  INCLUDES
#include "SenFragmentBase.h"

class CDelegateFragment : public CSenFragmentBase
    {
    public:  // Constructors and destructor

        static CDelegateFragment* NewL(const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName,
                                         const RAttributeArray& aAttributes);

        static CDelegateFragment* NewLC(const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName,
                                         const RAttributeArray& aAttributes);

        static CDelegateFragment* NewL(const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName,
                                         const RAttributeArray& aAttributes,
                                         TXmlEngElement& aParent);

        static CDelegateFragment* NewLC(const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName,
                                         const RAttributeArray& aAttributes,
                                         TXmlEngElement& aParent);

        /**
        * Destructor.
        */
        virtual ~CDelegateFragment();

    private:

        /**
        * C++ default constructor.
        */
        CDelegateFragment();
    };

#endif // DELEGATE_FRAGMENT_H

// End of File


