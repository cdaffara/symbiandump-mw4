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






#ifndef DELEGATE_DOM_FRAGMENT_H
#define DELEGATE_DOM_FRAGMENT_H

//  INCLUDES
#include "SenDomFragmentBase.h"

class CDelegateDomFragment : public CSenDomFragmentBase
    {
    public:  // Constructors and destructor

        static CDelegateDomFragment* NewL(const TDesC8& aNsUri,
                                          const TDesC8& aLocalName,
                                          const TDesC8& aQName,
                                          const RAttributeArray& aAttributes);

        static CDelegateDomFragment* NewLC(const TDesC8& aNsUri,
                                           const TDesC8& aLocalName,
                                           const TDesC8& aQName,
                                           const RAttributeArray& aAttributes);

        static CDelegateDomFragment* NewL(const TDesC8& aNsUri,
                                          const TDesC8& aLocalName,
                                          const TDesC8& aQName,
                                          const RAttributeArray& aAttributes,
                                          TXmlEngElement& aParent);

        static CDelegateDomFragment* NewLC(const TDesC8& aNsUri,
                                           const TDesC8& aLocalName,
                                           const TDesC8& aQName,
                                           const RAttributeArray& aAttributes,
                                           TXmlEngElement& aParent);

        static CDelegateDomFragment* NewL(const TDesC8& aNsUri,
                                          const TDesC8& aLocalName,
                                          const TDesC8& aQName,
                                          const RAttributeArray& aAttrs,
                                          TXmlEngElement& aParent,
                                          RSenDocument& aOwnerDocument);                                         

        static CDelegateDomFragment* NewLC(const TDesC8& aNsUri,
                                           const TDesC8& aLocalName,
                                           const TDesC8& aQName,
                                           const RAttributeArray& aAttrs,
                                           TXmlEngElement& aParent,
                                           RSenDocument& aOwnerDocument);                                         
        /**
        * Destructor.
        */
        virtual ~CDelegateDomFragment();

    private:

        /**
        * C++ default constructor.
        */
        CDelegateDomFragment();
    };

#endif // DELEGATE_DOM_FRAGMENT_H

// End of File


