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








// INCLUDE FILES
#include "wspolicy.h"
#include <f32file.h>
#include <xml/dom/xmlengdom.h>
#include <SenXmlUtils.h>
#include "senguidgen.h"
#include "SenXmlElement.h"
#include "SenDateUtils.h"
#include "wspolicywriter.h"
#include "senxmldebug.h"

using namespace WSPolicy;
namespace
{
    _LIT8(KIntersect, " intersect ");
    _LIT8(KMerge, " merge ");
    _LIT8(KValidUntil, "ValidUntil");
    const TUint KValidityTime = +7;
    const TUint KFlatBufSize = 128;
}

EXPORT_C CWSPolicy* CWSPolicy::NewL()
    {
    CWSPolicy* pSelf = CWSPolicy::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
    }

EXPORT_C CWSPolicy* CWSPolicy::NewLC()
    {
    CWSPolicy* pSelf = new (ELeave) CWSPolicy();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
    }

EXPORT_C CWSPolicy* CWSPolicy::NewL(TDesC8& aPolicyUri, TDesC8& aPolicy)
    {
    CWSPolicy* pSelf = CWSPolicy::NewLC(aPolicyUri, aPolicy);
    CleanupStack::Pop(pSelf);
    return pSelf;
    }
    
EXPORT_C CWSPolicy* CWSPolicy::NewLC(TDesC8& aPolicyUri, TDesC8& aPolicy)
    {
    CWSPolicy* pSelf = new (ELeave) CWSPolicy();
    CleanupStack::PushL(pSelf);
    pSelf->BaseConstructL(aPolicyUri, aPolicy);
    return pSelf;
    }

EXPORT_C CWSPolicy* CWSPolicy::NewL(CSenElement* aPolicy)
    {
     CWSPolicy* pSelf = CWSPolicy::NewLC(aPolicy);
    CleanupStack::Pop(pSelf);
    return pSelf;
    }
    
EXPORT_C CWSPolicy* CWSPolicy::NewLC(CSenElement* aPolicy)
    {
    CWSPolicy* pSelf = new (ELeave) CWSPolicy();
    CleanupStack::PushL(pSelf);
    pSelf->BaseConstructL(aPolicy);
    return pSelf;   
    }
CWSPolicy::~CWSPolicy()
    {
    delete iReader;
    delete iWriter;
    delete iInDoc;
    delete iOutDoc;
    delete iNormaPolicyAssert; //owned
    delete iPolicyId;
    delete iPolicyName;
    delete iPolicyUri;
    }

EXPORT_C CWSPolicy::CWSPolicy()
: iNormPolicy(NULL),
iNormaPolicyAssert(NULL),
iNormalized(EFalse),
iExpired(ETrue)

    {
    }
    
EXPORT_C void CWSPolicy::ConstructL()
    {
    iReader = CWSPolicyReader::NewL();
    iWriter = CWSPolicyWriter::NewL();
    iInDoc= CMyXml::NewL();
    iOutDoc = CMyXml::NewL();
    iNormPolicy = NULL;
    count = 0;

    iExpiry.UniversalTime();
    TTimeIntervalDays interval(KValidityTime);
    iExpiry+=interval;
    }
    
EXPORT_C TInt CWSPolicy::SetPolicyL(TDesC8& aPolicyUri, TDesC8& aPolicy)
    {
    TRAPD (error , BaseConstructL(aPolicyUri, aPolicy));
    return error;
    }
    
