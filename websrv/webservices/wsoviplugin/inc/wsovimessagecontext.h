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
* Description:    Header declaration
*
*/









 

#ifndef WSOvi_HANDLER_MESSAGE_CONTEXT_H
#define WSOvi_HANDLER_MESSAGE_CONTEXT_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "msenmessagecontext.h"
#include "wsovicons.h"
#include <SenTransportProperties.h>


// FORWARD DECLARATIONS
class CWSOviSessionContext;
class CSenChunk;

/**
 * Class:       
 */
class CWSOviMessageContext : public CBase, public MSenMessageContext
    {

    public:
        static CWSOviMessageContext* NewL(SenContext::TContextDirection aDirection);
        static CWSOviMessageContext* NewLC(SenContext::TContextDirection aDirection);
        static CWSOviMessageContext* NewLC(SenContext::TContextDirection aDirection,
                CWSOviSessionContext& aSessionContext);
        ~CWSOviMessageContext();


        
        //From MessageContext
        virtual CSenSoapMessage* GetCurrentSoapMessage();
        virtual TPtrC8 GetMessageL();
        virtual TInt Add(const TDesC8& aKey, CSenSoapEnvelope* aValue);
        virtual TInt Update(const TDesC8& aKey, CSenSoapEnvelope* aValue);
        virtual const CSenSoapEnvelope* GetSenSoapEnvelopeL(const TDesC8& aKey);
            
            
        //From MContext    
        virtual SenContext::TContextType Type() const;        
        virtual SenContext::TContextDirection Direction() const;

        //Desc handling methods do not take ownership. but create new object from passed value
        virtual TInt Add(const TDesC8& aKey, const TDesC8& aValue);
        virtual TInt Update(const TDesC8& aKey, const TDesC8& aValue);
        virtual const TDesC8* GetDesC8L(const TDesC8& aKey);
        virtual TInt Remove(const TDesC8& aKey);

        //Int handling methods do not take ownership. but create new object from passed value
        virtual TInt Add(const TDesC8& aKey, TInt aValue);
        virtual TInt Update(const TDesC8& aKey, TInt aValue);
        virtual const TInt* GetIntL(const TDesC8& aKey);


        //Element handling methods - ownership is transfered
        virtual TInt Add(const TDesC8& aKey, CSenElement* aValue);
        virtual TInt Update(const TDesC8& aKey, CSenElement* aValue);
        virtual const CSenElement* GetSenElementL(const TDesC8& aKey) ;

        //These are NOT supported:
        virtual TInt Add(const TDesC8& aKey, TAny* aValue);
        virtual TInt Update(const TDesC8& aKey, TAny* aValue) ;
        virtual TAny* GetAnyL(const TDesC8& aKey);
        virtual const CSenXmlReader* GetParser();
        virtual TInt Count() const;
        virtual TPtrC8 KeyAtL(TInt aIndex);
        virtual void Reset();
        virtual MSenMessage* MessageL();
        virtual TInt SetMessage(MSenMessage* apMessage, TBool aOwned = ETrue);
        virtual TInt SetProperties(MSenProperties* apOwnedTransportProperties, TBool aOwned = ETrue);
        virtual MSenProperties* Properties();
        virtual TInt Add(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned = ETrue);
        virtual TInt Update(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned = ETrue);
        virtual MSenMessage* GetMessageL(const TDesC8& aKey);
        virtual CSenChunk* CWSOviMessageContext::ChunkL();
        virtual TInt SetChunk(CSenChunk* apMsgAsChunk, TBool aOwned = ETrue);
        virtual TInt SetDirection(SenContext::TContextDirection aDirection);
        
        
        

        /**
         * Update message context from session context.
         *  If resending occurs we need copy again already cached info.
         *
         * @since S60 5.0
         */
        void UpdateFromSessionContextL(CWSOviSessionContext& aSessionContext);

        
        /**
        *   no onwership taken
        *   no copy is maked
        */
        TInt UpdateTP(CSenTransportProperties*& aTP);
        
        
        /**
        *   no onwership returned
        *   no copy is maked
        */
        
        CSenTransportProperties* TP();
        
        /**
        *   no onwership returned
        *   no copy is maked
        */
        RMapDescriptors& OAuthProperies();
        
    protected:
        /**
         * Function:   CWSOviMessageContext
         *
         * Discussion: Perform the first phase of two phase construction
         */
        CWSOviMessageContext(SenContext::TContextDirection aDirection);

        /**
         * Function:   ConstructL
         *
         * Discussion: Perform the second phase construction of a
         *             CImplementationClassPlus object.
         */
        void ConstructL();
        void ConstructL(CWSOviSessionContext& aSessionContext);
        
    private:
        void UpdateIntL(CWSOviSessionContext& aSessionContext, const TDesC8& aKey);
        void UpdateDesC8L(CWSOviSessionContext& aSessionContext, const TDesC8& aKey);

        SenContext::TContextDirection iDirection;
        RMapDescriptors iDescs;
        RMapDescriptors iOAuthProperties;
        RMapInt         iInts;
        CSenTransportProperties* iTP;//not owned
    };


#endif // WSOvi_HANDLER_MESSAGE_CONTEXT_H


    
// END OF FILE

