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

#include <SenServiceConnection.h> // framework IDs
#include <SenXmlElement.h> // check if this include is needed(?)
#include <SenXmlUtils.h>

// #include "SenDebug.h"

//#include "MSenIdentityManager.h"
#include "SenIdentityProvider.h"

// CONSTANTS




namespace
    {
    _LIT8(KDefaultIdentityAttributeName,    "default");
    _LIT8(KNewElementName,                  "IdentityProvider");
    _LIT8(KServiceID,                       "ServiceID");
//    _LIT8(KFakeIMEI8,                       "012345678901234");
    }

EXPORT_C CSenIdentityProvider::CSenIdentityProvider(
                                                TDescriptionClassType aType)
: CSenXmlServiceDescription(aType)
    {
    }

EXPORT_C const TDesC8& CSenIdentityProvider::NewElementName()
    {
    return KNewElementName();
    }

EXPORT_C CSenIdentityProvider* CSenIdentityProvider::NewL(
                                                    const TDesC8& aEndpoint)
    {
    CSenIdentityProvider* pNew = NewLC(aEndpoint);
    CleanupStack::Pop();
    return(pNew);
    }
EXPORT_C CSenIdentityProvider* CSenIdentityProvider::NewLC(
                                                    const TDesC8& aEndpoint)
    {
    CSenIdentityProvider* pNew =
        new (ELeave) CSenIdentityProvider(EIdentityProvider);

    CleanupStack::PushL(pNew);
    pNew->ConstructL(aEndpoint);
    return pNew;
    }

