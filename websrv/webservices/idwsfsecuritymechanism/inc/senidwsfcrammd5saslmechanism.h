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
* Description: Header declaration
*
*/








#ifndef SEN_IDWSF_CRAMMD5_SASL_MECHANISM_H
#define SEN_IDWSF_CRAMMD5_SASL_MECHANISM_H

// INCLUDES
#include <hash.h>

#include "sensecuritymechanism.h"
#include "senidwsfplainsaslmechanism.h"


// FORWARD DECLARATIONS
class CMessageDigest;
class MSenSaslMessage;

// CLASS DECLARATION
class CSenIdWsfCrammd5SaslMechanism :  public CSenIdWsfPlainSaslMechanism
    {
    public: // Constructors and destructor
    
        static CSenIdWsfCrammd5SaslMechanism* NewL(
                                    MSenCoreServiceManager& aServiceManager);
        static CSenIdWsfCrammd5SaslMechanism* NewLC(
                                    MSenCoreServiceManager& aServiceManager);
        
        virtual ~CSenIdWsfCrammd5SaslMechanism();

        // New functions
        
        virtual const TDesC8& Name();
        virtual TInt HandleResponseL(MSenSaslMessage& aResponse,
                                     MSenSaslMessage& aNewRequest);

    protected: // Data
        CSenIdWsfCrammd5SaslMechanism(MSenCoreServiceManager& aServiceManager);
    };

#endif // SEN_IDWSF_CRAMMD5_SASL_MECHANISM_H

// End of File
