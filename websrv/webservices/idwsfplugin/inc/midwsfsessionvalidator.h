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








#ifndef IDWSF_SESSION_VALIDATOR
#define IDWSF_SESSION_VALIDATOR

// INCLUDES
#include <e32base.h>

//#include "IdWsfServiceSession.h"

// FORWARD DECLARAtions
class MSenRemoteServiceConsumer;
class CIdWsfServiceSession;

// CLASS DECLARATION
class MIdWsfSessionValidator
    {
    public:
    
        /**
        * @return KErrNotFound, if no known security mechanism was found
        *         KErrNotReady, if no service session has been initialized
        */
        virtual TInt ValidateL(CIdWsfServiceSession& aSession,
                               MSenRemoteServiceConsumer& aValidator) = 0;

    };

#endif //IDWSF_SESSION_VALIDATOR

// End of File
