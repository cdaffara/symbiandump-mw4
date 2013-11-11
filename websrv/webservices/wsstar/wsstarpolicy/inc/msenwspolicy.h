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








#ifndef M_SEN_WSPOLICY_H
#define M_SEN_WSPOLICY_H

#include <e32std.h>

class CWSPolicy;
class CPolicyRegistry;
class CSenElement;

 enum TWSPolicyType
 {
    EInvalidType = 0,
    EWsPolicyType = 1,
    EWsWsdlPolicyType = 2,     
 };

class MSenWsPolicy
{

public: 
    virtual CWSPolicy* 	    GetPolicyDoc() = 0;
    virtual CSenElement*    GetPolicyDoc() = 0;
    virtual TBool 	        IsEmpty() = 0;
    virtual TBool 	        IsNormalized() = 0;
    virtual TBool 	        Remove() = 0;
    virtual TInt 	        Size() = 0;
    virtual TWSPolicyType  Type() = 0;
};

#endif //M_SEN_WSPOLICY_H


