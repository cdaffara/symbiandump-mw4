/*
* Copyright (c) 2002-2006 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef M_SEN_MESSAGE_CONTEXT_H
#define M_SEN_MESSAGE_CONTEXT_H

// INCLUDES
#include <e32std.h>
#include <e32std.h>
#include "msencontext.h"
#include "MSenProperties.h"
#include "MSenMessage.h"


// CONSTANTS
namespace SenContext
    {
    _LIT8(KSenCurrentSoapMessageCtxKey,             "urn:com.nokia.serene:message:soap:current");
    _LIT8(KRemoteConsumer,                          "RemoteConsumer");
    }

/**
* This is the message context interface, which is visible to CSenMessageHandler ECOM plug-in implementations.
*/

// FORWARD DECLARATIONS
class CSenSoapEnvelope;
class CSenSoapMessage;
class CSenChunk;

// CLASS DECLARATION
class MSenMessageContext : public MSenContext
    {
    public:

        /**
        * Getter for the current message held by this message context.
        * @return pointer to MSenMessage, or NULL if message is not 
        * within this context at the moment.
        */
        virtual MSenMessage* MessageL() = 0;	//codescannerwarnigns

        /**
        * Sets the current message for this context, potentially freeing 
        * currently owned message (de-alloc) if such exists. 
        * @param apMessage provides pointer to new message. Note that
        * by default the ownership *is* passed to this context.
        * @return status code: 
        *   KErrArgument if argument is NULL.
        *   KErrNone on success.
        */
        virtual TInt SetMessage(MSenMessage* apMessage, TBool aOwned = ETrue) = 0;
        

        /**
        * Getter for chunk which representates of the main message,
        * held by this message context.
        * @return pointer to CSenChunk, or NULL if message is not 
        * within this context at the moment.
        */
        virtual CSenChunk* ChunkL() = 0;	//codescannerwarnings


        /**
        * Sets the current message (as chunk) for this context, potentially freeing 
        * currently owned message (de-alloc) if such exists. 
        * @param apMsgAsChunk provides pointer to chunk. Note that
        * by default the ownership *is* passed to this context.
        * @return status code: 
        *   KErrArgument if argument is NULL.
        *   KErrNone on success.
        */
        virtual TInt SetChunk(CSenChunk* apMsgAsChunk, TBool aOwned = ETrue) = 0;

        /*
        * Setter for message specific (transport) properties
        * @param apProperties is the pointer to properties.
        * Note that by default the ownership *is* passed to this context.
        * @return status code: 
        *   KErrArgument if argument is NULL.
        *   KErrNone on success.
        */
        virtual TInt SetProperties(MSenProperties* apProperties, TBool aOwned = ETrue) = 0;

        /*
        * Getter for message specific (transport) properties
        * @return transport properites as MSenProperties pointer, or NULL
        * if properties object does not exist.
        */
        virtual MSenProperties* Properties() = 0;


        virtual TInt Add(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned = ETrue) = 0;
        virtual TInt Update(const TDesC8& aKey, MSenMessage* apMessage, TBool aOwned = ETrue) = 0;
        virtual MSenMessage* GetMessageL(const TDesC8& aKey) = 0;	//codescannerwarnings


        // ---
        
        // Old, deprecated methods

        /*
        * Method returns "current" CSenSoapMessage*.
        * @return value, or NULL, if not found.
        * Ownership is not transferred, but remains to be held by this context.   
        * Caller is supposed to know the type (class) of returned value from 
        * the pre-defined binding to certain key.
        * @deprecated: use Message() getter instead!
        */
        virtual CSenSoapMessage* GetCurrentSoapMessage() = 0;  // DEPRECATED: remove this method; replace with MSenMessage variant


        
        /*
        * Method returns original message serialized into UCS8 by
        * service consumer (msg=request), or service (msg=response),
        * depending direction of this context.
        * @return serialized message (outgoing one from
        * service consumer, or incoming, original response
        * which transport has received from (remote) service.
        * - or KNullDesC8, if not set.
        * Ownership is NOT transferred, but remains to be held by this context.   
        * Caller is supposed to know the type (class) of returned
        * value from the pre-defined binding to certain key.
        */
        virtual TPtrC8 GetMessageL() = 0; // DEPRECATED: keep but rename this method 

        /*
        * Adds new CSenSoapEnvelope* type value into context, identified by given key.
        * @param aKey identifies new value
        * @param aValue is the value being added. Ownership is transferred
        * to this context.
        * @return KErrNone on success, 
        *         KErrAlreadyExists, if the key is reserved,
        *         or some other system-wide error code.
        */
        virtual TInt Add(const TDesC8& aKey, CSenSoapEnvelope* aValue) = 0; // DEPRECATED: remove this method; replace with MSenMessage variant

        /*
        * Updates new CSenSoapEnvelope* type value into context, identified by given key.
        * If some value for the same key already exists, it is destroyed and replaced
        * by the new value.
        * @param aKey identifies new value
        * @param aValue is the updated value, which may replace
        *         any existing value, which used same key.
        *         Ownership is transferred to this context.
        * @return KErrNone on success, 
        *         or some other system-wide error code.
        */
        virtual TInt Update(const TDesC8& aKey, CSenSoapEnvelope* aValue) = 0;  // DEPRECATED: remove this method; replace with MSenMessage variant

        /*
        * Method returns CSenSoapEnvelope* type value.
        * 
        * @param aKey identifies SOAP envelope object
        * 
        * @return value behind given key, or NULL, if not found.
        * Ownership is NOT transferred, but remains to be held by
        * this context.   
        * Method leaves with KErrSenTypeMismatch error code,
        * if value at this index is not CSenSoapEnvelope* type.   
        */
        virtual const CSenSoapEnvelope* GetSenSoapEnvelopeL(const TDesC8& aKey) = 0;  // DEPRECATED: remove this method; replace with MSenMessage variant
    };

#endif // M_SEN_MESSAGE_CONTEXT_H

// End of File
