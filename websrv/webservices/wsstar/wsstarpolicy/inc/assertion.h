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








#ifndef C_ASSERTION_H
#define C_ASSERTION_H

#include <e32base.h>
#include <s32strm.h>
#include "massertion.h"


class CWSPolicyWriter;

class CAssertion: public  CBase, public MAssertion 
{ 

public:
    static CAssertion* NewL();
    static CAssertion* NewLC();
    virtual ~CAssertion();
public:


    virtual void 	AddTerm(MAssertion* aAssertion);
    virtual void 	AddTerms(RPolicyTerms& aAssertions);
    virtual void 	AddTermCopyL(MAssertion* aAssertion);
    virtual void 	AddTermsCopyL(RPolicyTerms& aAssertions);

    virtual void 	        MoveTermL(RPolicyTerms& aAssertions);
    virtual void 	        MoveTermsL(RPolicyTerms& aAssertions);


    virtual  MAssertion* 	GetParent();
    virtual RPolicyTerms& 	GetTerms();
    virtual  TBool 	HasParent();
    virtual  MAssertion* 	IntersectL(MAssertion* aAssertion);
    virtual  TBool 	IsEmpty();
    virtual  TBool 	IsNormalized();
    virtual  MAssertion* 	MergeL(MAssertion* aAssertion);
    virtual  MAssertion* 	NormalizeL();
    virtual TInt 	Remove(MAssertion* aAssertion);
    virtual  void 	SetNormalized(TBool aFlag);
    virtual  void 	SetParent(MAssertion* aParent);
    virtual  TAssertionType Type();


    virtual  MAssertion*   IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
    virtual  MAssertion*   MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
    virtual  MAssertion*   NormalizeL(CPolicyRegistry* aRegistry);

    TInt 	Size();    
    CAssertion();

    static TInt DeleteAssertion(MAssertion* aAssertion);    

private:
    
    void ConstructL();
    
private:
    RPolicyTerms iChildterms;
    MAssertion* iParent;
    TBool iNormalized;
    
//    RFileLogger iLog;
    
};

#endif //C_ASSERTION_H