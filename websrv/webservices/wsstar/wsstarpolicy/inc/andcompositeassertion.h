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







#ifndef C_AND_COMPOSITE_ASSERTION_H
#define C_AND_COMPOSITE_ASSERTION_H


#include "assertion.h"

class CAndCompositeAssertion : public CAssertion
{
public:
    static  CAndCompositeAssertion* NewL();
    static  CAndCompositeAssertion* NewLC();
    static  CAndCompositeAssertion* NewL(CAndCompositeAssertion* aValue);
    static  CAndCompositeAssertion* NewLC(CAndCompositeAssertion* aValue);
    
    virtual ~CAndCompositeAssertion();
//from CAssertion
    virtual  TAssertionType Type();
    virtual  void           AddTerm(MAssertion* aAssertion);
    virtual  void 	        AddTermCopyL(MAssertion* aAssertion);    
    virtual  MAssertion* 	NormalizeL(CPolicyRegistry* aRegistry);         
    virtual  MAssertion* IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
    virtual  MAssertion* 	MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
 
private:
     CAndCompositeAssertion();
     void ConstructL(); 
     void ConstructL(CAndCompositeAssertion* aValue); 
    
};
#endif //C_AND_COMPOSITE_ASSERTION_H              