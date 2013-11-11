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








#ifndef SEN_CONTEXT_H
#define SEN_CONTEXT_H


// INCLUDES
#include <e32base.h>
#include <e32std.h>
#include "msencontext.h"
#include "senpointermap.h"

// FORWARD DECLARATIONS
class CSenContextItem;
class CSenXmlReader;



// TYPE DEFS
typedef RSenPointerMap<TDesC8, CSenContextItem> RCtxItemMap; // pointer map is internal: Utils\inc

// CLASS DECLARATION
/**
* This class is ment for INTERNAL use only, never to be
* published "as it is". The reason for not to inherit from MSenContext
* is here: this allows one to utilize this base class in *children*
* of MSenContext, which get ready base for *basic* context implementation
* and thus only need to write new methods they define in their own
* interface (if it is all that is needed).
*/
class CSenContext : public CBase //, public MSenContext
    {
    public:
        /**
        * Standard 2-phased constructor
        * @param aDirection indicate the initial direction of this message context.
        * For requests, it is "outbound", and for responses or one way notifications
        * from some (remote) service) it is "inbound".
        * @param aXmlReader is a reference to XML parser (not owned)
        * @return pointer to new context
        */ 
        IMPORT_C static CSenContext* NewL(SenContext::TContextDirection aDirection);
        /**
        * Standard 2-phased constructor
        * @param aDirection indicate the initial direction of this message context.
        * For requests, it is "outbound", and for responses or one way notifications
        * from some (remote) service) it is "inbound".
        * @return pointer to new context, which is left on cleanup stack.
        */ 
        IMPORT_C static CSenContext* NewLC(SenContext::TContextDirection aDirection);

        // Destructor:
        IMPORT_C virtual ~CSenContext();

        // Defined in MSenContext (must remain equal):
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
        virtual TAny* GetAnyL(const TDesC8& aKey);		//codescannerwarnings
        virtual const CSenXmlReader* GetParser();
        virtual TInt Remove(const TDesC8& aKey);     	
        virtual TInt Count() const;
        virtual TPtrC8 KeyAtL(TInt aIndex);
        virtual void Reset();
        
        virtual TInt SetDirection(SenContext::TContextDirection aDirection);



        protected:
            IMPORT_C CSenContext(SenContext::TContextDirection aDirection);
            IMPORT_C void BaseConstructL();

            // Data:
            RCtxItemMap iItems;                      // owned

        private: 
            // Data: 
            SenContext::TContextDirection iDirection; 

    };

#endif // SEN_CONTEXT_H

// End of File
