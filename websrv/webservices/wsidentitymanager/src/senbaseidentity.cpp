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
#include <SenXmlElement.h>
#include <SenBaseFragment.h>
#include <SenXmlUtils.h>
#include <SenServiceConnection.h> // WSF error codes

#include "senbaseidentity.h"
#include "senwiseidentityprovider.h"
#include "sendebug.h" // internal Utils\inc - filelogging MACROs
#ifdef __CRYPTO_HW__
#include "sencryptoutils.h"
#include <SecEnv.h>
#include <SecEnvDevice.h>
#include <SecEnvType.h>
#include <SecEnvMgr.h>
const TInt KCryptoPADataLen     = 16;
const TInt KCryptoPACipherLen   = 48;
const TInt KPaCryptAuthLen     = 32;
const TInt KEncryptNoSalt = 3;
const TInt KDecrypt       = 2;
const TUint KPadding = 0;
const TUint KEncrypedLen = 128;
_LIT8(KCryptoPA, "PA_CRYPT");

#endif // __CRYPTO_HW__

EXPORT_C CSenBaseIdentity* CSenBaseIdentity::NewL(TPtrC8 aIMEI)
    {
    CSenBaseIdentity* pNew = NewLC(aIMEI);
    CleanupStack::Pop();
    return(pNew) ;
    }
EXPORT_C CSenBaseIdentity* CSenBaseIdentity::NewLC(TPtrC8 aIMEI)
    {
    CSenBaseIdentity* pNew = new (ELeave) CSenBaseIdentity(aIMEI);
    CleanupStack::PushL(pNew);
    pNew->ConstructL();
    return pNew;
    }
CSenBaseIdentity::CSenBaseIdentity(TPtrC8 aIMEI)
:   iProvider(NULL),
    iIMEI(aIMEI)
    {
    }

void CSenBaseIdentity::ConstructL()
    {
    BaseConstructL( KIdentityElementLocalName() );
    }


CSenIdentityProvider* CSenBaseIdentity::IdentityProviderL(MSenServiceDescription& aService )
    {

    TInt idpCount(iProviderList.Count());

    CSenIdentityProvider* pIdp = NULL;
    CSenIdentityProvider* pCandidate = NULL;

    for (TInt i=0; i<idpCount; i++)
        {
        pCandidate = (CSenIdentityProvider*)iProviderList[i];
        if(pCandidate->IsTrustedByL(aService))
            {
            pIdp = pCandidate;
            pCandidate = NULL;
            }
        if(pIdp == NULL && pCandidate->IsDefault()) 
            {
            pIdp = pCandidate;
            }

        if(pIdp)
            {
            break;
            }
        }
    return pIdp;
    }

CSenIdentityProvider* CSenBaseIdentity::IdentityProviderL(const TDesC8& aURI )
    {
    TInt idpCount(iProviderList.Count());
    CSenIdentityProvider* retVal = NULL;
    for (TInt i=0; i<idpCount; i++)
        {
            retVal = (CSenIdentityProvider*)iProviderList[i];
            if (retVal->Endpoint() == aURI)
                {
                break;
                }
            else
                {
                retVal = NULL;
                }
        }
    return retVal;
    }

CSenIdentityProvider* CSenBaseIdentity::IdentityProviderL(const CDesC8Array& aIdpIdList,
                                                          TBool aStrict )
    {
    TInt idpCount(iProviderList.Count());
    TInt patternCount(aIdpIdList.Count());

    if(patternCount==0)
        {
        if(aStrict)
            {
            return NULL;
            }
        else
            {
            return IdentityProviderL(); // return the default provider
            }
        }

    // to optimize the search, store the default idp
    CSenIdentityProvider* pDefault = NULL;

    for (TInt i=0; i<idpCount; i++)
        {
        for(TInt j=0; j<patternCount; j++)
            {
            TPtrC8 providerID = ((CSenIdentityProvider*)iProviderList[i])->ProviderID();
            if(providerID.Length()>0 && providerID == aIdpIdList[j])
                {
                return (CSenIdentityProvider*)iProviderList[i];
                }
            }
        if(!aStrict)
            {
            // to optimize the search, store the default idp, if found
            if(((CSenIdentityProvider*)iProviderList[i])->IsDefault())
                {
                pDefault = (CSenIdentityProvider*)iProviderList[i];
                }
            }
        }
    if(aStrict)
        {
        // default idp does not meet the search requirements
        // (not in strict pattern list)
        return NULL;
        }
    else
        {
        // if current item did not match with anything in the
        // pattern list, check if it's ok in non-strict perspective
        return pDefault;
        }
    }

