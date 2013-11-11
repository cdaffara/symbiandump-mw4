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
* Description:    Header declaration
*
*/











#ifndef WSSTAR_POLICY_MANAGER_H
#define WSSTAR_POLICY_MANAGER_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "sensessionhandler.h"
#include "wsstarhandlercontext.h"
#include "wsstarsessioncontext.h"
#include "wsstartrustclient.h"
#include "wsstarcons.h"
#include "wsstarservicesession.h"
#include "wspolicyregistry.h"
#include "sendebug.h"
#include "msenwspolicymanager.h"
#include "senwspolicymanager.h"
class CWSStarPolicyHandler;
class CWSStarDictionary;
class CPolicyRequest;
class CPolicyReq;
class CPolicyReqMap;
class CSenWSPolicyManager;

typedef RPointerArray<CPolicyReqMap> RPolicyRequestMap;
/**
 * Class:       
 *
 * Description: An implementation of the CWSStarHandler definition. This is concrete
 * class, instance of which ECOM framework gives to ECOM clients.
 */
class CWSStarPolicyManager : public CBase, public MSenWsPolicyManager
    {

    public:

        static CWSStarPolicyManager* NewL(TAny* aHandlerCtx);

        TInt InvokeL(const TDesC8& aContract);
        ~CWSStarPolicyManager();
        CSenElement* PolicyL(MSenServiceDescription* aSD);	//codescannerwarnings
        CSenElement* PolicyL(const TDesC8& aUri);	//codescannerwarnings
        TInt DownloadPolicyL( CSenWSDescription* apSD );
        TInt UpdatePolicyL(CWSStarPolicyHandler* aPolicyHandler);
        TInt UnRegisterWsPolicyL(MSenServiceDescription& aServiceDescription, TInt& aErrorTo);
        TInt PolicyConsumerAddedL(CSenServiceSession* aNewSession, MSenRemoteServiceConsumer& aRemoteConsumer);        
        
        RFileLogger* Log() const;
		void DataTrafficDetails(TSenDataTrafficDetails& aDetails); 		      
    private:
        /**
         * Function:   CWSStarPolicyManager
         *
         * Discussion: Perform the first phase of two phase construction
         */
        CWSStarPolicyManager(MSenHandlerContext& aCtx);
        
        /**
         * Function:   ConstructL
         *
         * Discussion: Perform the second phase construction
         *
         */
        void ConstructL();
        TInt GetHttpPolicyL( const TDesC8& aEndpoint, RExpiredPolicyArray& aRequestMap, CSenWSDescription* apSD );
        TInt IsContractExist(const TDesC8& aContract);
//new methods        
        TInt RegisterWsPolicyL(const TDesC8& aContract);        
        

        virtual TInt AddWsPolicy1L(CSenWSDescription* aPattern, const TDesC8& aUri,
                                               const TDesC8& aPolicy, TInt& aErrorTo);

        virtual TInt RemoveWsPolicy1L( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo);
                                                                                                                                                                                               
        virtual TInt RegisterWsPolicy1L(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo);

        virtual TInt UnRegisterWsPolicy1L(CSenWSDescription* aPattern,
                                                    TInt& aErrorTo);
                                                    
        virtual CSenElement*  WsPolicy1L( CSenWSDescription* aPattern, 
                                                const TDesC8& aUri, TInt& aErrorTo);

        virtual CSenElement*  WsPolicy1L( CSenWSDescription* aPattern, 
                                                TInt& aErrorTo);                    
        
        virtual CSenElement*  WsPolicy1L( const TDesC8& aProviderId, 
                                            TInt& aErrorTo);

        virtual TInt AddPolicyConsumer1L(const TDesC8& aUri, const TDesC8& aConsumerId);

        virtual TInt RemovePolicyConsumer1L(const TDesC8& aUri, const TDesC8& aConsumerId);
            
        TInt GetPolicyFromFileL( RExpiredPolicyArray& aRequestMap );
        
    private:
    
        MSenHandlerContext& iSenHandlerContext; //Not owned
        CPolicyRegistry* iRegistry;
        CWSStarDictionary* iDictionary;
        CSenWSPolicyManager* iPolicyManager;
        RPointerArray<HBufC8> iContracts;
        TSenDataTrafficDetails iPolicyDetails; 
        
    };

#endif // WSSTAR_POLICY_MANAGER_H

