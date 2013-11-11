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
* Description:  Supports registration of UI Lib modules.
*
*/



// INCLUDE FILES
#include    "CDownloadMgrUiLibRegistry.h"
#include    "CDownloadMgrUiDownloadsList.h"
#include    "CDownloadMgrUiUserInteractions.h"
#include    "DMgrUiLibPanic.h"
#include    "UiLibLogger.h"

/**
* Extension class.
* 
* This class also acts a common exit observer when UILib is used. It is used not only for 
* deciding if an embedded handler application is runnong or not, but also for indicating 
* if eg. a COD Service Flow is running.
*/
NONSHARABLE_CLASS( CUiLibRegistryExtension ) : public CBase
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CUiLibRegistryExtension* NewL
             ( CDownloadMgrUiLibRegistry& aRegistry );
        
        /**
        * Destructor.
        */
        virtual ~CUiLibRegistryExtension();

    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CUiLibRegistryExtension( CDownloadMgrUiLibRegistry& aRegistry );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // Data

        CDownloadMgrUiLibRegistry& iRegistry;
    };

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUiLibRegistryExtension::CUiLibRegistryExtension
// -----------------------------------------------------------------------------
//
CUiLibRegistryExtension::CUiLibRegistryExtension
    ( CDownloadMgrUiLibRegistry& aRegistry ) 
:   iRegistry( aRegistry )
    {
    }

// -----------------------------------------------------------------------------
// CUiLibRegistryExtension::ConstructL
// -----------------------------------------------------------------------------
//
void CUiLibRegistryExtension::ConstructL()
    {
    CLOG_ENTERFN("CUiLibRegistryExtension::ConstructL");
    CLOG_LEAVEFN("CUiLibRegistryExtension::ConstructL");
    }

