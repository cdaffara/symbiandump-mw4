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
* Description:  Supports Download Menu and Downloads List
*
*/



// INCLUDE FILES
//#include <platform/mw/Browser_platform_variant.hrh>
#include    <Browser_platform_variant.hrh>
#include    "CDownloadMgrUiDownloadsList.h"
#include    "CDownloadMgrUiLibRegistry.h"
#include    "CDownloadMgrUiDownloadMenu.h"
#include    "CDownloadsListArray.h"
#include    "CDownloadsListDlg.h"
#include    "AsyncEventHandlerArray.h"
#include    "DMgrUiLibPanic.h"
#include    "DownloadMgrUiLib.hrh"
#include    "CUserInteractionsUtils.h"
#include    "UiLibLogger.h"
#include    "MDownloadHandlerObserver.h"
#include    <e32std.h>
#include    <e32def.h>
#include    <DownloadMgrUiLib.rsg>
#include    <DownloadMgrUiLib.mbg>
#include    <coemain.h>
#include    <eikmenup.h>
#include    <gulicon.h>
#include    <uikon.hrh>
#include    <AknServerApp.h>
#include    <uriutils.h>

// following line is temporary: AVKON dependency removal
#undef BRDO_APP_GALLERY_SUPPORTED_FF

#ifdef BRDO_APP_GALLERY_SUPPORTED_FF
#include    <MGXFileManagerFactory.h>
#include    <CMGXFileManager.h>
#include    <MediaGalleryUID.h>  //Gallery UIDs
#endif

#include    <DocumentHandler.h>
#include    "bautils.h"
#include    <BrowserUiSDKCRKeys.h>
#include    <aknnotewrappers.h>
#include    <StringLoader.h>
#include    <apgtask.h>




// CONSTANTS
const TInt KUpdateProgressInfoInterval   = 2*1000000;  // 2 sec

const TInt KMostRecentSort  = 2;  // Most Recent Sort in File Manager
const TInt KLauchStandAlone = 1;  // Launch the File Manager in Stand Alone Mode

_LIT8(KSisxContentType, "x-epoc/x-sisx-app");

// FORWARD DECLARATIONS
class CDownloadsListExecuter;

// LOCAL FUNCTION PROTOTYPES

/**
* Find the given download in the model and give back the ui data and the index.
* It returns 0, if the given download is not found in aListModel and aIndex is set to KErrNotFound!
*/
inline TDownloadUiData* FindDlUiData( CDownloadsListArray& aListModel,
                                       RHttpDownload& aDownload,
                                       TInt& aIndex );

#ifdef __WINS__
TInt AskIapIdL( TUint32& aId );
#endif

/**
* Extension class.
*/
NONSHARABLE_CLASS( CDownloadsListExtension ) : public CBase,
                                               public MDownloadHandlerObserver
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDownloadsListExtension* NewL
             ( CDownloadMgrUiDownloadsList& aDownloadsList );

        /**
        * Destructor.
        */
        virtual ~CDownloadsListExtension();

    public: // New functions

        #ifdef __WINS__
        void EnsureIapIdIsSetL();
        #endif

        /**
        * Display the list of downloads now.
        * @since Series 60 2.8
        * @return -
        */
        void DisplayDownloadsListL();

        /**
        * Display the list of downloads in an active object's RunL.
        * @since Series 60 3.0
        * @return -
        */
        void DisplayDownloadsListAsynchronouslyL();

        /**
        * Display the list of downloads in an active object's RunL.
        * @since Series 60 3.0
        * @return -
        */
        void DisplayDownloadsListAsynchronouslyL( RHttpDownload& aHighlightDl );

        /**
        * Cancel displaying the list of downloads.
        * @since Series 60 2.8
        * @return -
        */
        void CancelDisplayingDownloadsList();

        /**
        * Is the List of Downloads visible?
        * @since Series 60 2.8
        * @return ETrue/EFalse
        */
        TBool IsDialogVisible() const;

        /**
        * Get a reference. Panics if null!
        */
        CDownloadsListExecuter& DlExecuter() const;

        /**
        * Cancel a progress info timer update.
        */
		void CancelProgressTimer();

        /**
        * Start a progress info timer update.
        */
		void StartProgressTimer();

    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CDownloadsListExtension( CDownloadMgrUiDownloadsList& aDownloadsList );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected: // From MDownloadHandlerObserver

        virtual void NotifyHandlerExit( RHttpDownload* aDownload, TInt aReason );

    public: // Data

        CDownloadMgrUiDownloadsList& iDownloadsList;
        CDownloadsListExecuter* iDownloadsListExecuter; ///< Owned.
        TBool* iDeleted; ///< Used because of the wait popup list
        TUint32 iIapId;
        TBool   iIapIdGotAndSet;

		TBool	iIsUpdateInfoAllowed;

	private: //Data
		CPeriodic*		iUpdateProgressInfoTimer;

		/*
		* callback function for the UpdateProgressInfo
		*/
		static TInt		UpdateProgressInfoCallback(TAny* aPtr);
    };



NONSHARABLE_CLASS( CDownloadsListExecuter ) : public CActive
    {
    public:  // Constructors and destructor

        /**
        * Two-phased constructor.
        */
        static CDownloadsListExecuter* NewL( CDownloadMgrUiDownloadsList& aDownloadsList,
                                             CDownloadsListExecuter** aReferer );

        /**
        * Two-phased constructor.
        */
        static CDownloadsListExecuter* NewL( CDownloadMgrUiDownloadsList& aDownloadsList,
                                             CDownloadsListExecuter** aReferer,
                                             RHttpDownload& aHighlightDl );

        /**
        * Destructor.
        */
        virtual ~CDownloadsListExecuter();

    public: // New

        /**
        * Invoke RunL.
        */
        void Start();

        /**
        * Check if the dialog already exists.
        */
        TBool DialogExists() const;

        /**
        * Get a reference. Panics if null!
        */
        CDownloadsListDlg& Dialog() const;

        /**
        * Execute the dialog now (wait dialog).
        */
        void ExecuteLD();

    private: // Constructors

        /**
        * C++ default constructor.
        */
        CDownloadsListExecuter( CDownloadMgrUiDownloadsList& aDownloadsList,
                                CDownloadsListExecuter** aReferer, RHttpDownload* aHighlightDl );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    private: // From CActive

        void DoCancel();
        void RunL();
        TInt RunError(TInt aError);

    private: // Data

        CDownloadMgrUiDownloadsList& iDownloadsList;
        TBool* iDeleted; ///< Used because of the wait popup list
        CDownloadsListDlg*          iDialog; ///< Not owned - only reference.
        CDownloadsListExecuter**    iReferer;
        RHttpDownload* iHighlightDl; ///< Not owned.
        TBool iProgressiveDownload;
    };



// -----------------------------------------------------------------------------
// CDownloadsListExecuter::CDownloadsListExecuter
// -----------------------------------------------------------------------------
//
CDownloadsListExecuter::CDownloadsListExecuter
    ( CDownloadMgrUiDownloadsList& aDownloadsList,
      CDownloadsListExecuter** aReferer,
      RHttpDownload* aHighlightDl )
:   CActive( CActive::EPriorityHigh ), // EPriorityHigh to be visible ASAP.
    iDownloadsList( aDownloadsList ),
    iReferer( aReferer ),
    iHighlightDl( aHighlightDl )
    {
    CActiveScheduler::Add( this );
    }

// -----------------------------------------------------------------------------
// CDownloadsListExecuter::ConstructL
// -----------------------------------------------------------------------------
//
void CDownloadsListExecuter::ConstructL()
    {
   	iProgressiveDownload = iDownloadsList.IsProgressiveDownloadEnabled();
    }

