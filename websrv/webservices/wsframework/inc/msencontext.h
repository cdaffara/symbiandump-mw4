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








#ifndef M_SEN_CONTEXT_H
#define M_SEN_CONTEXT_H


// INCLUDES
#include <e32std.h>

// FORWARD DECLARATIONS
class CSenElement;
class CSenXmlReader;

// CONSTANTS
namespace SenContext
    {
    // COMMON CONTEXT KEYS
    _LIT8(KSenCtxKeyCoreServiceManager,     "urn:com.nokia.serene:core:service-manager");
    _LIT8(KSenCtxKeySessionInitializer,     "urn:com.nokia.serene:framework:session-initializer");    
    _LIT8(KSenCtxKeyServiceSession,         "urn:com.nokia.serene:framework:service-session");        
    _LIT8(KSenCtxKeyTransportProperties,    "urn:com.nokia.serene:framework:transport-properties");   
    
    _LIT8(KSenCtxKeyMessage,                "urn:com.nokia.serene:framework:message"); 
    _LIT8(KSenCtxKeyChunk,                  "urn:com.nokia.serene:framework:chunk"); 
    _LIT8(KSenCtxKeyMessageProperties,      "urn:com.nokia.serene:framework:message:properties"); 
//    _LIT8(KSenCtxKeyXmlParser,              "urn:com.nokia.serene:framework:xml-parser"); 


    // DEPRECATED KEYS,
    _LIT8(KSenCtxKeyCurrentSoapMessage,  "urn:com.nokia.serene:framework:current-soap-msg"); 
    _LIT8(KSenCtxKeyCurrentMessageAsDesc,"urn:com.nokia.serene:framework:current-msg-desc");
    
        
    enum TContextType
        {
        EContextBase = 0,
        EHandlerContext, 
        ESessionContext, 
        EMessageContext, 
        ETransportContext
        };

    enum TContextDirection
        {
        EOutgoing = 1, 
        EIncoming,
        EBoth
        };
    }