CSenIdentityProvider* CSenBaseIdentity::IdentityProviderL(MSenServiceDescription& aService,
                                                          const CDesC8Array& aIdpIdList,
                                                          TBool aStrict)
    {
    TInt idpCount(iProviderList.Count());
    TInt patternCount(aIdpIdList.Count());

    if(patternCount==0)
        {
        if(aStrict)
            {
            return NULL;
            }
        else
            {
            return IdentityProviderL(); // return the default provider
            }
        }

    // to optimize the search, store the default idp
    CSenIdentityProvider* pDefault = NULL;
    CSenIdentityProvider* pTrusted = NULL;

    for (TInt i=0; i<idpCount; i++)
        {
        for(TInt j=0; j<patternCount; j++)
            {
            TPtrC8 providerID = 
                ((CSenIdentityProvider*)iProviderList[i])->ProviderID();
            if(providerID.Length()>0 && providerID == aIdpIdList[j])
                {
                if (((CSenIdentityProvider*)iProviderList[i])->IsTrustedByL(aService))
                    {
                    return (CSenIdentityProvider*)iProviderList[i];
                    }
                }
            }

        if(!aStrict)
            {
            if (!pTrusted)
                {
                if (((CSenIdentityProvider*)iProviderList[i])->IsTrustedByL(aService))
                    {
                    pTrusted = (CSenIdentityProvider*)iProviderList[i];
                    }
                }

            // to optimize the search, store the default IDP, if found
            if(((CSenIdentityProvider*)iProviderList[i])->IsDefault())
                {
                pDefault = (CSenIdentityProvider*)iProviderList[i];
                }
            }
        }


    if(aStrict)
        {
        // default idp does not meet the search requirements
        // (not in strict pattern list)
        return NULL;
        }
    else
        {
        // if current item did not match with anything in the
        // pattern list, check if it's ok in non-strict perspective
        if (pTrusted)
            {
            return pTrusted;
            }
        else
            {
            return pDefault;
            }
        }
    }

CSenIdentityProvider* CSenBaseIdentity::IdentityProviderL()
    {
    TInt idpCount(iProviderList.Count());

    CSenIdentityProvider* pDefaultIdp = NULL;

    for (TInt i=0; i<idpCount && !pDefaultIdp; i++)
        {
        pDefaultIdp = (CSenIdentityProvider*)iProviderList[i];
        if (!pDefaultIdp->IsDefault()) pDefaultIdp = NULL;
        }

    if (!pDefaultIdp && idpCount > 0)
        {
        // use the first IdentityProvider
        pDefaultIdp = (CSenIdentityProvider*)iProviderList[0];
        }

    return pDefaultIdp;
    }
const RIdentityProviderList& CSenBaseIdentity::IdentityProvidersL() const
    {
    return iProviderList;
    }