// -----------------------------------------------------------------------------
// CDownloadsListExecuter::NewL
// -----------------------------------------------------------------------------
//
CDownloadsListExecuter* CDownloadsListExecuter::NewL
    ( CDownloadMgrUiDownloadsList& aDownloadsList, CDownloadsListExecuter** aReferer )
    {
    CDownloadsListExecuter* self =
        new (ELeave) CDownloadsListExecuter( aDownloadsList, aReferer, 0 );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// -----------------------------------------------------------------------------
// CDownloadsListExecuter::NewL
// -----------------------------------------------------------------------------
//
CDownloadsListExecuter* CDownloadsListExecuter::NewL
    ( CDownloadMgrUiDownloadsList& aDownloadsList,
      CDownloadsListExecuter** aReferer,
      RHttpDownload& aHighlightDl )
    {
    CDownloadsListExecuter* self =
        new (ELeave) CDownloadsListExecuter
            ( aDownloadsList, aReferer, &aHighlightDl );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CDownloadsListExecuter::~CDownloadsListExecuter()
    {
    CLOG_ENTERFN("CDownloadsListExecuter::~CDownloadsListExecuter");
    Cancel();
    if ( iDeleted != 0 )
        {
        *iDeleted = ETrue;
        }
    CLOG_LEAVEFN("CDownloadsListExecuter::~CDownloadsListExecuter");
    }

// -----------------------------------------------------------------------------
// CDownloadsListExecuter::Start
// -----------------------------------------------------------------------------
//
void CDownloadsListExecuter::Start()
    {
    CLOG_ENTERFN("CDownloadsListExecuter::Start");
    if ( !IsActive() )
        {
        CLOG_WRITE(" not active");
        SetActive();
        TRequestStatus* p = &iStatus;
        User::RequestComplete( p, KErrNone );
        }
    CLOG_LEAVEFN("CDownloadsListExecuter::Start");
    }

// -----------------------------------------------------------------------------
// CDownloadsListExecuter::DialogExists
// -----------------------------------------------------------------------------
//
TBool CDownloadsListExecuter::DialogExists() const
    {
    return ( iDialog != 0 );
    }

// -----------------------------------------------------------------------------
// CDownloadsListExecuter::Dialog
// -----------------------------------------------------------------------------
//
CDownloadsListDlg& CDownloadsListExecuter::Dialog() const
    {
    CLOG_WRITE_FORMAT(" iDialog: %x",iDialog);
    return *iDialog;
    }

// -----------------------------------------------------------------------------
// CDownloadsListExecuter::ExecuteLD
// -----------------------------------------------------------------------------
//
void CDownloadsListExecuter::ExecuteLD()
    {
    CLOG_ENTERFN("CDownloadsListExecuter::ExecuteLD");

    delete iDialog;
    iDialog = NULL;
    iDialog = CDownloadsListDlg::NewL( iDownloadsList, iProgressiveDownload );
    // No leaving functions before ExecuteLD!

    // If there is download that should be highlighted:
    if ( iHighlightDl )
        {
        iDialog->HighlightDownload( *iHighlightDl );
        }

    // Execute wait dialog... While it is running, 'this' can be deleted:
    TBool deleted( EFalse );
    iDeleted = &deleted;
    TRAP_IGNORE( iDialog->ExecuteLD() );
    iDeleted = 0; // No more needed.
    CLOG_WRITE_FORMAT(" deleted: %d",deleted);
    if ( deleted )
        {
        // Do not use any member of this object
        }
    else
        {
            if( !iDownloadsList.GetDownloadHide() )
                {
                delete iDialog;
                }

            iDownloadsList.SetDownloadListHide( EFalse );

            iDialog = NULL;
            // Reset owner pointer to this object.
       	    if ( iReferer )
            {
            *iReferer = 0;
            CLOG_WRITE(" referer set was to 0");
            }
        // and delete this
        delete this;
        }

    CLOG_LEAVEFN("CDownloadsListExecuter::ExecuteLD");
    }

// -----------------------------------------------------------------------------
// CDownloadsListExecuter::DoCancel
// -----------------------------------------------------------------------------
//
void CDownloadsListExecuter::DoCancel()
    {
    CLOG_ENTERFN("CDownloadsListExecuter::DoCancel");
    delete iDialog;
    iDialog = 0;
    CLOG_LEAVEFN("CDownloadsListExecuter::DoCancel");
    }

// -----------------------------------------------------------------------------
// CDownloadsListExecuter::RunL
// -----------------------------------------------------------------------------
//
void CDownloadsListExecuter::RunL()
    {
    ExecuteLD();
    }

// -----------------------------------------------------------------------------
// CDownloadsListExecuter::RunError
// -----------------------------------------------------------------------------
//
TInt CDownloadsListExecuter::RunError(TInt /*aError*/)
    {
    CLOG_ENTERFN("CDownloadsListExecuter::RunError");

    // Reset owner pointer to this object.
    if ( iReferer )
        {
        *iReferer = 0;
        }
    delete this;

    CLOG_LEAVEFN("CDownloadsListExecuter::RunError");
    return 0;
    }

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CDownloadsListExtension::CDownloadsListExtension
// -----------------------------------------------------------------------------
//
CDownloadsListExtension::CDownloadsListExtension
    ( CDownloadMgrUiDownloadsList& aDownloadsList )
:   iDownloadsList( aDownloadsList ),
    iIapId( 0 ),
    iIapIdGotAndSet( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CDownloadsListExtension::ConstructL
// -----------------------------------------------------------------------------
//
void CDownloadsListExtension::ConstructL()
    {
	iUpdateProgressInfoTimer = CPeriodic::NewL(CActive::EPriorityStandard + 1 );
	iIsUpdateInfoAllowed = EFalse;
    }

// -----------------------------------------------------------------------------
// CDownloadsListExtension::NewL
// -----------------------------------------------------------------------------
//
CDownloadsListExtension* CDownloadsListExtension::NewL
    ( CDownloadMgrUiDownloadsList& aDownloadsList )
    {
    CDownloadsListExtension* self =
        new (ELeave) CDownloadsListExtension( aDownloadsList );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CDownloadsListExtension::~CDownloadsListExtension()
    {
    CLOG_ENTERFN("CDownloadsListExtension::~CDownloadsListExtension");
	CancelProgressTimer();
	delete iUpdateProgressInfoTimer;
    delete iDownloadsListExecuter;
    iDownloadsListExecuter = 0;
    if ( iDeleted != 0 )
        {
        *(iDeleted) = ETrue;
        }
    CLOG_LEAVEFN("CDownloadsListExtension::~CDownloadsListExtension");
    }

#ifdef __WINS__
// -----------------------------------------------------------------------------
// CDownloadsListExtension::EnsureIapIdIsSetL
// -----------------------------------------------------------------------------
//
void CDownloadsListExtension::EnsureIapIdIsSetL()
    {
    // We chanck iIapIdGotAndSet, because only one IAP ID can be
    // set to a DownloadMgr session.
    if ( !iIapIdGotAndSet )
        {
        iIapIdGotAndSet = AskIapIdL( iIapId );
        if ( iIapIdGotAndSet )
            {
            User::LeaveIfError
                ( iDownloadsList.iRegistryModel.DownloadMgr().SetIntAttribute
                                              ( EDlMgrIap, (TInt32)iIapId ) );
            }
        }
    }
#endif

// -----------------------------------------------------------------------------
// CDownloadsListExtension::NotifyHandlerExit
// -----------------------------------------------------------------------------
//
void CDownloadsListExtension::NotifyHandlerExit
    ( RHttpDownload* /*aDownload*/, TInt aReason )
    {
    CLOG_ENTERFN("CDownloadsListExtension::NotifyHandlerExit");

    // Propagate the event to the registered observer, too
    if ( iDownloadsList.iServerAppExitObserver != 0 )
        {
        CLOG_WRITE(" Propagating event...");
        iDownloadsList.iServerAppExitObserver->HandleServerAppExit( aReason );
        }

    CLOG_LEAVEFN("CDownloadsListExtension::NotifyHandlerExit");
    }

// -----------------------------------------------------------------------------
// CDownloadsListExtension::DisplayDownloadsListL
// -----------------------------------------------------------------------------
//
void CDownloadsListExtension::DisplayDownloadsListL()
    {
    CLOG_ENTERFN("CDownloadsListExtension::DisplayDownloadsListL");

    if ( iDownloadsListExecuter )
        {
        // Do nothing if visible?
        CLOG_WRITE(" already visible");
        }
    else
        {
        iDownloadsListExecuter =
            CDownloadsListExecuter::NewL( iDownloadsList, &iDownloadsListExecuter );
        iDownloadsListExecuter->ExecuteLD();
        }

    CLOG_LEAVEFN("CDownloadsListExtension::DisplayDownloadsListL");
    }

// -----------------------------------------------------------------------------
// CDownloadsListExtension::DisplayDownloadsListAsynchronouslyL
// -----------------------------------------------------------------------------
//
void CDownloadsListExtension::DisplayDownloadsListAsynchronouslyL()
    {
    CLOG_ENTERFN("CDownloadsListExtension::DisplayDownloadsListAsynchronouslyL");

    if ( iDownloadsListExecuter )
        {
        // Do nothing if visible?
        CLOG_WRITE(" already visible");
        }
    else
        {
        iDownloadsListExecuter =
            CDownloadsListExecuter::NewL( iDownloadsList, &iDownloadsListExecuter );
        iDownloadsListExecuter->Start();
        }

    CLOG_LEAVEFN("CDownloadsListExtension::DisplayDownloadsListAsynchronouslyL");
    }

// -----------------------------------------------------------------------------
// CDownloadsListExtension::DisplayDownloadsListAsynchronouslyL
// -----------------------------------------------------------------------------
//
void CDownloadsListExtension::DisplayDownloadsListAsynchronouslyL
    ( RHttpDownload& aHighlightDl )
    {
    CLOG_ENTERFN("CDownloadsListExtension::DisplayDownloadsListAsynchronouslyL2");

    if ( iDownloadsListExecuter )
        {
        // Do nothing if visible?
        CLOG_WRITE(" already visible");
        }
    else
        {
        iDownloadsListExecuter = CDownloadsListExecuter::NewL
            ( iDownloadsList, &iDownloadsListExecuter, aHighlightDl );
        iDownloadsListExecuter->Start();
        }

    CLOG_LEAVEFN("CDownloadsListExtension::DisplayDownloadsListAsynchronouslyL2");
    }

// -----------------------------------------------------------------------------
// CDownloadsListExtension::CancelDisplayingDownloadsList
// -----------------------------------------------------------------------------
//
void CDownloadsListExtension::CancelDisplayingDownloadsList()
    {
    CLOG_ENTERFN("CDownloadsListExtension::CancelDisplayingDownloadsList");

    if ( !iDownloadsListExecuter )
        {
        // Do nothing if not visible
        CLOG_WRITE(" Not visible");
        }
    else
        {
        iDownloadsListExecuter->Cancel();
        // Cancel() sets iDownloadsListExecuter to 0.
        }

    CLOG_LEAVEFN("CDownloadsListExtension::CancelDisplayingDownloadsList");
    }

// -----------------------------------------------------------------------------
// CDownloadsListExtension::IsDialogVisible
// -----------------------------------------------------------------------------
//
TBool CDownloadsListExtension::IsDialogVisible() const
    {
    TBool isDialogVisible = EFalse;
    if ( iDownloadsListExecuter != 0 )
        {
        isDialogVisible = iDownloadsListExecuter->DialogExists();
        }
    return isDialogVisible;
    }

// -----------------------------------------------------------------------------
// CDownloadsListExtension::DlExecuter
// -----------------------------------------------------------------------------
//
CDownloadsListExecuter& CDownloadsListExtension::DlExecuter() const
    {
    CLOG_WRITE_FORMAT(" iDownloadsListExecuter: %x",iDownloadsListExecuter);
    return *iDownloadsListExecuter;
    }

// -----------------------------------------------------------------------------
// CDownloadsListExtension::StartProgressTimer
// -----------------------------------------------------------------------------
//
void CDownloadsListExtension::StartProgressTimer()
{
	iUpdateProgressInfoTimer->Start(
		0,
		KUpdateProgressInfoInterval,
		TCallBack( UpdateProgressInfoCallback, this ) );
}


// -----------------------------------------------------------------------------
// CDownloadsListExtension::CancelProgressTimer
// -----------------------------------------------------------------------------
//
void CDownloadsListExtension::CancelProgressTimer()
{
	iUpdateProgressInfoTimer->Cancel();
	iIsUpdateInfoAllowed = EFalse;
}

// -----------------------------------------------------------------------------
// CDownloadsListExtension::UpdateProgressInfoCallback
// callback function for the TimerUpdateProgressInfo
// -----------------------------------------------------------------------------
//
TInt CDownloadsListExtension::UpdateProgressInfoCallback(TAny* aAny )
{
	CDownloadsListExtension* instance = (CDownloadsListExtension*) aAny;
	instance->iIsUpdateInfoAllowed = ETrue;
    return EFalse;
}
// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::CDownloadMgrUiDownloadsList
// -----------------------------------------------------------------------------
//
CDownloadMgrUiDownloadsList::CDownloadMgrUiDownloadsList
    ( CDownloadMgrUiLibRegistry& aRegistryModel )
:   CDownloadMgrUiBase( aRegistryModel ),
    iIsVisible( EFalse ),
    iIsCancelInProgress( EFalse ),
    iPlatformSupportsGallery( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::ConstructL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::ConstructL()
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadsList::ConstructL");

    BaseConstructL();
    CLOG_WRITE(" BaseConstructL");
    iExtension = CDownloadsListExtension::NewL( *this );
    CLOG_WRITE(" iExtension");
    iDownloadMenu = CDownloadMgrUiDownloadMenu::NewL( iCoeEnv );
    CLOG_WRITE(" iDownloadMenu");
    iUiUtils = CUserInteractionsUtils::NewL( *this, iRegistryModel );
    CLOG_WRITE(" iUiUtils");
    iEventHandlerArray = new(ELeave)CAsyncEventHandlerArray();
    CLOG_WRITE(" iEventHandlerArray");
	iIsTimerStarted = EFalse;
	iProgressiveDownload = EFalse;
	iRegistryModel.DownloadMgr().GetBoolAttribute(EDlMgrProgressiveDownload, iProgressiveDownload);
    CLOG_LEAVEFN("CDownloadMgrUiDownloadsList::ConstructL");

    //whether the platform supports gallery app or not; defined in browser_platfrom_variant.hrh
    #ifdef BRDO_APP_GALLERY_SUPPORTED_FF
    iPlatformSupportsGallery = ETrue;
    #endif

    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::NewL
// -----------------------------------------------------------------------------
//
CDownloadMgrUiDownloadsList* CDownloadMgrUiDownloadsList::NewL
( CDownloadMgrUiLibRegistry& aRegistryModel )
    {
    CDownloadMgrUiDownloadsList* self =
        new ( ELeave ) CDownloadMgrUiDownloadsList( aRegistryModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CDownloadMgrUiDownloadsList::~CDownloadMgrUiDownloadsList()
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadsList::~CDownloadMgrUiDownloadsList");

    CancelDisplayingDownloadsList();
    delete iDialog;
    iDialog = 0;
    CLOG_WRITE(" iDialog");
    delete iEventHandlerArray;
    iEventHandlerArray = 0;
    CLOG_WRITE(" iEventHandlerArray");
    delete iListModel;
    iListModel = NULL;
    CLOG_WRITE(" iListModel");
    delete iUiUtils;
    iUiUtils = NULL;
    CLOG_WRITE(" iUiUtils");
    delete iDownloadMenu;
    iDownloadMenu = NULL;
    CLOG_WRITE(" iDownloadMenu");
    // iExtension should be the last deleted.
    delete iExtension;
    iExtension = 0;
    CLOG_WRITE(" iExtension");
    delete iAIWServiceHandler;
    CLOG_LEAVEFN("CDownloadMgrUiDownloadsList::~CDownloadMgrUiDownloadsList");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::DownloadMenu
// -----------------------------------------------------------------------------
//
EXPORT_C
CDownloadMgrUiDownloadMenu& CDownloadMgrUiDownloadsList::DownloadMenu() const
    {
    __ASSERT_DEBUG( iDownloadMenu, Panic( EUiLibPanNull ) );
    return(*iDownloadMenu);
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::DisplayDownloadsListL
// -----------------------------------------------------------------------------
//
EXPORT_C
void CDownloadMgrUiDownloadsList::DisplayDownloadsListL()
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadsList::DisplayDownloadsListL");

    long noMediaDls = 0;
    iRegistryModel.DownloadMgr().GetIntAttribute( EDlMgrNoMediaDls, noMediaDls );
    TInt dlCount = iRegistryModel.DownloadMgr().CurrentDownloads().Count();

    if (dlCount > noMediaDls)
    	iExtension->DisplayDownloadsListAsynchronouslyL();

    CLOG_LEAVEFN("CDownloadMgrUiDownloadsList::DisplayDownloadsListL");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::DisplayDownloadsListL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::DisplayDownloadsListL
    ( RHttpDownload& aHighlightDl )
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadsList::DisplayDownloadsListL aHighlightDl");
    iExtension->DisplayDownloadsListAsynchronouslyL( aHighlightDl );
    CLOG_LEAVEFN("CDownloadMgrUiDownloadsList::DisplayDownloadsListL aHighlightDl");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::CancelDisplayingDownloadsList
// -----------------------------------------------------------------------------
//
EXPORT_C
void CDownloadMgrUiDownloadsList::CancelDisplayingDownloadsList()
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadsList::CancelDisplayingDownloadsList");
    delete iDialog;
    iDialog = 0;
    CLOG_WRITE(" iDialog");
    iExtension->CancelDisplayingDownloadsList();
    CLOG_LEAVEFN("CDownloadMgrUiDownloadsList::CancelDisplayingDownloadsList");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::IsVisible
// -----------------------------------------------------------------------------
//
EXPORT_C
TBool CDownloadMgrUiDownloadsList::IsVisible() const
    {
    return iExtension->IsDialogVisible();
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::Count
// -----------------------------------------------------------------------------
//
EXPORT_C
TInt CDownloadMgrUiDownloadsList::Count() const
    {
    return ((iListModel!=NULL)?iListModel->Count():0);
    }

// Not yet supported functions
EXPORT_C
void CDownloadMgrUiDownloadsList::GetIntAttributeL( const TUint /*aAttribute*/, TInt32& /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }

EXPORT_C
void CDownloadMgrUiDownloadsList::GetBoolAttributeL( const TUint /*aAttribute*/, TBool& /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }

EXPORT_C
void CDownloadMgrUiDownloadsList::GetStringAttributeL( const TUint /*aAttribute*/, TDes16& /*aValue*/  )
    {
    User::Leave( KErrNotSupported );
    }

EXPORT_C
void CDownloadMgrUiDownloadsList::GetStringAttributeL( const TUint /*aAttribute*/, TDes8& /*aValue*/  )
    {
    User::Leave( KErrNotSupported );
    }

EXPORT_C
void CDownloadMgrUiDownloadsList::SetIntAttributeL( const TUint /*aAttribute*/, TInt32 /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }

EXPORT_C
void CDownloadMgrUiDownloadsList::SetBoolAttributeL( const TUint /*aAttribute*/, TBool /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }

EXPORT_C
void CDownloadMgrUiDownloadsList::SetStringAttributeL( const TUint /*aAttribute*/, const TDesC16& /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }

EXPORT_C
void CDownloadMgrUiDownloadsList::SetStringAttributeL( const TUint /*aAttribute*/, const TDesC8& /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::AddDownloadsToListModelL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::AddDownloadsToListModelL
( CDownloadsListArray& aLBModel, RHttpDownloadMgr& aDownloadMgr )
    {
    TInt downloads = aDownloadMgr.CurrentDownloads().Count();
    for ( TInt i = 0; i < downloads; ++i )
        {
        RHttpDownload* download =
            (RHttpDownload*)(aDownloadMgr.CurrentDownloads().At(i));
        // Extract the necessary information from 'download' and
        // add a list box item.
        (void)AddDownloadToListModelL( aLBModel, *download );
        }
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::AddDownloadToListModelL
// This is a very expensive method, so when updating list box items,
// call the less expensive 'update' methods!
// -----------------------------------------------------------------------------
//
TInt CDownloadMgrUiDownloadsList::AddDownloadToListModelL
    ( CDownloadsListArray& aLBModel, RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadsList::AddDownloadToListModelL");
    CLOG_WRITE_FORMAT(" aDownload: 0x%x",&aDownload);

    TInt itemIndex( KErrNotFound );
    if ( aLBModel.Find( aDownload, itemIndex ) == KErrNone )
        {
        // It is already on the list - do not add it again.
        CLOG_WRITE_FORMAT(" aDownload already on list (%d)",itemIndex);
        }
    else
        {
        // Do not add the download to the list if the storage
        // media is not in the device.
        TBool noMedia( EFalse );
        User::LeaveIfError( aDownload.GetBoolAttribute
                          ( EDlAttrNoMedia, noMedia ) );
        CLOG_WRITE_FORMAT(" EDlAttrNoMedia: %d",noMedia);
        // Do not add the download to the list if it is hidden
        TBool isHidden( EFalse );
        User::LeaveIfError( aDownload.GetBoolAttribute
                          ( EDlAttrHidden, isHidden ) );
        CLOG_WRITE_FORMAT(" EDlAttrHidden: %d",isHidden);

        if ( noMedia || isHidden )
            {
            // Do not add it to the list - hide it from the user.
            }
        else
            {
            // Create the holder for the info of the download.
            // Quite big structure, allocate it on the heap!
            TDownloadUiData* dlData = new (ELeave) TDownloadUiData;
            CleanupStack::PushL( dlData );

            TInt err(KErrNone);

            //      Get progress info
            err = aDownload.GetIntAttribute
                ( EDlAttrState, dlData->iDownloadState );
            CLOG_WRITE_FORMAT(" EDlAttrState err: %d",err);
            User::LeaveIfError( err );

            //Changes for the bug JERI-7P8CF2
            //Changes made in the server side to fix the video center receiving unexpected events
            //Reassigning these events back to the changes done in server side
            if(dlData->iDownloadState  == EHttpDlCompleted )
		        {
                 	dlData->iDownloadState  = EHttpDlMultipleMOCompleted;
		        }
            else if(dlData->iDownloadState  == EHttpDlFailed )
		        {
		    	 dlData->iDownloadState  = EHttpDlMultipleMOFailed;
		        }

            err = aDownload.GetIntAttribute
                ( EDlAttrProgressState, dlData->iProgressState );
            CLOG_WRITE_FORMAT(" EDlAttrProgressState err: %d",err);
            User::LeaveIfError( err );
            err = aDownload.GetIntAttribute
                ( EDlAttrLength, dlData->iFullSize );
            CLOG_WRITE_FORMAT(" EDlAttrLength err: %d",err);
            if ( err != KErrNone && err != KErrNotFound )
                {
                User::LeaveIfError( err );
                }

            // Number of media objects in download ( inalbum)?
            err = aDownload.GetIntAttribute
                ( EDlAttrNumMediaObjects, dlData->iNumMediaObjects );
            CLOG_WRITE_FORMAT(" EDlAttrNumMediaObjects err: %d",err);
            User::LeaveIfError( err );

            //      Get file name
            // In case of album - if download is complete, query album name,
            // otherwise query track name
            if ((dlData->iNumMediaObjects > 1) &&
                (EHttpDlMultipleMOCompleted == dlData->iDownloadState))
                {
                err = aDownload.GetStringAttribute
                    ( EDlAttrAlbumName, dlData->iName );
                CLOG_WRITE_FORMAT(" EDlAttrAlbumName err: %d",err);
                }
            else
                {
                err = aDownload.GetStringAttribute
                    ( EDlAttrName, dlData->iName );
                CLOG_WRITE_FORMAT(" EDlAttrName err: %d",err);
                }

            if ( err != KErrNone && err != KErrNotFound )
                {
                User::LeaveIfError( err );
                }
            CLOG_WRITE_FORMAT(" EDlAttrName: %S",&(dlData->iName));

            // If download name is empty, use the file name got from URI
            if ( dlData->iName.Length() == 0 )
                {
                HBufC* url = HBufC::NewLC( KMaxUrlLength );
                TPtr urlPtr = url->Des();
                err = aDownload.GetStringAttribute( EDlAttrCurrentUrl, urlPtr );
                CLOG_WRITE_FORMAT(" EDlAttrCurrentUrl err: %d",err);
                if ( err != KErrNone && err != KErrNotFound )
                    {
                    User::LeaveIfError( err );
                    }
                CLOG_WRITE_FORMAT(" EDlAttrCurrentUrl: %S",url);
                if ( url->Length() > 0 )
                    {
                    // Extract file name, if possible
                    CUri8* converted = UriUtils::CreateUriL( *url );
                    CleanupStack::PushL( converted );
                    HBufC* extracted = converted->Uri().GetFileNameL( EUriFileNameTail );
                    CleanupStack::PushL( extracted );
                    // Use this for download name
                    // (Left() is used to avoid overflow.)
                    dlData->iName = extracted->Left( dlData->iName.MaxLength() );
                    CleanupStack::PopAndDestroy( extracted );
                    CleanupStack::PopAndDestroy( converted );
                    }
                CleanupStack::PopAndDestroy( url );
                }

            //      Get content type
            HBufC8* contentType;
            if ((dlData->iNumMediaObjects > 1) &&
                (EHttpDlMultipleMOCompleted == dlData->iDownloadState))
                {
                contentType = iUiUtils->ContentTypeL( aDownload, ETrue, KFirstMoIndex );
                }
            else
                {
                contentType = iUiUtils->ContentTypeL( aDownload, ETrue );
                }
            // Not pushed to cleanup stack - unnecessary.
            // There is a limit for dlData.iContentType
            if ( contentType->Length() > KMaxContentTypeLength )
                {
                // What can we do? Use an empty mime string.
                CLOG_WRITE_FORMAT(" Length overflow: %d",contentType->Length());
                delete contentType;
                contentType = 0;
                contentType = KNullDesC8().AllocL();
                }
            dlData->iContentType.Copy( *contentType );
            delete contentType;
            contentType = 0;

            // Downloaded size is always known, so it can leave.
            err = aDownload.GetIntAttribute
                ( EDlAttrDownloadedSize, dlData->iDownloadedSize );
            CLOG_WRITE_FORMAT(" EDlAttrDownloadedSize err: %d",err);
            User::LeaveIfError( err );

            // Is the download pausable?
            User::LeaveIfError( aDownload.GetBoolAttribute
                ( EDlAttrPausable, dlData->iPausable ) );
            CLOG_WRITE_FORMAT(" EDlAttrPausable: %d",dlData->iPausable);

            // Is the download on an external/removable memory (like MMC)?
            err = aDownload.GetBoolAttribute
                ( EDlAttrDestRemovable, dlData->iIsOnExternalMemory );
            CLOG_WRITE_FORMAT(" EDlAttrDestRemovable err: %d",err);
            // 'err' is ignored.
            CLOG_WRITE_FORMAT(" EDlAttrDestRemovable: %d",dlData->iIsOnExternalMemory);

            // Currently active media object's index (in album)?
            err = aDownload.GetIntAttribute
                ( EDlAttrActiveDownload, dlData->iActiveMoIndex );
            CLOG_WRITE_FORMAT(" EDlAttrActiveDownload err: %d",err);
            User::LeaveIfError( err );

            // First add the handler app's icon to model, so that we then
            // know the index of it that can then be set in the item text,
            // which is then also added to the model.
            dlData->iIconIndex =
                iListModel->AddHandlerAppIconL( dlData->iContentType );
            CLOG_WRITE(" AddHandlerAppIconL OK");

            // Add now the structure to the model:
            itemIndex = aLBModel.AppendL( *dlData, aDownload );
            CleanupStack::PopAndDestroy( dlData ); // dlData
            }
        }

    CLOG_LEAVEFN("CDownloadMgrUiDownloadsList::AddDownloadToListModelL");
    return ( itemIndex );
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::DeleteDownloadL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::DeleteDownloadL( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadsList::DeleteDownloadL");

    // Remove from the 'model' and update the 'view' (list) if visible.
    if ( iListModel == NULL )
        {
        // Do nothing.
        CLOG_WRITE(" iListModel == NULL");
        }
    else
        {
        TInt index( 0 );
        TInt err = iListModel->Find( aDownload, index );
        CLOG_WRITE_FORMAT(" err: %d",err);
        CLOG_WRITE_FORMAT(" index: %d",index);
        if ( !err )
            {
            if ( iListModel->Count() == 1 )
                {
                // Close the list before deleting the last item to avoid the visibility of 'No data' text.
                if ( iExtension->IsDialogVisible() )
                    {
                    iExtension->DlExecuter().Dialog().CancelPopup();
                    CLOG_WRITE(" CancelPopup OK");
                    }
                iListModel->Delete( index );
                }
            else
                {
                iListModel->Delete( index );

                // Update view
                if ( iExtension->IsDialogVisible() )
                    {
                    iExtension->DlExecuter().Dialog().HandleModelChangeL( EDownloadRemoved, index );
                    CLOG_WRITE(" HandleModelChangeL OK");
                    }
                }
            }
        }

    CLOG_LEAVEFN("CDownloadMgrUiDownloadsList::DeleteDownloadL");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::DeleteDownloadL
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiDownloadsList::IsOneProgressive()
   {
   TBool anyOneProgress = EFalse;
   if (iListModel == NULL)
      {
   	  return EFalse;
      }
   else
      {
      TInt count = Count();
      for ( TInt i = 0; i < count; ++i )
         {
      	 RHttpDownload& download = iListModel->Download(i);
         TInt err = download.GetBoolAttribute( EDlAttrProgressive, anyOneProgress );
          // If there is a COD download and check if it is attached. Then set it as attached while resuming
         TBool isCodDownload;
         download.GetBoolAttribute(EDlAttrCodDownload,isCodDownload);
         if(isCodDownload && anyOneProgress )
            {
            download.SetBoolAttribute( EDlAttrProgressive, ETrue );
            }
         THttpProgressState state;
         if( anyOneProgress )
         	{
         	download.GetIntAttribute(EDlAttrProgressState, (TInt32&)state);
         	if(state == EHttpProgContentFileMoved || state == EHttpProgContentFileMovedAndDestFNChanged )
         	    {
         	    anyOneProgress = 0;//once the move() has happened the progressive Download is finished
         	    }                  //and we can  bring MP to foreground and start another PD
           	}
         CLOG_WRITE_FORMAT(" EDlAttrProgressive err: %d",err);
         if ( anyOneProgress )
            return ETrue;
         }
      }
   return anyOneProgress;
   }


// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::NewDownloadCreatedL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::NewDownloadCreatedL( RHttpDownload& aDownload )
    {
    __ASSERT_DEBUG( iExtension->IsDialogVisible(), Panic( EUiLibPanCreatedNotVisible ) );
    // This download does not exist in the model yet, so add this.
    TInt indx = AddDownloadToListModelL( *iListModel, aDownload );
    iExtension->DlExecuter().Dialog().HandleModelChangeL( EDownloadAdded, indx );
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::UpdateProgressInfoL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::UpdateProgressInfoL
    ( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
    {
    __ASSERT_DEBUG( iExtension->IsDialogVisible(), Panic( EUiLibPanProgressNotVisible ) );
    // Update the download progress info at the current index.
    // First find the current download in the model.
    TInt index(0);
    TDownloadUiData* dlDataPtr = FindDlUiData( *iListModel, aDownload, index );
    if ( !dlDataPtr )
        {
        // do nothing
        }
    else
        {
        // Create a local copy
        TDownloadUiData dlData = *dlDataPtr;

        // Track changed? We need to change icon of new track
        TInt32 moIndex(0);
        User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrActiveDownload, moIndex ) );
        TBool activeIndexChanged = (dlData.iActiveMoIndex != moIndex);

        // Update download state - this must always be done!
        dlData.iDownloadState = aEvent.iDownloadState;
		dlData.iProgressState = aEvent.iProgressState;
		dlData.iActiveMoIndex = moIndex;
        aDownload.GetStringAttribute( EDlAttrName, dlData.iName );
        // Permanent redirection changes the filename.
        if ( aEvent.iProgressState == EHttpProgRedirectedPermanently ||
             aEvent.iProgressState == EHttpProgRedirectedTemporarily )
            {
            CLOG_WRITE(" Redirection");
            }
        else if ( aEvent.iProgressState == EHttpProgResponseHeaderReceived )
        	{
            // Is the download on an external/removable memory (like MMC)?
            aDownload.GetBoolAttribute
                ( EDlAttrDestRemovable, dlData.iIsOnExternalMemory );
            // Return value is ignored.
            CLOG_WRITE_FORMAT(" EDlAttrDestRemovable: %d",dlData.iIsOnExternalMemory);
        	}
        else if ( aEvent.iProgressState == EHttpProgDlNameChanged )
            {
            CLOG_WRITE(" EHttpProgDlNameChanged");
            }

        if (( aEvent.iProgressState == EHttpProgContentTypeChanged ) || activeIndexChanged )
            {
            // Update handler application icon
            // Also update the content type, because the handler application
            // icon depends from this.
            CLOG_WRITE_FORMAT(" EHttpProgContentTypeChanged: ActiveIndex - %d",dlData.iActiveMoIndex);
            HBufC8* contentType = iUiUtils->ContentTypeL( aDownload, ETrue );
            // Not pushed to cleanup stack - unnecessary.
            // There is a limit for dlData.iContentType
            if ( contentType->Length() > KMaxContentTypeLength )
                {
                // What can we do? Use an empty mime string.
                CLOG_WRITE_FORMAT(" Length overflow: %d",contentType->Length());
                delete contentType;
                contentType = 0;
                contentType = KNullDesC8().AllocL();
                }
            dlData.iContentType.Copy( *contentType );
            delete contentType;
            contentType = 0;
            dlData.iIconIndex = iListModel->AddHandlerAppIconL( dlData.iContentType );
            }
        // Update the progress info.
        User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrDownloadedSize,
                                                       dlData.iDownloadedSize ) );
        User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrLength,
                                                       dlData.iFullSize ) );
        // Update the download ui data in the 'model'.
        iListModel->UpdateL( index, dlData );
        // Update the 'view'
        iExtension->DlExecuter().Dialog().HandleModelChangeL( EDownloadChanged, index );
        }
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::DownloadPausedL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::DownloadPausedL
    ( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadsList::DownloadPausedL");

    __ASSERT_DEBUG( iExtension->IsDialogVisible(), Panic( EUiLibPanPausedNotVisible ) );
    // First find the current download in the model.
    TInt index(0);
    TDownloadUiData* dlDataPtr = FindDlUiData( *iListModel, aDownload, index );
    if ( !dlDataPtr )
        {
        // do nothing
        }
    else
        {
        // Create a local copy
        TDownloadUiData dlData = *dlDataPtr;

        // Update download state - this must always be done!
        dlData.iDownloadState = aEvent.iDownloadState;
		dlData.iProgressState = aEvent.iProgressState;
		User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrActiveDownload, dlData.iActiveMoIndex ) );
		aDownload.GetStringAttribute( EDlAttrName, dlData.iName );
        // Here we have to initialize the full size, if the
        // content-type header arrives!!
        if ( aEvent.iProgressState == EHttpContentTypeReceived )
            {
            TInt err = aDownload.GetIntAttribute( EDlAttrLength, dlData.iFullSize );
            if ( err != KErrNone && err != KErrNotFound )
                {
                User::LeaveIfError( err );
                }

            // Also update the content type, because the handler application
            // icon depends from this.
            HBufC8* contentType = iUiUtils->ContentTypeL( aDownload, ETrue );
            // Not pushed to cleanup stack - unnecessary.
            // There is a limit for dlData.iContentType
            if ( contentType->Length() > KMaxContentTypeLength )
                {
                // What can we do? Use an empty mime string.
                CLOG_WRITE_FORMAT(" Length overflow: %d",contentType->Length());
                delete contentType;
                contentType = 0;
                contentType = KNullDesC8().AllocL();
                }
            dlData.iContentType.Copy( *contentType );
            delete contentType;
            contentType = 0;
            dlData.iIconIndex = iListModel->AddHandlerAppIconL( dlData.iContentType );
            }

        // Update downloaded size (introduced, because for non-pausable downloads
        // paused command sets downloaded size back to 0 kB).
        User::LeaveIfError( aDownload.GetIntAttribute
                          ( EDlAttrDownloadedSize, dlData.iDownloadedSize ) );

        // Update the download ui data in the 'model'.
        iListModel->UpdateL( index, dlData );
        // Update the 'view'
        iExtension->DlExecuter().Dialog().HandleModelChangeL( EDownloadChanged, index );
        // Also need to update the middle softkey
        iExtension->DlExecuter().Dialog().HandleMiddleSoftKeyChangeL();
        }

    CLOG_LEAVEFN("CDownloadMgrUiDownloadsList::DownloadPausedL");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::DownloadCompletedL
// This is very similar to UpdateProgressInfoL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::DownloadCompletedL
    ( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
    {
    __ASSERT_DEBUG( iExtension->IsDialogVisible(), Panic( EUiLibPanCompletedNotVisible ) );
    // Update the download progress info at the current index.
    // First find the current download in the model.
    TInt index(0);
    TDownloadUiData* dlDataPtr = FindDlUiData( *iListModel, aDownload, index );
    if ( !dlDataPtr )
        {
        // do nothing
        }
    else
        {
        // Create a local copy
        TDownloadUiData dlData = *dlDataPtr;
        // Update download state - this must always be done!
        dlData.iDownloadState = aEvent.iDownloadState;
		dlData.iProgressState = aEvent.iProgressState;
		User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrActiveDownload, dlData.iActiveMoIndex ) );

		// In case of Album download, change the name to Album name
		if (dlData.iNumMediaObjects > 1)
		    {
		    aDownload.GetStringAttribute( EDlAttrAlbumName, dlData.iName );
		    }
		else if (dlData.iProgressState == EHttpProgContentFileMovedAndDestFNChanged)
			// Filename changed, get the updated filename
		    {
			CLOG_WRITE(" EHttpProgDlNameChanged");
 	        aDownload.GetStringAttribute( EDlAttrName, dlData.iName );
		    }

        __ASSERT_DEBUG( aEvent.iDownloadState == EHttpDlMultipleMOCompleted,
                        Panic( EUiLibPanDlStateNotCompleted ) );

        if( dlData.iProgressState == EHttpProgContentFileMoved  || dlData.iProgressState == EHttpProgContentFileMovedAndDestFNChanged )
           {
            HBufC8* contentType = iUiUtils->ContentTypeL( aDownload, ETrue, KFirstMoIndex );
            if ( contentType->Length() > KMaxContentTypeLength )
                {
                // What can we do? Use an empty mime string.
                CLOG_WRITE_FORMAT(" Length overflow: %d",contentType->Length());
                delete contentType;
                contentType = 0;
                contentType = KNullDesC8().AllocL();
                }
            dlData.iContentType.Copy( *contentType );
            delete contentType;
            dlData.iIconIndex =
                iListModel->AddHandlerAppIconL( dlData.iContentType );

            }
        // Unnecessary to update dlData.iDownloadedSize, as it is not displayed.
        // Update the download ui data in the 'model'.
        iListModel->UpdateL( index, dlData );
        // Update the 'view'
        iExtension->DlExecuter().Dialog().HandleModelChangeL( EDownloadChanged, index );
        // Also need to update the middle softkey
        iExtension->DlExecuter().Dialog().HandleMiddleSoftKeyChangeL();
        }
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::DownloadFailedL
// This is very similar to UpdateProgressInfoL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::DownloadFailedL
    ( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
    {
    __ASSERT_DEBUG( iExtension->IsDialogVisible(), Panic( EUiLibPanFailedNotVisible ) );
    // Update the download progress info at the current index.
    // First find the current download in the model.
    TInt index(0);
    TDownloadUiData* dlDataPtr = FindDlUiData( *iListModel, aDownload, index );
    if ( !dlDataPtr )
        {
        // do nothing
        }
    else
        {
        // Create a local copy
        TDownloadUiData dlData = *dlDataPtr;
        // Update download state - this must always be done!
        dlData.iDownloadState = aEvent.iDownloadState;
		dlData.iProgressState = aEvent.iProgressState;
		User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrActiveDownload, dlData.iActiveMoIndex ) );

        // By default, update the progress info. Only the downloaded size changes.
        User::LeaveIfError( aDownload.GetIntAttribute( EDlAttrDownloadedSize,
                                                       dlData.iDownloadedSize ) );
        // Update the download ui data in the 'model'.
        iListModel->UpdateL( index, dlData );
        // Update the 'view'
        iExtension->DlExecuter().Dialog().HandleModelChangeL( EDownloadChanged, index );
        }
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::PauseableStateChangedL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::PauseableStateChangedL( RHttpDownload& aDownload, TBool aPausable )
    {
    __ASSERT_DEBUG( iExtension->IsDialogVisible(), Panic( EUiLibPanFailedNotVisible ) );
    // Update the download progress info at the current index.
    // First find the current download in the model.
    TInt index(0);
    TDownloadUiData* dlDataPtr = FindDlUiData( *iListModel, aDownload, index );
    if ( !dlDataPtr )
        {
        // do nothing
        }
    else
        {
        // Create a local copy
        TDownloadUiData dlData = *dlDataPtr;
        // Update download state
        dlData.iPausable = aPausable;
        // Update the download ui data in the 'model'.
        iListModel->UpdateL( index, dlData );
        }
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::HideMenu
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::HideMenu()
    {
    if ( iExtension->DlExecuter().Dialog().MenuShowing() )
        {
        iExtension->DlExecuter().Dialog().HideMenu();
        }
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::HandleDMgrEventL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::HandleDMgrEventL
    ( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
    {
    // Implementation info: iEventHandlerArray is not used yet.

    if ( iExtension->IsDialogVisible() == EFalse )
        {
        // Don't respond.
        return;
        }

    CLOG_WRITE_EVENT("DlList",&aDownload,aEvent);

    switch ( aEvent.iDownloadState )
        {
        case EHttpDlCreated:
            {
            // Add new download to the list
            NewDownloadCreatedL( aDownload );
            break;
            }

        case EHttpDlInprogress:
            {
				// Update progress info every KUpdateProgressInfoInterval
				if (!iIsTimerStarted )
                {
					iIsTimerStarted = ETrue;
					// start the timer.
					iExtension->StartProgressTimer();
                }

				if (iExtension->iIsUpdateInfoAllowed)
                {
					UpdateProgressInfoL( aDownload, aEvent );
					iExtension->iIsUpdateInfoAllowed = EFalse;
                }
            break;
            }

        case EHttpDlPaused:
            {
            DownloadPausedL( aDownload, aEvent );
            break;
            }

        case EHttpDlMultipleMOCompleted:
            {
            DownloadCompletedL( aDownload, aEvent );
            break;
            }

        case EHttpDlMultipleMOFailed:
            {
            DownloadFailedL( aDownload, aEvent );
            break;
            }

        case EHttpDlMoved:
            {
            // Download moved to this session.
            // Add it to the list
            NewDownloadCreatedL( aDownload );
            break;
            }

        case EHttpDlMediaRemoved:
            {
            // Remove the download from the list
            DeleteDownloadL( aDownload );
            break;
            }

        case EHttpDlMediaInserted:
            {
            // Add new download to the list
            NewDownloadCreatedL( aDownload );
            break;
            }

        case EHttpDlPausable:
        case EHttpDlNonPausable:
            {
            // Update the download UI data
            PauseableStateChangedL( aDownload, aEvent.iDownloadState==EHttpDlPausable );

            // No need to reopen menu as we no more allow manually
            // pausing a download - PHAN-6KVK5R
            /*
            // Update menu if opened
            if ( iExtension->DlExecuter().Dialog().MenuShowing() )
                {
                // Hide & reopen: DynInitMenuPaneL is called
                iExtension->DlExecuter().Dialog().HideMenu();
                iExtension->DlExecuter().Dialog().DisplayMenuL();
                }
            */
            break;
            }

        case EHttpDlDeleted:
            {
            // Download is deleted from another client instance.
            DeleteDownloadL( aDownload );
            CLOG_WRITE(" EHttpDlDeleted - DeleteDownloadL OK");
            break;
            }

        case EHttpDlAlreadyRunning:
            {
            // Nothing to do.
            CLOG_WRITE(" EHttpDlAlreadyRunning - do nothing");
            break;
            }

        case EHttpDlDeleting:
            {
            // Download is going to be deleted.
            iUiUtils->CancelCancelConfirmationDialog();
            iUiUtils->CancelDeleteConfirmationDialog();
            DeleteDownloadL( aDownload );
            CLOG_WRITE(" EHttpDlDeleting - DeleteDownloadL OK");
            break;
            }

        default:
            {
            break;
            }
        }
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::PreLayoutDynInitL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::PreLayoutDynInitL( CDownloadsListDlg& aDialog )
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadsList::PreLayoutDynInitL");

    __ASSERT_DEBUG( iExtension->IsDialogVisible(), Panic( EUiLibPanNull ) );

    delete iListModel;
    iListModel = NULL;
    iListModel = CDownloadsListArray::NewL();
    CLOG_WRITE(" NewL OK");

    RHttpDownloadMgr& dMgr = iRegistryModel.DownloadMgr();
    AddDownloadsToListModelL( *iListModel, dMgr );
    CLOG_WRITE(" AddDownloadsToListModelL OK");
    //
    aDialog.SetModelL( *iListModel );

    CLOG_LEAVEFN("CDownloadMgrUiDownloadsList::PreLayoutDynInitL");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::ProcessCommandL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::ProcessCommandL
( CDownloadsListDlg& aDialog, TInt aCommandId )
    {
    __ASSERT_DEBUG( iExtension->IsDialogVisible(), Panic( EUiLibPanNull ) );
    __ASSERT_DEBUG( iListModel, Panic( EUiLibPanNoDownloadsListModel ) );

    RHttpDownload& currDownload = iListModel->Download( aDialog.CurrentItemIndex() );

    if( !iAIWServiceHandler )
        {
        AttachAIWInterestL();
        }

    TInt aiwServiceId( iAIWServiceHandler->ServiceCmdByMenuCmd( aCommandId ) );
    if ( aiwServiceId != KAiwCmdNone )
        {
        HandleAIWCommandL(aCommandId, currDownload);
        return;
        }

    switch ( aCommandId )
        {
        case EAknSoftkeyOpen: // handle middlesoft key
            {
            // First close the downloads list.
            CancelDisplayingDownloadsList();
            CLOG_WRITE(" CancelDisplayingDownloadsList OK");

			if(iProgressiveDownload)
			{
            // check if it's pd supported
            TBool isPdSupported = EFalse;
            // Get the PD player application's UID for the download
            TUid pdPlayerUid = { 0 };
            HBufC8* contentType = iUiUtils->ContentTypeL( currDownload, ETrue, KFirstMoIndex  );
            TDataType dataType( *contentType );
            delete contentType;
            contentType = NULL;
            CDocumentHandler* docHandler = CDocumentHandler::NewLC();
            isPdSupported = docHandler->CanHandleProgressivelyL( dataType, pdPlayerUid );
            CleanupStack::PopAndDestroy( docHandler ); // docHandler

            if ( isPdSupported && iUiUtils->IsNetworkPdCompatibleL() )
                {
                // Get the UI data for the current download
                TDownloadUiData& dlData = iListModel->DlUiData( aDialog.CurrentItemIndex() );
                TInt32 state( dlData.iProgressState );
                TBool isProgressively = ETrue;
                if ( state == EHttpProgContentFileMoved || state == EHttpProgContentFileMovedAndDestFNChanged )
                    {
                    // this will be used to tell multimedia app that content file
                    // has already been moved, and it does not need to attach
                    // to download - it can just read completed download from
                    // filesystem.
                    isProgressively = EFalse;
                    }
                // First close the downloads list.
                CancelDisplayingDownloadsList();
                iUiUtils->LaunchPdAppL(currDownload, isProgressively);
                }
            else
                {
                iUiUtils->HandleContentL( currDownload, *iExtension );
                CLOG_WRITE("HandleContentL OK");
                }
			}
		else
			{
    	    iUiUtils->HandleContentL( currDownload, *iExtension );
	        CLOG_WRITE("HandleContentL OK");
			}
           break;
            }


        case EDownloadsListCmdOpen:
            {
            // First close the downloads list.
            CancelDisplayingDownloadsList();
            CLOG_WRITE(" CancelDisplayingDownloadsList OK");

           iUiUtils->HandleContentL( currDownload, *iExtension );
           CLOG_WRITE("HandleContentL OK");
           // After the termination of the handler, download is removed
           // from the list of downloads in NotifyHandlerExitL().
            break;
            }

        case EDownloadsListCmdPlay:
            {
			#ifdef __DMGR_PD_TESTHARNESS
                // First close the downloads list.
                CancelDisplayingDownloadsList();
				iUiUtils->TestLaunchPdAppL(currDownload);
			#else
				// Get the UI data for the current download
                TDownloadUiData& dlData = iListModel->DlUiData( aDialog.CurrentItemIndex() );
                TInt32 state( dlData.iProgressState );
                TBool isProgressively = ETrue;
                if ( state == EHttpProgContentFileMoved || state == EHttpProgContentFileMovedAndDestFNChanged )
                    {
                    // this will be used to tell multimedia app that content file
                    // has already been moved, and it does not need to attach
                    // to download - it can just read completed download from
                    // filesystem.
                    isProgressively = EFalse;
                    }
                    // First close the downloads list.
                    CancelDisplayingDownloadsList();
					if (iUiUtils->IsNetworkPdCompatibleL() )
						{
						iUiUtils->LaunchPdAppL(currDownload, isProgressively);
						}
			#endif	//__DMGR_PD_TESTHARNESS

            break;
            }

        case EDownloadsListCmdSave:
            {
            TBool saved = iUiUtils->SaveContentL( currDownload );
            if ( saved )
                {
                // Remove the download from the Downloads List to
                // avoid confusion...
                DeleteDownloadL( currDownload );
                }
	        // The save (moving) procedure is asynchronous!
	        // CUserInteractionsEventHandler handles
	        // EHttpDlCompleted/EHttpProgMovingContentFile and
	        // EHttpDlCompleted/EHttpProgContentFileMoved.
            break;
            }

        case EDownloadsListCmdDelete:
            {
            // Retrieve the full path to filename before deletion
            // this way we check if the the actuall content file was deleted or not,
            // if not, we will manually delete it
            HBufC* fileName = HBufC::NewLC( KMaxPath );
            TPtr fileNamePtr = fileName->Des();
            User::LeaveIfError
               ( currDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );
            CLOG_WRITE_FORMAT(" EDlAttrDestFilename: %S", &fileNamePtr);

            HBufC* fileName1 = HBufC::NewLC(fileName->Length());
            TPtr fileNamePtr1 = fileName1->Des();
            fileNamePtr1.Copy(fileNamePtr);
            RFs fs;
            User::LeaveIfError(fs.Connect());
            CleanupClosePushL(fs);
            TFindFile file(fs);
            TPtrC ptr(KNullDesC);
            TInt found = file.FindByPath(fileNamePtr1,&ptr); //when second parameter to the API is Null then the it searches for the file in the Dir specified in the first parameter
            CleanupStack::PopAndDestroy(&fs);
            CleanupStack::PopAndDestroy(fileName1);
            // Delete in DMgr
            TBool deleted = iUiUtils->DeleteWithUserConfirmL( currDownload );
            if ( deleted )
                {
                if(!iUiUtils->IsCodDownload())
                	{
                      if(found == KErrNotFound)
                         {
                          HBufC* infoPrompt = StringLoader::LoadLC( R_DMUL_ERROR_FILE_NOT_FOUND);
                          CAknInformationNote* note = new(ELeave)  CAknInformationNote();
                          note->ExecuteLD(*infoPrompt);
                          CleanupStack::PopAndDestroy(infoPrompt);
                         }
    			   // Do not wait until the delete event is reported through the
    			   // observer callback, but update the list right now,
    			   // so the user will not be able to access and modify an already
    			   // deleted download:
    			   DeleteDownloadL( currDownload );
    			   TBool result( EFalse );
    			   RFs rfs;
    			   User::LeaveIfError( rfs.Connect() );
    			   CleanupClosePushL<RFs>( rfs );
    			   result = BaflUtils::FileExists( rfs, fileNamePtr );
    			   CLOG_WRITE_FORMAT(" BaflUtils::FileExists: %d", result);
    			   if ( result )
    			       { // if content file still exists on the device, delete it.
    			       	// don't care if it fails currently.
    			       result = BaflUtils::DeleteFile( rfs, fileNamePtr );
    			       }
    			   CleanupStack::PopAndDestroy( &rfs );

    			   // Notify Media Gallery about new media file

    			#ifdef BRDO_APP_GALLERY_SUPPORTED_FF

    			   CMGXFileManager* mgFileManager = MGXFileManagerFactory::NewFileManagerL(
    			       CEikonEnv::Static()->FsSession() );
    			   if( fileNamePtr.Length() > 0 )
    			       {
    			       TRAP_IGNORE( mgFileManager->UpdateL( fileNamePtr ) );
    			       TRAP_IGNORE( iUiUtils->UpdateDCFRepositoryL( fileNamePtr ) );
    			       }
    			   else
    			       {
    			       TRAP_IGNORE( mgFileManager->UpdateL() );
    			       }

    			   delete mgFileManager;
    			   mgFileManager = NULL;

    			 #else

    			  if( fileNamePtr.Length() > 0 )
    			       {
    			       TRAP_IGNORE( iUiUtils->UpdateDCFRepositoryL( fileNamePtr ) );
    			       }

    			 #endif

                	}
                 else
    	           	{
    	           	DeleteDownloadL( currDownload );
    	           	}
    			 }

               CleanupStack::PopAndDestroy( fileName );
               break;
           }
//PHAN-6KVK5R: Disallow Manually Pausing a Download
/*
        case EDownloadsListCmdPause:
            {
            User::LeaveIfError( currDownload.Pause() );
            break;
            }
*/
        case EDownloadsListCmdResume:
            {
            // Set the Internet access point ID on WINS* platforms
            // to the DownloadMgr session if not set yet.
            #ifdef __WINS__
            iExtension->EnsureIapIdIsSetL();
            #endif
            // Resume = start
            User::LeaveIfError( currDownload.Start() );
            break;
            }

        case EDownloadsListCmdResumeAll:
            {
            // Set the Internet access point ID on WINS* platforms
            // to the DownloadMgr session if not set yet.
            #ifdef __WINS__
            iExtension->EnsureIapIdIsSetL();
            #endif
            // Resume all paused or failed downloads.
            TInt count = Count();
            for ( TInt i = 0; i < count; ++i )
                {
                RHttpDownload& download = iListModel->Download(i);
                TInt32 dlState(0);
                TInt err = download.GetIntAttribute( EDlAttrState, dlState );

                //Changes for the bug JERI-7P8CF2
                //Changes made in the server side to fix the video center receiving unexpected events
                //Reassigning these events back to the changes done in server side
                if( !err && dlState  == EHttpDlCompleted )
		            {
                 	dlState  = EHttpDlMultipleMOCompleted;
		            }
                else if( !err && dlState  == EHttpDlFailed )
		            {
		    	     dlState  = EHttpDlMultipleMOFailed;
		            }

                if ( !err && ( dlState == EHttpDlPaused || dlState == EHttpDlMultipleMOFailed ) )
                    {
                    download.Start(); // return value is ignored.
                    }
                }
            break;
            }

        case EDownloadsListCmdCancel:
            {
            if (!iIsCancelInProgress)
            	{
            	iIsCancelInProgress = ETrue;
	            TBool cancelled = EFalse;
	            TRAPD( err, cancelled = iUiUtils->CancelWithUserConfirmL( currDownload ) );
	            if (err != KErrNone)
	            	{
	            	iIsCancelInProgress = EFalse;
	            	User::LeaveIfError(err);
	            	}

	            if ( cancelled )
	                {
	                DeleteDownloadL( currDownload );
	                }
	            iIsCancelInProgress = EFalse;
            	}
            break;
            }

        case EDownloadsListCmdCancelAll:
        	{
            if (!iIsCancelInProgress)
            	{
            	iIsCancelInProgress = ETrue;
            	TBool cancelled = EFalse;
	            TRAPD( err, cancelled = iUiUtils->CancelAllWithUserConfirmL( currDownload ) );
	            if (err != KErrNone)
	            	{
	            	iIsCancelInProgress = EFalse;
	            	User::LeaveIfError(err);
	            	}
	            TInt count = Count();
	            TInt i = 0;
	            while((count>0) && (i < count))
	                {
	                RHttpDownload& download = iListModel->Download(i);
                    i++;
	                if ( cancelled )
	                    {
	                    DeleteDownloadL( download );
	                    i--;
	                    count--;
	                    }
	                }
	            iIsCancelInProgress = EFalse;
            	}
        	break;
        	}

        case EDownloadsListCmdGallery:
            {

#ifdef BRDO_APP_GALLERY_SUPPORTED_FF

            TVwsViewId id = TVwsViewId(
                TUid::Uid( KMediaGalleryUID3 ),
                TUid::Uid( KMediaGalleryListViewUID ) );

            // By default, custom message is empty
            TPtrC8 customMessage( KNullDesC8 );

            CEikonEnv::Static()->EikAppUi()->ActivateViewL(
                    id,
                    TUid::Uid( KMediaGalleryCmdMoveFocusToAllFilesTab ),
                    customMessage );
#endif
            break;
            }

        case EDownloadsListCmdFileManager:
            {

            if ( iPlatformSupportsGallery )
                {
            	LaunchFileManagerApplication();
                }
            else
                {
            	CAiwServiceHandler* serviceHandler = CAiwServiceHandler::NewL();
                serviceHandler->AttachL( R_DMUL_FILEMANAGER_AIW_INTEREST );

                CAiwGenericParamList* inParams = CAiwGenericParamList::NewLC();

                //get the destination file path
                HBufC* fileName = HBufC::NewLC( KMaxPath );
                TPtr fileNamePtr = fileName->Des();
                User::LeaveIfError ( currDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );

                // Locate the last '\\' character and remove the file name so that we will get the folder name
                TInt len = fileNamePtr.LocateReverse('\\');
                TPtr ptr = fileNamePtr.LeftTPtr(len + 1);

                // Append the directory name to be opened
                inParams->AppendL(TAiwGenericParam(EGenericParamDir, TAiwVariant( ptr ) ) );
                //Append the sort method
                inParams->AppendL(TAiwGenericParam(EGenericParamDir, TAiwVariant( KMostRecentSort ) ) );
                //Append to define whether to open in standalone mode
                inParams->AppendL(TAiwGenericParam(EGenericParamDir, TAiwVariant( KLauchStandAlone ) ) );

                TRAPD( err, serviceHandler->ExecuteServiceCmdL( KAiwCmdEdit, *inParams, serviceHandler->OutParamListL() ));

    	   		CleanupStack::PopAndDestroy( fileName );
            	CleanupStack::PopAndDestroy( inParams );

        	    delete serviceHandler;

        	    //if there is any error, open the file manager in root folder
                if (err != KErrNone)
          	        {
          	        LaunchFileManagerApplication();
           		    }
                }

            break;
            }

        default:
            {
            break;
            }
        }
    }


// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::LaunchFileManagerApplication
// -----------------------------------------------------------------------------
//

void CDownloadMgrUiDownloadsList::LaunchFileManagerApplication()
{

    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    CRepository *repository=CRepository::NewL(KCRUidBrowser);
    TInt fileManagerId;
    User::LeaveIfError(repository->Get(KFileManagerUid , fileManagerId ));
    TUid id = TUid::Uid(fileManagerId);
    TApaTask task = taskList.FindApp( id );
    if ( task.Exists() )
        {
        task.BringToForeground();
        }
    else
        {
        RApaLsSession appArcSession;
        User::LeaveIfError( appArcSession.Connect() );
        CleanupClosePushL( appArcSession );
        TThreadId id1;
        User::LeaveIfError( appArcSession.StartDocument( KNullDesC, id, id1 ) );
        CleanupStack::PopAndDestroy( &appArcSession );
        }

}


// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::DynInitMenuPaneL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::DynInitMenuPaneL( CDownloadsListDlg& aDialog,
                                                    TInt aResourceId,
                                                    CEikMenuPane* aMenuPane )
    {
    CLOG_ENTERFN("CDownloadMgrUiDownloadsList::DynInitMenuPaneL");

    __ASSERT_DEBUG( iExtension->IsDialogVisible(), Panic( EUiLibPanNull ) );

    if( !iAIWServiceHandler )
        {
        AttachAIWInterestL();
        }

        TInt currentItemIndex = aDialog.CurrentItemIndex();
        __ASSERT_DEBUG( 0 <= currentItemIndex, Panic( EUiLibPanDLNegItemIndex ) );
        RHttpDownload& currDownload = iListModel->Download( currentItemIndex );
        CLOG_WRITE_FORMAT(" currDownload: 0x%x",&currDownload);

        // Get the UI data for the current download
        TDownloadUiData& dlData = iListModel->DlUiData( aDialog.CurrentItemIndex() );
        TInt32 state( dlData.iDownloadState );
        CLOG_WRITE_FORMAT(" state: %d",state);

        // The moved state is only a temporary state - the server
        // sets the download's original state back right after it,
        // thus we query the state again:
        if ( state == EHttpDlMoved )
            {
            User::LeaveIfError( currDownload.GetIntAttribute( EDlAttrState, state ) );

            //Changes for the bug JERI-7P8CF2
            //Changes made in the server side to fix the video center receiving unexpected events
            //Reassigning these events back to the changes done in server side
            if( state  == EHttpDlCompleted )
		        {
                state  = EHttpDlMultipleMOCompleted;
		        }
            else if(state  == EHttpDlFailed )
		        {
		        state  = EHttpDlMultipleMOFailed;
		        }

            CLOG_WRITE_FORMAT(" state re-query: %d",state);
            }
        __ASSERT_DEBUG( state != EHttpDlMoved, Panic( EUiLibPanStateIsStillMoved ) );

        TBool wasCompleted   = state == EHttpDlMultipleMOCompleted;

        // Get if it's theme
        TInt isThemeType = 0;
        HBufC8* contentType = HBufC8::NewLC(KMaxContentTypeLength);
        TPtr8 contentTypePtr = contentType->Des();
        User::LeaveIfError
                ( currDownload.GetStringAttribute( EDlAttrContentType, contentTypePtr ) );
        isThemeType = !contentType->Compare(KSisxContentType);
        CleanupStack::PopAndDestroy( contentType );

    if ( aResourceId == R_DMUL_DOWNLOADSLIST_MENU )
        {
        __ASSERT_DEBUG( 0<iListModel->Count(), Panic( EUiLibPanOptionsShownWhileNoDownloads ) );

        TBool isCreated     = state == EHttpDlCreated;
        // The fix of PHAN-6KVK5R makes this line no longer necessary
        // TBool isInProgress  = state == EHttpDlInprogress;
        TBool isPaused      = state == EHttpDlPaused;
        TBool isCompleted   = state == EHttpDlMultipleMOCompleted;
        TBool isFailed      = state == EHttpDlMultipleMOFailed;
        TBool isDrmDownload = EFalse;
        TBool isDrmRightsOnPhone = EFalse;
        TBool isPreviewRights = EFalse;
        // OMA DRM information is used only in case of completed downloads
        if ( isCompleted )
            {
            isDrmDownload = iUiUtils->DrmDownloadL( currDownload );
            if ( isDrmDownload )
                {
                isDrmRightsOnPhone = iUiUtils->DrmRightsOnThePhoneL
                                                    ( currDownload, isPreviewRights );
                }
            }
        TBool s60Supported( EFalse );
        TBool gallerySupported( EFalse );
        s60Supported = iUiUtils->IsContentTypeSupportedL( dlData.iContentType );
        gallerySupported = CDownloadUtils::IsGallerySupported(dlData.iContentType);
        CLOG_WRITE_FORMAT(" s60Supported: %d",s60Supported);
        CLOG_WRITE_FORMAT(" gallerySupported: %d",gallerySupported);

        TBool isCodDownload( EFalse );
        TInt err = currDownload.GetBoolAttribute( EDlAttrCodDownload, isCodDownload );
        CLOG_WRITE_FORMAT(" EDlAttrCodDownload err: %d",err);

        TBool isCodPdDownload( EFalse );
        err = currDownload.GetBoolAttribute( EDlAttrCodPdAvailable, isCodPdDownload );
        CLOG_WRITE_FORMAT(" EDlAttrCodPdAvailable err: %d",err);

        // Get if it can be handled progressively
        TBool canProgHandled = EFalse;
        TUid pdPlayerUid = { 0 };
        HBufC8* contentType = iUiUtils->ContentTypeL( currDownload, ETrue );
        TDataType dataType( *contentType );
        delete contentType;
        contentType = NULL;
        CDocumentHandler* docHandler = CDocumentHandler::NewLC();
        canProgHandled = docHandler->CanHandleProgressivelyL( dataType, pdPlayerUid );
        CleanupStack::PopAndDestroy( docHandler ); // docHandler

        //delete open file manager when download is not complete
        if( !(isCompleted))
            {
            aMenuPane->DeleteMenuItem( EDownloadsListCmdFileManager );
            }

        // For any gallery supported download,  "go to gallery" option should be displayed in the download list option when the download completes


        if ( (!( isCompleted && s60Supported && gallerySupported ) ) || (!iPlatformSupportsGallery) )
            {
			aMenuPane->DeleteMenuItem( EDownloadsListCmdGallery );
    		}

        if ( !( isCompleted && s60Supported && gallerySupported &&
              ( !isDrmDownload || ( isDrmDownload && isDrmRightsOnPhone ) ) ) )
            {
            aMenuPane->DeleteMenuItem( EDownloadsListCmdUseAs );
			}

	    else
	        {
	        TInt32 numMediaObjects = 0;
            User::LeaveIfError( currDownload.GetIntAttribute( EDlAttrNumMediaObjects, numMediaObjects ) );
	        if (numMediaObjects > KFirstMoIndex)
    	        {
	            aMenuPane->DeleteMenuItem( EDownloadsListCmdUseAs );
	            }
	        }

        if( iProgressiveDownload || isCompleted )
            {
            if ( canProgHandled )
                {
                aMenuPane->DeleteMenuItem( EDownloadsListCmdOpen );
                if (isCodDownload && !isCompleted )
                    {
                    if ( !( gallerySupported && canProgHandled  && isCodPdDownload &&
                       ( !isDrmDownload || ( isDrmDownload && isDrmRightsOnPhone ) ) ) || (isPaused || isFailed))
                       aMenuPane->DeleteMenuItem( EDownloadsListCmdPlay );
                    }
                    else
                    {
                        if ( !( gallerySupported && canProgHandled &&
                           ( !isDrmDownload || ( isDrmDownload && isDrmRightsOnPhone ))) || (isPaused || isFailed))
                           aMenuPane->DeleteMenuItem( EDownloadsListCmdPlay );
                    }
                }
            else
                {
                aMenuPane->DeleteMenuItem( EDownloadsListCmdPlay );

                if ( !( isCompleted && ( s60Supported ) &&
                      ( !isDrmDownload || ( isDrmDownload && isDrmRightsOnPhone ) ) ) )
                    {
                    aMenuPane->DeleteMenuItem( EDownloadsListCmdOpen );
                    }

         //there should not be open in menu when download is not complete and file is not supported by s60  or Gallery
              /*  else if( isCompleted && ( !s60Supported && !gallerySupported))
                    {
                    aMenuPane->DeleteMenuItem( EDownloadsListCmdOpen );
                    }*/
                }
             }

        else // if iProgressiveDownload == EFalse
            {
            aMenuPane->DeleteMenuItem( EDownloadsListCmdPlay );

            if ( !( isCompleted && s60Supported &&
                  ( !isDrmDownload || ( isDrmDownload && isDrmRightsOnPhone ) ) ) )
                {
                aMenuPane->DeleteMenuItem( EDownloadsListCmdOpen );
                }

            else if( isCompleted && !s60Supported && gallerySupported)
                {
                aMenuPane->DeleteMenuItem( EDownloadsListCmdOpen );
                }
            }

        // ELFG-732DK3: There is no need to have save option since it's already saved
        aMenuPane->DeleteMenuItem( EDownloadsListCmdSave );

        if ( !( isCompleted ) )
            {
            aMenuPane->DeleteMenuItem( EDownloadsListCmdDelete );
            }
        else
            {
            aMenuPane->DeleteMenuItem( EDownloadsListCmdCancel );
            }
//PHAN-6KVK5R: Disallow Manually Pausing a Download
/*
        if ( !isInProgress || !dlData.iPausable )
            {
            aMenuPane->DeleteMenuItem( EDownloadsListCmdPause );
            }
*/
        if ( !( isCreated || isPaused || isFailed ) )
            {
            aMenuPane->DeleteMenuItem( EDownloadsListCmdResume );
            }
        // Count paused downloads. Note that Creates and Failed downloads
        // are also considered as Paused, and they can be resumed.
        TInt pausedCount = iListModel->DownloadsCount
                           ( MASKED_DL_STATE(EHttpDlCreated) |
                             MASKED_DL_STATE(EHttpDlPaused) |
                             MASKED_DL_STATE(EHttpDlMultipleMOFailed) );
        CLOG_WRITE_FORMAT(" paused count: %d",pausedCount);
        //
        if ( !( 1 < pausedCount ) )
            {
            aMenuPane->DeleteMenuItem( EDownloadsListCmdResumeAll );
            }

        TInt downloadCount = iListModel->DownloadsCount
                             ( MASKED_DL_STATE(EHttpDlCreated) |
                               MASKED_DL_STATE(EHttpDlInprogress)|
                               MASKED_DL_STATE(EHttpDlPaused) );
        CLOG_WRITE_FORMAT(" download count: %d",downloadCount);

        if ( !( 1 < downloadCount ) )
            {
            aMenuPane->DeleteMenuItem( EDownloadsListCmdCancelAll );
            }

       	if ( iPlatformSupportsGallery )
       	    {
       		if ( isCompleted && gallerySupported && s60Supported )
                {
                aMenuPane->DeleteMenuItem( EDownloadsListCmdFileManager );
                }
       	    }
        }

    if ( wasCompleted && !isThemeType )
        {
        InitializeAIWPlugInMenusL( aResourceId, aMenuPane, currDownload );
        }
    CLOG_LEAVEFN("CDownloadMgrUiDownloadsList::DynInitMenuPaneL");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::OfferKeyEventL
// -----------------------------------------------------------------------------
//
TKeyResponse CDownloadMgrUiDownloadsList::OfferKeyEventL
    ( CDownloadsListDlg& aDialog, const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse resp = EKeyWasNotConsumed;

    if ( aType == EEventKey && aKeyEvent.iCode == EKeyOK )
        {
        CLOG_WRITE(" DisplayMenuL via Selection key");
        aDialog.DisplayMenuL();
        resp = EKeyWasConsumed;
        }
    else if ( aType == EEventKey && aKeyEvent.iCode == EKeyBackspace )
        {
        // Clear "C" key
        CLOG_WRITE(" Clear \"C\" key");
        // Delete or Cancel depending on state
        TDownloadUiData dlData = iListModel->DlUiData( aDialog.CurrentItemIndex() );
        if ( dlData.iDownloadState == EHttpDlMultipleMOCompleted )
            {
            ProcessCommandL( aDialog, EDownloadsListCmdDelete );
            }
        else
            {
            ProcessCommandL( aDialog, EDownloadsListCmdCancel );
            }
        resp = EKeyWasConsumed;
        }

    return resp;
    }

// -----------------------------------------------------------------------------
// FindDlUiData
// -----------------------------------------------------------------------------
//
inline TDownloadUiData* FindDlUiData( CDownloadsListArray& aListModel,
                                       RHttpDownload& aDownload,
                                       TInt& aIndex )
    {
    TInt err = aListModel.Find( aDownload, aIndex );
    if ( err == KErrNotFound )
        {
        aIndex = KErrNotFound;
        return 0;
        }
    else
        {
        return &( aListModel.DlUiData( aIndex ) );
        }
    };

// -----------------------------------------------------------------------------
// CDownloadMgrDownloadsList::AttachAIWInterestL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::AttachAIWInterestL()
    {
    //creates the  service handler object, this is called only in construction
    iAIWServiceHandler = CAiwServiceHandler::NewL();

    TResourceReader reader;
    iCoeEnv.CreateResourceReaderLC( reader,
                                    R_DMUL_AIW_INTEREST_MENUPANE_MAPPINGS );

    TInt count( reader.ReadInt16() );
    for ( TInt i( 0 ); i < count; i++ )
        {
        TInt interest( reader.ReadInt32() );
        TInt menupane( reader.ReadInt32() );
        iAIWServiceHandler->AttachMenuL( menupane, interest );
        }

    CleanupStack::PopAndDestroy();  // reader
    }

// ----------------------------------------------------
// CDownloadMgrUiDownloadsList::HandleAIWCommandL(TInt aCommand, RHttpDownload& aDownload)
// ----------------------------------------------------
//
void CDownloadMgrUiDownloadsList::HandleAIWCommandL(TInt aCommand, RHttpDownload& aDownload)
    {
    // The command is an AIW command
    CAiwGenericParamList& params = iAIWServiceHandler->InParamListL();
    TInt aiwOptions( 0 );

    ProvideInputParametersL( params, aiwOptions, aDownload );
    CAiwGenericParamList& output = iAIWServiceHandler->OutParamListL();
    iAIWServiceHandler->ExecuteMenuCmdL( aCommand, params, output, aiwOptions, this );
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::ProvideInputParametersL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::ProvideInputParametersL( CAiwGenericParamList& aParams,
                                                    TInt& /*aCommandOptions*/,
                                                    RHttpDownload& aDownload )
    {
    HBufC* fileName = HBufC::NewLC( KMaxPath );
    TPtr fileNamePtr = fileName->Des();
    User::LeaveIfError
               ( aDownload.GetStringAttribute( EDlAttrDestFilename, fileNamePtr ) );
    TAiwVariant fileNameVariant( fileNamePtr );
    TAiwGenericParam genericParamFileName( EGenericParamFile, fileNameVariant );
	aParams.AppendL( genericParamFileName );

	HBufC* contentType = HBufC::NewLC(KMaxContentTypeLength);
	TPtr contentTypePtr = contentType->Des();
    User::LeaveIfError
                ( aDownload.GetStringAttribute( EDlAttrContentType, contentTypePtr ) );
    fileNameVariant.Set(contentTypePtr);
    TAiwGenericParam genericParamMimeType( EGenericParamMIMEType, fileNameVariant );
    aParams.AppendL( genericParamMimeType );
    CleanupStack::PopAndDestroy( contentType );
    CleanupStack::PopAndDestroy( fileName );

    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::InitializeAIWPlugInMenusL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::InitializeAIWPlugInMenusL( TInt aResourceId,
                                                   CEikMenuPane* aMenuPane,
                                                   RHttpDownload& aDownload )
    {
    // Checks whether it is aiw menu or not
    if ( iAIWServiceHandler->IsAiwMenu( aResourceId ) )
        {
        CAiwGenericParamList* params = CAiwGenericParamList::NewLC();
        TInt i = 0;
        ProvideInputParametersL(*params, i, aDownload);

        TInt baseId = EAIWPluginMenu;

        if ( aMenuPane->IsCascadeMenuPane() )
            {
            baseId = EAIWPluginSubMenu;
            }

        // Adds the aiw plugin menus
        iAIWServiceHandler->InitializeMenuPaneL( *aMenuPane, aResourceId,
                                                 baseId, *params );

        CleanupStack::PopAndDestroy(); // params
        }
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiDownloadsList::SetDownloadListHide
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiDownloadsList::SetDownloadListHide( TBool aHide )
    {
    iDownloadListHide = aHide;
    }
#ifdef __WINS__
#include <commdb.h>
#include <aknlists.h>
// ----------------------------------------------------------------------------
// AskIapId(TUint32& aId)
// Ask IAP id from the user. Used due to efficiency only on WINS* platforms.
// ----------------------------------------------------------------------------
//
TInt AskIapIdL( TUint32& aId )
    {
    RArray<TUint32> idArray;
    CleanupClosePushL( idArray );

    CDesCArrayFlat* nameArray = new (ELeave) CDesCArrayFlat(16);
    CleanupStack::PushL( nameArray );

    // Get IAP names and ids from the database
    CCommsDatabase* TheDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( TheDb );

    TheDb->ShowHiddenRecords();

    CCommsDbTableView* view = TheDb->OpenTableLC( TPtrC(IAP) );

    TBuf<40> name;
    TUint32 id;
    TInt res = view->GotoFirstRecord();
    while( res == KErrNone )
    {
        view->ReadTextL( TPtrC(COMMDB_NAME), name );
        view->ReadUintL( TPtrC(COMMDB_ID), id );

        idArray.Insert( id, 0 );
        nameArray->InsertL( 0, name );

        res = view->GotoNextRecord();
        RDebug::Print(_L("IAP name, id: %S, %d"), &name, id);
    }

    CleanupStack::PopAndDestroy( view ); // view
    CleanupStack::PopAndDestroy( TheDb ); // TheDb

    // Create listbox and PUSH it.
    CEikTextListBox* list = new(ELeave) CAknSinglePopupMenuStyleListBox;
    CleanupStack::PushL( list );

    // Create popup list and PUSH it.
    CAknPopupList* popupList = CAknPopupList::NewL(
        list, R_AVKON_SOFTKEYS_OK_EMPTY__OK,
        AknPopupLayouts::EMenuWindow);
    CleanupStack::PushL( popupList );

    // initialize listbox.
    list->ConstructL( popupList, CEikListBox::ELeftDownInViewRect );
    list->CreateScrollBarFrameL( ETrue );
    list->ScrollBarFrame()->SetScrollBarVisibilityL(
        CEikScrollBarFrame::EOff,
        CEikScrollBarFrame::EAuto );

    // Set listitems.
    CTextListBoxModel* model = list->Model();
    model->SetItemTextArray( nameArray );
    model->SetOwnershipType( ELbmDoesNotOwnItemArray );

    // Set title
    popupList->SetTitleL( _L("Select IAP:") );

    // Show popup list.
    TInt popupOk = popupList->ExecuteLD();
    CleanupStack::Pop( popupList ); // popupList
    if ( popupOk )
        {
        TInt index = list->CurrentItemIndex();
        aId = idArray[index];
        }

    CleanupStack::PopAndDestroy( 3, &idArray );  // list, nameArray, idArray

    return popupOk;
    }
#endif // #ifdef __WINS__

/* End of file. */

