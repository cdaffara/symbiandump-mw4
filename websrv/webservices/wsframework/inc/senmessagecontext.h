/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Note that this class is ment for WS internal components,
*                and not ment to act as part of public API
*
*/








#ifndef SEN_MESSAGE_CONTEXT_H
#define SEN_MESSAGE_CONTEXT_H


// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "sencontext.h"
#include "senpointermap.h"
#include "msenmessagecontext.h"

// FORWARD DECLARATIONS
class CSenContextItem;
class MSenProperties;
class CSenXmlReader;
class MSenRemoteServiceConsumer;
class CSenChunk;


// CLASS DECLARATION
class CSenMessageContext : public  CSenContext, public MSenMessageContext
    {
    public:
        /**
        * Standard 2-phased constructor
        * @param aDirection indicate the initial direction of this message context.
        * For requests, it is "outbound", and for responses or one way notifications
        * from some (remote) service) it is "inbound".
        * @param apOwnedTp - pointer to (transport) properties, which ownership is taken.
        * These properties are typically a clone to the propeties currently owned
        * by transport plug-in (ECOM) instance. Cloning allows independency upon
        * changes that last during the life-time of a message transaction only.
        * Note that if apOwnedTp is NULL, this constructor will leave with KErrArgument.
        * @return pointer to new message context
        */ 
        IMPORT_C static  CSenMessageContext* NewL(SenContext::TContextDirection aDirection, 
                                                  MSenProperties* apOwnedTp);
        /**
        * Standard 2-phased constructor
        * @param aDirection indicate the initial direction of this message context.
        * For requests, it is "outbound", and for responses or one way notifications
        * from some (remote) service) it is "inbound".
        * @param apOwnedTp - pointer to transport properties, which ownership is taken.
        * These properties are typically a clone to the propeties currently owned
        * by transport plug-in (ECOM) instance. Cloning allows independency upon
        * changes that last during the life-time of a message transaction only.
        * Note that if apOwnedTp is NULL, this constructor will leave with KErrArgument.
        */ 
        IMPORT_C static  CSenMessageContext* NewLC(SenContext::TContextDirection aDirection,
                                                   MSenProperties* apOwnedTp);

        // Destructor:
        IMPORT_C virtual ~CSenMessageContext();
        
        // New methods:
        
        // From MSenMessageContext:
        virtual MSenMessage* MessageL();	//codescannerwarnigs
        virtual TInt SetMessage(MSenMessage* apMessage, TBool aOwned = ETrue);
        
        virtual CSenChunk* ChunkL();	//codescannerwarnings
        virtual TInt SetChunk(CSenChunk* apMsgAsChunk, TBool aOwned = ETrue);        

        virtual TInt SetProperties(MSenProperties* apProperties, TBool aOwned = ETrue);
        virtual MSenProperties* Properties();
        
        virtual TInt Add(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned = ETrue);
        virtual TInt Update(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned = ETrue);
        MSenMessage* GetMessageL(const TDesC8& aKey);	//codescannerwarnings
       
        
        // --> deprecated, remove from MSenMessageContext: <--
        virtual CSenSoapMessage* GetCurrentSoapMessage();
        virtual TPtrC8 GetMessageL();
        
        virtual TInt Add(const TDesC8& aKey, CSenSoapEnvelope* aValue);
        virtual TInt Update(const TDesC8& aKey, CSenSoapEnvelope* aValue);
        virtual const CSenSoapEnvelope* GetSenSoapEnvelopeL(const TDesC8& aKey);
        


        // From MSenContext:
        virtual SenContext::TContextType Type() const;        
    	virtual SenContext::TContextDirection Direction() const;
        virtual TInt Add(const TDesC8& aKey, const TDesC8& aValue);
        virtual TInt Update(const TDesC8& aKey, const TDesC8& aValue);
        virtual const TDesC8* GetDesC8L(const TDesC8& aKey);
        virtual TInt Add(const TDesC8& aKey, TInt aValue);
        virtual TInt Update(const TDesC8& aKey, TInt aValue);
        virtual const TInt* GetIntL(const TDesC8& aKey);
        virtual TInt Add(const TDesC8& aKey, CSenElement* aValue);
        virtual TInt Update(const TDesC8& aKey, CSenElement* aValue);
        virtual const CSenElement* GetSenElementL(const TDesC8& aKey);
        virtual TInt Add(const TDesC8& aKey, TAny* aValue);
        virtual TInt Update(const TDesC8& aKey, TAny* aValue);
        virtual TAny* GetAnyL(const TDesC8& aKey);  //codescannerwarnings
        virtual TInt Remove(const TDesC8& aKey);     	
        virtual TInt Count() const;
        virtual TPtrC8 KeyAtL(TInt aIndex);
        virtual void Reset();
        virtual const CSenXmlReader* GetParser();

        virtual TInt SetDirection( SenContext::TContextDirection aDirection );
        
    protected:
        IMPORT_C  CSenMessageContext(SenContext::TContextDirection aDirection,
                                     MSenProperties* apOwnedTp);
                                     
        IMPORT_C void BaseConstructL();

    private: 
        // Helper method(s):
        CSenContextItem* MessageToItem(MSenMessage* apMessage, TBool aOwned, TInt& aError);
        
        // Data: 
        MSenProperties* ipOwnedTp; // owned; delallocated via IsSafeToCast()
    };

#endif // SEN_MESSAGE_CONTEXT_H

// End of File
