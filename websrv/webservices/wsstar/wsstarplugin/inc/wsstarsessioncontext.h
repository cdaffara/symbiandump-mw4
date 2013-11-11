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









 

#ifndef WSSTAR_HANDLER_SESSION_CONTEXT_H
#define WSSTAR_HANDLER_SESSION_CONTEXT_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "msensessioncontext.h"
#include "MSenServiceDescription.h"
#include "msenmessagecontext.h"
#include "msenremoteserviceconsumer.h"
#include "wsstarcons.h"
#include "wsstarmessagecontext.h"


// DATA TYPES
typedef RSenPointerMap<TDesC8, CSenWSDescription> RContextMapDescription;


// FORWARD DECLARATIONS
class MSenRemoteServiceSession;
class CSenWSDescription;
class CWSStarRSTResponse;
class CWSStarServiceSession;
class CWSStarPolicyHandler;

/**
 * Implement Session Context.       
 */
class CWSStarSessionContext : public CBase, public MSenSessionContext
    {

    public:
        static CWSStarSessionContext* NewL(CSenXmlReader* aParser, MSenServiceDescription* aSD, CWSStarPolicyHandler* aPolicyHandler = NULL);
        static CWSStarSessionContext* NewLC(CSenXmlReader* aParser, MSenServiceDescription* aSD, CWSStarPolicyHandler* aPolicyHandler = NULL);
        ~CWSStarSessionContext();

        /**
         * Sets token context keys based on RST response.
         *
         * @param aRSTResponse parsed information about token.
         * @param aEntropy Entropy if provided by our plugin.
         * @return KErrNone if no error, one of system wide error codes otherwise
         */
         TInt SetTokenKeysL(CWSStarRSTResponse& aRSTResponse, HBufC8* aClientEntropy, TBool aSignalConsumers,TBool aSCT = EFalse);

        //from SessionContext    
        virtual const CSenWSDescription* GetSession();
        virtual const CSenWSDescription* GetInitializer();
        
        virtual TInt                             Add(const TDesC8& aKey, CSenWSDescription* aValue);
        virtual TInt                             Update(const TDesC8& aKey, CSenWSDescription* aValue);
        virtual const CSenWSDescription* GetSenWSDescriptionL(const TDesC8& aKey);

        //in fact, now relation between session and sessionCtx is 1 to 1, so no need for map & keys    
        virtual TInt                            Add(const TDesC8& aKey, MSenRemoteServiceSession& aValue); // protected context    
        virtual TInt                            Update(const TDesC8& aKey, MSenRemoteServiceSession& aValue);
        virtual const MSenRemoteServiceSession* GetSenRemoteServiceSessionL(const TDesC8& aKey);    
        
        //from MContext    
        virtual SenContext::TContextType Type() const;        
        virtual SenContext::TContextDirection Direction() const;
        //desc - ownership transfered
        virtual TInt Add(const TDesC8& aKey, const TDesC8& aValue);
        virtual TInt Update(const TDesC8& aKey, const TDesC8& aValue);
        virtual const TDesC8* GetDesC8L(const TDesC8& aKey);
        virtual TInt Remove(const TDesC8& aKey);

        //Int handling methods doesnt take ownership. but create new object from passed value
        virtual TInt Add(const TDesC8& aKey, TInt aValue);
        virtual TInt Update(const TDesC8& aKey, TInt aValue);
        virtual const TInt* GetIntL(const TDesC8& aKey);
        virtual TInt Add(const TDesC8& aKey, CSenElement* aValue);
        virtual TInt Update(const TDesC8& aKey, CSenElement* aValue);
        virtual const CSenElement* GetSenElementL(const TDesC8& aKey) ;
        virtual TInt Add(const TDesC8& aKey, TAny* aValue);
        virtual TInt Update(const TDesC8& aKey, TAny* aValue) ;
        virtual TAny* GetAnyL(const TDesC8& aKey);	//codescannerwarnings
        virtual const CSenXmlReader* GetParser();
        virtual TInt Count() const;
        virtual TPtrC8 KeyAtL(TInt aIndex);
        virtual void Reset() ;
        
        inline virtual TInt SetDirection( SenContext::TContextDirection /* aDirection */ ) { return KErrNotSupported; } // not implemented

        // New methods
        
        /**
         * Update session context after incoming message
         *
         * @since S60 4.0
         */
        void UpdateFromMessageInContextL(CWSStarMessageContext& aMessageContext);
        
        /**
         * Update session context after outgoing message has been prepared, 
         *  to check which elements has been consumed.
         *
         * @since S60 4.0
         */
        void UpdateFromMessageOutContextL(CWSStarMessageContext& aMessageContext);
        
        /**
         * Share token. Copy SCT and MT into new star session
         *
         * @since S60 4.0
         */
        TInt ShareTokenWithL(CWSStarSessionContext* aWSStarSessionCtxDst, TBool& aMTwasReplaceBySCT);

        TInt UpdatePolicyL(CWSStarPolicyHandler* aPolicyHandler, MSenServiceDescription* aSD);
        
    private:
        void UpdateDesC8L(CWSStarMessageContext& aMessageContext, const TDesC8& aKey);
        
        
    protected:
        /**
         * Function:   CWSStarSessionContext
         *
         * Discussion: Perform the first phase of two phase construction
         */
        CWSStarSessionContext(CSenXmlReader* aParser);

        /**
         * Function:   ConstructL
         *
         * Discussion: Perform the second phase construction of a
         *             CImplementationClassPlus object.
         */
        void ConstructL(MSenServiceDescription* aSD, CWSStarPolicyHandler* aPolicyHandler = NULL);
        
    private:
        MSenRemoteServiceConsumer* iRemoteServiceConsumer;
        CSenSoapEnvelope*      iCurrentEnvelope;
        MSenRemoteServiceSession* iSession;//nbot owned, session keeped by DAO
        RContextMapDescription iDescriptions;
        RMapInt                iInts;
        RMapDescriptors         iDescs;
        CSenXmlReader* iParser;//not owned
        CSenElement* iBody;                 //owned, but handlers can consume it
        RPointerArray<HBufC8>*  ipSignedParts;           //owned array, Also elements
        TAny*                       ipConsumer; // not owned
    };

#endif // WSSTAR_HANDLER_SESSION_CONTEXT_H
    
// END OF FILE

