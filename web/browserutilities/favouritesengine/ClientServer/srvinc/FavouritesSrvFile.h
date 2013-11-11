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
*      Declaration of class CFavouritesSrvFile.
*      
*
*/


#ifndef FAVOURITES_SRV_FILE_H
#define FAVOURITES_SRV_FILE_H

//  INCLUDES

#include <e32base.h>
#include <f32file.h>

// CLASS DECLARATION

/**
* Server side file object for Favourites Engine.
*/
NONSHARABLE_CLASS(CFavouritesSrvFile): public CBase
    {
    public:     // construct / destruct

        /**
        * Two phased constructor.
        */
        static CFavouritesSrvFile* NewL();

        /**
        * Destructor.
        */
        inline virtual ~CFavouritesSrvFile();

    public:     // new methods

		void SetName(TParse aParse);
		
        /**
        * Open existing file with shared read-access.
        * @param aName File name.
        */
        void OpenL();

        /**
        * Create new file / replace existing with exclusive write-access.
        * Existing content of the file, if any, is lost.
        * @param aName File name.
        */
        void ReplaceL();

        /**
        * Transfer handles to client.
        * @param aMessage Message.
        */
        void TransferToClientL( const RMessage2& aMessage );

    private:    // construct

        /**
        * Constructor.
        */
        inline CFavouritesSrvFile();

        /**
        * Second phase constructor.
        */
        void ConstructL();

    private:        // Data

		TParse iFileName; // The name of the file
		
        RFs iFs;        ///< File Server session handle.
        RFile iFile;    ///< File handle.
    };

#include "FavouritesSrvFile.inl"

#endif
            
// End of File
