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
* Description:  A class to help track memory leaks.
*
*/


#ifndef LEAK_TRACKER_H
#define LEAK_TRACKER_H

#warning The Feeds Engine API will be removed wk25. Please see http://wikis.in.nokia.com/Browser/APIMigration for more information

// Helper macros
#if defined(__WINSCW__)
//#define TRACK_LEAKS
#endif

// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES
struct LeakTracker_Value
    {
    TInt    valueId;
    TInt32  value;
    };

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION


/**
*  A class to help track memory leaks.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class CLeakTracker: public CBase
    {
    public:  // Types
        enum TInstType
            {
            ENone                       = 0,
            EAbstractValueHolder        = 1,
            EEditFeedDialog             = 2,
            EEditFolderDialog           = 3,
            EFeed                       = 4,
            EFeedContainer              = 5,
            EFeedHandler                = 6,
            EFeedParser                 = 7,
            EFeedsApplication           = 8,
            EFeedsAppUi                 = 9,
            EFeedsDocument              = 10,
            EFeedView                   = 11,
            EFileHandler                = 12,
            EFolderContainer            = 13,
            EFolderItem                 = 14,
            EFolderView                 = 15,
            EUpdateFeedTask             = 16,
            EImportFolderTask           = 17,
            EHttpConnection             = 18,
            EHttpHandler                = 19,
            EItem                       = 20,
            EItemIter                   = 21,
            ELibXml2                    = 22,
            ERichTextEditor             = 23,
            EOpmlParser                 = 24,
            EUpdateFolderViewTask       = 25,
            ERssFeedParser              = 26,
            ESettingsContainer          = 27,
            ESettingsView               = 28,
            ESingletons                 = 29,
            ETestHarness                = 30,
            ETopicContainer             = 31,
            ETopicView                  = 32,
            EUpdateTopicViewTask        = 33,
            EWaitDialog                 = 34,
            EXmlEncoding                = 35,
            EXmlEntity                  = 36,
            ETestHarnessContainer       = 37,
            ETestHarnessView            = 38,
            EFeedsServer                = 39,
            EFeedsDatabase              = 40,
            EAtomFeedParser             = 41,
            EUpdateAllFeedsTask         = 42,
            EFeedsServerSession         = 43,
            EBackgroundUpdater          = 44,
            EPackedFeed                 = 45,
            EPackedFolder               = 46,
            EFeedsViewMgr               = 47,
            EFeedsServerClient          = 48,
            EClientRequestHandler       = 49,
            EFeedsServerFeed            = 50,
            EFeedRequestHandler         = 51,
            EFeedsSeverFolderItem       = 52,
            EFolderItemRequestHandler   = 53,
            EEnclosure                  = 54,
            EMiniItem                   = 55
            };

    public:  // Constructors and destructor
        /**
        * Two-phased constructor.
        */
        static CLeakTracker* NewL(const TDesC& aSource);
        
        /**
        * Destructor.
        */        
        virtual ~CLeakTracker();

        
    public: // New methods
        /**
        * Increments the ref-count on the given class type
        *
        * @since 3.0
        * @param aType The class type to be incremented.
        * @return Void
        */
        void Increment(TInstType aType);

        /**
        * Decrements the ref-count on the given class type
        *
        * @since 3.0
        * @param aType The class type to be decremented.
        * @return Void
        */
        void Decrement(TInstType aType);


    private:
        /**
        * C++ default constructor.
        */
        CLeakTracker();
        
        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL(const TDesC& aSource);


    private:  // Data
        RArray<LeakTracker_Value> iValues;
        HBufC*                    iSource;
    };


#ifdef TRACK_LEAKS

extern CLeakTracker*  gLeakTracker;

#define LEAK_TRACKER_INCREMENT(aType) gLeakTracker->Increment(aType)
#define LEAK_TRACKER_DECREMENT(aType) gLeakTracker->Decrement(aType)

#else // !TRACK_LEAKS
#define LEAK_TRACKER_INCREMENT(aType)
#define LEAK_TRACKER_DECREMENT(aType)
#endif

/**
*  A class to help track memory leaks.
*
*  \b Library: FeedsEngine.lib
*
*  @since 3.0
*/
class TLeakTracker
    {
    public:
        /**
        * C++ default constructor.
        */
        inline TLeakTracker(CLeakTracker::TInstType aClassId):
                iClassId(aClassId)
            {
            LEAK_TRACKER_INCREMENT(iClassId);
            }
        
        /**
        * Destructor.
        */        
        inline ~TLeakTracker()
            {
            LEAK_TRACKER_DECREMENT(iClassId);
            }
            
    private:
        CLeakTracker::TInstType  iClassId;
    };
    
    
#endif      // LEAK_TRACKER_H
            
// End of File
