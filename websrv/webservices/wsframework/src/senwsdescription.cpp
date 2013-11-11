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








// INCLUDE FILESS
#include "senwsdescription.h"

EXPORT_C CSenWSDescription* CSenWSDescription::NewL()
    {
    CSenWSDescription* pNew = NewLC();
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenWSDescription* CSenWSDescription::NewLC()
    {
    CSenWSDescription* pNew =
        new (ELeave) CSenWSDescription(EWSDescription);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }

EXPORT_C CSenWSDescription* CSenWSDescription::NewL(const TDesC8& aNamespaceURI)
    {
    CSenWSDescription* pNew = NewLC(aNamespaceURI);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenWSDescription* CSenWSDescription::NewLC(const TDesC8& aNamespaceURI)
    {
    CSenWSDescription* pNew =
                new (ELeave) CSenWSDescription(EWSDescription);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aNamespaceURI);
    return pNew;
    }

EXPORT_C CSenWSDescription* CSenWSDescription::NewL(const TDesC8& aEndPoint,
                                                    const TDesC8& aContract)
    {
    CSenWSDescription* pNew = NewLC(aEndPoint, aContract);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenWSDescription* CSenWSDescription::NewLC(const TDesC8& aEndPoint,
                                                     const TDesC8& aContract)
    {
    CSenWSDescription* pNew =
        new (ELeave) CSenWSDescription(EWSDescription);
    CleanupStack::PushL(pNew);
    pNew->ConstructL(aEndPoint, aContract);
    return pNew;
    }
    
EXPORT_C CSenWSDescription::CSenWSDescription(TDescriptionClassType aType)
: CSenXmlServiceDescription(aType)
    {
    }

EXPORT_C CSenWSDescription::~CSenWSDescription()
    {
    }
    
EXPORT_C TBool CSenWSDescription::HasEqualPrimaryKeysL(MSenServiceDescription& aCandidate)
    {
    return CSenXmlServiceDescription::HasEqualPrimaryKeysL(aCandidate);
    }

EXPORT_C TBool CSenWSDescription::IsLocalL()
    {
    return CSenXmlServiceDescription::IsLocalL();
    }

EXPORT_C TInt CSenWSDescription::SetTransportCueL(const TDesC8& aTransportCue)
    {
    return CSenXmlServiceDescription::SetTransportCueL(aTransportCue);
    }

EXPORT_C TPtrC8 CSenWSDescription::TransportCue()
    {
    return CSenXmlServiceDescription::TransportCue();
    }

EXPORT_C TBool CSenWSDescription::HasSuperClass( TDescriptionClassType /* aType */ )
    {
    return EFalse;
    }

EXPORT_C MSenServicePolicy* CSenWSDescription::ServicePolicy()
    {
    return CSenXmlServiceDescription::ServicePolicy();
    }

EXPORT_C TInt CSenWSDescription::SetPolicyL(const TDesC8& aName)
    {
    return CSenXmlServiceDescription::SetPolicyL(aName);
    }

EXPORT_C TInt CSenWSDescription::SetPolicyL(const TDesC8& aName, const TDesC8& aValue)
    {
    return CSenXmlServiceDescription::SetPolicyL(aName, aValue);
    }

EXPORT_C TInt CSenWSDescription::SetPolicyL(const TDesC8& aName, const TDesC8& aValue, const TDesC8& aAttribName, const TDesC8& aAttribValue)
    {
    return CSenXmlServiceDescription::SetPolicyL(aName, aValue, aAttribName, aAttribValue);
    }

EXPORT_C TInt CSenWSDescription::SetProviderIdL( const TDesC8& aProviderID )
    {
    return CSenXmlServiceDescription::SetProviderIdL(aProviderID);
    }

// Setter for SNAP
EXPORT_C void CSenWSDescription::SetSnapIdL(TUint32 aSnapId)
    {
    CSenXmlServiceDescription::SetSnapIdL(aSnapId);
    }

// Getter for SNAP
EXPORT_C TInt CSenWSDescription::SnapId(TUint32& aCurrentSnapId)
    {
    return CSenXmlServiceDescription::SnapId(aCurrentSnapId);
    }

// End of File
