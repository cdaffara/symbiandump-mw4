/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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








#include "wsstaraddressingepr.h"

using namespace WSStarAddressing;

CWSStarAddressingEPR* CWSStarAddressingEPR::NewL(CSenElement& aParent, const TDesC8& aLocalName, 
                                                 const TDesC8& aContent, const TDesC8& aVersion)
    {
    CWSStarAddressingEPR* self = NewLC(aParent, aLocalName, aContent, aVersion);
    CleanupStack::Pop(self);
    return self;
    }

CWSStarAddressingEPR* CWSStarAddressingEPR::NewLC(CSenElement& aParent, const TDesC8& aLocalName, 
                                                  const TDesC8& aContent, const TDesC8& aVersion)
    {
    CWSStarAddressingEPR* self = new (ELeave) CWSStarAddressingEPR();
    CleanupStack::PushL (self);
    self->ConstructL(aParent, aLocalName, aContent, aVersion);
    return self;
    }


// Constructor
CWSStarAddressingEPR::CWSStarAddressingEPR()
    {
    }

// Destructor
CWSStarAddressingEPR::~CWSStarAddressingEPR()
    {
    }

// Second phase construction.
void CWSStarAddressingEPR::ConstructL(CSenElement& aParent, const TDesC8& aLocalName, 
                                        const TDesC8& aContent, const TDesC8& aVersion)
    {
    CSenXmlElement::BaseConstructL(aLocalName);
    SetNamespaceL(KAddressingPrefix(), aVersion);
    SetParent(&aParent);
    CSenElement* elemAddress = CreateElementL(KAddressingPrefix(), KAddressName);
    CleanupStack::PushL(elemAddress);
    elemAddress->SetContentL(aContent);
    AddElementL(*elemAddress);
    CleanupStack::Pop(elemAddress);
    }

void CWSStarAddressingEPR::AddDeviceAddressL(const TDesC8& aMwsNamespace, const TDesC8& aContent)
    {
    CSenElement* elemRefProp = CreateElementL(KAddressingPrefix(),KReferencePropertiesName);
    CleanupStack::PushL(elemRefProp);
    AddElementL(*elemRefProp);
    CleanupStack::Pop(elemRefProp);
    CSenElement& elemDeviceAddress = 
        elemRefProp->AddElementL(aMwsNamespace,WSStarAddressingEPR::KDeviceAddressName, 
                                    WSStarAddressingEPR::KDeviceAddressQName);
    elemDeviceAddress.SetContentL(aContent);
    }
// END OF FILE