// NOTE: takes ownership of the given apIdentityProvider
TInt CSenBaseIdentity::RegisterIdentityProviderL(CSenIdentityProvider* apIdentityProvider )
    {
    //SENDEBUG_L("CSenBaseIdentity::RegisterIdentityProviderL(*)");

    if(!apIdentityProvider)
        {
        SENDEBUG_L("RegisterIdentityProviderL(*) - Illegal argument: NULL");
        return KErrArgument;
        }
        
    if (apIdentityProvider->Endpoint() == KNullDesC8)
        {
        SENDEBUG_L("RegisterIdentityProviderL(*) - Error: no Endpoint in IDP");
        //Delete the object since ownership is with us
        delete apIdentityProvider;		
        apIdentityProvider = NULL;        
        return KErrSenNoEndpoint;
        }

    TInt retVal(KErrNone);

    // Now check for duplicates:
    CSenIdentityProvider* pDuplicate = NULL;

    retVal = FindMatchingIdentityProviderL(*apIdentityProvider, pDuplicate);
    if(pDuplicate)
        {
        //SENDEBUG((_L("  Duplicate(s) found, updating the IdP!.")));
        RPointerArray<CSenElement> newElements =
                            apIdentityProvider->AsElement().ElementsL();

        TInt elemCount = newElements.Count();
        TInt i(0);
        // Checking the aIdp elements
        while ( i < elemCount )
            {
            CSenElement* elem = newElements[i];
            TPtrC8 localName = elem->LocalName();

            RPointerArray<CSenElement> pDuplicateAsElems =
                                    pDuplicate->AsElement().ElementsL();
            TInt duplicateCount = pDuplicateAsElems.Count();
            TBool found = EFalse;
            TBool foundServiceID = EFalse;
            TInt j(0);

            // Checking if the duplicate has the element found from aIdp
            // If found update it, if not found insert new element.
            while ( j < duplicateCount )
                {
                CSenElement* dupElem = pDuplicateAsElems[j];
                if ( localName == dupElem->LocalName() )
                    {
                    if ( localName == KServiceIDElementLocalName )
                        {
                        if ( elem->Content() == dupElem->Content() )
                            {
                            foundServiceID = ETrue;
                            }
                        }
                    else
                        {
                        // note, this create the element if it
                        // is not found
                        if (elem->Content() != KNullDesC8)
                            {
                            dupElem->SetContentL( elem->Content() );
                            }
                        RPointerArray<CSenElement> nestedElems = elem->ElementsL();
                        for (TInt k = 0; k < nestedElems.Count(); k++)
                            {
                            CSenElement* dupNestEl = dupElem->Element(nestedElems[k]->LocalName());
                            if (dupNestEl)
                                {
                                dupNestEl->SetContentL(nestedElems[k]->Content());
                                }
                            else
                                {
                                CSenElement* newEl = dupElem->CreateElementL(nestedElems[k]->NsPrefix(),
                                                        nestedElems[k]->LocalName());
                                CleanupStack::PushL(newEl);
                                dupElem->AddElementL(*newEl);
                                CleanupStack::Pop(newEl);
                                newEl->SetContentL(nestedElems[k]->Content());
                                }
                            }
                        
                        //cleaning, if sth has been removed on backend
                        //nested duplicate - local
                        //nested - backend
                        RPointerArray<CSenElement> nestedDupElems = dupElem->ElementsL();
                        for (TInt k = 0; k < nestedDupElems.Count(); k++)
                            {
                            CSenElement* nestEl = elem->Element(nestedDupElems[k]->LocalName());
                            if (!nestEl)
                                {
                                nestedDupElems[k]->SetContentL(KNullDesC8);
                                }
                            }
                                                
                                                
                        found = ETrue;
                        break;
                        }
                    }
                j++;
                }
            
            if ( !found && !foundServiceID && elem)
                {
                // this is new kind of element, and not in original
                // idp, thus it needs to be copied because it is 
                // owned by idp being registered (which will be
                // deleted inside this method)
                CSenElement* pCopy = 
                    CSenXmlElement::NewL( elem->LocalName() ); 
                CleanupStack::PushL(pCopy);
                // copy the contents of serviceid 
                pCopy->CopyFromL(*elem);
                // append the copy into original idp
                pDuplicate->AsElement().AddElementL( *pCopy );
                CleanupStack::Pop(); // pCopy
                }
            i++;
            }
        // NOTE: ownership of the given argument apIdentityProvider
        // will NOT be TRANSFERRED to anywhere.
        // So: free the memory, contents copied and thus updated
        
        delete apIdentityProvider;
        apIdentityProvider = NULL;
        if(retVal > 0)
            retVal = KErrNone;
        }
    else if(retVal==KErrNotFound)
        {
        //SENDEBUG((_L("  No duplicate(s) found, will try to add..")));
        // Append into both provider list and as child element
        retVal = iProviderList.Append( apIdentityProvider );
        if(retVal==KErrNone)
            {
            // OWNERSHIP of apIdentityProvider is now here
            TInt leaveCode(KErrNone);
            TRAP( leaveCode, AsElement().AddElementL(apIdentityProvider->AsElement() ) );
            if(leaveCode!=KErrNone)
                {
                retVal = leaveCode;
                // no need to free the apIdentityProvider, it will
                // be destroyed in destructor as part of iProviderList
                }                                    
            }
        else
            {
            // free the orphan apIdentityProvider
            delete apIdentityProvider;
            apIdentityProvider = NULL;
            }
        }
    else // some other error occurred: free the orphan argument
        {
        //SENDEBUG((_L("An error occured  and no duplicate(s) were found.")));
        delete apIdentityProvider;
        apIdentityProvider = NULL;
        }
    return retVal;
    }
    
