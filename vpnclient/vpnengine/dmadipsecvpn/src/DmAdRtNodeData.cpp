/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
* Description: Implementation of CDmAdRtNodeData.
*
*/



#include <utf.h>

#include "DmAdRtNodeData.h"
#include "dmadddf.h"
#include "dmadutil.h"
#include "vpnlogger.h"

CDmAdRtNodeData* CDmAdRtNodeData::NewL(const TDesC8& aUri, MDmAdCallBack* aCallBack)
    {
    TRACE("CDmAdRtNodeData::NewL");
    
    CDmAdRtNodeData *self = new (ELeave) CDmAdRtNodeData();
    CleanupStack::PushL(self);
    self->ConstructL(aUri, aCallBack);
    CleanupStack::Pop();  // self
    return self;
    }

void CDmAdRtNodeData::ConstructL(const TDesC8& aUri, MDmAdCallBack* aCallBack)
    {
    TRACE("CDmAdRtNodeData::ConstructL");
    
    iUriType = UriTypeL(aUri);
    
    switch (iUriType)
        {
        case EDmAdUriTypeVpnAp:
            u.iVpnAp = CDmAdVpnApData::NewL(aCallBack);
            break;
        case EDmAdUriTypePolicy:
            u.iPolicy = CDmAdPolicyData::NewL();
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));            
            User::Leave(KErrGeneral);
            break;
        }
    }
    
CDmAdRtNodeData::CDmAdRtNodeData()
    {
    TRACE("CDmAdRtNodeData::CDmAdRtNodeData");
    }

CDmAdRtNodeData::~CDmAdRtNodeData()
    {
    TRACE("CDmAdRtNodeData::~CDmAdRtNodeData");
    
    switch (iUriType)
        {
        case EDmAdUriTypeVpnAp:
            delete u.iVpnAp;
            break;
        case EDmAdUriTypePolicy:
            delete u.iPolicy;
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Invariant();
            break;
        }
    }
    
//--------------------------------------------------------------------------

TDmAdUriType CDmAdRtNodeData::UriType(void) const
    {
    TRACE("CDmAdRtNodeData::UriType");
    return iUriType;
    }
    
TDmAdUriType CDmAdRtNodeData::UriTypeL(const TDesC8& aUri)
    {
    TRACE("CDmAdRtNodeData::UriTypeL");
    
    TDmAdUriType ret = EDmAdUriTypeVpnAp;
    CArrayFix<TPtrC8>* uriSegList;
    TDmAdUtil::ParseUriLC(aUri, uriSegList);
    
    if (uriSegList->Count() < 2)
        {
        DEBUG_LOG(_L("Uri seg count < 2"));
        User::Leave(KErrGeneral);
        }

    if (uriSegList->At(0).Compare(KDmAdNodeNokiaIpSecVpn) == 0)
        {
        if (uriSegList->At(1).Compare(KDmAdNodePolicy) == 0)
            {
            ret = EDmAdUriTypePolicy;
            }
        else if (uriSegList->At(1).Compare(KDmAdNodeAp) == 0)
            {
            ret = EDmAdUriTypeVpnAp;
            }
        else
            {
            DEBUG_LOG(_L("Unknown URI element"));            
            User::Leave(KErrGeneral);
            }
        }
    else
        {
        DEBUG_LOG(_L("Unknown URI element"));
        User::Leave(KErrGeneral);
        }

    CleanupStack::PopAndDestroy(); // uriSegList
    return ret;
    }
    
//--------------------------------------------------------------------------
    