// -----------------------------------------------------------------------------
// CUiLibRegistryExtension::NewL
// -----------------------------------------------------------------------------
//
CUiLibRegistryExtension* CUiLibRegistryExtension::NewL
    ( CDownloadMgrUiLibRegistry& aRegistry )
    {
    CUiLibRegistryExtension* self = 
        new (ELeave) CUiLibRegistryExtension( aRegistry );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CUiLibRegistryExtension::~CUiLibRegistryExtension()
    {
    CLOG_ENTERFN("CUiLibRegistryExtension::~CUiLibRegistryExtension");
    CLOG_LEAVEFN("CUiLibRegistryExtension::~CUiLibRegistryExtension");
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::CDownloadMgrUiLibRegistry
// -----------------------------------------------------------------------------
//
CDownloadMgrUiLibRegistry::CDownloadMgrUiLibRegistry( RHttpDownloadMgr& aDownloadMgr )
:   iDownloadMgr( aDownloadMgr )
    {
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::ConstructL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiLibRegistry::ConstructL()
    {
    // Create the log file
    LOG_CREATE;
    iExtension = CUiLibRegistryExtension::NewL( *this );
    //////////////////////
    // EDlMgrAppUid
    TInt32 appUid( 0 );
    User::LeaveIfError( iDownloadMgr.GetIntAttribute( EDlMgrAppUid, appUid ) );
    iClientAppUid = TUid::Uid( (TInt)appUid );
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::NewL
// -----------------------------------------------------------------------------
//
EXPORT_C 
CDownloadMgrUiLibRegistry* CDownloadMgrUiLibRegistry::NewL( RHttpDownloadMgr& aDownloadMgr )
    {
    CDownloadMgrUiLibRegistry* self = 
        new ( ELeave ) CDownloadMgrUiLibRegistry( aDownloadMgr );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
EXPORT_C 
CDownloadMgrUiLibRegistry::~CDownloadMgrUiLibRegistry()
    {
    CLOG_ENTERFN("CDownloadMgrUiLibRegistry::~CDownloadMgrUiLibRegistry");
    RemoveUiComponentsAsObservers();
    //
    delete iUserInteractions;
    iUserInteractions = 0;
    CLOG_WRITE("iUserInteractions deleted");
    delete iDownloadsList;
    iDownloadsList = 0;
    CLOG_WRITE("iDownloadsList deleted");
    delete iExtension;
    iExtension = 0;
    CLOG_LEAVEFN("CDownloadMgrUiLibRegistry::~CDownloadMgrUiLibRegistry");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::RegisterUserInteractionsL
// -----------------------------------------------------------------------------
//
EXPORT_C 
CDownloadMgrUiUserInteractions& CDownloadMgrUiLibRegistry::RegisterUserInteractionsL()
    {
    CLOG_ENTERFN("CDownloadMgrUiLibRegistry::RegisterUserInteractionsL");
    
    __ASSERT_ALWAYS( iUserInteractions == NULL, Panic( EUiLibPanNotNull ) );

    iUserInteractions = CDownloadMgrUiUserInteractions::NewL( *this );
    // Register UI component as observer if not done yet
    TRAPD( err, SetUiComponentsAsObserversL() );
        if ( err )
            {
            CLOG_WRITE_FORMAT(" err: %d", err);
            RemoveUiComponentsAsObservers();
            User::Leave( err );
            }
            
    CLOG_LEAVEFN("CDownloadMgrUiLibRegistry::RegisterUserInteractionsL");
    return *iUserInteractions;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::RegisterDownloadsListL
// -----------------------------------------------------------------------------
//
EXPORT_C 
CDownloadMgrUiDownloadsList& CDownloadMgrUiLibRegistry::RegisterDownloadsListL()
    {
    CLOG_ENTERFN("CDownloadMgrUiLibRegistry::RegisterDownloadsListL");
    
    __ASSERT_ALWAYS( iDownloadsList == NULL, Panic( EUiLibPanNotNull ) );

    iDownloadsList = CDownloadMgrUiDownloadsList::NewL( *this );
    // Register UI component as observer if not done yet
    TRAPD( err, SetUiComponentsAsObserversL() );
        if ( err )
            {
            CLOG_WRITE_FORMAT(" err: %d", err);
            RemoveUiComponentsAsObservers();
            User::Leave( err );
            }
            
    CLOG_LEAVEFN("CDownloadMgrUiLibRegistry::RegisterDownloadsListL");
    return *iDownloadsList;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::ClientAppUid
// -----------------------------------------------------------------------------
//
TUid CDownloadMgrUiLibRegistry::ClientAppUid() const
    {
    CLOG_WRITE_FORMAT("iClientAppUid: %d", iClientAppUid);
    return (iClientAppUid);
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::DownloadsListInstalled
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiLibRegistry::DownloadsListInstalled() const
    {
    CLOG_WRITE_FORMAT("DownloadsListInstalled: %x", iDownloadsList);
    return (iDownloadsList!=NULL);
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::DownloadsList
// -----------------------------------------------------------------------------
//
CDownloadMgrUiDownloadsList& CDownloadMgrUiLibRegistry::DownloadsList() const
    {
    __ASSERT_DEBUG( iDownloadsList, Panic( EUiLibPanNull ) );
    return (*iDownloadsList);
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::UserInteractionsInstalled
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiLibRegistry::UserInteractionsInstalled() const
    {
    CLOG_WRITE_FORMAT("UserInteractionsInstalled: %x", iUserInteractions);
    return (iUserInteractions!=NULL);
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::UserInteractions
// -----------------------------------------------------------------------------
//
CDownloadMgrUiUserInteractions& CDownloadMgrUiLibRegistry::UserInteractions() const
    {
    __ASSERT_DEBUG( iUserInteractions, Panic( EUiLibPanNull ) );
    return (*iUserInteractions);
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::DownloadMgr
// -----------------------------------------------------------------------------
//
RHttpDownloadMgr& CDownloadMgrUiLibRegistry::DownloadMgr() const
    {
    CLOG_WRITE_FORMAT("DownloadMgr: %x", &iDownloadMgr);
    return (iDownloadMgr);
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::DownloadCount
// -----------------------------------------------------------------------------
//
TInt CDownloadMgrUiLibRegistry::DownloadCount() const
    {
    return (iDownloadMgr.CurrentDownloads().Count());
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::SetUiComponentsAsObserversL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiLibRegistry::SetUiComponentsAsObserversL()
    {
    if ( iUserInteractions )
        {
        CLOG_WRITE(" AddObserverL iUserInteractions ...");
        iDownloadMgr.AddObserverL( *iUserInteractions );
        CLOG_WRITE(" AddObserverL iUserInteractions OK");
        }
    if ( iDownloadsList )
        {
        CLOG_WRITE(" AddObserverL iDownloadsList ...");
        iDownloadMgr.AddObserverL( *iDownloadsList );
        CLOG_WRITE(" AddObserverL iDownloadsList OK");
        }
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiLibRegistry::RemoveUiComponentsAsObservers
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiLibRegistry::RemoveUiComponentsAsObservers()
    {
    if ( iUserInteractions )
        {
        CLOG_WRITE(" RemoveObserver iUserInteractions ...");
        iDownloadMgr.RemoveObserver( *iUserInteractions );
        CLOG_WRITE(" RemoveObserver iUserInteractions OK");
        }
    if ( iDownloadsList )
        {
        CLOG_WRITE(" RemoveObserver iDownloadsList ...");
        iDownloadMgr.RemoveObserver( *iDownloadsList );
        CLOG_WRITE(" RemoveObserver iDownloadsList OK");
        }
    }

// End of file.
