/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  A class that fetches resources via RFile.
*
*/


#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H


// INCLUDES
#include <e32base.h>

#include "LeakTracker.h"
#include "UrlHandler.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CIdle;

// CLASS DECLARATION


/**
*  A class that fetches resources via RFile.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CFileHandler: public CUrlHandler
    {
    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CFileHandler* NewL();
        
        /**
        * Destructor.
        */        
        virtual ~CFileHandler();
    

    public:  // From MUrlHandler
        /**
        * Loads the given url -- asynchronously
        *
        * @since 3.0
        * @param aUrl The url to load
        * @param aObserver The load observer.
        * @return void.
        */
        virtual void LoadUrlL(const TDesC& aUrl, MLoadObserver& aObserver);


    private:
        /**
        * C++ default constructor.
        */
        CFileHandler();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
                
        /**
        * Loads and passes the buffer to the observer.
        *
        * @since 3.0
        * @param aPtr The this pointer.
        * @return Always returns EFalse.
        */
        static TInt LoadCompleted(TAny* aPtr);

        /**
        * Loads and passes the buffer to the observer.
        *
        * @since 3.0
        * @return The buffer.
        */
        HBufC8* LoadCompletedHelperL();


    private:
        TLeakTracker    iLeakTracker;
        
        HBufC16*        iUrl;
        MLoadObserver*  iObserver;
        CIdle*          iIdle;
    };

#endif      // FILE_HANDLER_H
            
// End of File