void CDmAdRtNodeData::UpdateLeafDataL(const TDesC8& aUri, const TDesC8& aObject)
    {
    TRACE("CDmAdRtNodeData::UpdateLeafDataL");
    
    switch (iUriType)
        {
        case EDmAdUriTypeVpnAp:
            UpdateLeafDataVpnApL(aUri, aObject);
            break;
        case EDmAdUriTypePolicy:
            UpdateLeafDataPolicyL(aUri, aObject);
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }

void CDmAdRtNodeData::UpdateLeafDataVpnApL(const TDesC8& aUri, const TDesC8& aObject)
    {
    TRACE("CDmAdRtNodeData::UpdateLeafDataVpnApL");
    
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafName) == 0)
        {
        u.iVpnAp->SetNameL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafId) == 0)
        {
        HBufC* policyRef = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aObject);
        CleanupStack::PushL(policyRef);
        u.iVpnAp->SetPolicyRefIdL(*policyRef);
        CleanupStack::PopAndDestroy(policyRef);
        }
    else if (lastSeg.Compare(KDmAdLeafConRef) == 0)
        {
        u.iVpnAp->SetConRefL(aObject);
        }
    else
        {
        DEBUG_LOG(_L("Node not found"));        
        User::Leave(KErrNotFound);
        }
    }

void CDmAdRtNodeData::UpdateLeafDataPolicyL(const TDesC8& aUri, const TDesC8& aObject)
    {
    TRACE("CDmAdRtNodeData::UpdateLeafDataPolicyL");
    
    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafName) == 0)
        {
        u.iPolicy->SetNameL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafId) == 0)
        {
        u.iPolicy->SetIdL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafVersion) == 0)
        {
        u.iPolicy->SetVersionL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafDescription) == 0)
        {
        u.iPolicy->SetDescriptionL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafIssuer) == 0)
        {
        u.iPolicy->SetIssuerL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafContact) == 0)
        {
        u.iPolicy->SetContactL(aObject);
        }
    else if (lastSeg.Compare(KDmAdLeafContent) == 0)
        {
        u.iPolicy->SetContentL(aObject);
        }
    else
        {
        DEBUG_LOG(_L("Node not found"));
        User::Leave(KErrNotFound);
        }
    }

void CDmAdRtNodeData::FetchLeafObjectLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject)
    {
    switch (iUriType)
        {
        case EDmAdUriTypeVpnAp:
            FetchLeafObjectVpnApLC(aUri, aLuid, aObject);
            break;
        case EDmAdUriTypePolicy:
            FetchLeafObjectPolicyLC(aUri, aLuid, aObject);
            break;
        default:
            DEBUG_LOG(_L("Unknown URI type"));
            User::Leave(KErrGeneral);
            break;
        }
    }
    
void CDmAdRtNodeData::FetchLeafObjectVpnApLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject)
    {
    CBufBase* object = CBufFlat::NewL(32);
    CleanupStack::PushL(object);

    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafName) == 0)
        {
        HBufC8* obj = u.iVpnAp->NameLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafThisRef) == 0)
        {
        HBufC8* obj = u.iVpnAp->ThisRefLC(aLuid);
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafId) == 0)
        {
        HBufC* policyRef = u.iVpnAp->PolicyRefIdLC();        
        HBufC8* obj = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*policyRef);
        CleanupStack::PushL(obj);
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(obj); 
        CleanupStack::PopAndDestroy(policyRef); 
        }
    else if (lastSeg.Compare(KDmAdLeafUri) == 0)
        {
        HBufC8* obj = u.iVpnAp->PolicyRefUriLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafConRef) == 0)
        {
        HBufC8* obj = u.iVpnAp->ConRefLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else
        {
        DEBUG_LOG(_L("Node not found"));
        User::Leave(KErrNotFound);
        }
    
    aObject = object;
    }
    
void CDmAdRtNodeData::FetchLeafObjectPolicyLC(const TDesC8& aUri, const TDesC8& aLuid, CBufBase*& aObject)
    {
    (void)aLuid;
    CBufBase* object = CBufFlat::NewL(32);
    CleanupStack::PushL(object);

    TPtrC8 lastSeg = TDmAdUtil::LastUriSeg(aUri);
    if (lastSeg.Compare(KDmAdLeafName) == 0)
        {
        HBufC8* obj = u.iPolicy->NameLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafId) == 0)
        {
        HBufC8* obj = u.iPolicy->IdLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafVersion) == 0)
        {
        HBufC8* obj = u.iPolicy->VersionLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafDescription) == 0)
        {
        HBufC8* obj = u.iPolicy->DescriptionLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafIssuer) == 0)
        {
        HBufC8* obj = u.iPolicy->IssuerLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafContact) == 0)
        {
        HBufC8* obj = u.iPolicy->ContactLC();
        object->InsertL(0, *obj);
        CleanupStack::PopAndDestroy(); //obj
        }
    else if (lastSeg.Compare(KDmAdLeafContent) == 0)
        {
        TPtrC8 obj(u.iPolicy->Content());
        object->InsertL(0, obj);
        }
    else
        {
        DEBUG_LOG(_L("Node not found"));
        User::Leave(KErrNotFound);
        }
    
    aObject = object;
    }
    

