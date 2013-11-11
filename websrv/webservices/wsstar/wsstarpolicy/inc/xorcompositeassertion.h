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







#ifndef C_XOR_COMPOSITE_ASSERTION_H
#define C_XOR_COMPOSITE_ASSERTION_H


#include "assertion.h"

class CXorCompositeAssertion : public CAssertion
{
public:
    static  CXorCompositeAssertion* NewL();
    static  CXorCompositeAssertion* NewLC();
    static  CXorCompositeAssertion* NewL(CXorCompositeAssertion* aValue);
    static  CXorCompositeAssertion* NewLC(CXorCompositeAssertion* aValue);
    
    virtual ~CXorCompositeAssertion();
//from CAssertion
    virtual  TAssertionType Type();
    virtual  void           AddTerm(MAssertion* aAssertion);
    virtual void 	AddTermCopyL(MAssertion* aAssertion);
    
    virtual  MAssertion* 	NormalizeL(CPolicyRegistry* aRegistry);         
    virtual  MAssertion*    IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
    virtual  MAssertion* 	MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
 
private:
     CXorCompositeAssertion();
     void ConstructL(CXorCompositeAssertion* aValue); 
     void ConstructL(); 
    
};
#endif //C_XOR_COMPOSITE_ASSERTION_H          