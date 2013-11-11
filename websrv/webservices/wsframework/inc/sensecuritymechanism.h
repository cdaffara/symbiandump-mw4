/*
* Copyright (c) 2002-2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:        Defines the ECom interface for Sen Security Mechanism plug-ins
*
*/








#ifndef SEN_SECURITY_MECHANISM_H
#define SEN_SECURITY_MECHANISM_H

// INCLUDES
#include <ecom/ecom.h>

#include "msensaslmessage.h"
#include "msencoreservicemanager.h"
#include "saslmechanism.h"

// CLASS DECLARATION

/**
* Custom ECOM interface definition. This interface is used by
* clients to find specific security mechanism plugin instance
* implementations which implement pure virtual functions.
*/
class CSenSecurityMechanism : public CSaslMechanism
    {
    public: // Constructors and destructor
        /**
        * Wraps ECom object instantitation, which returns the
        * default interface implementation.
        * Note: This is not a "normal" NewL method, since normally NewL
        * methods are only defined for concrete classes.
        * Note that the implementations of this interface
        * ALSO provide NewL methods. They are the familiar
        * EPOC NewL's, which create instance of classes.
        */
        static CSenSecurityMechanism* NewL(MSenCoreServiceManager& aManager);

        /**
        * Wraps ECom object instantitation, which returns the
        * interface implementation matching to given aCue.
        * IMPLEMENTATION_INFO::default_data.
        * For details, see EcomSIFDefinition.inl comments.
        * Note: This is not a "normal" NewL method, since normally NewL
        * methods are only defined for concrete classes.
        * Note that the implementations of this interface ALSO
        * provide NewL methods. They are the familiar EPOC NewL's,
        * which create instance of classes.
        * @param  aCue is the "name" of the requested implementation.
        *         Implementations advertise their cue as specified
        *         in their resource file field
        */
        static CSenSecurityMechanism* NewL(const TDesC8& aCue,
                                           MSenCoreServiceManager& aManager);

        /**
        * Destructor.
        */
        virtual ~CSenSecurityMechanism();

        // New functions
        
        // the actual security mechanism interface methods
        
        /**
        * @return the name of the mechanism, like "PLAIN".
        */
        virtual const TDesC8& Name() = 0;

        /**
        * @return a SASL request that is a response to the passed response.
        * Caller takes ownership of the returned object. Never returns NULL.
        */
        virtual TInt HandleResponseL(MSenSaslMessage& aResponse,
                                     MSenSaslMessage& aNewRequest) = 0;

    protected:
        CSenSecurityMechanism(MSenCoreServiceManager& aServiceManager);

    protected: // Data
        // to access base64, encoding/decoding
        MSenCoreServiceManager& iServiceManager;

    private:
        /**
        * Instance identifier key. When instance of an
        * implementation is created by ECOM framework, the
        * framework will assign UID for it. The UID is used in
        * destructor to notify framework that this instance is
        * being destroyed and resources can be released.
        */
        TUid iDtor_ID_Key;
    };

// This includes the implementation of the instantiation functions and
// destructor
#include "sensecuritymechanism.inl"

#endif // SEN_SECURITY_MECHANISM_H

// End of File