//======================================================================================
//======================================================================================
    
CDmAdVpnApData* CDmAdVpnApData::NewL(MDmAdCallBack* aCallBack)
    {
    TRACE("CDmAdVpnApData::NewL");
    
    CDmAdVpnApData *self = new (ELeave) CDmAdVpnApData(aCallBack);
    return self;
    }

    
CDmAdVpnApData::CDmAdVpnApData(MDmAdCallBack* aCallBack) : iCallBack(aCallBack)
    {
    TRACE("CDmAdVpnApData::CDmAdVpnApData");
    }

CDmAdVpnApData::~CDmAdVpnApData()
    {
    TRACE("CDmAdVpnApData::~CDmAdVpnApData");
    
    delete iName;
    delete iPolicyRefId;
    delete iConRef;
    }
    
void CDmAdVpnApData::SetNameL(const TDesC8& aName)
    {
    TRACE("CDmAdVpnApData::SetNameL");
    
    delete iName;
    iName = NULL;
    if (aName.Length() > 0)
        {
        iName = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aName);
        }
    }

HBufC8* CDmAdVpnApData::NameLC(void)
    {
    if (iName == 0)
        {
        return HBufC8::NewLC(1);
        }
    HBufC8* ret = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*iName);
    CleanupStack::PushL(ret);
    return ret;
    }

void CDmAdVpnApData::SetPolicyRefIdL(const TDesC& aPolicyRefId)
    {
    TRACE("CDmAdVpnApData::SetPolicyRefIdL");
    
    delete iPolicyRefId;
    iPolicyRefId = 0;
    if (aPolicyRefId.Length() > 0)
        {
        iPolicyRefId = aPolicyRefId.AllocL();
        }
    }

HBufC* CDmAdVpnApData::PolicyRefIdLC(void)
    {
    if (iPolicyRefId == 0)
        {
        return HBufC::NewLC(1);
        }
    HBufC* ret = iPolicyRefId->AllocLC();
    return ret;
    }

void CDmAdVpnApData::SetConRefL(const TDesC8& aConRef)
    {
    TRACE("CDmAdVpnApData::SetConRefL");
    
    delete iConRef;
    iConRef = 0;
    if (aConRef.Length() > 0)
        {
        iConRef = aConRef.AllocL();
        }
    }

HBufC8* CDmAdVpnApData::ConRefLC(void)
    {
    if (iConRef == 0)
        {
        return HBufC8::NewLC(1);
        }
    HBufC8* ret = iConRef->AllocLC();
    return ret;
    }
    
HBufC8* CDmAdVpnApData::ThisRefLC(const TDesC8& aLuid)
    {   
    HBufC8* thisRef = iCallBack->FindChildUriL(KDmAdFetchLinkAp, aLuid);
    if (thisRef == 0)
        {
        return HBufC8::NewLC(1);
        }
    CleanupStack::PushL(thisRef);
    return thisRef;
    }

HBufC8* CDmAdVpnApData::PolicyRefUriLC(void)
    {
    if (iPolicyRefId == 0)
        {
        return HBufC8::NewLC(1);
        }

    const TPtrC8 policyLuid((const TUint8*)iPolicyRefId->Ptr(), iPolicyRefId->Size());                        
    HBufC8* refUri = iCallBack->FindChildUriL(KDmAdFetchLinkPolicy, policyLuid);
    if (refUri == 0)
        {
        return HBufC8::NewLC(1);
        }
    CleanupStack::PushL(refUri);
    return refUri;
    }

