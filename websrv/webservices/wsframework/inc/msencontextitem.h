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








#ifndef M_SEN_CONTEXT_ITEM_H
#define M_SEN_CONTEXT_ITEM_H


// INCLUDES
#include <e32std.h>

// CONSTANTS

const TInt KErrSenCtxItemTypeMismatch = -30350; 


// CLASS DECLARATION
/**
* This is the abstract super class for all WSF context iten classes, 
* providing generic interface for resolving context item type, and
* equal properties.
*/
class MSenContextItem
    {
    public:
        enum TClass
            {
            EContextItem = 1, 
            EProtectedContextItem
            };

        enum TData
            {
            ETAny = 0,
            ETDesC8, 
            ETInt,
            ETUint,
            EMSenCoreServiceManager,
            ECSenElement,
            ECSenSoapMessage,
            ECSenChunk,
            EMSenMessage,
            ECSenMessageBase,
            ECSenSoapEnvelope2,
            ECSenXmlReader,
            ECSenWSDescription,
            EMSenRemoteServiceConsumer,
            EMSenRemoteHostlet,
            EMSenProperties,
            ECSenAtomMessage
            };
    
        // New functions
        
        /**
        * Method is used to resolve the class-type of the context item implementation
        * 
        * @return the class type of this handler
        */
        virtual MSenContextItem::TClass Type() const = 0;        
    };

#endif // M_SEN_CONTEXT_ITEM_H

// End of File
