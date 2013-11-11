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
* Description:      
*
*/






#include "delegatedomfragment.h"

using namespace Xml;

namespace
    {
    }

CDelegateDomFragment* CDelegateDomFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    CDelegateDomFragment* pNew = NewLC(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CDelegateDomFragment* CDelegateDomFragment::NewLC(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    CDelegateDomFragment* pNew = new (ELeave) CDelegateDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    return pNew;
    }

CDelegateDomFragment* CDelegateDomFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes,
    TXmlEngElement& aParent
    )
    {
    CDelegateDomFragment* pNew = NewLC(aNsUri, aLocalName,
                                aQName, aAttributes, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CDelegateDomFragment* CDelegateDomFragment::NewLC(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes,
    TXmlEngElement& aParent
    )
    {
    CDelegateDomFragment* pNew = new (ELeave) CDelegateDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    return pNew;
    }
    
CDelegateDomFragment* CDelegateDomFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes,
    TXmlEngElement& aParent,
    RSenDocument& aOwnerDocument
    )
    {
    CDelegateDomFragment* pNew = NewLC(aNsUri, aLocalName,
                                       aQName, aAttributes,
                                       aParent, aOwnerDocument);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CDelegateDomFragment* CDelegateDomFragment::NewLC(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes,
    TXmlEngElement& aParent,
    RSenDocument& aOwnerDocument
    )
    {
    CDelegateDomFragment* pNew = new (ELeave) CDelegateDomFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent,
                         aOwnerDocument);
    return pNew;
    }
    

CDelegateDomFragment::CDelegateDomFragment()
    {
    }

CDelegateDomFragment::~CDelegateDomFragment()
    {
    }

// END OF FILE

