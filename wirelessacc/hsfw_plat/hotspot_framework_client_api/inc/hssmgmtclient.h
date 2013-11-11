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
* Description:   Wrapper class for instantiating an implementation of
*                MHssMgmtInterface via ECom framework.
*
*/



#ifndef HSSMGMTCLIENT_H
#define HSSMGMTCLIENT_H

// INCLUDES
#include <ecom/ecom.h>
#include "hssmgmtinterface.h"

// CLASS DECLARATION
/**
* @brief Class for instantiating an implementation of MHssMgmtInterface via ECom.
*/
class CHssMgmtClient : public CBase, public MHssMgmtInterface
    {
    public:  // Methods

       // Constructors and destructor
        
        /**
        * Static constructor.
        * @return Pointer to the constructed object.
        */
        inline static CHssMgmtClient* NewL();
        
        /**
        * Destructor.
        */
        inline virtual ~CHssMgmtClient();
        
    private: // Data

        // Identifies the instance of an implementation created by
        // the ECOM framework.
        TUid iInstanceIdentifier;
    };

#include "hssmgmtclient.inl"

#endif // HSSMGMTCLIENT_H
            
// End of File
