/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef M_SEN_WS_POLICY_MANAGER_H
#define M_SEN_WS_POLICY_MANAGER_H

// INCLUDES
#include <e32base.h>

// FORWARD DECLARATIONS
class CSenWSDescription;
class CSenInternalWsPolicy;
class RSenWSPolicyPtr;
class CSenElement;

typedef RPointerArray<CSenInternalWsPolicy> RSenWSPolicyArray;
typedef RArray<RSenWSPolicyPtr> RSenPolicyPtrArray;

class MSenWsPolicyManager
    {
    public:


        virtual TInt AddWsPolicy1L(CSenWSDescription* aPattern, const TDesC8& aUri,
                                               const TDesC8& aPolicy, TInt& aErrorTo) = 0;

        virtual TInt RemoveWsPolicy1L( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo) = 0;
                                                                                                                                                                                               
        virtual TInt RegisterWsPolicy1L(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo) = 0;

        virtual TInt UnRegisterWsPolicy1L(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo) = 0;
                                                    
        virtual CSenElement*  WsPolicy1L( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo) = 0;

        virtual CSenElement*  WsPolicy1L( CSenWSDescription* aPattern, 
                                                TInt& aErrorTo) = 0;

        virtual CSenElement*  WsPolicy1L( const TDesC8& aProviderId, TInt& aErrorTo) = 0;

        virtual TInt AddPolicyConsumer1L(const TDesC8& aUri, const TDesC8& aConsumerId) = 0;

        virtual TInt RemovePolicyConsumer1L(const TDesC8& aUri, const TDesC8& aConsumerId) = 0;
        
                                                
    };

#endif // M_SEN_WS_POLICY_MANAGER_H

// End of File
