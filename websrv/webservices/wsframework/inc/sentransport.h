/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Defines the ECOM interface for Transport plug-in implementation
*
*/








#ifndef SEN_TRANSPORT_H
#define SEN_TRANSPORT_H

// INCLUDES
#include <ecom/ecom.h>

#include "sentransportbase.h"
#include "sentransportcontext.h"

// CLASS DECLARATION

/**
* Custom ECOM interface definition. This interface is used by
* clients to find specific transport plugins which implement 
* pure virtual functions defined in MSenTranport interface.
*/
class CSenTransport : public CSenTransportBase
    {
    enum TPanic
        {
        ETransportCtxNotInitialized = 1
        };

    public: // Constructors and destructor

        /**
        * Wraps ECom object instantiation, which returns the
        * default interface implementation.
        * Note: This is not a "normal" NewL method, since normally NewL
        * methods are only defined for concrete classes.
        * Note that the implementations of this interface
        * also provide NewL methods.
        * @param apCtx is the context, which ownership IS transferred
        * to this new transport plug-in instance
        * @return a pointer to new transport plug-in, 
        */
        static CSenTransport* NewL(CSenTransportContext* apCtx);

        //static CSenTransport* NewL(MSenCoreServiceManager& aManager);

        /**
        * Wraps ECom object instantitation, which returns the
        * interface implementation matching to given aCue.
        * IMPLEMENTATION_INFO::default_data.
        * For details, see EcomSIFDefinition.inl comments.
        * Note: This is not a "normal" NewL method, since normally NewL
        * methods are only defined for concrete classes.
        * Note that the implementations of this interface 
        * also provide NewL methods. 
        *
        * @param: aCue is the "name" of the requested implementation.
        *              Implementations advertise their cue as specified
        *              in their resource file field
        * @param apCtx is the context, which ownership IS transferred
        * to this new transport plug-in instance
        * @return a pointer to new transport plug-in, which is left
        *  on cleanup stack.
        */
        static CSenTransport* NewL(const TDesC8& aCue,
                                   CSenTransportContext* apCtx);
        //static CSenTransport* NewL(const TDesC8& aCue,
        //                           MSenCoreServiceManager& aManager);

        /**
        * Destructor.
        */
        virtual ~CSenTransport();

        // New functions
        
        // the actual security mechanism interface methods

    protected:

        /**
        * C++ default constructor.
        */
        CSenTransport(CSenTransportContext* apCtx, TBool aSessionOwned = ETrue);

        MSenTransportContext& Context();

    protected: // Data
        CSenTransportContext* ipCtx;     // owned

    private: // Data
        /** 
        * Instance identifier key. When instance of an implementation is created 
        * by ECOM framework, the framework will assign UID for it. 
        * The UID is used in destructor to notify framework that this instance is
      * being destroyed and resources can be released.
      */
        TUid iDtor_ID_Key;
    };

// This includes the implementation of the instantiation functions and destructor
#include "sentransport.inl"

#endif // SEN_TRANSPORT_H

// End of File
