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








#ifndef M_SEN_HANDLER_H
#define M_SEN_HANDLER_H

// INCLUDES
#include <e32std.h>

// CONSTANTS
namespace SenHandler
    {
    enum THandlerType
        {
        ESessionHandler = 1, // CSenMessageHandler instance
        EMessageHandler      // CSenSessionHandler instance
        };

    enum THandlerDirection
        {
        EOutgoing = 1, 
        EIncoming,
        EBoth   
        };
        
    enum THandlerPhase
        {
        EDiscovery = 1,
        EValidate,
        EMessage
        };
    }

// FORWARD DECLARATIONS
class MSenHandlerContext;

// CLASS DECLARATION
class MSenHandler
    {
    public:
    
        // New functions
        
        /**
        * Method is used to resolve concrete type (class)
        * for this handler instance.
        * @return the class type of this handler
        */
        virtual SenHandler::THandlerType Type() const = 0;        
    
    	/**
    	* Indicates the direction this handler is serving.
    	* @return enum, which tells where this handler will 
    	* be applied - outgoing, incoming, or on both directions.
    	*/
    	virtual SenHandler::THandlerDirection Direction() const = 0;

    	/**
    	* Dictates the phase on which this handler is performing.
    	* @return phase enum, telling at what phase of processing 
    	* will this hanlder be applied
    	*/
    	virtual SenHandler::THandlerPhase Phase() = 0;

        /**
        * Method is used to resolve real, system-widely
        * unique name for this handler. By default, this
        * typically returns the ECOM cue (default_data)
        * of the concrete plug-in instance.
        * @return the unique name of this handler
        */
        virtual TPtrC8 Name() const = 0;      

        /**
        * Handlers must also be capable to represent their structure and state 
        * (type, direction and phase) in UCS8 XML form. This can be achieved either 
        * by overriding this method, or by deriving from xml fragment class, like
        * CSenBaseFragment.
        * @ param aWs is the stream where to write UCS8 (UTF-8) XML document into.
        */
        virtual void WriteAsXMLToL(RWriteStream& aWs) = 0;
        
        /**
        * Method for getting XML representation of a handler.
        * 
        * @return UCS8 (UTF-8) XML document representation of this handler.
        * This XML will include handlers type, direction, phase and name.
        */
        virtual HBufC8* AsXmlL() = 0;
        
        /**
        * InitL method uses a handler context to initialize this 
        * handler instance, and it is called once right after
        * a ECOM instance lookup is done successfully. InitL()
        * method may also be used in re-initialization of 
        * ready allocated, preserved handler instancies owned
        * by a WSF.
        * @return KErrNone or some WSF specific or system-wide error code
        */
        virtual TInt InitL(MSenHandlerContext& aCtx) = 0;
        
    };

#endif // M_SEN_HANDLER_H

// End of File
