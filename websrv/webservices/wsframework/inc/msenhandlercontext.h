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








#ifndef M_SEN_HANDLER_CONTEXT_H
#define M_SEN_HANDLER_CONTEXT_H

// INCLUDES
#include <e32std.h>
#include "msencontext.h" // internal Framework\inc

// FORWARD DECLARE
class MSenCoreServiceManager;

// CLASS DECLARATION
class MSenHandlerContext : public MSenContext
    {
    public:
        /**
        * Adds new MSenCoreServiceManager& type value into context,
        * typically identified by this key: KSenCtxKeyCoreServiceManager.
        * @aKey identifies new value
        * @aValue is the value being added. 
        * Ownership is NOT transferred to this context.
        * @return KErrNone on success, 
        *         KErrAlreadyExists, if the key is reserved,
        *         or some other system-wide error code.
        */
        virtual TInt Add(MSenCoreServiceManager& aValue) = 0;

        /*
        * Method returns MSenCoreServiceManager* type value.
        * @return value at certain index, or NULL, if not found.
        * Ownership is not transferred, as it was never held by
        * this context.   
        */
        virtual MSenCoreServiceManager* GetSenCoreServiceManager() = 0;
    };

#endif // M_SEN_HANDLER_CONTEXT_H

// End of File