void CDmAdVpnApData::RestoreL(const TVpnApParms& aVpnApParms)
    {
    TRACE("CDmAdVpnApData::RestoreL");
    
    delete iName;
    iName = NULL;
    if (aVpnApParms.iName.Length() > 0)
        {
        iName = aVpnApParms.iName.AllocL();
        }
    
    delete iPolicyRefId;
    iPolicyRefId = 0;
    if (aVpnApParms.iPolicyId.Length() > 0)
        {
        iPolicyRefId = aVpnApParms.iPolicyId.AllocL();
        }

    delete iConRef;
    iConRef = NULL;
    if (aVpnApParms.iRealConnRef != 0)
        {
        HBufC8* luid = TDmAdUtil::IntToDes8LC(aVpnApParms.iRealConnRef);
        
        switch(aVpnApParms.iRealConnRefType)
            {        
            case TVpnApParms::EIapRealConnRef:
                iConRef = iCallBack->FindChildUriL(KDmAdFetchLinkAp, *luid);
                break;
            case TVpnApParms::ESnapRealConnRef:
                iConRef = iCallBack->FindChildUriL(KDmAdFetchLinkSnap, *luid);
                break;
            default:
                User::Invariant();
                break;                    
            }                
        CleanupStack::PopAndDestroy(); // luid
        }
    }

void CDmAdVpnApData::ToStoreL(TVpnApParms& aVpnApParms)
    {
    TRACE("CDmAdVpnApData::ToStoreL");
    
    if (iName == 0)
        {
        aVpnApParms.iName.SetLength(0);
        }
    else
        {
        if (iName->Length() > aVpnApParms.iName.MaxLength())
            {
            DEBUG_LOG(_L("iName too long"));            
            User::Leave(KErrOverflow);
            }
        aVpnApParms.iName.Copy(*iName);
        }
        
    if (iPolicyRefId == 0)
        {
        aVpnApParms.iPolicyId.SetLength(0);
        }
    else
        {
        if (iPolicyRefId->Length() > aVpnApParms.iPolicyId.MaxLength())
            {
            DEBUG_LOG(_L("iPolicyRefId too long"));                        
            User::Leave(KErrOverflow);
            }
        aVpnApParms.iPolicyId.Copy(*iPolicyRefId);
        }

    aVpnApParms.iRealConnRef = 0;
    if (iConRef != NULL)
        {
        if (iConRef->Find(KDmAdFetchLinkAp) == 0)
            {
            aVpnApParms.iRealConnRefType = TVpnApParms::EIapRealConnRef; 
            }
        else if (iConRef->Find(KDmAdFetchLinkSnap) == 0)
            {
            aVpnApParms.iRealConnRefType = TVpnApParms::ESnapRealConnRef;
            }
        else
            {
            User::Leave(KErrGeneral);
            }
            
        HBufC8* luid = iCallBack->GetLuidAllocL(*iConRef);
        CleanupStack::PushL(luid);
        if (luid->Length() > 0)
            {                        
            aVpnApParms.iRealConnRef = TDmAdUtil::DesToInt(*luid);            
            }
        CleanupStack::PopAndDestroy(luid);
        }
    }
    
//======================================================================================
//======================================================================================
    
CDmAdPolicyData* CDmAdPolicyData::NewL(void)
    {
    TRACE("CDmAdPolicyData::NewL");
    
    CDmAdPolicyData *self = new (ELeave) CDmAdPolicyData();
    return self;
    }

    
CDmAdPolicyData::CDmAdPolicyData()
    {
    TRACE("CDmAdPolicyData::CDmAdPolicyData");
    }

CDmAdPolicyData::~CDmAdPolicyData()
    {
    TRACE("CDmAdPolicyData::~CDmAdPolicyData");
    
    delete iName;
    delete iId;
    delete iVersion;
    delete iDescription;
    delete iIssuer;
    delete iContact;
    delete iContent;
    }
    
//--------------------------------------------------------------------------
    