// CLASS DECLARATION
/**
* This is the abstract super class for all WSF context classes, providing 
* convenience methods for basic, excessively used data types, and some
* foundation methods for general context properties, like context type and
* direction.
*
*/
class MSenContext
    {
    public:
    
        // New functions
        
        /**
        * Method is used to resolve concrete type (class)
        * for this context.
        * 
        * @return the class type of this handler
        */
        virtual SenContext::TContextType Type() const = 0;        
    
    	/**
    	* Indicates the direction this context is serving.
        * 
    	* @return enum, which tells where this handler will 
    	* be applied - outgoing, incoming, or on both directions.
    	*/
    	virtual SenContext::TContextDirection Direction() const = 0;
    	
        /**
        * Adds new const TDesC8& type value into context, identified by given key.
        * 
        * @param aKey identifies new value
        * @param aValue is the value being added. 
        * 
        * @return KErrNone on success, 
        *         KErrAlreadyExists, if the key is reserved,
        *         or some other system-wide error code.
        */
        virtual TInt Add(const TDesC8& aKey, const TDesC8& aValue) = 0;

        /**
        * Updates new const TDesC8& type value into context, identified by given key.
        * If some value for the same key already exists, it is destroyed and replaced 
        * by the new value. If key does not already exist, a new one will be added.
        * 
        * @param aKey identifies new value
        * @param aValue is the updated value, which may replace
        *         any existing value, which used same key.
        *         Ownership is transferred to this context.
        * 
        * @return KErrNone on success, 
        *         or some other system-wide error code.
        */
        virtual TInt Update(const TDesC8& aKey, const TDesC8& aValue) = 0;

        /**
        * Method returns TDesC8* type value.
        * 
        * @param aKey identifies descriptor object
        * 
        * @return value behind given key, or NULL, if
        * not found.
        * Ownership is  not transferred, but remains
        * to be held by this context.   
        * NOTE: Method leaves with KErrSenTypeMismatch error code,
        * if value behind this key of UCS8 descriptor type.   
        */
        virtual const TDesC8* GetDesC8L(const TDesC8& aKey) = 0;

        /**
        * Adds new TInt type value into context, identified by given key.
        * 
        * @param aKey identifies new value
        * @param aValue is the value being added.
        * @return KErrNone on success, 
        *         KErrAlreadyExists, if the key is reserved,
        *         or some other system-wide error code.
        */
        virtual TInt Add(const TDesC8& aKey, TInt aValue) = 0;

        /**
        * Updates new TInt type value into context, identified by given key.
        * If some value for the same key already exists, it is destroyed and
        * replaced by the new value.
        * 
        * @param aKey identifies new value
        * @param aValue is the updated value, which may replace
        *         any existing value, which used same key.
        * 
        * @return KErrNone on success, 
        *         or some other system-wide error code.
        */
        virtual TInt Update(const TDesC8& aKey, TInt aValue) = 0;

        /**
        * Method returns TInt* type value.
        * 
        * @param aKey identifies integer object
        * 
        * @return value behind given key, or NULL, if
        * not found.
        * Ownership is  not transferred, but remains
        * to be held by this context.   
        * Method leaves with KErrSenTypeMismatch error code,
        * if value behind this descriptor is not TInt* type.   
        */
        virtual const TInt* GetIntL(const TDesC8& aKey) = 0;

        /**
        * Adds new CSenElement* type value into context, identified by given key.
        * 
        * @param aKey identifies new value
        * @param aValue is the value being added. Ownership is transferred
        * to this context.
        * 
        * @return KErrNone on success, 
        *         KErrAlreadyExists, if the key is reserved,
        *         or some other system-wide error code.
        */
        virtual TInt Add(const TDesC8& aKey, CSenElement* aValue) = 0;

        /**
        * Updates new CSenElement* type value into context, identified by given key.
        * If some value for the same key already exists, it is destroyed and replaced
        * by the new value.
        * 
        * @param aKey identifies new value
        * @param aValue is the updated value, which may replace
        *         any existing value, which used same key.
        *         Ownership is transferred to this context.
        * 
        * @return KErrNone on success, 
        *         or some other system-wide error code.
        */
        virtual TInt Update(const TDesC8& aKey, CSenElement* aValue) = 0;

        /**
        * Method returns CSenElement* type value.
        * 
        * @param aKey identifies the XML element object
        * 
        * @return value behind given key, or NULL, if
        * not found.
        * Ownership is  not transferred, but remains
        * to be held by this context.   
        * Method leaves with KErrSenTypeMismatch error code,
        * if value behind this descriptor is not CSenElement* type.   
        */
        virtual const CSenElement* GetSenElementL(const TDesC8& aKey) = 0;

        /**
        * Adds new TAny* type value into context, identified by given key.
        * 
        * @param aKey identifies new value
        * @param aValue is the value being added. Ownership is NOT transferred
        * to this context.
        * 
        * @return KErrNone on success, 
        *         KErrAlreadyExists, if the key is reserved,
        *         or some other system-wide error code.
        */
        virtual TInt Add(const TDesC8& aKey, TAny* aValue) = 0;

        /**
        * Updates new TAny* type value into context, identified by given key.
        * If some value for the same key already exists, it is replaced with 
        * the new value.
        * 
        * @param aKey identifies new value
        * @param aValue is the updated value, which may replace
        *         any existing value, which used same key.
        *         Ownership is NOT transferred this context.
        * 
        * @return KErrNone on success, 
        *         or some other system-wide error code.
        */
        virtual TInt Update(const TDesC8& aKey, TAny* aValue) = 0;

        /**
        * Method returns TAny* type value.
        * 
        * @param aKey identifying the requested value
        * 
        * @return value behind given key, or NULL, if
        * not found.
        * Ownership can NOT be transferred, since it was NEVER
        * held by this context.   
        * Caller is supposed to know the type (class) of returned
        * value from the pre-defined binding to certain key.
        */
        virtual TAny* GetAnyL(const TDesC8& aKey) = 0;	//codescannerwarnings

        // virtual TInt AddParser(CSenXmlReader* aValue) = 0; // protected context
        // virtual TInt UpdateParser(CSenXmlReader* aValue) = 0;  // protected context
        /**
        * Method returns XML parser (CSenXmlReader*)
        * 
        * @return pointer to parser or NULL, if not
        * found.
        * Ownership of returned pointer is NOT transferred.
        * Method leaves with KErrSenTypeMismatch error code,
        * if value behind this descriptor is not CSenXmlReader* type.   
        */
        virtual const CSenXmlReader* GetParser() = 0;

        /**
        * Removes value from the context, possibly destroying the
        * value, too - if it is owned by this context.
        * 
        * @param aKey used to identify value which needs to be removed
        * 
        * @return the index of removed key-value pair, or
        * KErrNotFound, if such key was not found
        */
        virtual TInt Remove(const TDesC8& aKey) = 0;     	

        /**
        * Methods tells the count of context items (key-value pairs).
        * 
        * @return count of current items in this context.
        */
        virtual TInt Count() const = 0;

        /**
        * Resolves key at certain index, if it is
        * not out of bounds of the current context.
        * 
        * @return the key at certain index.
        * Method can be used to interate through
        * context values.
        * If index is out of bounds, method leaves
        * with either KErrUnderflow (negative index),
        *          or KErrOverflow(index>count-1)
        */
        virtual TPtrC8 KeyAtL(TInt aIndex) = 0;
        
        /**
        * Method clears (possibly destroys) the current contents
        * of this context, making it possible to re-use this instance.
        */
        virtual void Reset() = 0;


        
        /**
        * Sets the direction (inbound or outbound for this context).
        * @return KErrNone or error code (KErrNotSupported is expected,
        * if method is not implemented).
        */        
        virtual TInt SetDirection( SenContext::TContextDirection aDirection ) = 0;


    };

#endif // M_SEN_CONTEXT_H

// End of File