TInt CSenBaseIdentity::UpdateIdentityProviderL(CSenIdentityProvider& aIdentityProvider)
    {
    if (aIdentityProvider.Endpoint() == KNullDesC8)
        {
        SENDEBUG_L("RegisterIdentityProviderL(*) - Error: no Endpoint in IDP");
        return KErrSenNoEndpoint;
        }

    TInt retVal(KErrNone);

    // Now check for duplicates:
    CSenIdentityProvider* pDuplicate = NULL;

    retVal = FindMatchingIdentityProviderL(aIdentityProvider, pDuplicate);
    if(pDuplicate)
        {
        //SENDEBUG((_L("  Duplicate(s) found, updating the IdP!.")));
        RPointerArray<CSenElement> newElements =
                            aIdentityProvider.AsElement().ElementsL();

        TInt elemCount = newElements.Count();
        TInt i(0);
        // Checking the aIdp elements
        while ( i < elemCount )
            {
            CSenElement* elem = newElements[i];
            TPtrC8 localName = elem->LocalName();

            RPointerArray<CSenElement> pDuplicateAsElems =
                                    pDuplicate->AsElement().ElementsL();
            TInt duplicateCount = pDuplicateAsElems.Count();
            TBool found = EFalse;
            TBool foundServiceID = EFalse;
            TInt j(0);

            // Checking if the duplicate has the element found from aIdp
            // If found update it, if not found insert new element.
            while ( j < duplicateCount )
                {
                CSenElement* dupElem = pDuplicateAsElems[j];
                if ( localName == dupElem->LocalName() )
                    {
                    if ( localName == KServiceIDElementLocalName )
                        {
                        if ( elem->Content() == dupElem->Content() )
                            {
                            foundServiceID = ETrue;
                            }
                        }
                    else
                        {
                        // note, this create the element if it
                        // is not found
                        dupElem->SetContentL( elem->Content() );
                        found = ETrue;
                        break;
                        }
                    }
                j++;
                }
            
            if ( !found && !foundServiceID && elem)
                {
                // this is new kind of element, and not in original
                // idp, thus it needs to be copied because it is 
                // owned by idp being registered (which will be
                // deleted inside this method)
                CSenElement* pCopy = 
                    CSenXmlElement::NewL( elem->LocalName() ); 
                CleanupStack::PushL(pCopy);
                // copy the contents of serviceid 
                pCopy->CopyFromL(*elem);
                // append the copy into original idp
                pDuplicate->AsElement().AddElementL( *pCopy );
                CleanupStack::Pop(); // pCopy
                }
            i++;
            }
        if(retVal > 0)
            retVal = KErrNone;
        }

    return retVal;    
    }

// Note: non-strict search is used when unregistering: it must be possible
// to unregister a provider using only provider Id in cases where someone
// is unable to update and IDP (because provider id was *already *in use).
// This "someone", cannot figure out the endpoint -> so, only way to unregister
// blocking IDP is to call unregister(ProviderID) and then register the IDP
// using different ProviderID & endpoint pair.

// more about this "non strict" search used by unregistering: to get a MATCH
// usingof plain endpoint must be accepted ONLY if providerid was NOT in use
// with basic web services.
//

// FOR ID-WSF framework, mainly
TInt CSenBaseIdentity::FindMatchingIdentityProviderL(CSenIdentityProvider& aIdp,
                                                     CSenIdentityProvider*& apMatch)
    {
    //SENDEBUG((
    //  _L("CSenBaseIdentity::FindMatchingIdentityProviderL() - Started ")));

    TInt retVal(KErrNotFound);

    TInt idpCount(iProviderList.Count());
    apMatch = NULL;

    for (TInt i=0; i<idpCount; i++)
        {
        TPtrC8 databaseProvID = 
            ((CSenIdentityProvider*)iProviderList[i])->ProviderID();

        TPtrC8 databaseEndpoint = 
            ((CSenIdentityProvider*)iProviderList[i])->Endpoint();

        

        TPtrC8 addedProvID = aIdp.ProviderID();
        TPtrC8 addedEndpoint = aIdp.Endpoint();

        if(databaseProvID==KNullDesC8)
            {
            //  This is Basic Web Services IDP and only endpoint matters 
            if( databaseEndpoint == addedEndpoint )
                {
                retVal = i; // match was found: return the index of found element
                apMatch = iProviderList[i];
                }
            }
        else if(databaseProvID == addedProvID) 
            {
             // This is an ID-WSF IDP, where providerIDs match

            // We might have a match for non-strict ("provider id only" -type  search). 
            // Now we must check that there was
            if( databaseEndpoint == addedEndpoint )
                {
                apMatch = (CSenIdentityProvider*)iProviderList[i];
                retVal = i; // match was found: return the index of found IDP
                }
            else
                {
                // we need to return an error code indicating that provider id
                // is already mapped to different endpoint(!)
                retVal = KErrSenProviderIdInUseByAnotherEndpoint;
                }
            }
        }
    return retVal;
    }


