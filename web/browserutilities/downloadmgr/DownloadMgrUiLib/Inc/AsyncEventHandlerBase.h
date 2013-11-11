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
* Description:  Base class for event handlers
*
*/



#ifndef ASYNCEVENTHANDLERBASE_H
#define ASYNCEVENTHANDLERBASE_H

//  INCLUDES
#include <e32base.h>
#include <DownloadMgrClient.h>

// FORWARD DECLARATIONS
class RHttpDownload;
class MDownloadMgrUiLibRegModel;
class CAsyncEventHandlerArray;

// CLASS DECLARATION

/**
*  Base class for event handlers
*/
NONSHARABLE_CLASS( CAsyncEventHandlerBase ) : public CActive
    {
    public:  // Constructors and destructor
        
        /**
        * C++ default constructor. It invokes RunL().
        */
        CAsyncEventHandlerBase( RHttpDownload& aDownload, 
                                THttpDownloadEvent aEvent, 
                                MDownloadMgrUiLibRegModel& aRegistryModel, 
                                CAsyncEventHandlerArray& aOwner );

        /**
        * Destructor. It calls Cancel().
        */
        virtual ~CAsyncEventHandlerBase();

    public: // New functions

        /**
        * Getters
        * @return Reference to the download mamber.
        */
        RHttpDownload& Download();
        const RHttpDownload& Download() const;

    protected: // Data

        RHttpDownload& iDownload;
        THttpDownloadEvent iEvent;
        MDownloadMgrUiLibRegModel& iRegistryModel;
        CAsyncEventHandlerArray& iOwner;
    };

#endif /* ASYNCEVENTHANDLERBASE_H */