void CWSPolicy::BaseConstructL(TDesC8& aPolicyUri, TDesC8& aPolicy)
    {
    ConstructL();

    if(aPolicyUri.Length() > 0 && aPolicy.Length() > 0)
    {
        TInt error = iInDoc->ReadFromDescL(aPolicy);
        if(error != KErrNone)
            return;

        
        CSenElement* policy = iInDoc->XMLDocL();
        if(policy)
        {
            
    //        CSenElement* policy = Policy();
            TPtrC8 policyUri = CWSPolicyUtils::PolicyUriL(policy);

            if(policyUri == KNullDesC8())
            {
                iPolicyUri = aPolicyUri.AllocL();            
                AddPolicyMetadatEndpointL(policy);
            }
            else
            {
                iPolicyUri = policyUri.AllocL();
            }
            

            TPtrC8 name = CWSPolicyUtils::PolicyNameL(policy);
            if(name == KNullDesC8())
            {
                iPolicyName = aPolicyUri.AllocL();
                AddPolicyNameL(policy);    
                
            }
            else
            {
                iPolicyName = name.AllocL();
            }
            
            TPtrC8 id = PolicyIdL(policy);
            iPolicyId = id.AllocL();
            
            
            CSenElement* expiry = GetExpiryTagL();	//codescannerwarnings 
            if(EvaluateExpiryL(expiry))	//codescannerwarnings 
            {
               AddPolicyValidityL(policy);
            }
            else
            {
                AddPolicyValidityL(policy);
            }
            //We have modifed the original XMLElement. Now its safe to 
            //convert this element into PolicyAssertion
        }        
    }
    }

void CWSPolicy::BaseConstructL(CSenElement* aPolicy)
    {
    if(aPolicy)
        {
        TPtrC8 name = CWSPolicyUtils::PolicyNameL(aPolicy);
        TPtrC8 uri = CWSPolicyUtils::PolicyUriL(aPolicy);
        TPtrC8 id = PolicyIdL(aPolicy);

        if(id != KNullDesC8())
            iPolicyId = id.AllocL();
        
        HBufC8* policy = aPolicy->AsXmlL();
        CleanupStack::PushL(policy);
        BaseConstructL(uri, *policy);
        CleanupStack::PopAndDestroy(policy);
        }
    }
    
EXPORT_C TBool CWSPolicy::Normalized()
    { 
    return iNormalized;
    }
//returns the descriptor of NON-NORMALIZED/NORMALIZED Policy
EXPORT_C HBufC8* CWSPolicy::PolicyAsXmlL()
    {
    CSenElement* policy = iOutDoc->XMLDocL();
    if(policy && Normalized())
    {
        return policy->AsXmlL();
    }
    else
    {
        policy = iInDoc->XMLDocL();
        if(policy)
        {
            return policy->AsXmlL();
        }
    }
    return NULL;    
    }
//returns the xml element of NON-NORMALIZED/NORMALIZED Policy    
EXPORT_C    CSenElement* CWSPolicy::PolicyL()	//codescannerwarnings
    {
    CSenElement* policy = iOutDoc->XMLDocL();
    if(policy && Normalized())
    {
        return policy;
    }
    else
    {
        policy = iInDoc->XMLDocL();
        if(policy)
        {
            return policy;
        }
    }
    return NULL;    
    }
EXPORT_C CSenElement* CWSPolicy::NonNormalizedPolicyL()	//codescannerwarnings
{
    if(iInDoc)
    {
        CSenElement* policy = iInDoc->XMLDocL();
        if(policy)
        {
            return policy;
        }
    }
    return NULL;    
}
MAssertion* CWSPolicy::NormaAssertion()
{
    if(iNormaPolicyAssert)
        return iNormaPolicyAssert;
    else
        return NULL;
}
EXPORT_C TBool CWSPolicy::NormalizePolicyL(CPolicyRegistry* aRegistry)
    {
    TBool retval = EFalse;
    TRAPD(error, retval = NormalizePL(aRegistry));
    if(error ==KErrNone)
        return retval;
    else
        return EFalse;
    }