TInt CSenBaseIdentity::UnregisterIdentityProviderL(CSenIdentityProvider& aIdp )
    {
    TInt retVal(KErrNotFound);

    CSenIdentityProvider* pMatch = NULL;
    TInt index = FindMatchingIdentityProviderL(aIdp, pMatch);

    if ( index==KErrNotFound ||
         (aIdp.Endpoint()==KNullDesC8 &&
            index==KErrSenProviderIdInUseByAnotherEndpoint)
       )
        {
        // we need to make it possible to unregister an IDP using
        // plain provider ID
        TPtrC8 providerId = aIdp.ProviderID();
        index = IdentityProviderNamed( providerId, pMatch );
        }

    //  index is either >= 0, OR: if nothing was removed,
    // return KErrNotFound OR: provider id used by
    // another endpoint (new error code)
    if(index>=0 && pMatch)
        {
        // remove the match from database list, which allows us to
        // delete the IdP from memory.
        iProviderList.Remove(index);
        TRAP( retVal, AsElement().RemoveElement(pMatch->AsElement()); ) 
        delete pMatch;
        retVal = KErrNone;
        }

    return retVal; // if nothing was removed, this will return KErrNotFound
    }

TBool CSenBaseIdentity::AssociateServiceL(const TDesC8& aServiceID,
                                          const TDesC8& aProviderID)
    {
    TBool ret = EFalse;
    CSenIdentityProvider* prov = IdentityProviderNamed( aProviderID );
    if ( prov != NULL )
        {
        if ( !prov->IsTrustedByL( aServiceID ))
            {
            CSenElement& service =
                prov->AsElement().AddElementL(KServiceIDElementLocalName);

            service.SetContentL( aServiceID );
            }
        ret = ETrue;
        }
    return ret;
    }

TBool CSenBaseIdentity::DissociateServiceL(const TDesC8& aServiceID,
                                           const TDesC8& aProviderID)
    {
    TBool ret = EFalse;
    CSenIdentityProvider* prov = IdentityProviderNamed( aProviderID );
    if ( prov != NULL )
        {
        if ( prov->IsTrustedByL( aServiceID ))
            {
            RPointerArray<CSenElement> services =
                                    prov->AsElement().ElementsL();
            TInt serviceCount = services.Count();
            TInt i(0);
            while (i < serviceCount)
                {
                if (services[i]->LocalName() == KServiceIDElementLocalName)
                    {
                    CSenElement* elem = services[i];
                    if ( elem->Content() == aServiceID)
                        {
                        TInt retVal;
                        TRAP(retVal, prov->AsElement().RemoveElement(*elem));
                        if (retVal == KErrNone )
                            {
                            if ( elem )
                                {
                                delete elem;
                                }
                            ret = ETrue;
                            }
                        else
                            {
                            ret = EFalse;
                            }
                        break;
                        }
                    }
                i++;
                }
            }
        }
    return ret;
    }

void CSenBaseIdentity::StartElementL(const TDesC8& aNsUri,
                                     const TDesC8& aLocalName,
                                     const TDesC8& aQName,
                                     const RAttributeArray& aAttributes)
    {
    if (aLocalName == KSenIdpLocalname)
        {
        // create CSenWiseIdentityProvider
        iProvider =
            CSenWiseIdentityProvider::NewL(iIMEI, KNullDesC8, KNullDesC8);

        iProvider->SetOwner(*this);
        iProvider->SetAttributesL(aAttributes);
        iProvider->SetReader(*Reader());
        DelegateParsingL(*iProvider);
        }
    else
        {
        DelegateParsingL(aNsUri, aLocalName, aQName, aAttributes);
        }
    }