void CDmAdPolicyData::SetNameL(const TDesC8& aName)
    {
    TRACE("CDmAdPolicyData::SetNameL");
    
    delete iName;
    iName = 0;
    if (aName.Length() > 0)
        {
        iName = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aName);
        }
    }

HBufC8* CDmAdPolicyData::NameLC(void)
    {
    if (iName == 0)
        {
        return HBufC8::NewLC(1);
        }
    HBufC8* ret = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*iName);
    CleanupStack::PushL(ret);
    return ret;
    }

void CDmAdPolicyData::SetIdL(const TDesC8& aId)
    {
    TRACE("CDmAdPolicyData::SetIdL");
    
    delete iId;
    iId = NULL;
    if (aId.Length() > 0)
        {
        iId = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aId);
        }
    }

HBufC8* CDmAdPolicyData::IdLC(void)
    {
    if (iId == 0)
        {
        return HBufC8::NewLC(1);
        }
    HBufC8* ret = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*iId);
    CleanupStack::PushL(ret);
    return ret;
    }

void CDmAdPolicyData::SetVersionL(const TDesC8& aVersion)
    {
    TRACE("CDmAdPolicyData::SetVersionL");
    
    delete iVersion;
    iVersion = 0;
    if (aVersion.Length() > 0)
        {
        iVersion = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aVersion);
        }
    }

HBufC8* CDmAdPolicyData::VersionLC(void)
    {
    if (iVersion == 0)
        {
        return HBufC8::NewLC(1);
        }
    HBufC8* ret = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*iVersion);
    CleanupStack::PushL(ret);
    return ret;
    }

void CDmAdPolicyData::SetDescriptionL(const TDesC8& aDescription)
    {
    TRACE("CDmAdPolicyData::SetDescriptionL");
    
    delete iDescription;
    iDescription = NULL;
    if (aDescription.Length() > 0)
        {
        iDescription = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aDescription);
        }
    }

HBufC8* CDmAdPolicyData::DescriptionLC(void)
    {
    if (iDescription == 0)
        {
        return HBufC8::NewLC(1);
        }
    HBufC8* ret = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*iDescription);
    CleanupStack::PushL(ret);
    return ret;
    }

void CDmAdPolicyData::SetIssuerL(const TDesC8& aIssuer)
    {
    TRACE("CDmAdPolicyData::SetIssuerL");
    
    delete iIssuer;
    iIssuer = NULL;
    if (aIssuer.Length() > 0)
        {
        iIssuer = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aIssuer);
        }
    }

HBufC8* CDmAdPolicyData::IssuerLC(void)
    {
    if (iIssuer == 0)
        {
        return HBufC8::NewLC(1);
        }
    HBufC8* ret = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*iIssuer);
    CleanupStack::PushL(ret);
    return ret;
    }

void CDmAdPolicyData::SetContactL(const TDesC8& aContact)
    {
    TRACE("CDmAdPolicyData::SetContactL");
    
    delete iContact;
    iContact = NULL;
    if (aContact.Length() > 0)
        {
        iContact = CnvUtfConverter::ConvertToUnicodeFromUtf8L(aContact);
        }
    }

HBufC8* CDmAdPolicyData::ContactLC(void)
    {    
    if (iContact == 0)
        {
        return HBufC8::NewLC(1);
        }
    HBufC8* ret = CnvUtfConverter::ConvertFromUnicodeToUtf8L(*iContact);
    CleanupStack::PushL(ret);
    return ret;
    }

void CDmAdPolicyData::SetContentL(const TDesC8& aContent)
    {
    TRACE("CDmAdPolicyData::SetContentL");
    
    delete iContent;
    iContent = NULL;
    if (aContent.Length() > 0)
        {
        iContent = aContent.AllocL();
        }
    }

TPtrC8 CDmAdPolicyData::Content(void)
    {
    TRACE("CDmAdPolicyData::Content");
    
    TPtrC8 ret(KNullDesC8);
    if (iContent != 0)
        {
        ret.Set(*iContent);
        }
    return ret;
    }

//--------------------------------------------------------------------------
    
