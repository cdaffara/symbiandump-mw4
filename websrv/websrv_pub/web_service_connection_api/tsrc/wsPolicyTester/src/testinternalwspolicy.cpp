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







#include <e32std.h>
#include <f32file.h>
#include "testinternalwspolicy.h"

CTestInternalWSPolicy* CTestInternalWSPolicy::NewL()
{
	return((CTestInternalWSPolicy*)CSenInternalWsPolicy::NewL());
}

CTestInternalWSPolicy* CTestInternalWSPolicy::NewLC()
{
	return((CTestInternalWSPolicy*)CSenInternalWsPolicy::NewLC());
}

CTestInternalWSPolicy* CTestInternalWSPolicy::NewL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
{
	return((CTestInternalWSPolicy*)CSenInternalWsPolicy::NewL(aNsUri,aLocalName,aQName,aAttributes));
}

CTestInternalWSPolicy* CTestInternalWSPolicy::NewLC(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes)
{
	return((CTestInternalWSPolicy*)CSenInternalWsPolicy::NewLC(aNsUri,aLocalName,aQName,aAttributes));
}

CTestInternalWSPolicy* CTestInternalWSPolicy::NewL(const TDesC8& aNsUri,
                                                           const TDesC8& aLocalName,
                                                           const TDesC8& aQName,
                                                           const RAttributeArray& aAttributes,
                                                           CSenElement& aParent)
{
	return((CTestInternalWSPolicy*)CSenInternalWsPolicy::NewL(aNsUri,aLocalName,aQName,aAttributes,aParent));
}                                                           

CTestInternalWSPolicy* CTestInternalWSPolicy::NewLC(const TDesC8& aNsUri,
                                                           const TDesC8& aLocalName,
                                                           const TDesC8& aQName,
                                                           const RAttributeArray& aAttributes,
                                                           CSenElement& aParent)
{
	return((CTestInternalWSPolicy*)CSenInternalWsPolicy::NewLC(aNsUri,aLocalName,aQName,aAttributes,aParent));
}
    
CTestInternalWSPolicy::~CTestInternalWSPolicy()
{
	
}

TBool CTestInternalWSPolicy::IsApplicableL(const TDesC8& aPolicyEndpoint)
{
	return(CSenInternalWsPolicy::IsApplicableL(aPolicyEndpoint));
}

TBool CTestInternalWSPolicy::IsApplicableL(CSenInternalWsPolicy* aInternalPolicy)
{
	return(CSenInternalWsPolicy::IsApplicableL(aInternalPolicy));
}

TBool CTestInternalWSPolicy::IsApplicableL(CSenWSDescription& aPattern)
{
	return(CSenInternalWsPolicy::IsApplicableL(aPattern));
}

TBool CTestInternalWSPolicy::UpdateMetadataEndpointL(CSenInternalWsPolicy* aInternalPolicy, TInt& aExist)
{
	return(CSenInternalWsPolicy::UpdateMetadataEndpointL(aInternalPolicy,aExist));
}

TInt CTestInternalWSPolicy::AddMetadataPolicyL(CSenElement* aElement)
{
	return(CSenInternalWsPolicy::AddMetadataPolicyL(aElement));
}
TInt CTestInternalWSPolicy::RemoveMetadataPolicyL(const TDesC8& aUri)
{
	return(CSenInternalWsPolicy::RemoveMetadataPolicyL(aUri));
}
CSenElement* CTestInternalWSPolicy::WsPolicyByUriL(const TDesC8& aUri)
{
	return(CSenInternalWsPolicy::WsPolicyByUriL(aUri));
}
CSenElement* CTestInternalWSPolicy::WsPolicyByName(const TDesC8& aName)
{
	return(CSenInternalWsPolicy::WsPolicyByName(aName));
}
TTime CTestInternalWSPolicy::ValidUntilL(const TDesC8& aUri)
{
	return(CSenInternalWsPolicy::ValidUntilL(aUri));
}
    

////////////////////////////////////////////////////////////////////////
/////////////////////CTestWSPolicyIdentifier///////////////////////////
//////////////////////////////////////////////////////////////////////


CTestWSPolicyIdentifier* CTestWSPolicyIdentifier::NewL()
{
	return ((CTestWSPolicyIdentifier*)CSenWSPolicyIdentifier::NewL());
}

CTestWSPolicyIdentifier* CTestWSPolicyIdentifier::NewLC()
{
	return ((CTestWSPolicyIdentifier*)CSenWSPolicyIdentifier::NewLC());
}
	
CTestWSPolicyIdentifier* CTestWSPolicyIdentifier::NewL(const TDesC8& aNsUri,
                                         				const TDesC8& aLocalName,
                                         				const TDesC8& aQName,
                                         				const RAttributeArray& aAttributes)
{
	return ((CTestWSPolicyIdentifier*)CSenWSPolicyIdentifier::NewL(aNsUri,aLocalName,aQName,aAttributes));
}