void CSenBaseIdentity::EndElementL(const TDesC8& aNsUri,
                                   const TDesC8& aLocalName,
                                   const TDesC8& aQName)
    {
    if(aLocalName == KSenIdpLocalname)
        {
        if(iProvider)
            {
#ifdef __CRYPTO_HW__
            TBuf8<KEncrypedLen> decPwd;
            TInt leaveCode(KErrNone); 
            TRAP(leaveCode, DecryptPasswordL(iProvider->Password(), decPwd));
                if (leaveCode == KErrNone)
                    {
                    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("Identity ::DecryptPasswordL success")));
                    }
                else
                    {
                    TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("Identity ::DecryptPasswordL Failed")));
                    }
            iProvider->SetUserInfoL(iProvider->UserName(),iProvider->UserName(), decPwd);
			
#endif // __CRYPTO_HW__
            RegisterIdentityProviderL(iProvider);
            iProvider=NULL;
            }
        }
    else
        {
        CSenBaseFragment::EndElementL(aNsUri,aLocalName,aQName);
        }
    }

const TDesC8& CSenBaseIdentity::Type()
    {
    return KNullDesC8;
    }

const TDesC8& CSenBaseIdentity::Name()
    {
    return KNullDesC8;
    }

const TDesC8& CSenBaseIdentity::UserName()
    {
    return KNullDesC8;
    }


HBufC8* CSenBaseIdentity::AsXmlL()
    {
    return this->AsElement().AsXmlL();
    }

HBufC* CSenBaseIdentity::AsXmlUnicodeL()
    {
    return this->AsElement().AsXmlUnicodeL();
    }

void CSenBaseIdentity::WriteAsXMLToL(RWriteStream& aWriteStream)
    {
#ifdef __CRYPTO_HW__	
    TInt idpCount(iProviderList.Count());
    
    RArray<RBuf8 *> password;
    for(TInt i=0; i<idpCount; i++)
        {
        CSenIdentityProvider* idp = (CSenIdentityProvider*)iProviderList[i];
        
        TBuf8<KEncrypedLen> encPwd;
        TInt leaveCode(KErrNone);            
        //EncryptPasswordL(idp->Password(), encPwd);
        TRAP(leaveCode, EncryptPasswordL(idp->Password(), encPwd));
    if (leaveCode == KErrNone)
        {
        
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("Identity ::EncryptPasswordL success")));
        }
    else
        {
        
        TLSLOG_FORMAT((KSenCoreServiceManagerLogChannelBase  , KMinLogLevel, _L8("Identity ::EncryptPasswordL Failed")));
        }
        RBuf8 *tempPass = new RBuf8;
        tempPass->Create(idp->Password());
        password.Append(tempPass); 
        idp->SetUserInfoL(idp->UserName(),idp->UserName(),encPwd);
        }
#endif // __CRYPTO_HW__
    this->AsElement().WriteAsXMLToL(aWriteStream);
#ifdef __CRYPTO_HW__
    for(TInt i=0; i<idpCount; i++)
        {
        CSenIdentityProvider* idp = (CSenIdentityProvider*)iProviderList[i];
        idp->SetUserInfoL(idp->UserName(),idp->UserName(),*password[i]);
        password[i]->Close();
        delete password[i];
        }
#endif // __CRYPTO_HW__		
    }

EXPORT_C CSenBaseIdentity::~CSenBaseIdentity()
    {
    if(iProvider)
        {
        CSenElement* removed =
            CSenBaseIdentity::AsElement().RemoveElement(iProvider->AsElement());
        // prevend impossible "duplicate delete"
        if(removed)
            {
            delete iProvider; // a leave occured between
                              // StartElementL and EndElementL...
            }
        iProvider = NULL; // to prevent accidental duplicate delete
        }

    TInt idpCount(iProviderList.Count());
    for(TInt i=idpCount-1; i>=0; i--)
        {
        CSenElement* removed =
            CSenBaseIdentity::AsElement().RemoveElement(
                ((CSenIdentityProvider*)iProviderList[i])->AsElement());
        // prevend impossible "duplicate delete"
        if(removed)
            {
            delete iProviderList[i];
            iProviderList.Remove(i);    // 6.11.2004
            }
        }
        
    iProviderList.ResetAndDestroy(); 
    }

CSenIdentityProvider* CSenBaseIdentity::IdentityProviderNamed(const TDesC8& aProviderId)
    {
    CSenIdentityProvider* pMatch = NULL;
    IdentityProviderNamed( aProviderId, pMatch );
    return pMatch;
    }