TInt CWSPolicy::NormalizePL(CPolicyRegistry* aRegistry)
    {
        CPolicyAssertion* poolicy = iReader->GetPolicyL(PolicyL());	//codescannerwarnings
    
    if(!poolicy)
        {
        return EFalse;
        }
    if(poolicy->IsNormalized())
        {
        return iNormalized;
        }
    TInt error = KErrNone;    
    
    MAssertion* normalized = NULL;
    TRAP(error, normalized = poolicy->NormalizeL(aRegistry));
	if(error == KErrNone && normalized)
	    {
        iNormalized = ETrue;
        iNormaPolicyAssert = (CPolicyAssertion*)normalized;
        delete poolicy;
//        delete iInDoc;
//        iInDoc = NULL;

        CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        TPtrC8 p = iWriter->WriteToBufL(*pBuf, iNormaPolicyAssert);
        TInt error = iOutDoc->ReadFromDescL(p);
        CleanupStack::PopAndDestroy(pBuf);

        if(error == KErrNone)
            {
            iNormPolicy = iOutDoc->XMLDocL();
            if(iNormPolicy)
                {
                TPtrC8 policyUri = CWSPolicyUtils::PolicyUriL(iNormPolicy);
                if(policyUri == KNullDesC8())
                    {
                    AddPolicyMetadatEndpointL(iNormPolicy);
                    }

                TPtrC8 name = CWSPolicyUtils::PolicyNameL(iNormPolicy);
                if(name == KNullDesC8())
                    {
                    AddPolicyNameL(iNormPolicy); 
                    AddPolicyIdL(iNormPolicy);
                    
                    }
                }
            }
        else
            {
            iNormPolicy = NULL;
            }
            

	    return ETrue;
	    }
	else
	    {
	    return EFalse;      
	    }
    
    }
    
EXPORT_C CWSPolicy* CWSPolicy::IntersectPolicyL(CWSPolicy* aPolicy)
    {
    CWSPolicy* intersectedPolicy = NULL; 
    TRAPD(error, intersectedPolicy = IntersectPL(aPolicy));	//codescannerwarnings
    if(error ==KErrNone)    
        return intersectedPolicy;
    else
        {
        delete intersectedPolicy;
        return NULL;
        }
    
    }
CWSPolicy* CWSPolicy::IntersectPL(CWSPolicy* aPolicy)	//codescannerwarnings
    {
    MAssertion* pol1 = NormaAssertion();
    MAssertion* pol2 = aPolicy->NormaAssertion();

//Only normalized policies can be intersected so if we provide a non normalized policy we will get NULL    
    if(pol1->IsNormalized() && pol2->IsNormalized())    
    {
        MAssertion* result = pol1->IntersectL(pol2);

        if(result)
        {
            CPolicyAssertion* intersected = (CPolicyAssertion*)result;
            
            CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
            CleanupStack::PushL(pBuf);
            TPtrC8 p = iWriter->WriteToBufL(*pBuf, intersected);
            delete intersected; //we dont need it any more
            CWSPolicy* intersectedPolicy = NULL;            
            if(p.Length() > 0)
            {
                TPtrC8 policy2Uri = CWSPolicyUtils::PolicyUriL(aPolicy->PolicyL());  //codescannerwarnings
                HBufC8* nameBuf = HBufC8::NewLC(iPolicyUri->Length()+ policy2Uri.Length()+11);
                TPtr8 name = nameBuf->Des();
                name.Append(iPolicyUri->Des());
                name.Append(KIntersect);
                name.Append(policy2Uri);
            
                intersectedPolicy = CWSPolicy::NewL(name, p);
                CleanupStack::PopAndDestroy(nameBuf);
                if(intersectedPolicy)
                {
                    if(intersectedPolicy->NormalizePolicyL(NULL))
                    {
                        //return intersectedPolicy;
                    }
                    else //cannot normalize intersected so return NULL
                    {
                        delete intersectedPolicy;
                        intersectedPolicy = NULL;
                    }
                }
                else
                {
                    delete intersectedPolicy;
                    intersectedPolicy = NULL;
                }
                
            }
            
            CleanupStack::PopAndDestroy(pBuf);
            return intersectedPolicy;
        }
    }
    
    return NULL;
    }


EXPORT_C CWSPolicy* CWSPolicy::MergePolicyL(CWSPolicy* aPolicy)
    {
    CWSPolicy* mergedPolicy = NULL; 
    TRAPD(error, mergedPolicy = MergePL(aPolicy));
    if(error ==KErrNone)    
        return mergedPolicy;
    else
        {
        delete mergedPolicy;
        return NULL;
        }

    }

