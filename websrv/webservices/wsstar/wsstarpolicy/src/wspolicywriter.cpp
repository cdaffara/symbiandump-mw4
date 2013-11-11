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







#include "wspolicywriter.h"
#include "wspolicyreader.h"

#include "policyassertion.h"
#include "andcompositeassertion.h"
#include "xorcompositeassertion.h"
#include "assertion.h"
#include "primitiveassertion.h"
#include "policyreferenceassertion.h"
using namespace WSPolicy;
using namespace PolicyWriter;    

CWSPolicyWriter* CWSPolicyWriter::NewL()
{
     CWSPolicyWriter* pSelf = CWSPolicyWriter::NewLC();
    CleanupStack::Pop(pSelf);
    return pSelf;
    
}
CWSPolicyWriter* CWSPolicyWriter::NewLC()
{
       CWSPolicyWriter* pSelf = new (ELeave) CWSPolicyWriter();
    CleanupStack::PushL(pSelf);
    pSelf->ConstructL();
    return pSelf;
     
}
CWSPolicyWriter::~CWSPolicyWriter()
{
    iNamespcaes.Reset();
}
CWSPolicyWriter::CWSPolicyWriter():iNamespcaes(ETrue,ETrue)
{
}
void CWSPolicyWriter::ConstructL()
{
count = 0;    
}

TPtrC8 CWSPolicyWriter::WriteToBufL(CBufBase& aBuf, CPolicyAssertion* aEle)
    {
    RBufWriteStream bufWs(aBuf);
    CleanupClosePushL(bufWs);
    WritePolicyL(aEle, bufWs);
    CleanupStack::PopAndDestroy(); // bufWs.Close();
//    TPtrC8 p = aBuf.Ptr(0);
//    TInt size(p.Length());
    iNamespcaes.Reset();
    return aBuf.Ptr(0);
    }
HBufC8* CWSPolicyWriter::WriteAllAsXMlL(CPolicyAssertion* aPolicy)
{
   if(!aPolicy)
    return NULL;
   
    CBufFlat *pBuf = CBufFlat::NewL(128);
    CleanupStack::PushL(pBuf);
    TPtrC8 p = WriteToBufL(*pBuf, aPolicy);

    TInt size(p.Length());
    HBufC8* pRet = NULL; //p.AllocL();   
    
    if (size > 0)
    {
        pRet = HBufC8::NewLC(size);
        TPtr8 retP = pRet->Des();
        retP.Copy(p.Ptr(),size);
        CleanupStack::Pop(); //pRet
        WriteToFileAllL(retP);            
    }
    
    CleanupStack::PopAndDestroy(); // pBuf;
    iNamespcaes.Reset();    
    return pRet;    
}
HBufC8* CWSPolicyWriter::WriteAllAsXmlL(CSenElement* /*aXml*/)
{
return NULL;    
}
         
