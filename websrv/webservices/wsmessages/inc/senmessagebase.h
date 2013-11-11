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
* Description:        Simplest concrete base class in WSF message class hierarchy.
*                This is not a public class.
*
*/








#ifndef SEN_MESSAGE_BASE_H
#define SEN_MESSAGE_BASE_H

#include <e32std.h>

#include <MSenMessage.h>

// FORWARD DECLARATIONS
class CSenChunk;
class CSenXmlReader;
class MSenProperties;
class MSenMessageContext;

// CLASS DECLARATION
/**
* This is abstract interface defines set of WSF message classes, providing 
* convenience for other components, which typically cast this to some 
* subclass via IsSafeToCast() method.
* @lib SenMessages.lib
* @since Series60 4.0
*/
class CSenMessageBase : public CBase, public MSenMessage
    {
    public:
        IMPORT_C static CSenMessageBase* NewL(CSenChunk& aChunk);
        IMPORT_C static CSenMessageBase* NewLC(CSenChunk& aChunk);
        
        // Destructor 
        IMPORT_C virtual ~CSenMessageBase();
        
        // New methods
        virtual TClass Type();
        virtual TDirection Direction();
        virtual TInt SetContext(MSenMessageContext* apOwnedContext);
        virtual MSenMessageContext* Context();
        virtual TInt SetProperties(MSenProperties* apOwnedProperties);
        virtual MSenProperties* Properties();
        
        virtual TBool IsSafeToCast(TClass aType);
        virtual TInt TxnId();
        virtual MSenMessage* CloneL();
        
      
    protected:
        IMPORT_C void BaseConstructL();
    private:
        // C++ constructor
        IMPORT_C CSenMessageBase(CSenChunk& aChunk);
        
    protected: // Data
        CSenChunk& iChunk;
        MSenProperties* ipProperties; // owned
        TInt iTxnId;
        TPtrC8 iSerializedMessage;
        TPtrC8 iSerializedProperties;
        MSenMessageContext* ipContext; // NOT owned
    };

#endif // SEN_MESSAGE_BASE_H

// End of File


