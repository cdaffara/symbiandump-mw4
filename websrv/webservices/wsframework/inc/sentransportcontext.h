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








#ifndef SEN_TRANSPORT_CONTEXT_H
#define SEN_TRANSPORT_CONTEXT_H


// INCLUDES
#include <e32std.h>

#include "msentransportcontext.h"
#include "senpointermap.h"
#include "sencontext.h"

// FORWARD DECLARE
class MSenCoreServiceManager;
class CSenWSDescription;
class CSenServiceSession;
class CSenContextItem;

// TYPE DEFS
typedef RSenPointerMap<TDesC8, CSenContextItem> RCtxItemMap;

// CLASS DECLARATION
/**
* This is the abstract super class for all WSF context iten classes, 
* providing generic interface for resolving context item type, and
* equal properties.
*/
class CSenTransportContext : public CSenContext, public MSenTransportContext
    {
    public:
        
        /**
        * Standard 2-phased constructor
        * @param aCore provides reference to to core, which is stored into
        * this transport context. There is only single core instance in WS-stack.
        * @param aSessionInitializer is the XML service description used to 
        * initialize the connection (HC or SC) to the core.
        * @return pointer to new transport context
        */ 
        IMPORT_C static CSenTransportContext* NewL(MSenCoreServiceManager& aCore,
                                                   CSenWSDescription& aSessionInitializer);
        /**
        * Standard 2-phased constructor
        * @param aCore provides reference to to core, which is stored into
        * this transport context. There is only single core instance in WS-stack.
        * @param aSessionInitializer is the XML service description used to 
        * initialize the connection (HC or SC) to the core.
        * @return pointer to new transport context, which is left on cleanup stack.
        */ 
        IMPORT_C static CSenTransportContext* NewLC(MSenCoreServiceManager& aCore,
                                                    CSenWSDescription& aSessionInitializer);

        /**
        * Standard 2-phased constructor
        * @param aCore provides reference to to core, which is stored into
        * this transport context. There is only single core instance in WS-stack.
        * @param aSessionInitializer is the XML service description used to 
        * initialize the connection (HC or SC) to the core.
        * @param aInvoker is the session which is about to create new transport plug-in 
        * instance, into which this transport context is utilized.
        * @return pointer to new transport context
        */ 
        IMPORT_C static CSenTransportContext* NewL(MSenCoreServiceManager& aCore,
                                                   CSenWSDescription& aSessionInitializer,
                                                   CSenServiceSession& aInvoker);
        /**
        * Standard 2-phased constructor
        * @param aCore provides reference to to core, which is stored into
        * this transport context. There is only single core instance in WS-stack.
        * @param aSessionInitializer is the XML service description used to 
        * initialize the connection (HC or SC) to the core.
        * @param aInvoker is the session which is about to create new transport plug-in 
        * instance, into which this transport context is utilized.
        * @return pointer to new transport context, which is left on cleanup stack.
        */ 
        IMPORT_C static CSenTransportContext* NewLC(MSenCoreServiceManager& aCore,
                                                    CSenWSDescription& aSessionInitializer,
                                                    CSenServiceSession& aInvoker);

        // Destructor:
        IMPORT_C virtual ~CSenTransportContext();

        // From MSenTransportContext:
        // virtual TInt AddCore(MSenCoreServiceManager& aValue);
        virtual MSenCoreServiceManager& GetCoreL();
        //virtual MSenCoreServiceManager* GetCore();
        //virtual TInt AddSession(CSenServiceSession* aValue); 
        //virtual TInt UpdateSession(CSenServiceSession* aValue);
        virtual const CSenServiceSession* GetSession();
        //CSenServiceSession& GetSessionL();
        // virtual TInt AddInitializer(CSenWSDescription* aValue);
        // virtual TInt UpdateInitializer(CSenWSDescription* aValue);
        virtual CSenWSDescription& GetInitializerL();
        
        //virtual const CSenWSDescription* GetInitializer();
        //virtual TInt AddServiceDescription(const TDesC8& aKey, CSenWSDescription* aValue);
        //virtual TInt UpdateServiceDescription(const TDesC8& aKey, CSenWSDescription* aValue);
        //virtual const CSenWSDescription* GetServiceDescriptionL(const TDesC8& aKey);
        //virtual TInt Add(const TDesC8& aKey, MSenRemoteServiceSession& aValue); // protected context
        //virtual TInt Update(const TDesC8& aKey, MSenRemoteServiceSession& aValue);
        //virtual const MSenRemoteServiceSession* GetSenRemoteServiceSessionL(const TDesC8& aKey);

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
        virtual TAny* GetAnyL(const TDesC8& aKey);	//codescannerwarnings
        virtual const CSenXmlReader* GetParser();

        virtual TInt Remove(const TDesC8& aKey);     	
        virtual TInt Count() const;
        virtual TPtrC8 KeyAtL(TInt aIndex);
        virtual void Reset();
        
        virtual TInt SetDirection( SenContext::TContextDirection aDirection );

        
    protected:
        IMPORT_C CSenTransportContext();
        IMPORT_C void BaseConstructL(MSenCoreServiceManager& aCore,
                                     CSenWSDescription& aSessionInitializer,
                                     CSenServiceSession* apInvoker);
    };

#endif // SEN_TRANSPORT_CONTEXT_H

// End of File