EXPORT_C CSenIdentityProvider* CSenIdentityProvider::NewL(
                                                    const TDesC8& aEndpoint,
                                                    const TDesC8& aContract )
    {
    CSenIdentityProvider* pNew = NewLC(aEndpoint, aContract);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenIdentityProvider* CSenIdentityProvider::NewLC(
                                                    const TDesC8& aEndpoint,
                                                    const TDesC8& aContract )
    {
    CSenIdentityProvider* pNew =
        new (ELeave) CSenIdentityProvider(EIdentityProvider);

    CleanupStack::PushL(pNew);
    pNew->ConstructL(aEndpoint, aContract);
    return pNew;
    }

EXPORT_C CSenIdentityProvider* CSenIdentityProvider::NewL(
                                                    const TDesC8& aEndpoint,
                                                    const TDesC8& aContract,
                                                    const TDesC8& aProviderID)
    {
    CSenIdentityProvider* pNew = NewLC(aEndpoint, aContract, aProviderID);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenIdentityProvider* CSenIdentityProvider::NewLC(
                                                    const TDesC8& aEndpoint,
                                                    const TDesC8& aContract,
                                                    const TDesC8& aProviderID)
    {
    CSenIdentityProvider* pNew =
        new (ELeave) CSenIdentityProvider(EIdentityProvider);

    CleanupStack::PushL(pNew);
    pNew->ConstructL(aEndpoint, aContract, aProviderID);
    return pNew;
    }

EXPORT_C CSenIdentityProvider* CSenIdentityProvider::NewL(
                                                    const TDesC8& aEndpoint,
                                                    const TDesC8& aContract,
                                                    const TDesC8& aProviderID,
                                                    const TDesC8& aServiceID)
    {
    CSenIdentityProvider* pNew = NewLC(aEndpoint, aContract,
                                        aProviderID, aServiceID);
    CleanupStack::Pop();
    return(pNew) ;
    }

EXPORT_C CSenIdentityProvider* CSenIdentityProvider::NewLC(
                                                    const TDesC8& aEndpoint,
                                                    const TDesC8& aContract,
                                                    const TDesC8& aProviderID,
                                                    const TDesC8& aServiceID)
    {
    CSenIdentityProvider* pNew =
        new (ELeave) CSenIdentityProvider(EIdentityProvider);

    CleanupStack::PushL(pNew);
    pNew->ConstructL(aEndpoint, aContract, aProviderID, aServiceID);
    return pNew;
    }



EXPORT_C void CSenIdentityProvider::ConstructL(const TDesC8& aEndpoint)
    {
    if(aEndpoint.Length()>0)
        {
        CSenXmlServiceDescription::ConstructL(aEndpoint, KSenIdpLibertyIdWsfAsContract);
        SetFrameworkIdL( KDefaultIdWsfFrameworkID );
        }
    else
        {
        CSenXmlServiceDescription::ConstructL();
        }
    }

EXPORT_C void CSenIdentityProvider::ConstructL( const TDesC8& aEndpoint,
                                                const TDesC8& aContract)
    {
    if(aContract.Length()>0)
        {
        CSenXmlServiceDescription::ConstructL(aEndpoint, aContract);
        }
    else
        {
        // Defaults to ID-WSF framework authentication service contract:
        // - "urn:liberty:as:2004-04".
        ConstructL(aEndpoint);
        }
    }

EXPORT_C void CSenIdentityProvider::ConstructL( const TDesC8& aEndpoint,
                                                const TDesC8& aContract,
                                                const TDesC8& aProviderID)
    {
    ConstructL(aEndpoint, aContract);
    if(aProviderID.Length()>0)
        {
        SetProviderID(aProviderID);
        }
    // else: no <ProviderID> child element will be added
    }

EXPORT_C void CSenIdentityProvider::ConstructL( const TDesC8& aEndpoint,
                                                const TDesC8& aContract,
                                                const TDesC8& aProviderID,
                                                const TDesC8& aServiceID)
    {
    ConstructL(aEndpoint, aContract, aProviderID);
    if(aServiceID.Length()>0)
        {
        SetServiceID(aServiceID);
        }
    // else: no <ServiceID> child element will be added
    }

//////////////////////////////////////////////////////////////////////////


EXPORT_C CSenIdentityProvider::~CSenIdentityProvider()
    {
    }

EXPORT_C TPtrC8 CSenIdentityProvider::UserName()
    {
    TPtrC8 username = AuthzID();
    if (username.Length()==0)
        {
        return AdvisoryAuthnID();
        }
    return username;
    }

EXPORT_C TPtrC8 CSenIdentityProvider::AuthzID()
    {
    return ContentOf( KSenIdpAuthzIDLocalname() );
    }

EXPORT_C TPtrC8 CSenIdentityProvider::AdvisoryAuthnID()
    {
    return ContentOf( KSenIdpAdvisoryAuthnIdLocalname() );
    }

EXPORT_C TPtrC8 CSenIdentityProvider::ProviderID()
    {
    return ContentOf( KSenIdpProviderIdLocalname() );
    }

EXPORT_C TPtrC8 CSenIdentityProvider::Password()
    {
    return ContentOf( KSenIdpPasswordLocalname() );
    }

EXPORT_C const TDesC8& CSenIdentityProvider::IMEI()
    {
    return KFakeIMEI8();
    }

EXPORT_C TBool CSenIdentityProvider::IsDefault()
    {
    TBool ret = EFalse;
    if ( AsElement().AttrValue( KDefaultIdentityAttributeName ) )
        {
        ret = ETrue;
        }

    return ret;
    }
//EXPORT_C TInt CSenIdentityProvider::SetUserInfo(const TDesC8& aAuthzID,
//                                                const TDesC8& aAdvisoryAuthnID,
//                                                const TDesC8& aPassword )
//    {
//    TInt leaveCode(KErrNone);
//    TRAP(leaveCode, SetUserInfoL(aAuthzID, aAdvisoryAuthnID, aPassword));
//    return leaveCode;
//    }
    
EXPORT_C TInt CSenIdentityProvider::SetUserInfoL(const TDesC8& aAuthzID,
                                                const TDesC8& aAdvisoryAuthnID,
                                                const TDesC8& aPassword )
    {
    // For MS this function sud be commented as WLM already send xml encoded password
    //HBufC8* encPassword = SenXmlUtils::EncodeHttpCharactersLC(aPassword);
    if(aAuthzID.Length()>0)
        {
        SetContentOfL(KSenIdpAuthzIDLocalname, aAuthzID);
        }
    if(aAdvisoryAuthnID.Length()>0)
        {
        SetContentOfL(KSenIdpAdvisoryAuthnIdLocalname, aAdvisoryAuthnID);
        }
    //if(aPassword.Length()>0) // in order to clean password, when listing accounts
        {
        SetContentOfL(KSenIdpPasswordLocalname, aPassword );
        }
    //CleanupStack::PopAndDestroy(encPassword);
    return KErrNone;
    }

EXPORT_C TInt CSenIdentityProvider::SetAccountExtensionsL(const TDesC8& aDetail)
    {
    SetContentOfL(KSenAccountExtensions, aDetail);
    //CSenElement* elem = NULL;
    //AddElementL(elem);
    return KErrNone;
    }

EXPORT_C TPtrC8 CSenIdentityProvider::AccountExtensions(CSenElement*& aAccExt)
    {
    aAccExt = this->AsElement().Element(KSenAccountExtensions());
    return ContentOf( KSenAccountExtensions() );
    }

EXPORT_C TBool CSenIdentityProvider::IsTrustedByL(
                        MSenServiceDescription& aServiceDescription )
    {
    TBool retVal(EFalse);
    TPtrC8 endpoint = aServiceDescription.Endpoint();
    TPtrC8 contract = aServiceDescription.Contract();

    TPtrC8 providerID = ProviderID();
    if(providerID.Length()>0)
        {
        if((endpoint.Length() > 0) && (providerID.Length() > 0)
                       && endpoint == providerID)
            {
            retVal = ETrue;
            }
        }

    if(!retVal) // no need to allocate new array etc if we already have a trust
        {
        // owned elsewhere:
        RPointerArray<CSenElement> services = AsElement().ElementsL();
        TInt serviceCount = services.Count();
        TInt i(0);
        while (!retVal && i < serviceCount)
            {
            if(services[i]->LocalName() == KServiceID)
                {
                TPtrC8 serviceID = services[i]->Content();

                if(serviceID.Length()>0 &&
                        ( (endpoint.Length() > 0 && serviceID == endpoint)
                                       ||
                          (contract.Length() > 0 && serviceID == contract))
                   )
                    {
                    retVal = ETrue;
                    }
                }
            i++;
            }
        }

    return retVal;
    }

EXPORT_C TBool CSenIdentityProvider::IsTrustedByL( const TDesC8& aProviderIdOrServiceId )
    {
    //SENDEBUG_L("CSenIdentityProvider::IsTrustedByL");
    TBool trusted(EFalse);
    TPtrC8 providerID = ProviderID();
    if(providerID.Length()>0)
        {
        if(aProviderIdOrServiceId == providerID)
            {
            //SENDEBUG_L("  The ProviderID's are equal.");
            trusted = ETrue;
            }
        }

    if(!trusted) // no need for new array etc if we already have a trust
        {
        RPointerArray<CSenElement> services = AsElement().ElementsL();
        TInt serviceCount = services.Count();
        TInt i(0);
        while (!trusted && i < serviceCount)
            {
            if (services[i]->LocalName() == KServiceID )
                {
                TPtrC8 serviceID = services[i]->Content();
                if (serviceID.Length()>0 && serviceID == aProviderIdOrServiceId)
                    {
                    trusted = ETrue;
                    }
                }
            i++;
            }
        }
    return trusted;
    }

EXPORT_C TInt CSenIdentityProvider::SetProviderID( const TDesC8& aProviderID )
    {
    TInt retVal(KErrNone);
    TInt leaveCode(KErrNone);
    TRAP( leaveCode, retVal = SetProviderIdL( aProviderID ); )
    if ( leaveCode )
        {
        retVal = leaveCode;
        }
    return retVal;
    }
    
EXPORT_C TInt CSenIdentityProvider::SetProviderIdL( const TDesC8& aProviderID )
    {
    if ( aProviderID.Length() == 0 )
        {
        return KErrArgument; // zero-length provider ID
        }
    else 
        {
        SetContentOfL( KSenIdpProviderIdLocalname, aProviderID );
        return KErrNone;
        }
    }

EXPORT_C TInt CSenIdentityProvider::SetServiceID( const TDesC8& aServiceID )
    {
    TInt retVal(KErrNone);
    if(aServiceID.Length()>0)
        {
        TRAP(retVal, SetContentOfL(KServiceID, aServiceID);)
        }
    return retVal;
    }

// @deprecated. This method is no longer in use. Kept only to keep BC with 3.0
EXPORT_C HBufC8* CSenIdentityProvider::HttpCredentialsL( MSenIdentityManager& /* aMgr */ )
    {
    return NULL;
    }

// End of File

