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








#ifndef CPOLICYREFERENCE_ASSERTION_H
#define CPOLICYREFERENCE_ASSERTION_H

#include "assertion.h"
class CPolicyReferenceAssertion :public CAssertion
{
public:
    static  CPolicyReferenceAssertion* NewL(TDesC8& aUri);
    static  CPolicyReferenceAssertion* NewLC(TDesC8& aUri);
    static  CPolicyReferenceAssertion* NewL(CPolicyReferenceAssertion* aValue);
    static  CPolicyReferenceAssertion* NewLC(CPolicyReferenceAssertion* aValue);
    
    virtual ~CPolicyReferenceAssertion();
    
//from CAssertion
    virtual void 	        AddTerm(MAssertion* aAssertion);
    virtual void 	        AddTerms(RPolicyTerms& aAssertions);
    virtual void 	        AddTermCopyL(MAssertion* aAssertion);
    virtual void 	        AddTermsCopyL(RPolicyTerms& aAssertions);
 
    virtual void 	        MoveTermL(RPolicyTerms& aAssertions);
    virtual void 	        MoveTermsL(RPolicyTerms& aAssertions);

    virtual MAssertion* 	GetParent();
    virtual TBool 	        HasParent();
    virtual MAssertion* 	IntersectL(MAssertion* aAssertion);
    virtual TBool 	        IsEmpty();
    virtual TBool 	        IsNormalized();
    virtual MAssertion* 	MergeL(MAssertion* aAssertion);
    virtual MAssertion* 	NormalizeL();
    virtual TBool 	        Remove(MAssertion* aAssertion);
    virtual void 	        SetNormalized(TBool aFlag);
    virtual void 	        SetParent(MAssertion* aParent);
    virtual TInt 	        Size();
    virtual TAssertionType  Type();

    virtual MAssertion*   IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
    virtual MAssertion*   MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
    virtual MAssertion*   NormalizeL(CPolicyRegistry* aRegistry);
    
public:    
    TPtrC8 PolicyUri();
private:
     CPolicyReferenceAssertion();
     void ConstructL(TDesC8& aUri); 
     void ConstructL(CPolicyReferenceAssertion* aValue); 
    
    HBufC8* iUri;
};

#endif //CPOLICYREFERENCE_ASSERTION_H