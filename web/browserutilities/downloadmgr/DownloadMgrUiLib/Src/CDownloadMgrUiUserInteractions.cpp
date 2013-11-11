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
* Description:  Supports user interaction dialogs for downloads
*
*/



// INCLUDE FILES
#include    "CDownloadMgrUiUserInteractions.h"
#include    "CUserInteractionsUtils.h"
#include    "CDownloadMgrUiDownloadsList.h"
#include    "CDownloadMgrUiLibRegistry.h"
#include    "UserInteractionsEventHandler.h"
#include    "AsyncEventHandlerArray.h"
#include    "UiLibLogger.h"
#include    "DMgrUiLibPanic.h"
#include    "MDownloadHandlerObserver.h"
#include    <e32std.h>
#include    <e32def.h>
#include    <StringLoader.h>
#include    <DownloadMgrUiLib.rsg>
#include    <AknQueryDialog.h>
#include    <avkon.rsg>
#include    <eikon.hrh>
#include    <eikappui.h>
#include    <apparc.h>
#include    <AknServerApp.h>
#include    <DRMCommon.h>

// LOCAL CONSTANTS AND MACROS
const TInt KPostponedDownloadsGranularity = 1;

/**
* Extension class.
*/
NONSHARABLE_CLASS( CUserInteractionsExtension ) : public CBase,
                                                  public MDownloadHandlerObserver
    {
    public:  // Constructors and destructor
        
        /**
        * Two-phased constructor.
        */
        static CUserInteractionsExtension* NewL
             ( CDownloadMgrUiUserInteractions& aUserInteractions );
        
        /**
        * Destructor.
        */
        virtual ~CUserInteractionsExtension();

    public: // New functions

        /**
        * Postpone handling the given COD download. The download is placed to a 
        * LIFO queue. Call SchedulePostponedDownloadL() to invoke the handling 
        * of a download in the list.
        * @param aDownload The download to be postponed.
        */
        void PostponeCodHandlingL( RHttpDownload& aDownload );

        /**
        * Check if the given download is postponed.
        * @param aDownload The download in question.
        * @return true/false.
        */
        TBool IsPostponed( RHttpDownload& aDownload ) const;

        /**
        * Schedule a postponed download for running.
        */
        void SchedulePostponedDownloadL();

    protected: // Constructors

        /**
        * C++ default constructor.
        */
        CUserInteractionsExtension( CDownloadMgrUiUserInteractions& aUserInteractions );

        /**
        * By default Symbian 2nd phase constructor is private.
        */
        void ConstructL();

    protected: // From MDownloadHandlerObserver

        void NotifyHandlerExit( RHttpDownload* aDownload, TInt aReason );

    public: // Data

        CDownloadMgrUiUserInteractions& iUserInteractions;
        TBuf<KMaxPath> iFileName; ///< Buffer for file name.
        // Attributes
        TBool iIsCompletedStateHandled; ///< Event handling preferencies
        TBool iSuppressDownloadConfirmation;
        CArrayPtrFlat< RHttpDownload >* iPostponedCodDownloads; ///< Owned.
    };


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUserInteractionsExtension::CUserInteractionsExtension
// -----------------------------------------------------------------------------
//
CUserInteractionsExtension::CUserInteractionsExtension
    ( CDownloadMgrUiUserInteractions& aUserInteractions )
:   iUserInteractions( aUserInteractions ), 
    iFileName( KNullDesC ), 
    iIsCompletedStateHandled( ETrue )
    {
    }