CTestWSPolicyIdentifier* CTestWSPolicyIdentifier::NewLC(const TDesC8& aNsUri,
                                         				const TDesC8& aLocalName,
                                         				const TDesC8& aQName,
                                         				const RAttributeArray& aAttributes)
{
	return ((CTestWSPolicyIdentifier*)CSenWSPolicyIdentifier::NewLC(aNsUri,aLocalName,aQName,aAttributes));
}
	
CTestWSPolicyIdentifier* CTestWSPolicyIdentifier::NewL(const TDesC8& aNsUri,
                                         				const TDesC8& aLocalName,
                                         				const TDesC8& aQName,
                                         				const RAttributeArray& aAttributes,
                                         				CSenElement& aParent)
{
	return ((CTestWSPolicyIdentifier*)CSenWSPolicyIdentifier::NewL(aNsUri,aLocalName,aQName,aAttributes,aParent));
}

CTestWSPolicyIdentifier* CTestWSPolicyIdentifier::NewLC(const TDesC8& aNsUri,
                                         				const TDesC8& aLocalName,
                                         				const TDesC8& aQName,
                                         				const RAttributeArray& aAttributes,
                                         				CSenElement& aParent)
{
	return ((CTestWSPolicyIdentifier*)CSenWSPolicyIdentifier::NewLC(aNsUri,aLocalName,aQName,aAttributes,aParent));
}

TInt CTestWSPolicyIdentifier::SetPropertyL(const TDesC8& aName,const TDesC8& aValue)
{
	return(CSenWSPolicyProperties::SetPropertyL(aName,aValue));
}
                                                         
TInt CTestWSPolicyIdentifier::PropertyL(const TDesC8& aName, TPtrC8& aValue)
{
	return(CSenWSPolicyProperties::PropertyL(aName,aValue));
}
void CTestWSPolicyIdentifier::SetIdL(TInt aId)
{
	return(CSenWSPolicyIdentifier::SetIdL(aId));
}
TInt CTestWSPolicyIdentifier::IdL()
{
	return(CSenWSPolicyIdentifier::IdL());
}

TInt CTestWSPolicyIdentifier::AddMetadataEndpointL(CSenElement* aElement)
{
	return(CSenWSPolicyIdentifier::AddMetadataEndpointL(aElement));
}

TPtrC8 CTestWSPolicyIdentifier::MetadataEndpointL()
{
	return(CSenWSPolicyIdentifier::MetadataEndpointL());
}

CSenElement* CTestWSPolicyIdentifier::MetadataEndpointElementL()
{
	return(CSenWSPolicyIdentifier::MetadataEndpointElementL());
}

TPtrC8 CTestWSPolicyIdentifier::ElementAttribValueL(CSenElement* aElement, const TDesC8& aName)
{
	return(CSenWSPolicyIdentifier::ElementAttribValueL(aElement,aName));
}

TBool CTestWSPolicyIdentifier::HasAttributeL(CSenElement* aElement, const TDesC8& aName)
{
	return(CSenWSPolicyIdentifier::HasAttributeL(aElement,aName));
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////CTestPolicyWriter///////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CTestPolicyWriter* CTestPolicyWriter::NewL()
{
	return ((CTestPolicyWriter*)CWSPolicyWriter::NewL());
}

CTestPolicyWriter* CTestPolicyWriter::NewLC()
{
	return ((CTestPolicyWriter*)CWSPolicyWriter::NewLC());
}
void CTestPolicyWriter::WriteToFileAllL(CSenElement* aXml)
{
	return (CWSPolicyWriter::WriteToFileAllL(aXml));
}
void CTestPolicyWriter::WriteToFileAllL(TDesC8& aXml)
{
	return (CWSPolicyWriter::WriteToFileAllL(aXml));
}
HBufC8* CTestPolicyWriter::WriteAllAsXmlL(CPolicyAssertion* aPolicy)
{
//	return (CWSPolicyWriter::WriteAllAsXmlL(aPolicy));
}
/////////////////////////////////////////////////////////////////////////////////
///////////////////////////////CTestPolicyReader/////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CTestPolicyReader* CTestPolicyReader::NewL()
{
	return ((CTestPolicyReader*)CWSPolicyReader::NewL());
}

CTestPolicyReader* CTestPolicyReader::NewLC()
{
	return ((CTestPolicyReader*)CWSPolicyReader::NewLC());
}
CPolicyAssertion* CTestPolicyReader::GetPolicyL(CSenElement* aPolicy)
{
	return (CWSPolicyReader::GetPolicyL(aPolicy));
}

/////////////////////////////////////////////////////////////////////////////////
/////////////////////////////////CTestPolicyAssertion///////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////

CPolicyAssertion* CTestPolicyAssertion::NewL()
{
	return (CPolicyAssertion::NewL());
}

CTestPolicyAssertion* CTestPolicyAssertion::NewLC()
{
	return ((CTestPolicyAssertion*)CPolicyAssertion::NewLC());
}
