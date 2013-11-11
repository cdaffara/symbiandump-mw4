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






#include "delegatefragment.h"

using namespace Xml;

namespace
    {
    }

CDelegateFragment* CDelegateFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    CDelegateFragment* pNew = NewLC(aNsUri, aLocalName, aQName, aAttributes);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CDelegateFragment* CDelegateFragment::NewLC(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes
    )
    {
    CDelegateFragment* pNew = new (ELeave) CDelegateFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes);
    return pNew;
    }

CDelegateFragment* CDelegateFragment::NewL(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes,
    TXmlEngElement& aParent
    )
    {
    CDelegateFragment* pNew = NewLC(aNsUri, aLocalName,
                                aQName, aAttributes, aParent);
    CleanupStack::Pop(); // pNew;
    return pNew;
    }

CDelegateFragment* CDelegateFragment::NewLC(
    const TDesC8& aNsUri,
    const TDesC8& aLocalName,
    const TDesC8& aQName,
    const RAttributeArray& aAttributes,
    TXmlEngElement& aParent
    )
    {
    CDelegateFragment* pNew = new (ELeave) CDelegateFragment;
    CleanupStack::PushL(pNew);
    pNew->BaseConstructL(aNsUri, aLocalName, aQName, aAttributes, aParent);
    return pNew;
    }

CDelegateFragment::CDelegateFragment()
    {
    }

CDelegateFragment::~CDelegateFragment()
    {
    }

// End of File


