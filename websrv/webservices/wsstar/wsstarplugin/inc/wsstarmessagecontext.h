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









 

#ifndef WSSTAR_HANDLER_MESSAGE_CONTEXT_H
#define WSSTAR_HANDLER_MESSAGE_CONTEXT_H

// INCLUDES
#include <e32std.h>
#include <flogger.h>
#include "msenmessagecontext.h"
#include "wsstarcons.h"
#include "MSenProperties.h"


// FORWARD DECLARATIONS
class CWSStarSessionContext;
class CSenChunk;

/**
 * Class:       
 */
class CWSStarMessageContext : public CBase, public MSenMessageContext
    {

    public:
        static CWSStarMessageContext* NewL(SenContext::TContextDirection aDirection, CSenXmlReader*& aParser);
        static CWSStarMessageContext* NewLC(SenContext::TContextDirection aDirection, CSenXmlReader*& aParser);
        static CWSStarMessageContext* NewLC(SenContext::TContextDirection aDirection,
                CSenXmlReader*& aParser, CWSStarSessionContext& aSessionContext);
        ~CWSStarMessageContext();


        void SetOwnedEnvelope(TBool aOwned = ETrue) 
            {
            iOwnedEnvelope = aOwned;
            }
        
        //From MessageContext
        virtual CSenSoapMessage* GetCurrentSoapMessage();
        virtual TPtrC8 GetMessageL();



        //Implementation in these is in fact is equal to GetCurrentSoapMessae. 
        //In current shape, message context keep only one CSenSoapMessage
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
        virtual TAny* GetAnyL(const TDesC8& aKey);	//codescannerwarnings
        virtual const CSenXmlReader* GetParser();
        virtual TInt Count() const;
        virtual TPtrC8 KeyAtL(TInt aIndex);
        virtual void Reset();
        virtual MSenMessage* MessageL();	//codescannerwarnigs
        virtual TInt SetMessage(MSenMessage* apMessage, TBool aOwned = ETrue);
        virtual TInt SetProperties(MSenProperties* apOwnedTransportProperties, TBool aOwned = ETrue);
        virtual MSenProperties* Properties();
        virtual TInt Add(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned = ETrue);
        virtual TInt Update(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned = ETrue);
        virtual MSenMessage* GetMessageL(const TDesC8& aKey);// codescannerwarnings
        virtual CSenChunk* CWSStarMessageContext::ChunkL(); // codescannerwarnings
        virtual TInt SetChunk(CSenChunk* apMsgAsChunk, TBool aOwned = ETrue); // not implemented
        virtual TInt SetDirection(SenContext::TContextDirection aDirection); // not implemented
        
        
        


    //new
        /**
         * Update message context from session context.
         *  If resending occurs we need copy again already cached info.
         *
         * @since S60 4.0
         */
        void UpdateFromSessionContextL(CWSStarSessionContext& aSessionContext);

    private:
        void UpdateIntL(CWSStarSessionContext& aSessionContext, const TDesC8& aKey);
        void UpdateDesC8L(CWSStarSessionContext& aSessionContext, const TDesC8& aKey);

    protected:
        /**
         * Function:   CWSStarMessageContext
         *
         * Discussion: Perform the first phase of two phase construction
         */
        CWSStarMessageContext(SenContext::TContextDirection aDirection, CSenXmlReader*& aParser);
        
        

        /**
         * Function:   ConstructL
         *
         * Discussion: Perform the second phase construction of a
         *             CImplementationClassPlus object.
         */
        void ConstructL();
        void ConstructL(CWSStarSessionContext& aSessionContext);
        
    private:
        SenContext::TContextDirection iDirection;
        TBool iOwnedEnvelope;
        CSenSoapEnvelope* iCurrentEnvelope; //depends on iOwnedEnvelope
        TPtrC8            iCurrentEnvelopeTxt; //depends on iOwnedEnvelope
        const CSenElement* iBody;                 //not owned, session Ctx own
    	const CSenXmlReader* iParser;
        RMapDescriptors iDescs;
        RMapInt         iInts;
        RPointerArray<HBufC8>*  ipSignedParts;           //not owned (both elements and array)
    };


#endif // WSSTAR_HANDLER_MESSAGE_CONTEXT_H


    
// END OF FILE

