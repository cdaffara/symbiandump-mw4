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






 
#ifndef C_PRIMITIVE_ASSERTION_H
#define C_PRIMITIVE_ASSERTION_H


#include "assertion.h"
#include "SenBaseAttribute.h"
class CPolicyAssertion;
class CAssertionAttrib;
class CQName;

typedef RSenPointerMap<TDesC8, TDesC8> RAttributeMap;

class CPrimitiveAssertion : public CAssertion
{
public:
    static  CPrimitiveAssertion* NewL();
    static  CPrimitiveAssertion* NewLC();

    static  CPrimitiveAssertion* NewL(CPrimitiveAssertion* aValue);
    static  CPrimitiveAssertion* NewLC(CPrimitiveAssertion* aValue);

    static  CPrimitiveAssertion* NewL(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri);
    static  CPrimitiveAssertion* NewLC(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri);

    virtual ~CPrimitiveAssertion();
//from CAssertion
    virtual  MAssertion* 	GetParent();
    virtual RPolicyTerms& 	GetTerms();
    virtual  TAssertionType Type();
    virtual  TBool 	HasParent();
    virtual  TBool 	IsEmpty();
    virtual  MAssertion* 	NormalizeL();         
    virtual  MAssertion* 	NormalizeL(CPolicyRegistry* aRegistry);         
    virtual  MAssertion* IntersectL(MAssertion* aAssertion);
    virtual  MAssertion* IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
    virtual  MAssertion* 	MergeL(MAssertion* aAssertion);
    virtual  MAssertion* 	MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
    virtual TBool 	Remove(MAssertion* aAssertion);
    
    virtual  TInt 	Size();
public:    
 
    void  AddAttributeL(TDesC8& aQname, TDesC8& aValue);
    TPtrC8 GetAttributeValue(const TDesC8& aQname);
    RAttributeMap&  Attributes();
    TInt SetAttributesL(RAttributeMap& aAttributes);
    TInt  RemoveAttribute(const TDesC8& aQname);

    CQName* QName();
    TPtrC8 Name();
    TPtrC8 StrValue();
    void SetStrValueL(TDesC8& aStrValue);
    TAny* Value(); 

    TBool IsOptional();
    TInt SetOptional(TBool aOptional);
    CQName* iQname;
private:
    CPrimitiveAssertion();
    void ConstructL();
    void ConstructL(CPrimitiveAssertion* aValue);
    void ConstructL(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri);
    TInt SetTermsL(RPolicyTerms& aTerms);
    RPolicyTerms GetTerms(CPolicyAssertion* aPolicy); 
    TBool IsEmptyPolicy(CPolicyAssertion* aPolicy); 
    CPolicyAssertion* GetSinglePolicyL(RPolicyTerms aChildTerms);
    CPrimitiveAssertion* GetSelfWithoutTermsL();
private:
    MAssertion* iOwner;
	RAttributeMap* iAttr; 
	TBool iIsOptional;
	HBufC8* iStrValue;
	TAny* iValue;
      
    
};

class CQName :public CBase
{
public:
    static  CQName* NewL(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri);
    static  CQName* NewLC(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri);
    ~CQName();
public:
    TPtrC8 Name();
    TPtrC8 QName();
    TPtrC8 Prefix();
    TPtrC8 Uri();

private:
    CQName();
    void ConstructL(TDesC8& aName, TDesC8& aPrefix, TDesC8& aUri);
    HBufC8* iName;
    HBufC8* iQName;
    HBufC8* iPrefix;
    HBufC8* iUri;
};
#endif //C_PRIMITIVE_ASSERTION_H              