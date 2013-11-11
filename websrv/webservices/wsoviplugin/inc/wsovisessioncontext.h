/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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






 

#ifndef WSOvi_HANDLER_SESSION_CONTEXT_H
#define WSOvi_HANDLER_SESSION_CONTEXT_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "msensessioncontext.h"
#include "MSenServiceDescription.h"
#include "msenmessagecontext.h"
#include "msenremoteserviceconsumer.h"
#include "wsovicons.h"
#include "wsovimessagecontext.h"


// DATA TYPES
typedef RSenPointerMap<TDesC8, CSenWSDescription> RContextMapDescription;


// FORWARD DECLARATIONS
class MSenRemoteServiceSession;
class CSenWSDescription;
class CWSOviServiceSession;

/**
 * Implement Session Context.       
 */
class CWSOviSessionContext : public CBase, public MSenSessionContext
    {

    public:
        static CWSOviSessionContext* NewL();
        static CWSOviSessionContext* NewLC();
        ~CWSOviSessionContext();

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
        virtual TAny* GetAnyL(const TDesC8& aKey);
        virtual const CSenXmlReader* GetParser();
        virtual TInt Count() const;
        virtual TPtrC8 KeyAtL(TInt aIndex);
        virtual void Reset() ;
        
        inline virtual TInt SetDirection( SenContext::TContextDirection /* aDirection */ ) { return KErrNotSupported; } // not implemented

        // New methods
        TInt SetTokenKeysL(const TDesC8& aToken);
        /**
         * Update session context after incoming message
         *
         * @since S60 5.0
         */
        void UpdateFromMessageInContextL(CWSOviMessageContext& aMessageContext);
        
        /**
         * Update session context after outgoing message has been prepared, 
         *  to check which elements has been consumed.
         *
         * @since S60 5.0
         */
        void UpdateFromMessageOutContextL(CWSOviMessageContext& aMessageContext);
        
        TInt ShareTokenWithL(CWSOviSessionContext* aWSOviSessionCtxDst);        
    private:
        void UpdateDesC8L(CWSOviMessageContext& aMessageContext, const TDesC8& aKey);
        
        
    protected:
        /**
         * Function:   CWSOviSessionContext
         *
         * Discussion: Perform the first phase of two phase construction
         */
        CWSOviSessionContext();

        /**
         * Function:   ConstructL
         *
         * Discussion: Perform the second phase construction of a
         *             CImplementationClassPlus object.
         */
        void ConstructL();
        
    private:
        MSenRemoteServiceConsumer* iRemoteServiceConsumer;
        MSenRemoteServiceSession* iSession;//nbot owned, session keeped by DAO
        RContextMapDescription iDescriptions;
        RMapInt                iInts;
        RMapDescriptors         iDescs;
        TAny*                       ipConsumer; // not owned
        TAny*                       iTP; // not owned
    };

#endif // WSOvi_HANDLER_SESSION_CONTEXT_H
    
// END OF FILE

