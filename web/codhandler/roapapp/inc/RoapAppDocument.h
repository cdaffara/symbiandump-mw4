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
* Description: 
*      Declaration of class CRoapAppDocument.   
*      
*
*/


#ifndef ROAP_APP_DOCUMENT_H
#define ROAP_APP_DOCUMENT_H

// INCLUDES

#include <e32base.h>
#include <GenericParamConsumer.h>
#include <RoapEng.h>

// FORWARD DECLARATIONS

class CEikAppUi;
class CRequestCompleteCallback;
class CRoapAppData;

// CLASS DECLARATION

/**
* Dummy document class for RoapApp application (application is not file-based).
*/
class CRoapAppDocument: public CAiwGenericParamConsumer
    {

    public:     // Constructors and destructor

        /**
        * Two-phased constructor. Leaves on failure.
        * @param aApp Application.
        * @return The created document.
        */
        static CRoapAppDocument* NewL( CEikApplication& aApp );

        /**
        * Destructor.
        */
        virtual ~CRoapAppDocument();

    public:     // Member access

        /**
        * Access the model.
        * @return The model.
        */
        Roap::CRoapEng& Engine() const;

        /**
        * Get data.
        * @return Data.
        */
        const CRoapAppData& Data() const;

    public:     // from CEikDocument

        /**
        * Create Application Ui.
        * @return Pointer to App Ui.
        */
        CEikAppUi* CreateAppUiL();

        /**
        * Open a file.
        * @param aDoOpen Don't know what it is.
        * @param aFilename File name.
        * @param aFs File Server Session to be used.
        * @return NULL (ROAP Trigger file is not EPOC-style store based).
        */
        CFileStore* OpenFileL
            (
            TBool aDoOpen,
            const TDesC& aFilename,
            RFs& aFs
            );

        /**
        * Open a file.
        * @param aFs File Server Session to be used.
        * @param aFile File.
        * @return NULL (ROAP Trigger file is not EPOC-style store based).
        */
        CFileStore* OpenFileL( RFs& aFs, RFile& aFile );

        /**
        * Open a file.
        * @param aFile File.
        * @param aFileStore Return value placeholder.
        * Returns NULL (ROAP Trigger file is not EPOC-style store based).
        */
        void OpenFileL( CFileStore*& aFileStore, RFile& aFile );
        
    private:    // Constructors

        /**
        * Constructor.
        * @param aApp Application.
        */
        CRoapAppDocument( CEikApplication& aApp );

        /**
        * Second phase constructor. Leaves on failure.
        */
        void ConstructL();

    private:    // data

        Roap::CRoapEng* iEngine;    ///< Engine. Owned.
        CRoapAppData* iData;        ///< Data. Owned.
        TBool iFileOpened; 
        RPointerArray<CDRMRights> iParsedRights;
    };

#endif /* def ROAP_APP_DOCUMENT_H */
