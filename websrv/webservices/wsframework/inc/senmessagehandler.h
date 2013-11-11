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
* Description:        Defines the ECOM interface for Handler plug-in implementation
*
*/








#ifndef SEN_MESSAGE_HANDLER_H
#define SEN_MESSAGE_HANDLER_H

// INCLUDES
#include <ecom/ecom.h>

#include "senhandlerbase.h"     // internal Framework\inc
#include "msenhandlercontext.h" // internal Framework\inc
#include "msenmessagecontext.h" // internal Framework\inc

// CLASS DECLARATION

/**
* Custom ECOM interface definition. This interface is used by
* clients to find specific handler plugins which implement 
* pure virtual functions defined in MSenTranport interface.
*/
class CSenMessageHandler : public CSenHandlerBase
    {
    public:
        
        // Constructors and destructor:

        /**
        * Wraps ECom object instantiation, which returns the
        * default ECOM implementation for this interface.
        * Note: This is not a "normal" NewL method, since 
        * normally NewL methods are only defined for concrete
        * classes. Note that the implementations of this interface
        * also provide NewL methods.
        *
        * Additionally, this static constructor performs InitL()
        * using the provided handler context for the newly allocated
        * handler instance.
        * 
        * @param aCtx is handle to the handler specific context
        */
        static CSenMessageHandler* NewL(MSenHandlerContext& aCtx);

        /**
        * Wraps ECom object instantiation, which returns the
        * interface implementation matching to given aCue,
        * IMPLEMENTATION_INFO::default_data.
        * For details, see EcomSIFDefinition.inl comments.
        * Note: This is not a "normal" NewL method, since normally NewL
        * methods are only defined for concrete classes.
        * Note that the implementations of this interface 
        * also provide NewL methods. 
        *
        * Additionally, this static constructor performs InitL()
        * using the provided handler context for the newly allocated
        * handler instance.
        *
        * @param aCue is the "name" of the requested implementation.
        *             Implementations advertise their cue as specified
        *             in their resource file field
        * @param aCtx is handle to the handler specific context
        */
        static CSenMessageHandler* NewL(const TDesC8& aCue,
                                        MSenHandlerContext& aCtx);
        /**
        * Destructor.
        */
        virtual ~CSenMessageHandler();

        // From MSenHandler:
        /**
        * Method is used to resolve concrete type (class)
        * for this handler instance.
        * 
        * @return the class type of this handler
        */
        inline SenHandler::THandlerType Type() const;        

        /**
        * Method is used to resolve real, system-wide
        * unique name for this handler. By default, this
        * typically returns the ECOM cue (default_data)
        * of the concrete plug-in instance.
        * 
        * @return the unique name of this handler
        */
        inline TPtrC8 Name() const;        
        

        // New functions:
        
        /**
        * InvokeL is the most emergent method in message handler interface.
        * Each sub-class must implement this method thus providing message
        * processing routine.
        * 
        * @param aCtx is the message context that will be accessed and 
        * typically altered by implemented handler, in order to provide
        * modifications on the layer in message construction, validation,
        * or some other focused area that the handler was intended to take
        * responsibility for. For example, certain handler might add
        * some new (SOAP) headers into the message through this context.
        * @return KErrNone or some WSF specific, or system-wide error code
        */
        virtual TInt InvokeL(MSenMessageContext& aCtx) = 0;
        
    protected:

        /**
        * C++ default constructor.
        */
        CSenMessageHandler(MSenHandlerContext& aCtx);

    protected: // Data
        MSenHandlerContext& iHandlerContext;    

    private: // Data
        /** 
        * Instance identifier key. When instance of an implementation is created 
        * by ECOM framework, the framework will assign UID for it. 
        * The UID is used in destructor to notify framework that this instance is
        * being destroyed and resources can be released.
        */
        TUid iDtor_ID_Key;
        
        HBufC8* iName;
    };

// This includes the implementation of the instantiation functions and destructor
#include "senmessagehandler.inl"

#endif // SEN_MESSAGE_HANDLER_H

// End of File
