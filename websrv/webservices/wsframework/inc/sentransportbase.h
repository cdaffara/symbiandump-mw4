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
* Description:        Abstract class providing CBase 
*                functionality in top of MSenTransport
*
*/









#ifndef SEN_TRANSPORT_BASE_H
#define SEN_TRANSPORT_BASE_H

// INCLUDES
#include <e32std.h>
#include <e32base.h> // for CActive
#include "msentransport.h"

/**
* This base class makes it possible to create a "Transport Factory",
* which may produce instancies of different CSenTransportBase subclasses.
* It might be useful to be able to create both ECOM and non-ECOM versions
* of transports, if such factory pattern is further utilized.
*/

// CLASS DECLARATION

class CSenTransportBase : public CBase, public MSenTransport
    {
    public:
        inline virtual ~CSenTransportBase() { };
        CSenTransportBase(TBool aSessionOwned): iSessionOwned(aSessionOwned) 
            {
            }
        TBool SessionOwned() 
            {
            return iSessionOwned;
            }

    private:
        TBool iSessionOwned;
    };

#endif // SEN_TRANSPORT_BASE_H

// End of File