void CDmAdPolicyData::DetailsRestoreL(const TVpnPolicyDetails& aDetails)
    {
    TRACE("CDmAdPolicyData::DetailsRestoreL");
    
    delete iName;
    iName = NULL;
    if (aDetails.iName.Length() > 0)
        {
        iName = aDetails.iName.AllocL();
        }
    
    delete iId;
    iId = 0;
    if (aDetails.iId.Length() > 0)
        {
        iId = aDetails.iId.AllocL();
        }
    
    delete iVersion;
    iVersion = 0;
    if (aDetails.iVersion.Length() > 0)
        {
        iVersion = aDetails.iVersion.AllocL();
        }
    
    delete iDescription;
    iDescription = 0;
    if (aDetails.iDescription.Length() > 0)
        {
        iDescription = aDetails.iDescription.AllocL();
        }
    
    delete iIssuer;
    iIssuer = 0;
    if (aDetails.iIssuerName.Length() > 0)
        {
        iIssuer = aDetails.iIssuerName.AllocL();
        }
    
    delete iContact;
    iContact = 0;
    if (aDetails.iContactInfo.Length() > 0)
        {
        iContact = aDetails.iContactInfo.AllocL();
        }
    }

void CDmAdPolicyData::DetailsToStoreL(TVpnPolicyDetails& aDetails)
    {
    TRACE("CDmAdPolicyData::DetailsToStoreL");
    
    if (iName == 0)
        {
        aDetails.iName.SetLength(0);
        }
    else
        {
        if (iName->Length() > aDetails.iName.MaxLength())
            {
            DEBUG_LOG(_L("iName too long"));            
            User::Leave(KErrOverflow);
            }
        aDetails.iName.Copy(*iName);
        }
        
    if (iId == 0)
        {
        aDetails.iId.SetLength(0);
        }
    else
        {
        if (iId->Length() > aDetails.iId.MaxLength())
            {
            DEBUG_LOG(_L("iId too long"));
            User::Leave(KErrOverflow);
            }
        aDetails.iId.Copy(*iId);
        }
        
    if (iVersion == 0)
        {
        aDetails.iVersion.SetLength(0);
        }
    else
        {
        if (iVersion->Length() > aDetails.iVersion.MaxLength())
            {
            DEBUG_LOG(_L("iVersion too long"));            
            User::Leave(KErrOverflow);
            }
        aDetails.iVersion.Copy(*iVersion);
        }
        
    if (iDescription == 0)
        {
        aDetails.iDescription.SetLength(0);
        }
    else
        {
        if (iDescription->Length() > aDetails.iDescription.MaxLength())
            {
            DEBUG_LOG(_L("iDescription too long"));            
            User::Leave(KErrOverflow);
            }
        aDetails.iDescription.Copy(*iDescription);
        }
        
    if (iIssuer == 0)
        {
        aDetails.iIssuerName.SetLength(0);
        }
    else
        {
        if (iIssuer->Length() > aDetails.iIssuerName.MaxLength())
            {
            DEBUG_LOG(_L("iIssuer too long"));            
            User::Leave(KErrOverflow);
            }
        aDetails.iIssuerName.Copy(*iIssuer);
        }
        
    if (iContact == 0)
        {
        aDetails.iContactInfo.SetLength(0);
        }
    else
        {
        if (iContact->Length() > aDetails.iContactInfo.MaxLength())
            {
            DEBUG_LOG(_L("iContact too long"));            
            User::Leave(KErrOverflow);
            }
        aDetails.iContactInfo.Copy(*iContact);
        }
    }

void CDmAdPolicyData::ContentRestoreL(const TDesC8& aContent)
    {
    TRACE("CDmAdPolicyData::ContentRestoreL");
    
    delete iContent;
    iContent = NULL;
    if (aContent.Length() > 0)
        {
        iContent = aContent.AllocL();
        }
    }

TPtrC8 CDmAdPolicyData::ContentToStore(void)
    {
    TRACE("CDmAdPolicyData::ContentToStore");
    
    TPtrC8 ret(KNullDesC8);
    if (iContent != 0)
        {
        ret.Set(*iContent);
        }
    return ret;
    }
