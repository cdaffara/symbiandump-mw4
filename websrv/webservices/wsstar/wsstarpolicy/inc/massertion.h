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








#ifndef MASSERTION_H
#define MASSERTION_H

#include <e32std.h>
#include "wspolicyregistry.h"

class MAssertion;
class CPolicyRegistry;

typedef RPointerArray<MAssertion> RPolicyTerms;

 enum TAssertionType
 {
    EInvalidType = 0,
    ECompositeAndType = 1,
    ECompositePolicyType = 2,
    ECompositeXorType = 3,
    EPolicyReferenceType = 4,
    EPrimitiveType = 5
     
 };

class MAssertion
{

public: 
    virtual void 	        AddTerm(MAssertion* aAssertion) = 0;
    virtual void 	        AddTerms(RPolicyTerms& aAssertions) = 0;
    virtual void 	        AddTermCopyL(MAssertion* aAssertion) = 0;
    virtual void 	        AddTermsCopyL(RPolicyTerms& aAssertions) = 0;
 
    virtual void 	        MoveTermL(RPolicyTerms& aAssertions) = 0;
    virtual void 	        MoveTermsL(RPolicyTerms& aAssertions) = 0;

    virtual MAssertion* 	GetParent() = 0;
    virtual RPolicyTerms& 	GetTerms() = 0;
    virtual TBool 	        HasParent() = 0;
    virtual MAssertion* 	IntersectL(MAssertion* aAssertion) = 0;
    virtual TBool 	        IsEmpty() = 0;
    virtual TBool 	        IsNormalized() = 0;
    virtual MAssertion* 	MergeL(MAssertion* aAssertion) = 0;
    virtual MAssertion* 	NormalizeL() = 0;
    virtual TBool 	        Remove(MAssertion* aAssertion) = 0;
    virtual void 	        SetNormalized(TBool aFlag) = 0;
    virtual void 	        SetParent(MAssertion* aParent) = 0;
    virtual TInt 	        Size() = 0;
    virtual TAssertionType  Type() = 0;

    virtual MAssertion*   IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry) = 0;
    virtual MAssertion*   MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry) = 0;
    virtual MAssertion*   NormalizeL(CPolicyRegistry* aRegistry) = 0;
    

};

#endif //MASSERTION_H