CWSPolicy* CWSPolicy::MergePL(CWSPolicy* aPolicy)
    {
    MAssertion* pol1 = NormaAssertion();
    MAssertion* pol2 = aPolicy->NormaAssertion();

//Only normalized policies can be merged so if we provide a non normalized policy we will get NULL    
    if(pol1->IsNormalized() && pol2->IsNormalized())    
    {
        MAssertion* result = pol1->MergeL(pol2);
  
        if(result)
        {
            CPolicyAssertion* merged = (CPolicyAssertion*)result;

            CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
            CleanupStack::PushL(pBuf);
            TPtrC8 p = iWriter->WriteToBufL(*pBuf, merged);
            delete merged; //we dont need it any more
            CWSPolicy* mergedPolicy = NULL;            
            if(p.Length() > 0)
            {
                TPtrC8 policy2Uri = CWSPolicyUtils::PolicyUriL(aPolicy->PolicyL());  //codescannerwarnings
                HBufC8* nameBuf = HBufC8::NewLC(iPolicyUri->Length()+ policy2Uri.Length()+7);
                TPtr8 name = nameBuf->Des();
                name.Append(iPolicyUri->Des());
                name.Append(KMerge);
                name.Append(policy2Uri);
            
                mergedPolicy = CWSPolicy::NewL(name, p);
                CleanupStack::PopAndDestroy(nameBuf);                
                if(mergedPolicy)
                {
                    if(mergedPolicy->NormalizePolicyL(NULL))
                    {
//                        return mergedPolicy;
                    }
                    else //cannot normalize merged so return NULL
                    {
                        delete mergedPolicy;
                        mergedPolicy = NULL;
                    }
                }
                else
                {
                    delete mergedPolicy;
                    mergedPolicy = NULL;
                    
                }
                
            }
            CleanupStack::PopAndDestroy(pBuf);
            return mergedPolicy;
        }
    }
    
    return NULL;
    }

CSenElement* CWSPolicy::NormalizePolicy2L(CSenElement& aPolicy, CPolicyRegistry* aRegistry)
{
    CPolicyAssertion* poolicy = iReader->GetPolicyL(&aPolicy);
    
    if(!poolicy)
        {
        return NULL;
        }
    if(poolicy->IsNormalized())
        {
        return iNormPolicy;
        }
    TInt error = KErrNone;    
    
    MAssertion* normalized = NULL;
    TRAP(error, normalized = poolicy->NormalizeL(aRegistry));
	if(error == KErrNone && normalized)
	    {
        iNormalized = ETrue;
        iNormaPolicyAssert = (CPolicyAssertion*)normalized;
        delete poolicy;
//        delete iInDoc;
//        iInDoc = NULL;

        CBufFlat *pBuf = CBufFlat::NewL(KFlatBufSize);
        CleanupStack::PushL(pBuf);
        TPtrC8 p = iWriter->WriteToBufL(*pBuf, iNormaPolicyAssert);        
        delete iNormaPolicyAssert;
        iNormaPolicyAssert = NULL;
        TInt error = iOutDoc->ReadFromDescL(p);
        CleanupStack::PopAndDestroy(pBuf);

        if(error == KErrNone)
            {
            iNormPolicy = iOutDoc->XMLDocL();
            if(iNormPolicy)
                {
                iNormPolicy = iOutDoc->ExtractElement();
                }
            }
        else
            {
            iNormPolicy = NULL;
            }
	    }
	return iNormPolicy;
    
}
TPtrC8 CWSPolicy::PolicyIdL(CSenElement* aPolicy)
    {
    RPointerArray<CSenBaseAttribute>& attrs = aPolicy->AttributesL();

    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)

        if(bs->Name().Compare(WSPolicy::KWsuId) == 0)
            {
             return bs->Value();
            }
         }
    GeneratePolicyIdL(aPolicy);  //codescannerwarnings
    
    return PolicyIdL(aPolicy);
    }

