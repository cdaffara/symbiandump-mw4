/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Declaration of class CDrmHandler.   
*
*/


#ifndef DRM_HANDLER_H
#define DRM_HANDLER_H

// INCLUDES

#include <e32base.h>
#include <DRMRightsClient.h>

// FORWARD DECLARATIONS

class DRMCommon;

// CLASS DECLARATION

/**
* DRM handler.
* This class is a container for DRM-related class objects and also provides
* the cleanup. The content itself is processed as any other (non-DRM) content,
* by CodEngBase.
*/
NONSHARABLE_CLASS( CDrmHandler ): public CBase
    {
    public:     // Constructors and destructor.

        /**
        * Two phased constructor. Leaves on failure.
        * @param aFname File name.
        * @return The created object.
        */      
        static CDrmHandler* NewL( const TDesC& aFname );

        /**
        * Destructor.
        */
        ~CDrmHandler();

    public:     // new methods

        /**
        * Cleanup DRM rights.
        * This is to be called if RO received but install-notify failed.
        */
        void Cleanup();

        /**
        * Get MIME type (of content inside DCF).
        * @return Type inside DCF.
        */
        TPtrC8 Type() const;

    private:    // Constructors and destructor.

        /**
        * Constructor.
        */
        CDrmHandler();

        /**
        * Second phase constructor. Leaves on failure.
        * @param aFname File name.
        */
        void ConstructL( const TDesC& aFname );

    private:    // new methods
    
        /**
        * Check if MIME type aType is supported by DRM Engine; leave with
        * KMimeNotSupported if not. (Can leave with general failures as well).
        * @param aType Content type to check.
        */
        void CheckTypeL( const TDataType& aType ) const;

    private:    // data

        DRMCommon* iDrmCommon;          ///< DRM Engine or NULL. Owned.
        RDRMRightsClient iRightsClient; ///< DRM Rights Client. Owned.
        HBufC8* iContentUri;            ///< Content-URI or NULL. Owned.
        HBufC8* iType;                  ///< MIME type or NULL. Owned.

    };

#endif /* def DRM_HANDLER_H */