TInt CSenBaseIdentity::IdentityProviderNamed( const TDesC8& aProviderID,
                                              CSenIdentityProvider*&  apMatch)
    {
    TInt idpCount(iProviderList.Count());
    TInt retVal(KErrNotFound); // index, or an error
    apMatch = NULL;
    for(TInt i=0; i<idpCount; i++)
        {
        TPtrC8 providerId = 
            ((CSenIdentityProvider*)iProviderList[i])->ProviderID();
        if(providerId.Length()>0 && providerId == aProviderID)
            {
            retVal = i; // store the index
            apMatch = (CSenIdentityProvider*)iProviderList[i]; // found a match
            break;
            }
        }
    return retVal;
    }

TInt CSenBaseIdentity::ContainsIdentityProviderL(TBool& aContains,
                                                 CSenIdentityProvider& aIDP)
    {
    aContains = EFalse;
    TInt idpCount(iProviderList.Count());
    
    for(TInt i=0; i<idpCount; i++)
        {
            if (iProviderList[i]->ConsistsOfL(aIDP)
                )
                {
                aContains = ETrue;
                break;  // No need to loop the rest of the providers, we are
                        // happy with the first hit
                }
        }
    return KErrNone;
    }
#ifdef __CRYPTO_HW__    
void CSenBaseIdentity::EncryptPasswordL(const TDesC8& aData, TDes8& aCipherText)
    {
#ifndef __WINSCW__
		  TUint cipherLen;
		  TBuf8<KEncrypedLen> temp;
		  
		  // add padding if length is  less than 16
		if (aData.Length() < KCryptoPADataLen) 
			  {
			  const TInt remainder = aData.Length() % KCryptoPADataLen;
			  temp.Copy(aData);
			  temp.AppendFill( KPadding, KCryptoPADataLen - remainder );
			  cipherLen = temp.Length() + KPaCryptAuthLen ;
			  aCipherText.SetLength(cipherLen);
			  }
		else
			  {
			  temp.Copy(aData);
			  cipherLen = aData.Length() + KPaCryptAuthLen ;
			  aCipherText.SetLength(cipherLen);
			  }
		    
	    	TBuf8<20> paId;
		    paId.Copy(KCryptoPA);
		   
		    CSecEnv* secEnv = CSecEnv::NewL();
		    TInt err = secEnv->ProtectedApplicationExecute(paId, (TAny*)temp.Ptr(), temp.Length(), 
		                                                     (TAny*)aCipherText.Ptr(), cipherLen, KEncryptNoSalt); 
		    delete secEnv;
	    
	    // Encode data into Base64 format
	    	HBufC8 *b64encPwd = SenCryptoUtils::EncodeBase64L(aCipherText);
    	if(b64encPwd)
	      {
	      aCipherText = (*b64encPwd);
	      delete b64encPwd;
	      }
	    	
	          
#else

aCipherText = aData;
//return KErrNotSupported;
#endif
    
    }

void CSenBaseIdentity::DecryptPasswordL(const TDesC8& aCipherText, TDes8& aData)
    {
#ifndef __WINSCW__    	
    TUint dataLen = aCipherText.Length() - KPaCryptAuthLen ;
    aData.SetLength(dataLen);
    TBuf8<20> paId;
    paId.Copy(KCryptoPA);
   // Decode data from Base64 format
    HBufC8 *b64decPwd = SenCryptoUtils::DecodeBase64L(aCipherText);
    	TDesC8 aTmpTxt= (*b64decPwd);
    // Decrypt using PA	
    CSecEnv* secEnv = CSecEnv::NewL();
    TInt err = secEnv->ProtectedApplicationExecute(paId, (TAny*)aTmpTxt.Ptr(), aTmpTxt.Length(), 
                                                     (TAny*)aData.Ptr(), dataLen, KDecrypt);
    // remove padding
    if (aData.Length() == KCryptoPADataLen)
        {
        RBuf8 decPwd;
        decPwd.Create(KCryptoPADataLen);
        decPwd.Copy(aData);
        TInt pos = decPwd.Locate(KPadding);
        if(pos != KErrNotFound)
          {
          decPwd.SetLength(pos);
          aData.SetLength(decPwd.Length());
          aData = decPwd;
          }
        decPwd.Close();
        }
    delete secEnv;                                                  
		if(b64decPwd)
				{
				delete b64decPwd;
				}
#else

aData = aCipherText ;
//return KErrNotSupported;
#endif    
}    
#endif // __CRYPTO_HW__