TPtrC8 CWSPolicy::PolicyNameL(CSenElement* aPolicy)
    {
    RPointerArray<CSenBaseAttribute>& attrs = aPolicy->AttributesL();
    
    CSenBaseAttribute* bs = NULL;
    TInt ele_count = attrs.Count();
         
    for (TInt j=0; j < ele_count; j++)
        {
            
        bs = (attrs)[j];
        if(bs)

        if(bs->Name().Compare(KName) == 0)
            {
             return bs->Value();
            }
         }
    return KNullDesC8();
    }

TInt CWSPolicy::AddPolicyNameL(CSenElement* aPolicy)
    {
  SenXmlUtils::AddAttributeL(*aPolicy, KName, iPolicyName->Des());
  return KErrNone; 
   
    }
TInt CWSPolicy::AddPolicyMetadatEndpointL(CSenElement* aPolicy)
    {
  SenXmlUtils::AddAttributeL(*aPolicy, MSWSPolicy::KMetaDataLocalName, iPolicyUri->Des());
  return KErrNone; 
   
    }
TInt CWSPolicy::AddPolicyIdL(CSenElement* aPolicy)
    {
    aPolicy->AddAttrL(KWsuId, iPolicyId->Des());
    aPolicy->AddNamespaceL(KWsuPrefix, KWsuNsUri);
    return KErrNone; 
   
    }
void CWSPolicy::GeneratePolicyIdL(CSenElement* aPolicy)  //codescannerwarnings
    {

    CSenGuidGen* pGuidGenerator = CSenGuidGen::NewLC();
    HBufC8* pPolicyId = pGuidGenerator->GetRandomGuid8LC();//codescannerwarning
    TPtr8 policyIdDes = pPolicyId->Des();
    policyIdDes.LowerCase();

    aPolicy->AddAttrL(KWsuId, policyIdDes);
    aPolicy->AddNamespaceL(KWsuPrefix, KWsuNsUri);
            
    CleanupStack::PopAndDestroy(pPolicyId);
    CleanupStack::PopAndDestroy(pGuidGenerator);
    }

TInt CWSPolicy::AddPolicyValidityL(CSenElement* aPolicy)
    {
    TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
    SenDateUtils::ToXmlDateTimeUtf82L(ts, iExpiry);
    if(ts.Length() > 0)
        {
        SenXmlUtils::AddAttributeL(*aPolicy, KValidUntil, ts);
        return KErrNone; 
        }
    return KErrNotFound;        
    }
    
EXPORT_C TBool CWSPolicy::IsExpiredPolicyL()  //codescannerwarnings
    {
    CSenElement* expiry = GetExpiryTagL();	//codescannerwarnings 
    if(expiry && expiry->HasContent())
        {
        TPtrC8 expiryTime = expiry->Content();
     
        TTime now;
        now.UniversalTime();
     
        TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
        SenDateUtils::ToXmlDateTimeUtf82L(ts, now);
        
        if(ts > expiryTime)
            return ETrue;
        else
            return EFalse;
        }
    return EFalse; 
    }
    
CSenElement* CWSPolicy::GetExpiryTagL()	//codescannerwarnings 
    {
     CSenElement* documentRoot = PolicyL();  //codescannerwarnings //(CSenElement*)&iPolicyNormalizer->AsElement().Root();
    if(documentRoot)     
    {
        RPointerArray<CSenElement> timeStamps;
        CleanupClosePushL(timeStamps);       
        CSenElement* validity = NULL;
        const CSenNamespace* msNsUri = documentRoot->Namespace(MSWSPolicy::KMSNsPrefix);
        if(msNsUri)
            {
            if(documentRoot->ElementsL(timeStamps, msNsUri->URI(), MSWSPolicy::KMetaDataValidity) == KErrNone)    
                {
                TInt validityStampsCount = timeStamps.Count();
                
                if(validityStampsCount > 0)
                    {
                    validity = timeStamps[0];//mws:metadatavalidity 
                    CleanupStack::PopAndDestroy();
                    return validity->Element(msNsUri->URI(), MSWSPolicy::KExpiry);
                    }
                }
            
            }
        else
            {
            if(documentRoot->ElementsL(timeStamps, MSWSPolicy::KMetaDataValidity) == KErrNone)
                {
                        
                TInt validityStampsCount = timeStamps.Count();
                
                if(validityStampsCount > 0)
                    {
                    validity = timeStamps[0];//mws:metadatavalidity 
                    CleanupStack::PopAndDestroy();
                    return validity->Element(MSWSPolicy::KExpiry);
                    }
                }
            
            }

        CleanupStack::PopAndDestroy();
    }
    return NULL;
    }
    
