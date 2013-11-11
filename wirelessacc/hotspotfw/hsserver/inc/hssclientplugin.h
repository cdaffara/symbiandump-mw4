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
*                MHssClientInterface via ECom framework.
*
*/



#ifndef HSSCLIENTPLUGIN_H
#define HSSCLIENTPLUGIN_H

// INCLUDES
#include <ecom/ecom.h>
#include "hssclientinterface.h"
#include "hotspotclientserver.h"

// CLASS DECLARATION
/**
* @brief Class for instantiating an implementation of MHssMgmtInterface via ECom.
*/
class CHssClientPlugin : public CBase, public MHssClientInterface
    {
    public:  // Methods

       // Constructors and destructor
        
        /**
        * Static constructor.
        * @return Pointer to the constructed object.
        */
        inline static CHssClientPlugin* NewL( const TUid aUid, TDesC8& aUidText );
        
        /**
        * Destructor.
        */
        inline virtual ~CHssClientPlugin();
        
    private: // Data

        // Identifies the instance of an implementation created by
        // the ECOM framework.
        TUid iInstanceIdentifier;
    };

#include "hssclientplugin.inl"

#endif // CHssClientPlugin_H
            
// End of File
