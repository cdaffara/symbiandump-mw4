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







#ifndef C_POLICY_ASSERTION_H
#define C_POLICY_ASSERTION_H
#include "assertion.h"

class CPolicyAssertion : public CAssertion
{
public:
    static  CPolicyAssertion* NewL();
    static  CPolicyAssertion* NewLC();

    static  CPolicyAssertion* NewL(CPolicyAssertion* aValue);
    static  CPolicyAssertion* NewLC(CPolicyAssertion* aValue);
    
    virtual ~CPolicyAssertion();
//from CAssertion
    virtual  TAssertionType Type();
    virtual  MAssertion* 	NormalizeL();
    virtual  MAssertion* 	NormalizeL(CPolicyRegistry* aRegistry);         
    virtual  MAssertion* IntersectL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
    virtual  MAssertion* 	MergeL(MAssertion* aAssertion, CPolicyRegistry* aRegistry);
 
public:          
    void SetNameL(TDesC8& aName);          
    void SetIdL(TDesC8& aId);
    TPtrC8 	Name();     
    TPtrC8 	Id();
    TPtrC8 PolicyURI();
          
private:
     CPolicyAssertion();
     void ConstructL();
     void ConstructL(CPolicyAssertion* aValue);
private:
     HBufC8* iName;
     HBufC8* iId;
     HBufC8* iMetadataEndpoint;
 
    
};
#endif //C_POLICY_ASSERTION_H