TBool CWSPolicy::EvaluateExpiryL(CSenElement* aExpiry)	//codescannerwarnings 
{ //we have metadatavalidity element here ,
//extract the Expiry value, and if its "UTC + 7 days" then replace it with expiry time
    TInt updateTime(KErrNotFound);
    if(aExpiry && aExpiry->HasContent())
    {
        TPtrC8 timeTag = aExpiry->Content();
        
        TInt utc = timeTag.Find(_L8("UTC"));        
        TInt days = timeTag.Find(_L8("days"));
        if(utc != KErrNotFound && days != KErrNotFound)
        {   //We found UTC +x days timestamp
            //replace this time stamp with expiry time starting from now
        
            TInt plus = timeTag.Locate('+');
            if(plus != KErrNotFound)
            {
                TInt timeLength = days-plus-2;
                TPtrC8 timeValue =   timeTag.Mid(plus+2,timeLength-1);
                if(timeValue != KNullDesC8)
                {
                    TInt value = 0;        
                    TLex8 lex(timeValue);
                    TInt retVal = lex.Val(value);
                    if(retVal == KErrNone)
                    {
                        if(value > 0)
                        {
                            TTime now;
                            now.UniversalTime();
                            TTimeIntervalDays interval(value);
                            now+=interval;
                            
                            TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
                            SenDateUtils::ToXmlDateTimeUtf82L(ts, now);
                            if(ts.Length() > 0)
                            {
                                aExpiry->SetContentL(ts);
                                iExpiry = now;
                                updateTime = KErrNone;
                            }
                        }
                    }
                    else if(retVal == KErrGeneral || retVal == KErrOverflow)
                    {
                        
                    }

                }
            }
        }
        else //the content is not in "UTC +7 days" format so try the XML time format
        {
            TTime now;
            now.UniversalTime();
     
            TBuf8<SenDateUtils::KXmlDateTimeMaxLength> ts;
            SenDateUtils::ToXmlDateTimeUtf82L(ts, now);
            TTime expiry = SenDateUtils::FromXmlDateTimeL(timeTag);
            if(expiry > now)
            {
              iExpiry = expiry;
              updateTime = KErrNone;  
            }
        
            
        }
    }

    if(updateTime == KErrNone)
        return ETrue;    
    else
        return EFalse;
}

CMyXml* CMyXml::NewL()
{
     CMyXml* pSelf = CMyXml::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
}
CMyXml* CMyXml::NewLC()
{
       CMyXml* pSelf = new (ELeave) CMyXml();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
  
}
CMyXml::~CMyXml()
{
}
CMyXml::CMyXml() :iParsed(EFalse)
{
    
}
void CMyXml::ConstructL()
{
}
CSenElement* CMyXml::XMLDocL()
{
    if(iParsed)
    {
        CSenElement* ele = NULL;
        TRAPD (error, ele = &AsElement());
        if(error == KErrNone)
        {
            if (ele->LocalName().Compare(KWsPolicy) == 0)
            {
                return ele;
            }        
        }
    }

    return NULL;
}
TInt CMyXml::ReadFromDescL(const TDesC8& aData)
{


    TInt size = aData.Length();
    if(size > 0)
        {
        iXmlReader = CSenXmlReader::NewL(KXmlParserMimeType); // use libxml2 sax parser + CodeScannerWarnings     
        TRAPD (err, ParseL(aData);)
        delete(iXmlReader);
        iXmlReader = NULL;
        
        if(err == KErrNone)
            iParsed = ETrue;
        
        return err;
        }
    return KErrArgument;        
}   
// END OF FILE
