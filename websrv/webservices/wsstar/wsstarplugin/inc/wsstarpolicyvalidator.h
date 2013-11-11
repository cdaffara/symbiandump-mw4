/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef WSSTAR_POLICY_VALIDATER_H
#define WSSTAR_POLICY_VALIDATER_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>

typedef RSenPointerMap<TDesC8, TDesC8> RPolicyRequestMap2;

/**
 * Class:       
 *
 * Description: An implementation of the Validation of policy from ServicePolicy
 * differenet methods validates Clinet and Service poplicy
 */
class CWSStarPolicyValidator : public CBase
    {
    public:
        static CWSStarPolicyValidator* NewL(RFileLogger& aLogger);
        static CWSStarPolicyValidator* NewLC(RFileLogger& aLogger);
        ~CWSStarPolicyValidator();

        TInt ValidatePolicyArrayL(CSenElement* aPolicyArray, CPolicyRegistry* aRegistry, RPolicyRequestMap2& aRequestMap);
        TInt ValidateClientPolicyL(CSenElement* aClientPolicy, CPolicyRegistry* aRegistry, RPolicyRequestMap2& aRequestMap);
        TPtrC8 PolicyNameL(CSenElement* aPolicy);
        RFileLogger* Log() const;
        	
    private:       
        
        TPtrC8 PolicyIdL(CSenElement* aPolicy);
        TInt IsUriAlreadyExist(TPtrC8 aUriName, RPolicyRequestMap& aRequestMap);
        TBool ValidateExpiryL(CSenElement* aExpiry);
        CSenElement* GetExpiryTagL();
        TBool IsExpiredPolicyL(CSenElement* aExpiry);
        
        CWSStarPolicyValidator();
        void ConstructL(RFileLogger& aLogger);
        RFileLogger* iLog;
    }; 
#endif // WSSTAR_POLICY_HANDLER_H

