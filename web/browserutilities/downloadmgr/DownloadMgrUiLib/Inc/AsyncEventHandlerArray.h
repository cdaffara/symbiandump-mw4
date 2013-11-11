/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Array of event handlers
*
*/



#ifndef ASYNCEVENTHANDLERARRAY_H
#define ASYNCEVENTHANDLERARRAY_H

//  INCLUDES
#include "AsyncEventHandlerBase.h"
#include <e32base.h>

// FORWARD DECLARATIONS
//class CAsyncEventHandlerBase;

// CLASS DECLARATION

/**
*  Base class for event handlers
*/
NONSHARABLE_CLASS( CAsyncEventHandlerArray ) : public CArrayPtrFlat<CAsyncEventHandlerBase>
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor.
        */
        CAsyncEventHandlerArray();

        /**
        * Destructor.
        */
        virtual ~CAsyncEventHandlerArray();

    public: // New functions

        /**
        * Remove the handler from the array.
        */
        void Remove( CAsyncEventHandlerBase* aEventHandler );

    private: // Data

        TBool iDestructing; // ETrue when the destructor is running.
    };

#endif /* ASYNCEVENTHANDLERARRAY_H */
