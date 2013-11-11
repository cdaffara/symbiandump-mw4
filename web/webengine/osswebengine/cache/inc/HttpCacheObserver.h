/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Definition of CHttpCacheObserver
*
*/

#ifndef CHTTPCACHEOBSERVER_H
#define CHTTPCACHEOBSERVER_H

//  INCLUDES
#include <e32base.h>
#include <f32file.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS
class CHttpCacheHandler;

// CLASS DECLARATION

/**
*
*  @lib
*  @since 3.1
*/
NONSHARABLE_CLASS(CHttpCacheObserver) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        * @since 3.1
        * @param
        * @param
        * @return CacheHandler object.
        */
        static CHttpCacheObserver* NewL( const HBufC* aDirectory, const HBufC* aIndexFile,
            CHttpCacheHandler* aHttpCacheHandler);

        /**
        * Destructor.
        */
        virtual ~CHttpCacheObserver();

    public: // new functions
        void StartObserver();

        inline TBool Updated() const { return (IsActive() && iStatus != KRequestPending); };
        void Queue();
        
    public: // from base class CActive
        void RunL();
        TInt RunError(TInt aError);
        void DoCancel();

    private:

        /**
        * Construct.
        * @since 3.1
        * @param
        * @param
        * @return CacheObserver object.
        */
        CHttpCacheObserver(CHttpCacheHandler* aHttpCacheHandler);

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const HBufC* aDirectory, const HBufC* aIndexFile);

    private:    // Data

        // name and path of index file
        HBufC* iFileName; // owned
        RFs iFsSession; // not owned
        CHttpCacheHandler* iHttpCacheHandler;
    };

#endif      // CHTTPCACHEOBSERVER_H

// End of File
