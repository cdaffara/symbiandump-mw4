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
* Description:        Abstract class providing CBase 
*                functionality on top of MSenHandler
*
*/









#ifndef SEN_HANDLER_BASE_H
#define SEN_HANDLER_BASE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h> // for CActive
#include <SenBaseFragment.h>
#include "msenhandler.h"
#include "msenhandlercontext.h"

// FORWARD DECLARATIONS
class MSenHandlerContext;


// CLASS DECLARATION
/**
* This base class makes it possible to create a "Handler Factory",
* which may produce instances of different CSenHandlerBase subclasses.
* [ It even might be useful to be able to create both ECOM and non-ECOM
*   versions of handlers, if such factory pattern is further utilized.]
*
* Mainly, this abstract class exists to offer re-usable basic functionality
* to its direct subclasses. Functionality includes default handling of
* MSenHandlerContext properties, and base functions and "services".
*/
class CSenHandlerBase : public CSenBaseFragment, public MSenHandler
    {
    public:
        /**
        * C++ destructor
        */
        IMPORT_C virtual ~CSenHandlerBase();

        /**
        * Handlers must also be capable to represent their structure and state 
        * (type, direction and phase) in UCS8 XML form. This can be achieved either 
        * by overriding this method, or by deriving from xml fragment class, like
        * CSenBaseFragment.
        * 
        * @param aWs is the stream where to write UCS8 (UTF-8) XML document into.
        */
        IMPORT_C virtual void WriteAsXMLToL(RWriteStream& aWs);
        
        /**
        * This method returns an XML document that will include handler types, direction, phase and name.
        * 
        * @return UCS8 (UTF-8) XML document representation of this handler.
        */
        IMPORT_C virtual HBufC8* AsXmlL();

    protected:
        /**
        * C++ constructor
        */
        IMPORT_C CSenHandlerBase();

        /**
        * Default mechanism for processing received handler specific context.
        * Typically, handlers receive their context as construction phase
        * argument.
        * 
        * @param aCtx The context that contains the configuration description of the handler.
        */
        IMPORT_C virtual void ProcessHandlerContextL(MSenHandlerContext& aCtx);
    };

#endif // SEN_HANDLER_BASE_H

// End of File