TInt CWSPolicyWriter::WritePolicyL(CPolicyAssertion* aEle, RWriteStream& aWriteStream)
{
    aWriteStream.WriteL(KStartElement);
    aWriteStream.WriteL(KWspPolicyPrefix);
    TPtrC8 name(KSenXmlNsAttNamePlusColonWsp);
    if(IsNamespaceAdded(name) == KErrNotFound)
    {
        aWriteStream.WriteL(KSenXmlNsAttNamePlusColonWsp); //namespace
        aWriteStream.WriteL(KSenEqualsDblQuot);
        aWriteStream.WriteL(KWsPolicyNsUri); //namespace
        aWriteStream.WriteL(KSenDblQuot);
        TPtrC8 nameuri(KWsPolicyNsUri);
        AddNameSpaceL(name, nameuri);	//codescannerwarnings
    }
    
/*
		if (policy.getId() != null) {

			writer.writeNamespace(PolicyConstants.WSU_NAMESPACE_PREFIX,
					PolicyConstants.WSU_NAMESPACE_URI);
			writer.setPrefix(PolicyConstants.WSU_NAMESPACE_PREFIX,
					PolicyConstants.WSU_NAMESPACE_URI);

			writer.writeAttribute("wsu", PolicyConstants.WSU_NAMESPACE_URI,
					"Id", policy.getId());
		}
*/
		RPolicyTerms terms = aEle->GetTerms();
		
		if(terms.Count() > 0)
		{
		    aWriteStream.WriteL(KSenGreaterThan);		    
       		for (TInt i = 0; i< terms.Count(); i++) 
       		{
			MAssertion* term = terms[i];
			WriteAssertionL(term, aWriteStream);
		    }
		}
		else
		{
		    aWriteStream.WriteL(KSenGreaterThan);
		}
        
        
        aWriteStream.WriteL(KSenLessThanSlash);
        aWriteStream.WriteL(KWspPolicyPrefix);
		aWriteStream.WriteL(KSenGreaterThan);

        return KErrNone;
}
TInt CWSPolicyWriter::WriteAndAssertionL(CAndCompositeAssertion* aEle, RWriteStream& aWriteStream)
{
    aWriteStream.WriteL(KStartElement);
    aWriteStream.WriteL(KWspAndPrefix);
    aWriteStream.WriteL(KSenGreaterThan);
	RPolicyTerms terms = aEle->GetTerms();
	
	if(terms.Count() > 0)
	{
    	WriteTermsL(terms, aWriteStream);
	}
		
    aWriteStream.WriteL(KSenLessThanSlash);
    aWriteStream.WriteL(KWspAndPrefix);
    aWriteStream.WriteL(KSenGreaterThan);  
    
    return KErrNone; 
}
TInt CWSPolicyWriter::WriteXorAssertionL(CXorCompositeAssertion* aEle, RWriteStream& aWriteStream)
{
    aWriteStream.WriteL(KStartElement);
    aWriteStream.WriteL(KWspXorPrefix);
    aWriteStream.WriteL(KSenGreaterThan);
	RPolicyTerms terms = aEle->GetTerms();
	
	if(terms.Count() > 0)
	{
    	WriteTermsL(terms, aWriteStream);
	}

		
    aWriteStream.WriteL(KSenLessThanSlash);
    aWriteStream.WriteL(KWspXorPrefix);
    aWriteStream.WriteL(KSenGreaterThan);  
    return KErrNone; 
}
TInt CWSPolicyWriter::WriteAssertionL(MAssertion* aEle, RWriteStream& aWriteStream)
{
    if(!aEle)
        return KErrNone; 
    
		if (aEle->Type() == EPrimitiveType)
		{
			WritePrimitiveAssertionL((CPrimitiveAssertion*) aEle, aWriteStream);

		}
		else if (aEle->Type() == ECompositeXorType)
		{
			WriteXorAssertionL((CXorCompositeAssertion*) aEle,	aWriteStream);
		}
		else if (aEle->Type()== EPolicyReferenceType ) 
		{
			WritePolicyReferenceL((CPolicyReferenceAssertion*)aEle, aWriteStream);
		} 
		else if (aEle->Type() == ECompositePolicyType)
		{
			WritePolicyL((CPolicyAssertion*) aEle, aWriteStream);
		
		}
		else if (aEle->Type() == ECompositeAndType)
		{
			WriteAndAssertionL((CAndCompositeAssertion*) aEle,aWriteStream);

		} 
		else 
		{
		}
    
        return KErrNone;
}
TInt CWSPolicyWriter::WritePolicyReferenceL(CPolicyReferenceAssertion* aAssertion, RWriteStream& aWriteStream)
{

    aWriteStream.WriteL(KStartElement);
    aWriteStream.WriteL(KWspPolicyReferencePrefix);
    aWriteStream.WriteL(KSenSpace);
    aWriteStream.WriteL(KWspPolicyReferenceUri);

    aWriteStream.WriteL(KSenEqualsDblQuot);
    aWriteStream.WriteL(aAssertion->PolicyUri());
    aWriteStream.WriteL(KSenDblQuot);
    aWriteStream.WriteL(KSenGreaterThan);
    
    
    aWriteStream.WriteL(KSenLessThanSlash);
    aWriteStream.WriteL(KWspPolicyReferencePrefix);
    aWriteStream.WriteL(KSenGreaterThan);  

    return KErrNone;
}
TInt CWSPolicyWriter::WritePrimitiveAssertionL(CPrimitiveAssertion* aAssertion, RWriteStream& aWriteStream)
{
	TPtrC8 qname = aAssertion->Name();
	aWriteStream.WriteL(KStartElement);
    aWriteStream.WriteL(qname);

    TBuf8<200> name(KSenXmlNsAttNamePlusColon);
    name.Append(aAssertion->iQname->Prefix());
//    if(IsNamespaceAdded(name) == KErrNotFound)
    {
        aWriteStream.WriteL(name); //namespace
        aWriteStream.WriteL(KSenEqualsDblQuot);
        aWriteStream.WriteL(aAssertion->iQname->Uri()); //namespace
        aWriteStream.WriteL(KSenDblQuot);
        TPtrC8 nameuri(aAssertion->iQname->Uri());
        AddNameSpaceL(name, nameuri);	//codescannerwarnings
    }
    
	WriteAttributesL(aAssertion, aWriteStream);
    aWriteStream.WriteL(KSenGreaterThan);	
    
    TPtrC8 strValue = aAssertion->StrValue();    

	if (strValue != KNullDesC8()) {
	
		aWriteStream.WriteL(strValue);
	}

    
	RPolicyTerms terms = aAssertion->GetTerms();
	if(terms.Count() > 0)
	{
	    
    	WriteTermsL(terms, aWriteStream);
	}

	aWriteStream.WriteL(KSenLessThanSlash);
    aWriteStream.WriteL(qname);
    aWriteStream.WriteL(KSenGreaterThan);   
    return KErrNone; 
}
TInt CWSPolicyWriter::WriteTermsL(RPointerArray<MAssertion>& aTerms, RWriteStream& aWriteStream)
{

    for (TInt i=0; i<aTerms.Count(); i++ )
    {
        CAssertion* obj = (CAssertion*)aTerms[i];
        if(obj)
        {
            WriteAssertionL(obj, aWriteStream);
        }
    }
    return KErrNone;
}
void WriteAttrToL(RWriteStream& aWriteStream, TDesC8& aName, TDesC8& aValue)
{
    aWriteStream.WriteL(KSenSpace);
    aWriteStream.WriteL(aName);
    aWriteStream.WriteL(KSenEqualsDblQuot);
    aWriteStream.WriteL(aValue);
    aWriteStream.WriteL(KSenDblQuot);    
}

