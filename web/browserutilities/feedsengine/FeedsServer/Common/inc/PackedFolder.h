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
* Description:  Is a serialized version of CFolderIem suitable for transfer between
*                client and server.
*
*/


#ifndef PACKED_FOLDER_H
#define PACKED_FOLDER_H


// INCLUDES
#include <e32base.h>

#include "LeakTracker.h"
#include "Packed.h"

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  Is a serialized version of CFolderItem suitable for transfer between client and server.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.1
*/
class CPackedFolder: public CPacked
    {
    public: // Destructor
        /**
        * Two-phased constructor.
        */
        static CPackedFolder* NewL();

        /**
        * Destructor.
        */        
        virtual ~CPackedFolder();
        
        
    public: // New Methods
        /**
        * Insert the begining of a folder.
        *
        * @since 3.1
        * @param aTitle The folder's title.
        * @param aEntryId The unique id for this entry.
        * @return Void
        */
        void FolderBeginsL(const TDesC& aTitle, TInt aEntryId = 0, TInt aStatus = KErrNone);
        
        /**
        * Insert the ending of a folder.
        *
        * @since 3.1
        * @return Void
        */
        void FolderEndsL();
        
        /**
        * Insert a feed.
        *
        * @since 3.1
        * @param aTitle The feed's title.
        * @param aFeedUrl The feed's url.
        * @param aLastUpdate The feed's timestamp.
        * @param aUnreadCount The number of unread topics.
        * @param aEntryId The unique id for this entry.
        * @param aFeedId The id of the feed.
        * @return void.
        */
        void AddFeedL( const TDesC& aTitle, const TDesC& aFeedUrl, 
                const TTime& aLastUpdate, TInt aFreq, TInt aStatus = KErrNone, TInt aUnreadCount = 0, TInt aEntryId = 0, TInt aFeedId = 0);

        /**
        * Insert the begining of a mini item.
        *
        * @since 3.2
        * @return Void
        */
        void ItemBeginsL();

        /**
        * Insert the ending of a mini item.
        *
        * @since 3.2
        * @return Void
        */
        void ItemEndsL();

        /**
        * Called when when packing is done.
        *
        * @since 3.1
        * @return void.
        */
        void DoneL();

        /**
        * Extract the folder item's attributes.
        *
        * @since 3.1
        * @param aTitle The item's title.
        * @param aUrl The item's url.
        * @param aEntryId The item's entry id.
        * @param aFeedId The item's feed id or 0 if its a folder.
        * @param aTimestamp The item's timestamp.
        * @param aUnreadCount The number of unread topics.
        * @param aStatus The feed status.
        * @param aFreq The frequency for auto updating.
        * @return void.
        */
        void ExtractAttributes(TPtrC& aTitle, TPtrC& aUrl, TInt& aEntryId, 
                TInt& aFeedId, TTime& aTimestamp, TInt& aUnreadCount, TInt &aStatus, TInt& aFreq) const;


    protected:  // New Methods
        /**
        * C++ default constructor.
        */
        CPackedFolder(TInt aTokenArrayInc, TInt aStringTableInc);
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();
        
    private:
        TLeakTracker  iLeakTracker;
    };


#endif      // PACKED_FOLDER_H
            
// End of File
