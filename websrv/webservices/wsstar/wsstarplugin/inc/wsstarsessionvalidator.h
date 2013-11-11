/*
* Copyright (c) 2006-2006 Nokia Corporation and/or its subsidiary(-ies). 
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








#ifndef WSSTAR_SESSION_VALIDATOR_H
#define WSSTAR_SESSION_VALIDATOR_H

// INCLUDES
#include <e32base.h>

#include "msenremoteserviceconsumer.h"

// FORWARD DECLARAtions
class CWSStarServiceSession;

// CLASS DECLARATION
class MWSStarSessionValidator
    {
    public:
    
        /**
        * @return KErrNotFound, if no known security mechanism was found
        *         KErrNotReady, if no service session has been initialized
        */
        virtual TInt ValidateL(CWSStarServiceSession& aSession,
                               TBool aRenewing,
                               HBufC8*& aErrorMessage,
                               TBool aOnlySharing) = 0;

    };

#endif //WSSTAR_SESSION_VALIDATOR_H

// End of File