void CWSPolicyWriter::WriteAttributesL(CPrimitiveAssertion* aAssertion, RWriteStream& aWriteStream)
{

	RAttributeMap& attributes = aAssertion->Attributes();
	
	TInt attCount = attributes.Count();
	
	for (TInt i=0; i<attCount; i++ )
    {
        const TDesC8* nameptr= attributes.KeyAt(i);
        const TDesC8* valptr= attributes.ValueAt(i);
        HBufC8* tt = nameptr->AllocL();
        HBufC8* tt2 = valptr->AllocL();
        TPtrC8 name =  tt->Des();   
        TPtrC8 value =  tt2->Des();
        WriteAttrToL(aWriteStream, name, value);
        delete tt;
        delete tt2;
        
    }
}

TPtrC8 CWSPolicyWriter::GetAttrib(TPtrC8 /*aReference*/, CSenElement* /*aEle*/)
{
    return KNullDesC8();
}
TBool CWSPolicyWriter::IsNamespaceAdded(TPtrC8 aName)
{
    return iNamespcaes.Find(aName);
}

TInt CWSPolicyWriter::AddNameSpaceL(TPtrC8 aName, TPtrC8 aUri)	//codescannerwarnings
{
    if(IsNamespaceAdded(aName) == KErrNotFound)
    {
        return iNamespcaes.Append(aName.AllocL(),aUri.AllocL());            
    }
    return KErrGeneral;
}

//#ifdef _SENDEBUG  
void CWSPolicyWriter::WriteToFileAllL(CSenElement* aXml)
{
  RFs fss;
  User::LeaveIfError(fss.Connect());
  CleanupClosePushL(fss);
    
  TBuf<255> file;
  _LIT( KFileName, "c:\\logs\\normalizer%d.xml");
  file.Format(KFileName, count);
  count++;
  TPtrC OutputFile(file);
    
   RFile  xmlFile;
   xmlFile.Replace(fss , OutputFile, EFileWrite );
   HBufC8* temp = aXml->AsXmlL();
   CleanupStack::PushL(temp);
   xmlFile.Write(temp->Des());
   xmlFile.Close();
   CleanupStack::PopAndDestroy(temp);
   CleanupStack::PopAndDestroy(1); // fss
}
void CWSPolicyWriter::WriteToFileAllL(TDesC8& aXml)
{
  RFs fss;
  User::LeaveIfError(fss.Connect());
  CleanupClosePushL(fss);
    
  TBuf<255> file;
  _LIT( KFileName, "c:\\logs\\normalizer%d.xml");
  file.Format(KFileName, count);
  count++;
  TPtrC OutputFile(file);
    
   RFile  xmlFile;
   xmlFile.Replace(fss , OutputFile, EFileWrite );
   xmlFile.Write(aXml);
   xmlFile.Close();
   CleanupStack::PopAndDestroy(1); // fss
}

//#endif
