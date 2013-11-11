/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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








#include "senidentifier.h"
#include "sencertutils.h"

#include "senlogger.h"

EXPORT_C CSenIdentifier* CSenIdentifier::NewL()
    {
    CSenIdentifier* self = NewLC();
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSenIdentifier* CSenIdentifier::NewLC()
    {
    CSenIdentifier* self = new (ELeave) CSenIdentifier();
    CleanupStack::PushL(self);
    self->ConstructL(SenIdentifier::EUnknown, KNullDesC8(), KNullDesC8());
    return self;
    }

EXPORT_C CSenIdentifier* CSenIdentifier::NewL(const CSenIdentifier& aParent)
    {
    CSenIdentifier* self = NewLC(aParent);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSenIdentifier* CSenIdentifier::NewLC(const CSenIdentifier& aParent)
    {
    CSenIdentifier* self = new (ELeave) CSenIdentifier();
    CleanupStack::PushL(self);
    self->ConstructL(aParent);
    return self;
    }


EXPORT_C CSenIdentifier* CSenIdentifier::NewL(SenIdentifier::TIdentifierType aIdentifierType, 
                                              const TDesC8& aIdentifierValue)
    {
    CSenIdentifier* self = NewLC(aIdentifierType, aIdentifierValue);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSenIdentifier* CSenIdentifier::NewLC(SenIdentifier::TIdentifierType aIdentifierType, 
                                               const TDesC8& aIdentifierValue)
    {
    CSenIdentifier* self = new (ELeave) CSenIdentifier();
    CleanupStack::PushL(self);
    self->ConstructL(aIdentifierType, aIdentifierValue, KNullDesC8());
    return self;
    }


EXPORT_C CSenIdentifier* CSenIdentifier::NewL(SenIdentifier::TIdentifierType aIdentifierType, 
                                              const TCertInfo& aCertInfo)
    {
    CSenIdentifier* self = NewLC(aIdentifierType, aCertInfo);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSenIdentifier* CSenIdentifier::NewLC(SenIdentifier::TIdentifierType aIdentifierType, 
                                               const TCertInfo& aCertInfo)
    {
    CSenIdentifier* self = new (ELeave) CSenIdentifier();
    CleanupStack::PushL(self);
    HBufC8* pCertInfo = SenCertUtils::CertInfoToStringLC(aCertInfo);
    self->ConstructL(aIdentifierType, *pCertInfo, KNullDesC8);
    CleanupStack::PopAndDestroy(); // CertInfoToStringLC
    return self;
    }


EXPORT_C CSenIdentifier* CSenIdentifier::NewL(SenIdentifier::TIdentifierType aIdentifierType, 
                                              const TDesC8& aIdentifierValue, 
                                              const TDesC8& aFriendlyName)
    {
    CSenIdentifier* self = NewLC(aIdentifierType, aIdentifierValue, aFriendlyName);
    CleanupStack::Pop(self);
    return self;
    }

EXPORT_C CSenIdentifier* CSenIdentifier::NewLC(SenIdentifier::TIdentifierType aIdentifierType, 
                                               const TDesC8& aIdentifierValue, 
                                               const TDesC8& aFriendlyName)
    {
    CSenIdentifier* self = new (ELeave) CSenIdentifier();
    CleanupStack::PushL(self);
    self->ConstructL(aIdentifierType, aIdentifierValue, aFriendlyName);
    return self;
    }


void CSenIdentifier::ConstructL(SenIdentifier::TIdentifierType aIdentifierType, 
                                const TDesC8& aIdentifierValue, 
                                const TDesC8& aFriendlyName)
    {
    TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "CSenIdentifier::ConstructL - Start");

    iType = aIdentifierType;
    ipValue = aIdentifierValue.AllocL();
    ipFriendlyName = aFriendlyName.AllocL();
    ipStatements = CSenAttributes::NewL();
    ipParent = this;
    TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "CSenIdentifier::ConstructL - End");
    }

void CSenIdentifier::ConstructL(const CSenIdentifier& aParent)
    {
    TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "CSenIdentifier::ConstructL - Start");
    iType = aParent.iType;
    ipValue = aParent.Value().AllocL();
    ipFriendlyName = aParent.ipFriendlyName->AllocL();
    ipStatements = CSenAttributes::NewL(aParent.Statements());

    // Note that all childs in chain get the root as their parent(!):
    ipParent = CSenIdentifier::NewL(aParent.Parent()); 
    TLSLOG_L(KSenUtilsLogChannel, KMinLogLevel, "CSenIdentifier::ConstructL - End");
    }

CSenIdentifier::CSenIdentifier() : 
    ipValue(NULL),
    ipFriendlyName(NULL),
    ipStatements(NULL)
    {

    iType = SenIdentifier::EUnknown;
    }


EXPORT_C CSenIdentifier::~CSenIdentifier()
    {
    delete ipValue;         // it is safe to delete NULL in Symbian C++
    delete ipFriendlyName;
    delete ipStatements;

    delete Proxy(); // destroy the parent (proxy), if such is owned 
    /*
    if ( IsProxy() )  // atm equal to: delete Proxy();
        {   
        delete ipParent; // mm: was identifier ment to take ownership of its parent?
        }
    */
    }


EXPORT_C TPtrC8 CSenIdentifier::Value() const
    {
    return *ipValue;
    }


EXPORT_C SenIdentifier::TIdentifierType CSenIdentifier::Type() const
    {
    return iType;
    }


EXPORT_C TBool CSenIdentifier::Equals(const CSenIdentifier& otherIdentifier)
    {
    TBool result = EFalse;
    result = (iType == otherIdentifier.Type()) && (0 == ipValue->Compare(otherIdentifier.Value()));
    return(result);
    }

// == ParentFor
EXPORT_C void CSenIdentifier::ProxyFor( CSenIdentifier* aIdentifier )
    {
    ipParent = aIdentifier;
    }

/*

EXPORT_C TBool CSenIdentifier::IsProxy() const
    {
    // if parent is NULL, then this is the root
    // and is thus not a proxy for another identifier

    return(!(ipParent == NULL));
    }

// == Parent()
EXPORT_C const CSenIdentifier* CSenIdentifier::Proxy() const
    {
    return(ipParent);    
    }


EXPORT_C const CSenIdentifier* CSenIdentifier::Root() const
    {
    return( (IsProxy()) ? ipParent : this );   
    }
*/

EXPORT_C TBool CSenIdentifier::IsProxy() const
    {
    // If parent is this, then this is the root
    // and thus not a proxy for another identifier
    return(!(ipParent == this));
    }

// return the parent (or this, if this is not a proxy)
EXPORT_C const CSenIdentifier* CSenIdentifier::Proxy() const
    {
    return( IsProxy() ? ipParent : NULL );    
    }

// return the root of (possible parent) 
EXPORT_C const CSenIdentifier* CSenIdentifier::Root() const
    {
    return( IsProxy() ? ipParent->Root() : this );   
    }

EXPORT_C void CSenIdentifier::AddAttributeL( CSenAttribute& aAttribute )	//codescannerwarnings
    {
    ipStatements->AddAttributeL(aAttribute);	//codescannerwarnings
    }


EXPORT_C const CSenAttributes* CSenIdentifier::Attributes() const
    {
    return ipStatements;
    }

EXPORT_C TBool CSenIdentifier::operator==(const CSenIdentifier& a)
    {
    return (this==&a);
    }

const CSenIdentifier& CSenIdentifier::Parent() const
    {
    // if(!ipParent) User::Leave( <define leave code >
    return *ipParent;
    }

const CSenAttributes& CSenIdentifier::Statements() const
    {
    // if(!ipStatements) User::Leave( <define leave code >
    return *ipStatements;
    }

// End of file





