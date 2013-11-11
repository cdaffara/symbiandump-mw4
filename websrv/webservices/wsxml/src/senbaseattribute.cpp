/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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








// INCLUDE FILES
#include "SenBaseAttribute.h"
#include "SenXmlUtils.h"

EXPORT_C CSenBaseAttribute* CSenBaseAttribute::NewL(const TDesC8& aName,
                                                    const TDesC8& aValue)
    {
    CSenBaseAttribute* pNew = new (ELeave) CSenBaseAttribute;
    CleanupStack::PushL(pNew);
    SenXmlUtils::LeaveOnXmlEscapesL(aName);
    pNew->BaseConstructL(aName, aValue);
    CleanupStack::Pop();
    return pNew;
    }

EXPORT_C CSenBaseAttribute* CSenBaseAttribute::NewL(const TDesC8& aQName,
                                                    const TDesC8& aName,
                                                    const TDesC8& aValue)
    {
    CSenBaseAttribute* pNew = new (ELeave) CSenBaseAttribute;
    CleanupStack::PushL(pNew);
    SenXmlUtils::LeaveOnInvalidElementNameL(aName);
    SenXmlUtils::LeaveOnInvalidElementNameL(aQName);
    pNew->BaseConstructL(aQName, aName, aValue);
    CleanupStack::Pop();
    return pNew;
    }


EXPORT_C CSenBaseAttribute::CSenBaseAttribute()
    {
    }

EXPORT_C void CSenBaseAttribute::BaseConstructL(const TDesC8& aName,
                                                const TDesC8& aValue)
    {
    ipName = aName.AllocL();
    ipValue = aValue.AllocL();
    }

EXPORT_C void CSenBaseAttribute::BaseConstructL(const TDesC8& aQName,
                                                const TDesC8& /* aName */,
                                                const TDesC8& aValue)
    {
    ipValue = aValue.AllocL();
    ipName = aQName.AllocL(); // name means nothing here..
    }



EXPORT_C CSenBaseAttribute::~CSenBaseAttribute()
    {
    delete ipName;
    delete ipValue;
    }

const TDesC8& CSenBaseAttribute::Name() const
    {
    return *ipName;
    }

const TDesC8& CSenBaseAttribute::Value() const
    {
    return *ipValue;
    }

const TDesC8& CSenBaseAttribute::SetValueL(const TDesC8& aValue)
    {
    HBufC8* pValue = aValue.AllocL();
    delete ipValue;
    ipValue = pValue;
    return *ipValue;
    }

// End of File


