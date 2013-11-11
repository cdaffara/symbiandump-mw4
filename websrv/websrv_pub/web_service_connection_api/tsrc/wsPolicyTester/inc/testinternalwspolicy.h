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
* Description: Header declaration
*
*/






#ifndef CTESTINTERNALWSPOLICY_H
#define CTESTINTERNALWSPOLICY_H

#include <e32std.h>
#include <f32file.h>
#include "SenDomFragment.h"
#include <seninternalwspolicy.h>
#include <wspolicywriter.h>
#include <wspolicyreader.h>
#include <policyassertion.h>

//#include <SenWSPolicyIdentifier.h>


class CTestInternalWSPolicy :public CSenInternalWsPolicy//, CSenWSPolicyIdentifier
{
public:    
    static CTestInternalWSPolicy* NewL();
    static CTestInternalWSPolicy* NewLC();
    static CTestInternalWSPolicy* NewL(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes);
    static CTestInternalWSPolicy* NewLC(const TDesC8& aNsUri,
                                            const TDesC8& aLocalName,
                                            const TDesC8& aQName,
                                            const RAttributeArray& aAttributes);
	static CTestInternalWSPolicy* NewL(const TDesC8& aNsUri,
                                                           const TDesC8& aLocalName,
                                                           const TDesC8& aQName,
                                                           const RAttributeArray& aAttributes,
                                                           CSenElement& aParent);
    static CTestInternalWSPolicy* NewLC(const TDesC8& aNsUri,
                                                           const TDesC8& aLocalName,
                                                           const TDesC8& aQName,
                                                           const RAttributeArray& aAttributes,
                                                           CSenElement& aParent);
	TBool IsApplicableL(const TDesC8& aPolicyEndpoint);
	TBool IsApplicableL(CSenInternalWsPolicy* aInternalPolicy);
	TBool IsApplicableL(CSenWSDescription& aPattern);
	
	TBool UpdateMetadataEndpointL(CSenInternalWsPolicy* aInternalPolicy, TInt& aExist);
	
	TInt AddMetadataPolicyL(CSenElement* aElement);
	TInt RemoveMetadataPolicyL(const TDesC8& aUri);
    CSenElement* WsPolicyByUriL(const TDesC8& aUri);
    CSenElement* WsPolicyByName(const TDesC8& aName);
	TTime ValidUntilL(const TDesC8& aUri);
        
    ~CTestInternalWSPolicy();
    //CTestInternalWSPolicy();
    
    
};

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


class CTestWSPolicyIdentifier : public CSenWSPolicyIdentifier
{
public:
	static CTestWSPolicyIdentifier* NewL();
	static CTestWSPolicyIdentifier* NewLC();
	static CTestWSPolicyIdentifier* NewL(const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName,
                                         const RAttributeArray& aAttributes);
	static CTestWSPolicyIdentifier* NewLC(const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName,
                                         const RAttributeArray& aAttributes);
	static CTestWSPolicyIdentifier* NewL(const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName,
                                         const RAttributeArray& aAttributes,
                                         CSenElement& aParent);
	static CTestWSPolicyIdentifier* NewLC(const TDesC8& aNsUri,
                                         const TDesC8& aLocalName,
                                         const TDesC8& aQName,
                                         const RAttributeArray& aAttributes,
                                         CSenElement& aParent);

	TInt SetPropertyL(const TDesC8& aName,const TDesC8& aValue);
	TInt PropertyL(const TDesC8& aName, TPtrC8& aValue);
	void SetIdL(TInt aId);
	TInt IdL();
	TInt AddMetadataEndpointL(CSenElement* aElement);
	TPtrC8 MetadataEndpointL();
	CSenElement* MetadataEndpointElementL();
	TPtrC8 ElementAttribValueL(CSenElement* aElement, const TDesC8& aName);
	TBool HasAttributeL(CSenElement* aElement, const TDesC8& aName);


};

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


class CTestPolicyWriter : public CWSPolicyWriter
{
	public:
	
	static CTestPolicyWriter* NewL();
	static CTestPolicyWriter* NewLC();
	void WriteToFileAllL(CSenElement* aXml);
	void WriteToFileAllL(TDesC8& aXml);
	HBufC8* WriteAllAsXmlL(CPolicyAssertion* aXml);
};


/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


class CTestPolicyReader : public CWSPolicyReader
{
	public:
	
	static CTestPolicyReader* NewL();
	static CTestPolicyReader* NewLC();
	CPolicyAssertion* GetPolicyL(CSenElement* aEle);
};

#endif

/////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////
////////////////////////////////////////////////////////////////////////////////


class CTestPolicyAssertion : public CPolicyAssertion
{
	public:
	
	static CPolicyAssertion* NewL();
	static CTestPolicyAssertion* NewLC();
};