// -----------------------------------------------------------------------------
// CUserInteractionsExtension::ConstructL
// -----------------------------------------------------------------------------
//
void CUserInteractionsExtension::ConstructL()
    {
    CLOG_ENTERFN("CUserInteractionsExtension::ConstructL");
    CLOG_LEAVEFN("CUserInteractionsExtension::ConstructL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsExtension::NewL
// -----------------------------------------------------------------------------
//
CUserInteractionsExtension* CUserInteractionsExtension::NewL
    ( CDownloadMgrUiUserInteractions& aUserInteractions )
    {
    CUserInteractionsExtension* self = 
        new (ELeave) CUserInteractionsExtension( aUserInteractions );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CUserInteractionsExtension::~CUserInteractionsExtension()
    {
    CLOG_ENTERFN("CUserInteractionsExtension::~CUserInteractionsExtension");
    delete iPostponedCodDownloads;
    CLOG_WRITE(" iPostponedCodDownloads OK");
    CLOG_LEAVEFN("CUserInteractionsExtension::~CUserInteractionsExtension");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsExtension::PostponeCodHandlingL
// -----------------------------------------------------------------------------
//
void CUserInteractionsExtension::PostponeCodHandlingL( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CUserInteractionsExtension::PostponeCodHandlingL");

    if ( !iPostponedCodDownloads )
        {
        iPostponedCodDownloads = new (ELeave) CArrayPtrFlat< RHttpDownload >
                                            ( KPostponedDownloadsGranularity );
        CLOG_WRITE(" new OK");
        }
    // Insert download at the end of the array
    iPostponedCodDownloads->AppendL( &aDownload );

    CLOG_LEAVEFN("CUserInteractionsExtension::PostponeCodHandlingL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsExtension::IsPostponed
// -----------------------------------------------------------------------------
//
TBool CUserInteractionsExtension::IsPostponed( RHttpDownload& aDownload ) const
    {
    CLOG_ENTERFN("CUserInteractionsExtension::IsPostponed");
    
    TInt index( KErrNotFound );
    TKeyArrayFix cmpKey( 0, ECmpTUint32 );
    TInt err = KErrNotFound;
    if ( iPostponedCodDownloads )
        {
        err = iPostponedCodDownloads->Find( &aDownload, cmpKey, index );
        CLOG_WRITE_FORMAT(" err: %d", err);
        CLOG_WRITE_FORMAT(" index: %d", index);
        }
        
    CLOG_LEAVEFN("CUserInteractionsExtension::IsPostponed");
    return !err;
    }

// -----------------------------------------------------------------------------
// CUserInteractionsExtension::SchedulePostponedDownloadL
// -----------------------------------------------------------------------------
//
void CUserInteractionsExtension::SchedulePostponedDownloadL()
    {
    CLOG_ENTERFN("CUserInteractionsExtension::SchedulePostponedDownloadL");
    
    // Start next download handling
    if ( iPostponedCodDownloads )
        {
        TInt postponedCodDlCount = iPostponedCodDownloads->Count();
        CLOG_WRITE_FORMAT(" postponedCodDlCount: %d", postponedCodDlCount);
        if ( postponedCodDlCount )
            {
            // Restart the first COD download in the list
            RHttpDownload* codDl = (RHttpDownload*) iPostponedCodDownloads->At(0);
            TInt errorRet = codDl->Start();
            CLOG_WRITE_FORMAT(" errorRet: %d", errorRet);
            User::LeaveIfError( errorRet );
            // Remove the download from the lifo list, but do not delete it!
            iPostponedCodDownloads->Delete(0);
            }
        }
        
    CLOG_LEAVEFN("CUserInteractionsExtension::SchedulePostponedDownloadL");
    }

// -----------------------------------------------------------------------------
// CUserInteractionsExtension::NotifyHandlerExit
// -----------------------------------------------------------------------------
//
void CUserInteractionsExtension::NotifyHandlerExit( RHttpDownload* aDownload, 
                                                     TInt aReason )
    {
    CLOG_ENTERFN("CUserInteractionsExtension::NotifyHandlerExit");
    CLOG_WRITE_FORMAT(" aReason: %d", aReason);

    // Delete the download if the handler terminated without error
    if ( aReason == KErrNone || aReason == EEikCmdExit || aReason == EAknCmdExit )
        {
        if ( aDownload )
            {
            /* return value is ignored */aDownload->Delete();
            CLOG_WRITE(" Delete OK");
            }
        }

    // Propagate the event to the registered observer, too
    if ( iUserInteractions.iServerAppExitObserver != 0 )
        {
        CLOG_WRITE(" Propagating event...");
        iUserInteractions.iServerAppExitObserver->HandleServerAppExit( aReason );
        }

    CLOG_LEAVEFN("CUserInteractionsExtension::NotifyHandlerExit");
    }


// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::CDownloadMgrUiUserInteractions
// -----------------------------------------------------------------------------
//
CDownloadMgrUiUserInteractions::CDownloadMgrUiUserInteractions
    ( CDownloadMgrUiLibRegistry& aRegistryModel )
:   CDownloadMgrUiBase( aRegistryModel ),
    iDlgActive ( EFalse ),
    iProgressiveDownloadLaunched ( EFalse )
    {
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::ConstructL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiUserInteractions::ConstructL()
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::ConstructL");

    BaseConstructL();
    CLOG_WRITE(" BaseConstructL");
    iExtension = CUserInteractionsExtension::NewL( *this );
    CLOG_WRITE(" iExtension");
    iUiUtils = CUserInteractionsUtils::NewL( *this, iRegistryModel );
    CLOG_WRITE(" iUiUtils");
    iEventHandlerArray = new(ELeave)CAsyncEventHandlerArray();
    CLOG_WRITE(" iEventHandlerArray");
    // Cancel the Soft Notifications belonging to this client
    iUiUtils->CancelSoftNotifStndL
        ( TVwsViewId(iRegistryModel.ClientAppUid(),TUid::Null()), 
          TUid::Null(), KNullDesC8 );
    iUiUtils->CancelSoftNotifEmbL
        ( TVwsViewId(iRegistryModel.ClientAppUid(),TUid::Null()), 
          TUid::Null(), KNullDesC8 );

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::ConstructL");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::NewL
// -----------------------------------------------------------------------------
//
CDownloadMgrUiUserInteractions* CDownloadMgrUiUserInteractions::NewL
    ( CDownloadMgrUiLibRegistry& aRegistryModel )
    {
    CDownloadMgrUiUserInteractions* self = 
        new ( ELeave ) CDownloadMgrUiUserInteractions( aRegistryModel );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

// Destructor
CDownloadMgrUiUserInteractions::~CDownloadMgrUiUserInteractions()
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::~CDownloadMgrUiUserInteractions");

    delete iEventHandlerArray;
    iEventHandlerArray = 0;
    CLOG_WRITE(" iEventHandlerArray");
    delete iUiUtils;
    iUiUtils = 0;
    CLOG_WRITE(" iUiUtils");
    // iExtension should be the last deleted.
    delete iExtension;
    iExtension = 0;
    CLOG_WRITE(" iExtension");

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::~CDownloadMgrUiUserInteractions");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::OkToExitL
// -----------------------------------------------------------------------------
//
EXPORT_C 
TBool CDownloadMgrUiUserInteractions::OkToExitL()
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::OkToExitL");

    TBool okToExit = EFalse;
    TInt downloadCnt = iRegistryModel.DownloadCount();
    CLOG_WRITE_FORMAT(" downloadCnt: %d", downloadCnt);
    // do nothing if this is 0
    if ( downloadCnt == 0 )
        {
        okToExit = ETrue;
        CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::OkToExitL");
        return okToExit;
        }

    // will delete client's completed, not hidden downloads
    THttpDownloadState state;
    THttpProgressState progState;
    TBool isHiddenDel = EFalse;
    TBool isCodDownload( EFalse );
    TBool isProg (EFalse);
    // check if progressive download, if yes, will NOT delete from the list, otherwise will
    const CDownloadArray& downloadsDel = iRegistryModel.DownloadMgr().CurrentDownloads();
    for ( TInt i = downloadCnt - 1; i >= 0; --i )
        {
        RHttpDownload* dl = downloadsDel.At(i); // current download
        dl->GetIntAttribute(EDlAttrState, (TInt32&)state);
        
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
        
        if ( state == EHttpDlMultipleMOCompleted )
            {
            dl->GetBoolAttribute( EDlAttrHidden, isHiddenDel ); // return value ignored
            dl->GetIntAttribute( EDlAttrProgressState, (TInt32&)progState );
            dl->GetBoolAttribute( EDlAttrProgressive, isProg ); 
            if ( (!isHiddenDel && ( progState == EHttpProgContentFileMoved || progState == EHttpProgContentFileMovedAndDestFNChanged )&& !isProg) )
                {
                User::LeaveIfError( dl->Delete() );
                }
            }
        }
       
    downloadCnt = iRegistryModel.DownloadCount();

    // Client is intrested in only those downloads for that the 
    // media has not been removed or pausable (non-pausable downloads 
    // will be deleted).
    TInt32 noMediaDownloads(0);
    iRegistryModel.DownloadMgr().GetIntAttribute( EDlMgrNoMediaDls, noMediaDownloads );
    CLOG_WRITE_FORMAT(" noMediaDownloads: %d", noMediaDownloads);
    // Return value is ignored.
    // If it returns an error, then 'noMediaDownloads' is considered 0.

    // Count hidden downloads, too.
    TInt32 hiddenDownloads = 0;
    TBool isHidden;
    const CDownloadArray& downloads = iRegistryModel.DownloadMgr().CurrentDownloads();
    for ( TInt i = 0; i < downloadCnt; ++i )
        {
        RHttpDownload* dl = downloads.At(i); // current download
        dl->GetBoolAttribute( EDlAttrHidden, isHidden ); // return value ignored
        if ( isHidden )
            {
            ++hiddenDownloads;
            }
        }
       
    CLOG_WRITE_FORMAT(" hiddenDownloads: %d", hiddenDownloads);

    // Check if there is progressively played download
    TInt32 progressivelyPlayed = 0;
    for ( TInt i = downloadCnt - 1; i >=0; --i )
	{
        RHttpDownload* dl = downloads.At(i); //current download
        dl->GetBoolAttribute( EDlAttrProgressive, isProg );
        if (isProg && state==EHttpDlMultipleMOCompleted) 
		{
                progressivelyPlayed++;
                break;
		}
	}
      
    TBool isPausable; 
    for ( TInt i = 0; i < downloadCnt; ++i )
        {
            RHttpDownload* dl = downloads.At(i);
            dl->GetBoolAttribute( EDlAttrPausable, isPausable );
            if(!isPausable)
            {
                   break;
            }
        }
    downloadCnt = downloadCnt - noMediaDownloads - hiddenDownloads - progressivelyPlayed;
    CLOG_WRITE_FORMAT(" downloads result: %d", downloadCnt);
    if ( downloadCnt < 1 ||  isPausable)
        {
        okToExit = ETrue;
        }
    else
        {
        HBufC* prompt = StringLoader::LoadLC( downloadCnt==1 ? 
                        R_DMUL_EXIT_CONF_SING : R_DMUL_EXIT_CONF_PLUR );
        CAknQueryDialog* conf = CAknQueryDialog::NewL();
        conf->PrepareLC( R_DMUL_EXIT_CONF );
        conf->SetPromptL( *prompt );
        TInt resp = conf->RunLD();
        CleanupStack::PopAndDestroy( prompt ); // prompt
        
        downloadCnt = iRegistryModel.DownloadCount();

        TBool isProgressive (EFalse);
        if ( resp == EAknSoftkeyYes || resp == EAknSoftkeyOk )
		    {
            for ( TInt i = downloadCnt - 1; i >=0; --i )
		        {
	            RHttpDownload* dl = downloads.At(i); //current download
                dl->GetBoolAttribute( EDlAttrProgressive, isProgressive );
                dl->GetBoolAttribute( EDlAttrPausable , isPausable );
                if (!( isProgressive || isPausable ) ) // delete only no-PDL downloads and Non pausable Downloads
    			    {
                    // Delete not attached downloads.
    	            dl->Delete(); // Return value ignored.
			        }
		        }
            okToExit = ETrue;
    		}
        else
            {
            if ( iRegistryModel.DownloadsListInstalled() )
                {
                // Make visible
                iRegistryModel.DownloadsList().DisplayDownloadsListL();
                }
            }
        }

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::OkToExitL");
    return okToExit;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::PrepareToExit
// -----------------------------------------------------------------------------
//
EXPORT_C 
TInt CDownloadMgrUiUserInteractions::PrepareToExit( CEikAppUi& aAppUi, 
                                                    TClientAppExitType aExitType, 
                                                    TVwsViewId aViewId, 
                                                    TUid aCustomMessageId, 
                                                    const TDesC8& aViewActivationMsg )
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::PrepareToExit");

    // send a message to all running PD applications and tell that browser is exiting
    TRAP_IGNORE( iUiUtils->SendMsgTerminateToPdAppsL( iProgressiveDownloadLaunched ));

    TRAPD( err, PrepareToExitL( &aAppUi, aExitType, aViewId, aCustomMessageId, 
                                aViewActivationMsg ) );
	CLOG_WRITE_FORMAT(" err: %d",err);

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::PrepareToExit");
    return err;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::PrepareToExit
// -----------------------------------------------------------------------------
//
EXPORT_C 
TInt CDownloadMgrUiUserInteractions::PrepareToExit( TUint32 aAppUid,
                                                    TUint32 aViewId, 
                                                    TUint32 aCustomMessageId )
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::PrepareToExit 2");

    // send a message to all running PD applications and tell that browser is exiting
    TRAP_IGNORE( iUiUtils->SendMsgTerminateToPdAppsL( iProgressiveDownloadLaunched ));

    // Convert parameters to the necessary form
    TVwsViewId viewId( TUid::Uid(aAppUid), TUid::Uid(aViewId) );
    TUid customMessageId( TUid::Uid(aCustomMessageId) );
    
    TRAPD( err, PrepareToExitL( 0, ETerminatedBySystem, viewId, customMessageId, 
                                KNullDesC8 ) );
	CLOG_WRITE_FORMAT(" err: %d",err);

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::PrepareToExit 2");
    return err;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::HandleDownloadL
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiUserInteractions::HandleDownloadL( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::HandleDownloadL");
    CLOG_WRITE_FORMAT("aDownload: 0x%x",&aDownload);

    // Handle only completed downloads.
    // check if it is COD download. They do not require any handling - just delete them.
    if ( CodDownloadL( aDownload ) )
        {
        // Nothing to do with COD - just delete it.
        User::LeaveIfError( aDownload.Delete() );
        CLOG_WRITE(" Delete OK");
        }
    else
        {
        // Handling depends on EDlAttrAction
        TInt32 action(0);
        User::LeaveIfError( aDownload.GetIntAttribute
                            ( EDlAttrAction, action ) );
        CLOG_WRITE_FORMAT(" EDlAttrAction: %d",action);

        if ( action == EDoNothing )
            {
            // Do nothing
            }
        else if ( action & EMove )
            {
            // Move content to the location pointed by EDlAttrDestFilename.
            User::LeaveIfError( aDownload.Move() );
            }
        else if ( action & EPdLaunch )
            {
        	// again do nothing since it's already launched during the process 
            }
        else
            {
            // Defaulting to Launch
            HBufC8* ctype = iUiUtils->ContentTypeL( aDownload, ETrue );
            CleanupStack::PushL( ctype );
            TBool isLaunchType( KSisxApplication().Find( *ctype ) != KErrNotFound || KWidgetMimeType().Find (*ctype) != KErrNotFound );
            if(!isLaunchType)
                {
                isLaunchType = KPipApplication().Find( *ctype ) != KErrNotFound ;
                }
            if(!isLaunchType)
                {
                isLaunchType = KSharingConfig().Find( *ctype ) != KErrNotFound ;
                }                
            if( isLaunchType )
                {
                // launch content
                iUiUtils->HandleContentL( aDownload, *iExtension );
                }
            else if ( iUiUtils->DrmDownloadL( aDownload ) )
                {

                if( iUiUtils->IsCorruptedDcfL( aDownload ) )
                    {
                    aDownload.Delete();
                    // Show 'File corrupted' info note.
                    iUiUtils->InfoNoteL( R_DMUL_OK_NOTE, R_DMUL_ERROR_CORRUPTED );
                    }
                else
                    {
                    TBool isBadMimeInDcf = iUiUtils->IsBadMimeInDcfL( aDownload );
                    TBool isSupported = EFalse;
                    if ( isBadMimeInDcf )
                        {
                        isSupported = EFalse;
                        }
                    else
                        {
                        isSupported = iUiUtils->IsContentTypeSupportedL( aDownload );
                        }
                    TBool previewRights;
                    if ( iUiUtils->DrmRightsOnThePhoneL( aDownload, previewRights ) )
                        {
                        // get content type of media file in drm content
                        isLaunchType = KDrmInnerContentTypesToLaunch().Find( *ctype ) != KErrNotFound;
                        if( isLaunchType )
                            {
                            // launch content
                            iUiUtils->HandleContentL( aDownload, *iExtension );
                            }
                        else if ( previewRights )
                            {
                            // Normal behaviour
                            HandleNormalDownloadL( aDownload );
                            }
                        else
                            {
                            HandleCompletionWithQueryL( aDownload, IsUiBusy(), isSupported, ETrue, ETrue );
                            }
                        }
                    else
                        {
                        // No rights on the phone
                        HandleCompletionWithQueryL( aDownload, IsUiBusy(), isSupported, ETrue, EFalse );
                        }
                    }
                }
            else // Normal download
                {
                HandleNormalDownloadL( aDownload );
                }
             CleanupStack::PopAndDestroy( ctype );    
            }
        }

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::HandleDownloadL");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::GetIntAttributeL
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiUserInteractions::GetIntAttributeL( const TUint /*aAttribute*/, TInt32& /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }
    
// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::GetBoolAttributeL
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiUserInteractions::GetBoolAttributeL( const TUint aAttribute, TBool& aValue )
    {
    if ( aAttribute == EAttrSuppressDownloadConfirmation )
        {
        aValue = iExtension->iSuppressDownloadConfirmation;
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }
    
// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::GetStringAttributeL
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiUserInteractions::GetStringAttributeL( const TUint /*aAttribute*/, TDes16& /*aValue*/  )
    {
    User::Leave( KErrNotSupported );
    }
    
// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::GetStringAttributeL
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiUserInteractions::GetStringAttributeL( const TUint /*aAttribute*/, TDes8& /*aValue*/  )
    {
    User::Leave( KErrNotSupported );
    }
    
// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::SetIntAttributeL
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiUserInteractions::SetIntAttributeL( const TUint /*aAttribute*/, TInt32 /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }
    
// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::SetBoolAttributeL
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiUserInteractions::SetBoolAttributeL( const TUint aAttribute, TBool aValue )
    {
    if ( aAttribute == EAttrSuppressDownloadConfirmation )
        {
        iExtension->iSuppressDownloadConfirmation = aValue;
        }
    else
        {
        User::Leave( KErrNotSupported );
        }
    }
    
// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::SetStringAttributeL
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiUserInteractions::SetStringAttributeL( const TUint /*aAttribute*/, const TDesC16& /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }
    
// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::SetStringAttributeL
// -----------------------------------------------------------------------------
//
EXPORT_C 
void CDownloadMgrUiUserInteractions::SetStringAttributeL( const TUint /*aAttribute*/, const TDesC8& /*aValue*/ )
    {
    User::Leave( KErrNotSupported );
    }
    
// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::HandleCompletionWithQueryL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiUserInteractions::HandleCompletionWithQueryL
    ( RHttpDownload& aDownload,
      TBool aIsUiBusy,
      TBool aIsSupported,
      TBool aIsDrm,
      TBool aDrmRightsOnPhone )
    {
    if( !iDlgActive )
        {
        iDlgActive = ETrue;
        TRAPD( err, DoHandleCompletionWithQueryL( aDownload, aIsUiBusy,
            aIsSupported, aIsDrm, aDrmRightsOnPhone ));
        iDlgActive = EFalse;
        User::LeaveIfError( err );
        }
    }
  
// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::DoHandleCompletionWithQueryL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiUserInteractions::DoHandleCompletionWithQueryL
    ( RHttpDownload& aDownload, TBool CLOG_ONLY( aIsUiBusy ), 
                                                      TBool aIsSupported, 
                                                      TBool aIsDrm, 
                                                      TBool aDrmRightsOnPhone )
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::HandleCompletionWithQueryL");
    CLOG_WRITE_FORMAT(" aIsUiBusy: %d",aIsUiBusy);
    CLOG_WRITE_FORMAT(" aIsSupported: %d",aIsSupported);
    CLOG_WRITE_FORMAT(" aIsDrm: %d",aIsDrm);
    CLOG_WRITE_FORMAT(" aDrmRightsOnPhone: %d",aDrmRightsOnPhone);

    // File name :
    User::LeaveIfError( aDownload.GetStringAttribute
                      ( EDlAttrName, iExtension->iFileName ) );

    HBufC* queryText = StringLoader::LoadLC
                       ( R_DMUL_COMPLETED_CONF, iExtension->iFileName );

    CAknQueryDialog* dlg = CAknQueryDialog::NewL();
    // Set up the initial dialog. Some parameters are changed then.
    dlg->PrepareLC( R_DMUL_COMPL_DOWNLOAD_CONF );
    CLOG_WRITE(" PrepareLC OK");
    dlg->SetPromptL( *queryText );
    
    // Set up softkeys:
    TInt cbaResourceId(0);
    TInt okSkMeans(0);
    if ( aIsDrm && aIsSupported )
        {
        if ( !aDrmRightsOnPhone )
            {
            // aIsUiBusy must be EFalse!
            cbaResourceId = R_DMUL_SOFTKEYS_COMPL_DRM_SAVE;
            okSkMeans = EAknSoftkeySave;
            }
        else
            {
            cbaResourceId = R_DMUL_SOFTKEYS_COMPL_DRM;
            okSkMeans = EAknSoftkeyShow;
            }
        }
    else
        {
        // aIsUiBusy must be EFalse!
        if ( aIsSupported )
            {
            cbaResourceId = R_DMUL_SOFTKEYS_COMPL_DL;
            okSkMeans = EAknSoftkeyShow;
            }
        else
            {
            cbaResourceId = R_DMUL_SOFTKEYS_UNSUPP_COMPL_DL;
            okSkMeans = EAknSoftkeySave;
            }
        }
    dlg->ButtonGroupContainer().SetCommandSetL( cbaResourceId );

    // Execute it.
    TInt skId = dlg->RunLD();
    CLOG_WRITE_FORMAT(" RunLD %d",skId);
    dlg = NULL;

    CleanupStack::PopAndDestroy( /*queryText*/ ); // queryText

    // Translate OK key to the proper meaning
    if ( skId == EAknSoftkeyOk )
        {
        skId = okSkMeans;
        }
        
    if ( skId == EAknSoftkeyShow )
        {
        // First clode the downloads list.
        if ( iRegistryModel.DownloadsListInstalled() )
            {
            iRegistryModel.DownloadsList().CancelDisplayingDownloadsList();
            }
        iUiUtils->HandleContentL( aDownload, *iExtension );
        // After the termination of the handler, download is removed 
        // from the list of downloads in NotifyHandlerExit().
        }
    else if ( skId == EAknSoftkeySave )
        {
        iUiUtils->SaveContentL( aDownload );
        // The save (moving) procedure is asynchronous!
        // When ends, HandleDMgrEventL() is called.
        }
    else
        {
        // Do nothing.
        }

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::HandleCompletionWithQueryL");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::PrepareToExitL
// Behaviour depends on:
// - EDlMgrExitAction is EExitDelete or not
// - some download is non-pausable - these will always be deleted by DM engine
// - hidden downloads
// - no media downloads are taken into account!
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiUserInteractions::PrepareToExitL( CEikAppUi* /*aAppUi*/, 
                                                     TClientAppExitType aExitType, 
                                                     TVwsViewId aViewId, 
                                                     TUid aCustomMessageId, 
                                                     const TDesC8& aViewActivationMsg )
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::PrepareToExitL");

    __ASSERT_ALWAYS( aExitType==ETerminatedBySystem, Panic( EUiLibPanCalledWhenUserExit ) );

    TInt32 exitAction( EExitNothing );
    User::LeaveIfError( iRegistryModel.DownloadMgr().
                        GetIntAttribute( EDlMgrExitAction, exitAction ) );
    CLOG_WRITE_FORMAT(" exitAction: %d",exitAction);
    
    if ( exitAction == EExitDelete )
        {
        // No need to initialize soft notifications - downloads will be deleted
        CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::PrepareToExitL");
        return;
        }
        
    // count non-pausable and hidden downloads
    TInt downloadCnt = iRegistryModel.DownloadCount();
    TInt ignoredDownloads = 0;
    TBool isPausable(0);
    TBool isHidden(0);
    TBool isNoMedia(0);
    TInt err(0);
    THttpDownloadState state;
    const CDownloadArray& downloads = iRegistryModel.DownloadMgr().CurrentDownloads();
    for( TInt i = 0; i < downloadCnt; ++i )
        {
        RHttpDownload* dl = downloads.At(i); // current download
        err = dl->GetBoolAttribute( EDlAttrPausable, isPausable );
        if ( !err )
            {
            err = dl->GetBoolAttribute( EDlAttrHidden, isHidden );
            }
        if( !err )
            {
            err = dl->GetIntAttribute(EDlAttrState, (TInt32&)state);
            //Changes for the bug JERI-7P8CF2
            //Changes made in the server side to fix  the video center receiving unexpected events
            //Reassigning these events back to the changes done in server side
            if(!err && state  == EHttpDlCompleted )
		       {
                state  = EHttpDlMultipleMOCompleted;
		       }
            else if(!err && state  == EHttpDlFailed )
		       {
		       state  = EHttpDlMultipleMOFailed;
		       }
          
            }
        if ( !err )
        	{
        	err = dl->GetBoolAttribute( EDlAttrNoMedia, isNoMedia );
        	}
        CLOG_WRITE_FORMAT(" err: %d",err);
        if ( !err && ( !isPausable || isHidden ||isNoMedia || state == EHttpDlMultipleMOCompleted  ) )
            {
            ++ignoredDownloads;
            }
        }
    CLOG_WRITE_FORMAT(" downloadCnt: %d",downloadCnt);
    CLOG_WRITE_FORMAT(" ignoredDownloads: %d",ignoredDownloads);
        
    // the new downloadCnt holds only those downloads, that count from the 
    // point of soft notifications
    downloadCnt = downloadCnt - ignoredDownloads;
    
    // Is the App server app?
    TBool startedAsServerApp = ((CEikonEnv&)iCoeEnv).StartedAsServerApp();
    CLOG_WRITE_FORMAT(" startedAsServerApp: %d",startedAsServerApp);
    if ( startedAsServerApp )
        {
        if ( downloadCnt > 0 )
            {
            // Display GSN-Emb.
            iUiUtils->InitializeSoftNotifEmbL
                ( aViewId, aCustomMessageId, aViewActivationMsg );

            // And check if there is a running stand-alone instance from the 
            // same application. If there is no such, then initialize GSN-Stnd.
            if ( IsStandAloneAppRunning() )
                {
                // The stand-alone app will show GSN, if necessary when terminated.
                }
            else
                {
                // It has to initialize GSN-Stnd
                iUiUtils->InitializeSoftNotifStndL
                    ( aViewId, aCustomMessageId, aViewActivationMsg );
                }
            }
        else
            {
            // No downloads left in - don't show GSN.
            }
        }
    else                                        // Stand-alone
        {
        if ( downloadCnt > 0 )
            {
            // Display GSN-Stnd.
            iUiUtils->InitializeSoftNotifStndL
                ( aViewId, aCustomMessageId, aViewActivationMsg );
            }
        else
            {
            // No downloads left in - don't show GSN.
            }
        }

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::PrepareToExitL");
    }
    
// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::IsStandAloneAppRunning
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiUserInteractions::IsStandAloneAppRunning()
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::IsStandAloneAppRunning");

    TBool ret(0);
    if ( !((CEikonEnv&)iCoeEnv).StartedAsServerApp() )
        {
        // This is the stand-alone app.
        ret = ETrue;
        }
    else
        {
        TApaTaskList apaTaskList( iCoeEnv.WsSession() );
        TUid appUid = iRegistryModel.ClientAppUid();
        TApaTask apaTask = apaTaskList.FindApp( appUid );
        ret = apaTask.Exists();
        }

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::IsStandAloneAppRunning");
    return ret;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::DeleteEventHandlerShowingDlConfirmation
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiUserInteractions::DeleteEventHandlerShowingDlConfirmation
                                   ( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::DeleteEventHandlerShowingDlConfirmation");

    // Find the event handler of this download
    TInt handlerCount = iEventHandlerArray->Count();
    for( TInt i = 0; i < handlerCount; ++i )
        {
        CUserInteractionsEventHandler* handlerI = 
            (CUserInteractionsEventHandler*)iEventHandlerArray->At(i);
        if ( &(handlerI->Download()) == &aDownload )
            {
            // found one.
            if ( handlerI->DownloadConfirmationShown() )
                {
                delete handlerI;
                iEventHandlerArray->Remove( handlerI );
                --i;
                --handlerCount;
                }
            }
        }

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::DeleteEventHandlerShowingDlConfirmation");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::DeleteEventHandlers
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiUserInteractions::DeleteEventHandlers( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::DeleteEventHandlers");

    // Find the event handler of this download
    TInt handlerCount = iEventHandlerArray->Count();
    for( TInt i = 0; i < handlerCount; ++i )
        {
        CUserInteractionsEventHandler* handlerI = 
            (CUserInteractionsEventHandler*)iEventHandlerArray->At(i);
        if ( &(handlerI->Download()) == &aDownload )
            {
            // found one.
            delete handlerI;
            iEventHandlerArray->Remove( handlerI );
            --i;
            --handlerCount;
            }
        }

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::DeleteEventHandlers");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::DownloadConfirmationsShown
// -----------------------------------------------------------------------------
//
TInt CDownloadMgrUiUserInteractions::DownloadConfirmationsShown() const
    {
    TInt count( 0 );
    TInt totalCount = iEventHandlerArray->Count();
    CUserInteractionsEventHandler* eventHandler = NULL;
    for ( TInt i = 0; i < totalCount; ++i )
        {
        eventHandler = (CUserInteractionsEventHandler*)(*iEventHandlerArray)[i];
        if ( eventHandler->DownloadConfirmationShown() )
            {
            count++;
            }
        }
    return count;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::IsUiBusy
// UI is busy, if:
// - DocHandler exists in CUserInteractionsUtils.
// - CodHandler ServiceFlow is running.
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiUserInteractions::IsUiBusy() const
    {
    TBool isUiBusy( ETrue ); // True by default.
    if ( iUiUtils->IsUiBusy() )
        {
        isUiBusy = ETrue;
        }
    else if ( IsCodServiceFlowRunning() )
        {
        isUiBusy = ETrue;
        }
    else
        {
        isUiBusy = EFalse;
        }
    return isUiBusy;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::PostponeCodHandlingL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiUserInteractions::PostponeCodHandlingL( RHttpDownload& aDownload )
    {
    iExtension->PostponeCodHandlingL( aDownload );
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::IsPostponed
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiUserInteractions::IsPostponed( RHttpDownload& aDownload ) const
    {
    return iExtension->IsPostponed( aDownload );
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::SchedulePostponedDownloadL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiUserInteractions::SchedulePostponedDownloadL()
    {
    iExtension->SchedulePostponedDownloadL();
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::IsCodServiceFlowRunning
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiUserInteractions::IsCodServiceFlowRunning() const
    {
    TBool isCodServiceFlowRunning( ETrue ); // true by default
    TRAP_IGNORE( isCodServiceFlowRunning = IsCodServiceFlowRunningL() );
    return isCodServiceFlowRunning;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::IsCodServiceFlowRunning
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiUserInteractions::IsCodServiceFlowRunning
                                    ( RHttpDownload& aDownload ) const
    {
    TBool isCodServiceFlowRunning( ETrue ); // true by default
    TRAP_IGNORE( isCodServiceFlowRunning = IsCodServiceFlowRunningL( aDownload ) );
    return isCodServiceFlowRunning;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::IsCodServiceFlowRunningL
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiUserInteractions::IsCodServiceFlowRunningL() const
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::IsCodServiceFlowRunningL");
    
    TBool isCodServiceFlowRunning( EFalse ); // false by default!
    // Walk through the list of downloads.
    RHttpDownloadMgr& dMgr = iRegistryModel.DownloadMgr();
    TInt downloads = dMgr.CurrentDownloads().Count();
    for ( TInt i = 0; i < downloads; ++i )
        {
        RHttpDownload* download = 
            (RHttpDownload*)(dMgr.CurrentDownloads().At(i));
        CLOG_WRITE_FORMAT(" download: %x",download);
        
        if ( IsCodServiceFlowRunningL( *download ) )
            {
            CLOG_WRITE(" Found!");
            isCodServiceFlowRunning = ETrue;
            break;
            }
        }

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::IsCodServiceFlowRunningL");
    return isCodServiceFlowRunning;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::IsCodServiceFlowRunningL
// CodHandler ServiceFlow is running, if a COD download is hidden, but it is 
// not in the so called postponed downloads list.
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiUserInteractions::IsCodServiceFlowRunningL
                                    ( RHttpDownload& aDownload ) const
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::IsCodServiceFlowRunningL DL");
    
    TBool isCodServiceFlowRunning( EFalse ); // false by default!
    // COD download?
    TBool isCodDownload( EFalse );
    User::LeaveIfError( aDownload.GetBoolAttribute
                      ( EDlAttrCodDownload, isCodDownload ) );
    CLOG_WRITE_FORMAT(" isCodDownload: %d",isCodDownload);
    if ( isCodDownload )
        {
        // check if hidden?
        TBool isHidden( EFalse );
        User::LeaveIfError( aDownload.GetBoolAttribute
                          ( EDlAttrHidden, isHidden ) );
        CLOG_WRITE_FORMAT(" isHidden: %d",isHidden);
        if ( isHidden )
            {
            // if it is not in postponed downloads list...
            if ( !IsPostponed( aDownload ) )
                {
                isCodServiceFlowRunning = ETrue;
                }
            }
        }

    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::IsCodServiceFlowRunningL DL");
    return isCodServiceFlowRunning;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::HandleNormalDownloadL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiUserInteractions::HandleNormalDownloadL( RHttpDownload& aDownload )
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::HandleNormalDownloadL");

    if ( IsUiBusy() )
        {
        // It's handling a downloaded content. Do not open a new.
        CLOG_WRITE(" IsUiBusy() true");
        }
    else
        {
        iRegistryModel.DownloadsList().DisplayDownloadsListL(aDownload);
        } 
    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::HandleNormalDownloadL");
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::CodDownloadL
// -----------------------------------------------------------------------------
//
TBool CDownloadMgrUiUserInteractions::CodDownloadL( RHttpDownload& aDownload ) const
    {
    CLOG_ENTERFN("CDownloadMgrUiUserInteractions::CodDownloadL");

    TBool isCodDownload( EFalse );
    User::LeaveIfError( aDownload.GetBoolAttribute
                        ( EDlAttrCodDownload, isCodDownload ) );
                        
    CLOG_WRITE_FORMAT(" ret: %d",isCodDownload);
    CLOG_LEAVEFN("CDownloadMgrUiUserInteractions::CodDownloadL");
    return isCodDownload;
    }

// -----------------------------------------------------------------------------
// CDownloadMgrUiUserInteractions::HandleDMgrEventL
// -----------------------------------------------------------------------------
//
void CDownloadMgrUiUserInteractions::HandleDMgrEventL
    ( RHttpDownload& aDownload, THttpDownloadEvent aEvent )
    {
    CLOG_WRITE_EVENT("UsrInt",&aDownload,aEvent);

    if ( aEvent.iDownloadState == EHttpDlMediaInserted ||  
         aEvent.iDownloadState == EHttpDlPausable || 
         aEvent.iDownloadState == EHttpDlNonPausable )
        {
        // This event is not handled by User Interactions Event Handler
        }
    else if ( aEvent.iDownloadState == EHttpDlDeleted || 
              aEvent.iDownloadState == EHttpDlDeleting )
        {
        DeleteEventHandlers( aDownload );
        iUiUtils->DownloadHasBeenDeleted( &aDownload );
        }
    else if ( aEvent.iDownloadState == EHttpDlAlreadyRunning )
        {
        // Display information note
        CUserInteractionsUtils::InfoNoteL( R_DMUL_OK_NOTE, R_DMUL_NOTE_DOWNLOADING );
        }
    else if ( aEvent.iDownloadState == EHttpDlInprogress )
        {
        if ( aEvent.iProgressState == EHttpProgResponseHeaderReceived || 
             aEvent.iProgressState == EHttpContTypeRecognitionAvail || 
             aEvent.iProgressState == EHttpProgSupportedMultiPart || 
             aEvent.iProgressState == EHttpProgDlNameChanged )             
            {
            // Create the asynchronous event handler that will handle the event.
            CUserInteractionsEventHandler* asyncHandler = 
                new (ELeave) CUserInteractionsEventHandler
                ( aDownload, aEvent, iRegistryModel, *iEventHandlerArray, *iUiUtils );
            CleanupStack::PushL( asyncHandler );
            // iEventHandlerArray will own it
            iEventHandlerArray->AppendL( asyncHandler );
            CleanupStack::Pop( asyncHandler ); // asyncHandler - ownership transferred
            }
        // If COD is downloaded, close download confirmation query for this download:
        // COD shows accept/reject query too.
        else if ( aEvent.iProgressState == EHttpProgCodDescriptorDownloaded )
            {
            // delete event handlers showing download confirmation.
            DeleteEventHandlerShowingDlConfirmation( aDownload );
            }
        else if ( aEvent.iProgressState == EHttpProgCodDescriptorAccepted || 
                  aEvent.iProgressState == EHttpProgCodLoadEnd            ||
			      aEvent.iProgressState == EHttpProgCodPdAvailable ) 
            {
            // Open downloads list.
            // Create the asynchronous event handler that will handle the event, because 
            // it will run a 'wait' dialog.
            CUserInteractionsEventHandler* asyncHandler = 
                new (ELeave) CUserInteractionsEventHandler
                ( aDownload, aEvent, iRegistryModel, *iEventHandlerArray, *iUiUtils );
            CleanupStack::PushL( asyncHandler );
            // iEventHandlerArray will own it
            iEventHandlerArray->AppendL( asyncHandler );
            CleanupStack::Pop( asyncHandler ); // asyncHandler - ownership transferred
            }
        }
    else if ( aEvent.iDownloadState == EHttpDlPaused )
        {
        if ( aEvent.iProgressState == EHttpContentTypeReceived )
            {
            // Create the asynchronous event handler that will handle the event.
            CUserInteractionsEventHandler* asyncHandler = 
                new (ELeave) CUserInteractionsEventHandler
                ( aDownload, aEvent, iRegistryModel, *iEventHandlerArray, *iUiUtils );
            CleanupStack::PushL( asyncHandler );
            // iEventHandlerArray will own it
            iEventHandlerArray->AppendL( asyncHandler );
            CleanupStack::Pop( asyncHandler ); // asyncHandler - ownership transferred
            }
        }
    else
        {
        // In case of completed or failed download, delete event handlers 
        // that are showing download confirmation.
        if ( aEvent.iDownloadState == EHttpDlMultipleMOCompleted || 
             aEvent.iDownloadState == EHttpDlMultipleMOFailed )
            {
            DeleteEventHandlerShowingDlConfirmation( aDownload );
            }
            
        // Create the asynchronous event handler that will handle the event.
        CUserInteractionsEventHandler* asyncHandler = 
            new (ELeave) CUserInteractionsEventHandler
            ( aDownload, aEvent, iRegistryModel, *iEventHandlerArray, *iUiUtils );
        CleanupStack::PushL( asyncHandler );
        // iEventHandlerArray will own it
        iEventHandlerArray->AppendL( asyncHandler );
        CleanupStack::Pop( asyncHandler ); // asyncHandler - ownership transferred
        }
    }

/* End of file. */

