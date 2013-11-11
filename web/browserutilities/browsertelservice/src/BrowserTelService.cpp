/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/


// INCLUDE FILES
#include <BrowserTelService.rsg>
#include <coemain.h>
#include    <data_caging_path_literals.hrh>

// For phonebook API
//#include <CPbkContactItem.h>    // Phonebook Contact
//#include <CPbkContactEngine.h>  // Phonebook Engine
//#include <CPbkFieldsInfo.h>     // Phonebook field types
//#include <RPbkViewResourceFile.h>
//#include <CPbkContactEditorDlg.h>   // Phonebook editor
// For searching a matching number from contact's db
//#include <TPbkContactItemField.h>
#include <cntfldst.h>

#include <featmgr.h>
// 
#include <txtrich.h>
#include <badesca.h>
#include <s32strm.h>

// For email API
#include <sendui.h>
#include <CMessageData.h>
#include <TSendingCapabilities.h>
#include <SenduiMtmUids.h>
 
// For BaflUtils
#include <bautils.h>

// Stringloader
#include <StringLoader.h>

// TApaTask, TApaTaskList
#include <apgtask.h>

// Connection management
#include <in_sock.h>
#include <es_enum.h>

#include <ApUtils.h>
#include <ApDataHandler.h>
#include <ApAccessPointItem.h>
#include <commdb.h>

#include <f32file.h>
// Const file name for make call
#include <mmtsy_names.h>

#include <BrowserUiSDKCRKeys.h>
#include <centralrepository.h>

#include <xml/cxml/nwx_status.h>

#include <DialUtils.h>

#include "BrowserTelService.h"
#include "BrowserTelServiceDlgs.h"
#include "TelServiceLogger.h"

#include "BrowserUtilitiesVariant.hrh" // KBrowserUtilitiesMailtoSchemeToMmsEditor

const TInt KParseArrayAllocation = 10;
const TInt KTimerDelay = 2500000;
//constants 
const TUid KUidMsgTypeSMTP                      = {0x10001028}; // 268439592 

_LIT( KValidDTMFChars,"w/p*#0123456789ABCDabcd," );
_LIT( KDTMFWaitChars, "/wp,*#" );
#ifdef __BROWSER_TEL_SERVICES_CALLUI__
_LIT( KDTMFInvalidWaitChars, "/," );
_LIT( KDTMFValidPause, "p" );
#endif // __BROWSER_TEL_SERVICES_CALLUI__
// load resource file
_LIT( KResourceFileName, "Z:browsertelservice.RSC" ); 

// Path of the temp file
_LIT(KTempDrive, "d:");
_LIT( KTempPath, "\\system\\temp\\" );

// The library to be loaded.
_LIT( KDialUtilsLoadLib, "DialUtils.dll" );

const TInt KKimonoBase = 26000;

/*
 This is a conversion table from browsertelservice internal
 error codes to global and localized error ids.
 {local error code, global error id}
 Exception is KErrArgument-NW_STAT_WTAI_INVALID_ARGUMENT pair, because
 case of wrong dtmf sequence has no WTAI correspondent error code.
*/
const TInt KErrorConvTable[][2] = 
    {
    {KErrInvocationError, NW_STAT_WTAI_INVOCATIONERROR},
    {KErrNoCallActive, NW_STAT_WTAI_NOCALLACTIVE},
    {KErrUserNoAnswer, NW_STAT_WTAI_NOANSWER},
    {KErrNoService, NW_STAT_WTAI_NOSERVICE},
    {KErrUserBusy, NW_STAT_WTAI_USERBUSY},
    {KErrPBPhoneBookFull, NW_STAT_WTAI_PHONEBOOKFULL},
    {KErrPBWriteError, NW_STAT_WTAI_PBWRITEERROR},
    {KErrPBNumberTooLong, NW_STAT_WTAI_PBNUMBERTOOLONG},
    {KErrPBNameTooLong, NW_STAT_WTAI_PBNAMETOOLONG},
    {KErrUnspecified, NW_STAT_WTAI_UNSPECIFIED_ERROR},
    {KErrArgument,NW_STAT_WTAI_INVALID_ARGUMENT},
    {KErrEtelBusyDetected, NW_STAT_WTAI_USERBUSY},
    {KErrEtelNoAnswer, NW_STAT_WTAI_NOANSWER},
    {0,0}
    };

// ================= MEMBER FUNCTIONS =======================

//-----------------------------------------------------------------------------
// CBrowserTelService::NewLC()
//-----------------------------------------------------------------------------
//
EXPORT_C CBrowserTelService* CBrowserTelService::NewLC(
							MBrowserTelServiceObserver* aObserver )
	{
    CBrowserTelService* self = new( ELeave ) CBrowserTelService;
    CleanupStack::PushL( self );
    self->ConstructL( aObserver );
    return self;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::NewL()
//---------------------------------------------------------------------------------------
//
EXPORT_C CBrowserTelService* CBrowserTelService::NewL(
							MBrowserTelServiceObserver* aObserver )
	{
    CBrowserTelService* self = CBrowserTelService::NewLC( aObserver );
    CleanupStack::Pop();
    return self;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::ConstructL()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::ConstructL( MBrowserTelServiceObserver* aObserver )
	{
    TELSERVICE_CREATE;

	iServiceHandler = NULL;
	
    SetState( EIdle );
    
    if( aObserver )
		{
        iObserverList.Append( aObserver );
		}

    iSynch = EFalse;
	iIdle = CIdle::NewL( CActive::EPriorityIdle );

    // Prepare telservice logger.
    TELSERVICE_CREATE;
	
	// Resouce file loading
    TParse* fileParser = new (ELeave) TParse;
    CleanupStack::PushL( fileParser );
    
    fileParser->Set( KResourceFileName, &KDC_RESOURCE_FILES_DIR, NULL ); 
    iResourceFileName = fileParser->FullName();
    iResourceLoader.OpenL( iResourceFileName );
    iResourceOpened = ETrue;

    CleanupStack::PopAndDestroy( fileParser ); // fileParser	

    iErrorUi = CErrorUI::NewL();

#ifdef __BROWSER_TEL_SERVICES_CALLUI__    
    iDialData = CAiwDialData::NewL();
#endif // __BROWSER_TEL_SERVICES_CALLUI__

    CActiveScheduler::Add( this );
	}

//---------------------------------------------------------------------------------------
// Default C++ constructor
//---------------------------------------------------------------------------------------
//
CBrowserTelService::CBrowserTelService() 
:   CActive( EPriorityNormal ), 
    iResourceFileName( KNullDesC ), 
    iResourceLoader( *CCoeEnv::Static() ),
    iResourceOpened( EFalse )
	{
	}

//---------------------------------------------------------------------------------------
// Destructor
//---------------------------------------------------------------------------------------
//
EXPORT_C CBrowserTelService::~CBrowserTelService()
	{
    TELSERVICE_ENTERFN( "~CBrowserTelService()" )

	delete iServiceHandler;
    iServiceHandler = NULL;

    // Cancel all current activity
    Cancel();

    // Close all open sessions
    CloseAllSessions();

#ifdef __BROWSER_TEL_SERVICES_CALLUI__    
	if (iDialData)
			{
			delete iDialData;
			}
#endif // __BROWSER_TEL_SERVICES_CALLUI__
			
    if( iSendableDTMFNumbers )
        {
        iSendableDTMFNumbers->ResetAndDestroy();
        delete iSendableDTMFNumbers;
        }

    if( iDlgDTMFNumbers )
        {
        iDlgDTMFNumbers->ResetAndDestroy();
        delete iDlgDTMFNumbers;
        }

    // Empty the observer list
    iObserverList.Reset();
/*
	if ( iResourceFileOffset )
        {
		CCoeEnv::Static()->DeleteResourceFile( iResourceFileOffset );
		}
	 */
    if ( iResourceOpened )
        {
        iResourceLoader.Close();
        iResourceOpened = EFalse;
        }
    
#ifdef __BROWSER_TEL_SERVICES_CALLUI__
    
    if( iDTMFNumber )
        {
        delete iDTMFNumber;
        iDTMFNumber = NULL;
        }

#endif // __BROWSER_TEL_SERVICES_CALLUI__

	delete iIdle;
    delete iErrorUi;

    TELSERVICE_LEAVEFN( "~CBrowserTelService()" )
    TELSERVICE_DELETE
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::InitializeL()
//---------------------------------------------------------------------------------------
//
EXPORT_C HBufC* CBrowserTelService::ParsePhoneNumberL( TDes* aNumber )
    {
    TELSERVICE_WRITE( "ParsePhoneNumberL()" )
    return FindAndRipDTMFSequenceL( aNumber );
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::MakeCall()
//---------------------------------------------------------------------------------------
//
EXPORT_C TInt CBrowserTelService::MakeCall( TDesC& aNumber, 
										    TBool aSendDTMFAlwaysConfirm )
	{
    TELSERVICE_ENTERFN( "MakeCall() synchronous" )
       
    TInt retVal( KErrNone );

    iRetVal = KErrNone;

    // Flag to indicate that synchronous method was called
    iSynch = ETrue;
    TRAP( retVal, MakeCallL( aNumber, aSendDTMFAlwaysConfirm ) );

    if( retVal != KErrNone )
		{
        ErrorHandler( retVal );

        retVal = KErrCancel;
        }
    iSynch = EFalse;

    TELSERVICE_LEAVEFN( "MakeCall() synchronous" )
    return retVal;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::MakeCall()
//---------------------------------------------------------------------------------------
//
EXPORT_C void CBrowserTelService::MakeCall( TRequestStatus& aStatus, 
										    TDesC& aNumber, 
										    TBool aSendDTMFAlwaysConfirm )
	{
    TELSERVICE_ENTERFN( "MakeCall() asynchronous" )

    iRetVal = KErrNone;
    
    aStatus = KRequestPending;
    iUserRequestStatus = &aStatus;
    iSynch = EFalse;

    TRAPD( err, MakeCall( aNumber, aSendDTMFAlwaysConfirm ) );

    TELSERVICE_WRITE_FORMAT( "Asynch makecall start: %d", err )

    if( err )
        {
        ErrorHandler( err );

        User::RequestComplete( iUserRequestStatus, err );

        CleanupBuffers();
        }

    TELSERVICE_LEAVEFN( "MakeCall() asynchronous" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::CancelMakeCall()
//---------------------------------------------------------------------------------------
//
EXPORT_C void CBrowserTelService::CancelMakeCall()
	{
    TELSERVICE_ENTERFN( "CancelMakeCall()" )
  
#ifdef __BROWSER_TEL_SERVICES_CALLUI__

    // There is no ongoing request

#else //  __BROWSER_TEL_SERVICES_CALLUI__

    Cancel();
    SetStateIdleAndNotifyObservers();

#endif // __BROWSER_TEL_SERVICES_CALLUI__

    TELSERVICE_LEAVEFN( "CancelMakeCall()" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::MakeCallL()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::MakeCallL( TDesC& aNumber, TBool aSendDTMFAlwaysConfirm, TBool voipCall )
    {
    iDTMFAlwaysConfirm = aSendDTMFAlwaysConfirm;
    iRetVal = KErrNone;

    if( State() != EIdle )
		{
        return;
        }

    if( aNumber.Length() == 0 )
        {
        User::Leave( KErrInvocationError );
        }


#ifdef __BROWSER_TEL_SERVICES_CALLUI__
    if( aNumber.Length() > AIWDialDataExt::KMaximumPhoneNumberLength )
        {
        User::Leave( KErrPBNumberTooLong );
        }

    HBufC* orgNumber = HBufC::NewLC( AIWDialDataExt::KMaximumPhoneNumberLength );
#else // __BROWSER_TEL_SERVICES_CALLUI__

    if( aNumber.Length() > RMobilePhone::KMaxMobileTelNumberSize )
        {
        User::Leave( KErrPBNumberTooLong );
        }

    HBufC* orgNumber = HBufC::NewLC( RMobilePhone::KMaxMobileTelNumberSize );

#endif // __BROWSER_TEL_SERVICES_CALLUI__
    
    orgNumber->Des().Copy( aNumber );

    if( FeatureManager::FeatureSupported( KFeatureIdJapanPrefixChange ) )
        {
        RLibrary dialUtils;
        if( dialUtils.Load( KDialUtilsLoadLib ) == KErrNone )
            {
            CleanupClosePushL( dialUtils );

            // Call function CreateDialUtilsFactoryL()
            TInt res = dialUtils.Lookup( 1 )();
            CDialUtilsFactory * dialUtilsFactory = 
                reinterpret_cast< CDialUtilsFactory * >( res );

            // This way the library is not needed at the linking time.
            // Create Dial Utils (see that the function deletes the factory
            // instance):
            CDialUtilsApi* dialUtilsApi = dialUtilsFactory->CDialUtilsApiLD();
            dialUtilsFactory = NULL;

            // Then use the Dial Utils API, for example:
            if( dialUtilsApi->CheckNumber( *orgNumber ) == KErrOverflow )
                // no we have the converted number
                {
                User::Leave( KErrPBNumberTooLong );
                }

            delete dialUtilsApi;
            CleanupStack::PopAndDestroy( &dialUtils );
            }
        }

    // Handle postd format
    TPtr orgNumberPtr( orgNumber->Des() );    
    HBufC* postdParm = FindPostdParmL( orgNumberPtr );
    
    if(voipCall)
    {
    	FindAndRipVoipParametersL( orgNumberPtr );
    }
    
        
    // Rip URI parameters from Tel URI, in order to that
    // number validation and DTMF works as earlier
    // Parameters will be added back in later phase
    HBufC* params = FindAndRipURIParametersL( orgNumberPtr );
    CleanupStack::PushL( params );

    HBufC* number = ValidateNumberL( *orgNumber );
    CleanupStack::PushL( number );
    
    TPtr numberPtr = number->Des();
    iDTMFNumber= FindAndRipDTMFSequenceL( &numberPtr );

    TBool isPostd(EFalse);
    if ( ! iDTMFNumber )
        {
        iDTMFNumber = postdParm; 
        if ( iDTMFNumber )
            {
            isPostd = ETrue; 
            }
        }
    else 
        {
        delete postdParm;     
        }
#ifndef __BROWSER_TEL_SERVICES_CALLUI__

    iSendableDTMFNumbers = ParseDTMFSequencesL( iDTMFNumber );
    iDlgDTMFNumbers = ParseDTMFSequencesForDlgL( iDTMFNumber );

#endif // __BROWSER_TEL_SERVICES_CALLUI__

    if( ShowDialogL( *number, EConfirmMakeCall ) )
        {            

#ifndef __WINS__
        DisconnectActiveCSDConnectionL();
#endif

#ifdef __BROWSER_TEL_SERVICES_CALLUI__

        HBufC* temp = NULL;
        if( iDTMFNumber )
            {
            // Confirm the user (s)he wants to send the DTMF
            if( SendDMFConfQueryL( iDTMFNumber->Des() ) )
                {
                if ( !isPostd )
                    {                    
                    // Add iDTMFNumber to number
                    temp = HBufC::NewLC( number->Length() + iDTMFNumber->Length() );
                    temp->Des().Copy( number->Des() );
                    temp->Des().Append( iDTMFNumber->Des() );
                    }
                else 
                    {
                    // Add p+iDTMFNumber to number for postd
                    temp = HBufC::NewLC( number->Length() + iDTMFNumber->Length() + 1 /*for the 'p' character*/ );
                    temp->Des().Copy( number->Des() );
                    temp->Des().Append( 'p' );
                    temp->Des().Append( iDTMFNumber->Des() );                        
                    }
                }
            else
                {
                // Call number without dtmf
                temp = number->AllocLC();
                }
            }
        else
            {
            temp = number->AllocLC();
            }

        temp = temp->ReAllocL( temp->Length() + params->Length() );
        CleanupStack::Pop( temp ); // location may have changed, update cleanupstack
        CleanupStack::PushL( temp );
        temp->Des().Append( *params );  

        // ... and make the call
        DoMakeCallL( *temp, voipCall );

        CleanupStack::PopAndDestroy( temp ); // temp

#else // __BROWSER_TEL_SERVICES_CALLUI__

        // ... and make the call
        DoMakeCallL( *number, voipCall );

#endif // __BROWSER_TEL_SERVICES_CALLUI__
        }
    else
        {
        User::Leave( KErrCancel );
        }

    CleanupStack::PopAndDestroy( 3 );   // orgNumber, params, number
    }

#ifdef __BROWSER_TEL_SERVICES_CALLUI__

//---------------------------------------------------------------------------------------
// CBrowserTelService::SendDMFConfQueryL()
//---------------------------------------------------------------------------------------
//
TBool CBrowserTelService::SendDMFConfQueryL( const TDesC& aDTMF )
    {
    TBool retVal( EFalse );


    TELSERVICE_ENTERFN( "SendDMFConfQueryL()" )

    if( 0 == aDTMF.Length() )
        {
        return retVal;
        }

    CRepository* repository = CRepository::NewL( KCRUidBrowser );
    CleanupStack::PushL (repository );
    repository->Get( KBrowserConfirmedDTMFOnce, iConfirmedOnce );    
        
    TELSERVICE_WRITE( "display send dtmf dialog" )

    if( iDTMFAlwaysConfirm )
        {
        retVal = ShowDialogL( aDTMF, EConfirmSendDTMF );
        }
    else if( iDTMFAlwaysConfirm == EFalse && 
             iConfirmedOnce == EFalse )
        {
        retVal = ShowDialogL( aDTMF, 
							  EConfirmSendDTMF );
        iConfirmedOnce = ETrue;
        repository->Set( KBrowserConfirmedDTMFOnce, iConfirmedOnce ); 		
        }

    CleanupStack::PopAndDestroy( repository ); 
    
    return retVal;
    }

#endif // __BROWSER_TEL_SERVICES_CALLUI__

//---------------------------------------------------------------------------------------
// CBrowserTelService::CleanupBuffs()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::CleanupBuffers()
    {
    if( iSendableDTMFNumbers )
        {
        iSendableDTMFNumbers->ResetAndDestroy();
        delete iSendableDTMFNumbers;
		iSendableDTMFNumbers = NULL;
        }

    if( iDlgDTMFNumbers )
        {
        iDlgDTMFNumbers->ResetAndDestroy();
        delete iDlgDTMFNumbers;
		iDlgDTMFNumbers = NULL;
        }
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::DoMakeCallL()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::DoMakeCallL( TDesC& aNumber, TBool voipCall )
	{
    TELSERVICE_ENTERFN( "DoMakeCallL()" )
   
#ifndef __BROWSER_TEL_SERVICES_CALLUI__

    RTelServer::TPhoneInfo info;
    RPhone::TLineInfo lineInfo;
	
    // Connect to the telephony server and load the TSY.
    User::LeaveIfError( iRTelServer.Connect() );
    User::LeaveIfError( iRTelServer.LoadPhoneModule( KMmTsyModuleName ) );
    // Get the details for the first (and only) phone.
    User::LeaveIfError( iRTelServer.GetPhoneInfo( 0, info ) );
    // Open the phone.
    User::LeaveIfError( iRPhone.Open( iRTelServer, info.iName ) );
    // Get the information for the voice line, line 0.
    User::LeaveIfError( iRPhone.GetLineInfo( 0, lineInfo ) );
    // Open the line. iName will now be "VoiceLine1".
    User::LeaveIfError( iRLine.Open( iRPhone, KMmTsyVoice1LineName ) );     
	TInt count;
	// Gets the number of calls opened from a line.	
	User::LeaveIfError( iRLine.EnumerateCall( count ) );
	if ( count > 2 )
		{
		HBufC* text = StringLoader::LoadL(
			R_TEXT_CALL_NOT_ALLOWED, 
			 CEikonEnv::Static() );
		CleanupStack::PushL( text );
		TDialerDialogs::ErrorNoteL( *text );
		CleanupStack::PopAndDestroy();// text
		User::Leave( KErrNone );		
		}

	// Open a new phone call.
    User::LeaveIfError( iRCall.OpenNewCall( iRLine ) );

    // Initialize state
    SetState( EDialing );

    // Actually dial the specified number.
    iRCall.Dial( iStatus, aNumber );
    SetActive();

    NotifyObservers();

    if( iSynch )
		{
        TELSERVICE_WRITE( "starting DoMakeCallL wait loop" )
		iWait.Start();  // start the wait loop. DoMakeCallL() method won't 
						// return until iWait->AsyncStop() is called in RunL()
        TELSERVICE_WRITE( "DoMakeCallL wait loop stopped" )

        if( iRetVal == KErrNone )
            {
            if( HandleInternalSendDTMFL() )
                {
                TELSERVICE_WRITE( "starting HandleInternalSendDTMFL wait loop" )
                iInternalDTMFWaitStarted = ETrue;
                iWait.Start();
				iRAdvGsmPhone.Close();
                TELSERVICE_WRITE( "HandleInternalSendDTMFL wait loop stopped" )

                if( iWaitDlg )
                    {
                    iWaitDlg->ProcessFinishedL();
                    }
                iInternalDTMFWaitStarted = EFalse;
                }
            }
        else
            {
            User::Leave( iRetVal );
		    }
	    }

#else // __BROWSER_TEL_SERVICES_CALLUI__

	if(!iServiceHandler)
		{
    	iServiceHandler = CAiwServiceHandler::NewL();
		}
    iServiceHandler->AttachL( R_BROWSERTELSERVICES_MENU_AIW_INTERESTS );


    CAiwGenericParamList& paramList = iServiceHandler->InParamListL();
    CAiwGenericParamList& output = iServiceHandler->OutParamListL();

    TPtrC number = aNumber;

    iDialData->SetPhoneNumberL( number );
    if( !voipCall )
    {
    	iDialData->SetCallType( CAiwDialData::EAIWVideo );
    }
    else
    {
    	iDialData->SetCallType( CAiwDialData::EAIWVoiP );
    }
    iDialData->FillInParamListL( paramList );

    iServiceHandler->ExecuteServiceCmdL(
                    KAiwCmdCall,
                    paramList,
                    output,
                    0,      // No options used.
                    this ); //

    if( iSynch )
        {
        iWait.Start();
        }
 
#endif // __BROWSER_TEL_SERVICES_CALLUI__

    TELSERVICE_LEAVEFN( "DoMakeCallL()" )
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::SendDTMFL()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::SendDTMFL( TDesC& aNumber, TBool aSendDTMFAlwaysConfirm )
    {
    iDTMFAlwaysConfirm = aSendDTMFAlwaysConfirm;

    iRetVal = KErrNone;
	CheckIsThereActivateVoiceCallOnL();

    if( State() == EConnected )
	    {
        CheckDTMFNumberL( aNumber );

        // Set flag to indicate that synchronous method was called
        iSynch = ETrue;

        delete iSendableDTMFNumbers;
        iSendableDTMFNumbers = NULL;

        delete iDlgDTMFNumbers;
        iDlgDTMFNumbers = NULL;

        iSendableDTMFNumbers = ParseDTMFSequencesL( &aNumber );
        iDlgDTMFNumbers = ParseDTMFSequencesForDlgL( &aNumber );

        if( HandleInternalSendDTMFL() )
            {
            TELSERVICE_WRITE( "starting HandleInternalSendDTMFL wait loop" )
            iInternalDTMFWaitStarted = ETrue;
            iWait.Start();
            TELSERVICE_WRITE( "HandleInternalSendDTMFL wait loop stopped" )

            if( iWaitDlg )
                {
                iWaitDlg->ProcessFinishedL();
                }
            iInternalDTMFWaitStarted = EFalse;
            }

        // Unset flag
        iSynch = EFalse;
	    }
    else    // no active voice connection 
		{
        User::Leave( KErrNoCallActive );
		}
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::SendDTMFL()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::SendDTMFL( TRequestStatus& aStatus,TDesC& aNumber, 
						            TBool aSendDTMFAlwaysConfirm )
    {
    iDTMFAlwaysConfirm = aSendDTMFAlwaysConfirm;

    aStatus = KRequestPending;
    iUserRequestStatus = &aStatus;
	CheckIsThereActivateVoiceCallOnL();
	
    if( State() == EConnected )
		{
        CheckDTMFNumberL( aNumber );

        if( iSendableDTMFNumbers )
            {
            iSendableDTMFNumbers->ResetAndDestroy();
            }
        delete iSendableDTMFNumbers;
        iSendableDTMFNumbers = NULL;

        if( iDlgDTMFNumbers )
            {
            iDlgDTMFNumbers->ResetAndDestroy();
            }
        delete iDlgDTMFNumbers;
        iDlgDTMFNumbers = NULL;

        iSendableDTMFNumbers = ParseDTMFSequencesL( &aNumber );
        iDlgDTMFNumbers = ParseDTMFSequencesForDlgL( &aNumber );

        HandleInternalSendDTMFL();
		}
    else    // no active voice connection 
		{
        User::Leave( KErrNoCallActive );
		}
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::DoSendDTMFL()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::DoSendDTMFL( TDesC& aNumber )
	{
    TELSERVICE_ENTERFN( "DoSendDTMFL()" )

    CheckIsThereActivateVoiceCallOnL();
    
    if( State() == EConnected )
		{
        // Cancel any outstanding requests
        Cancel();

        RTelServer::TPhoneInfo info;
        User::LeaveIfError( iRTelServer.GetPhoneInfo( 0, info ) );
        User::LeaveIfError( iRAdvGsmPhone.Open( iRTelServer, info.iName ) );
        
        // Set state
        SetState( EDialingDTMF );
        // Notify the observer
        NotifyObservers();

        // Send DTMF tones
        if( aNumber.Length() )
            {
            iRAdvGsmPhone.SendDTMFTones( iStatus, aNumber );
            }
        else
            {
            TRequestStatus* status = &iStatus;
            User::RequestComplete( status, KErrNone );
            }

        SetActive();

        if( iSynch )
			{
            iWait.Start();  // start the wait loop. DoSendDTMFL() 
							// method won't return 
                            // until iWait->AsyncStop() is called in RunL() 
			}
		}
    else
        {
        User::Leave( KErrCancel );
        }

    TELSERVICE_LEAVEFN( "DoSendDTMFL()" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::ValidateNumberL()
//---------------------------------------------------------------------------------------
//
HBufC* CBrowserTelService::ValidateNumberL( TDesC& aNumber )
	{
    TELSERVICE_ENTERFN( "ValidateNumberL()" )

    HBufC* buf = HBufC::NewLC( aNumber.Length() );
    TPtr ptr = buf->Des();
	ptr.Copy( aNumber );

    for( TInt i = 0; i < ptr.Length(); i++ )
        {
        TChar currentChar = ptr[i];

        if( currentChar.IsSpace() )
            {
            ptr.Delete( i, 1 );
            i--;
            }
        else if( !currentChar.IsDigit() )
            {
            if( currentChar == '+' )
                {
                if( i != 0 )
                    {
                    User::Leave( KErrInvocationError );
                    }
                }
            else
                {
                currentChar.UpperCase();

                switch( currentChar )
                    {
                    case '*':
                    case '#':
                    case 'A':
                    case 'B':
                    case 'C':
                    case 'D':
                    case 'P':
                    case 'W':
                    case ',':
                    case '/':
                        if( i == 0 )
                            {
                            User::Leave( KErrInvocationError );
                            }
                        break;
						
					case '.': 
					case '-':
					case '(':
					case ')':
                        {
			            ptr.Delete( i, 1 );
						i--;
                        }
						break; 

                    default:
                        // invalid character in the phone number
                        {
                        User::Leave( KErrInvocationError );
                        }
                        break;
                    }
                }
            }
        }

    CleanupStack::Pop( buf );   // buf

    TELSERVICE_LEAVEFN( "ValidateNumberL()" )
	return buf;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::FindAndHandlePostdL()
//---------------------------------------------------------------------------------------
//
HBufC* CBrowserTelService::FindPostdParmL( TDes& aNumber )
    {
    TELSERVICE_ENTERFN( "FindPostdParmL()" )
    
    HBufC* buf = NULL;    
    _LIT( KPostd, ";postd=" ); 
    TInt ret = aNumber.Find( KPostd );
    if ( ret != KErrNotFound )
        {
        buf = aNumber.Mid( ret + KPostd().Length() ).AllocL(); 
        aNumber.SetLength ( ret );
        }
    TELSERVICE_LEAVEFN( "FindPostdParmL()" )
    return buf;            
    }


//---------------------------------------------------------------------------------------
// CBrowserTelService::FindAndRipDTMFSequenceL()
//---------------------------------------------------------------------------------------
//
HBufC* CBrowserTelService::FindAndRipDTMFSequenceL( TDes* aNumber )
    {
    TELSERVICE_ENTERFN( "FindAndRipDTMFSequnceL()" )

    HBufC* buf = NULL;    

	TPtrC waitChars;
	waitChars.Set( KDTMFWaitChars );

    // Look for the first DTMF-wait char and extract the
    // dtmf string from the phonenumber
    for( TInt i = 0; i < aNumber->Length(); i++ )
        {
        if( ( waitChars.Locate( (*aNumber)[i] ) != KErrNotFound ) )
            {
#ifdef __BROWSER_TEL_SERVICES_CALLUI__

            buf = aNumber->Mid(i).AllocL();
            aNumber->Delete( i, buf->Length() );

            // Remove all the invalid wait chars from 
            // the sequence which is not accepted by CallUi
	        TPtrC invalidWaitChars;
	        invalidWaitChars.Set( KDTMFInvalidWaitChars );

            for( TInt j = 0; j < buf->Length(); j++ )
                {
                if( ( invalidWaitChars.Locate( (*buf)[j] ) != KErrNotFound ) )
                    {
                    TPtr ptr = buf->Des();
                    ptr.Replace( j, 1,KDTMFValidPause() );
                    }
                }

#else // __BROWSER_TEL_SERVICES_CALLUI__

            if( (*aNumber)[i] == '#' ||
                (*aNumber)[i] == '*' )
                // these chars are parts of the dtms sequence
                {
                buf = aNumber->Mid(i).AllocL();
                aNumber->Delete( i, buf->Length() );
                }
            else
                {
                // +1 -> wait char is not part of the dtmf sequence
                buf = aNumber->Mid(i+1).AllocL();
                aNumber->Delete( i, buf->Length() + 1);
                }

#endif // __BROWSER_TEL_SERVICES_CALLUI__
            break;
            }
        }
    TELSERVICE_LEAVEFN( "FindAndRipDTMFSequenceL()" )
    return buf;    
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::FindAndRipURIParametersL()
//---------------------------------------------------------------------------------------
//
HBufC* CBrowserTelService::FindAndRipURIParametersL( TDes& aNumber )
    {
    TELSERVICE_ENTERFN( "FindAndRipURIParametersL()" )

    HBufC* buf = NULL;    
    TInt offset = aNumber.Locate( TChar( ';' ) );
    if( offset > 0 ) // Separator is not allowed to be a first char
        {
        __ASSERT_DEBUG( offset <= aNumber.Length(), User::Leave( KErrGeneral ) );
        buf = aNumber.Mid( offset ).AllocL();         
        aNumber.SetLength( offset );           
        }
    else
        {
        buf = HBufC::NewL( 0 );
        }

    TELSERVICE_LEAVEFN( "FindAndRipURIParametersL()" )
    return buf;    
    }
    
    
//---------------------------------------------------------------------------------------
// CBrowserTelService::FindAndRipVoipParametersL()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::FindAndRipVoipParametersL( TDes& aNumber )
    {
    TELSERVICE_ENTERFN( "FindAndRipVoipParametersL()" )

    HBufC* buf = NULL;    
    TInt offset = aNumber.Locate( TChar( '@' ) );
    if( offset > 0 ) // Separator is not allowed to be a first char
        {
        __ASSERT_DEBUG( offset <= aNumber.Length(), User::Leave( KErrGeneral ) );
        buf = aNumber.Mid( offset ).AllocL();         
        aNumber.SetLength( offset );           
        }
    else
        {
        buf = HBufC::NewL( 0 );
        }

    TELSERVICE_LEAVEFN( "FindAndRipVoipParametersL()" )
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::ValidateDTMFNumberL()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::ValidateDTMFNumberL( TDes* aNumber )
    {
    TELSERVICE_ENTERFN( "ValidateDTMFNumberL()" )

    TPtrC validDTMFChars;
    validDTMFChars.Set( KValidDTMFChars );

    if( aNumber )
        {
        for( TInt i = 0; i < aNumber->Length(); i++ )
            {
            if( ( validDTMFChars.Locate( (*aNumber)[i] ) == KErrNotFound ) )
                {
                // Invalid character found, remove it!
                aNumber->Delete( i, 1 );
                i--;
                }
            }
        }
    TELSERVICE_LEAVEFN( "ValidateDTMFNumberL()" )
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::ParseDTMFSequencesForDlg()
//---------------------------------------------------------------------------------------
//
CArrayPtrFlat<HBufC>* CBrowserTelService::ParseDTMFSequencesForDlgL(
										  const TDesC* aDTMFString )
    {
    TELSERVICE_ENTERFN( "ParseDTMFSequencesForDlgL()" )
    CArrayPtrFlat<HBufC>* parsedArray = NULL;
    
    if( aDTMFString )
    {
    parsedArray = new ( ELeave ) CArrayPtrFlat<HBufC>( KParseArrayAllocation ); 
    
    TLex dtmfSequence( *aDTMFString );

    TChar previousChar = dtmfSequence.Peek();   // first char

    while( !dtmfSequence.Eos() )
        {
        //
        if ( dtmfSequence.Offset() ) // if not at start of line
            {
            TChar nextChar = dtmfSequence.Peek();
            if( nextChar == '/' || nextChar == 'w' || nextChar == 'p' ||
				nextChar == ',' )
                {
                // if the previous wait char was either 'w' or '/' then skip it
                // and mark new extraction mark.
                if( previousChar == 'w' || previousChar == '/' )
                    {
                    dtmfSequence.Mark();
                    }
                else
                    {
                    parsedArray->AppendL(
						dtmfSequence.RemainderFromMark().Alloc() );
                    dtmfSequence.Mark();            
                    }
                }
            previousChar = nextChar;
            }
    
        dtmfSequence.Inc();
        }

    parsedArray->AppendL( dtmfSequence.RemainderFromMark().Alloc() );
    }
    TELSERVICE_LEAVEFN( "ParseDTMFSequencesForDlgL()" )
    return parsedArray;
    }


//---------------------------------------------------------------------------------------
// CBrowserTelService::ParseDTMFSequencesForDlg()
//---------------------------------------------------------------------------------------
//
CArrayPtrFlat<HBufC>* CBrowserTelService::ParseDTMFSequencesL(
									const TDesC* aDTMFString )
    {
    TELSERVICE_ENTERFN( "ParseDTFMSequencesL()" )
    CArrayPtrFlat<HBufC>* parsedArray = NULL;

    if( aDTMFString )
		{		
		parsedArray = new ( ELeave ) CArrayPtrFlat<HBufC>( 
			KParseArrayAllocation );
        CleanupStack::PushL( parsedArray );
    
		TLex dtmfSequence( *aDTMFString );

		TChar previousChar = dtmfSequence.Peek();   // first char

		while( !dtmfSequence.Eos() )
			{
			//
			if ( dtmfSequence.Offset() ) // if not at start of line
				{
				TChar nextChar = dtmfSequence.Peek();
				if( nextChar == '/' || nextChar == 'w' || 
					nextChar == 'p' || nextChar == ',' )
					{
					// if the previous wait char was either 'w' or '/' 
					// then skip it
					// and mark new extraction mark.
					if( previousChar == 'w' || previousChar == '/' )
						{
						dtmfSequence.Mark();
						}
					else
						{
						parsedArray->AppendL(
							dtmfSequence.MarkedToken().Alloc() );
						dtmfSequence.Mark();            
						}
					}
				previousChar = nextChar;
				}
    
			dtmfSequence.Inc();
			}

		parsedArray->AppendL( dtmfSequence.RemainderFromMark().Alloc() );

        CleanupStack::Pop( parsedArray );
		}
    TELSERVICE_LEAVEFN( "ParseDTMFSequencesL()" )
    return parsedArray;
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::DisplaySendDTMFDialog()
//---------------------------------------------------------------------------------------
//
TBool CBrowserTelService::DisplaySendDTMFDialog( TDes* aNumber )  
    {
    TELSERVICE_ENTERFN( "DisplaySendDTMFDialog()" )
    TBool ret = EFalse;

    if( aNumber )
        {
        TInt length = aNumber->Length();
        if( length )
            {
            TChar waitChar = RipWaitChar( aNumber );
            
            switch( waitChar )
                {
                case '/':
                case 'w':
					{
                    ret = ETrue;
					}
                    break;

                case 'p':
                case ',':
					{
                    ret = EFalse;
					}
                    break;

                default:
					{
                     ret = ((iDTMFAlwaysConfirm) || (!iConfirmedOnce));
					}
                    break;
                }
            }
        }
    TELSERVICE_LEAVEFN( "DisplaySendDTMFDialog()" )
    return ret;
    }  

//---------------------------------------------------------------------------------------
// CBrowserTelService::RipWaitChar()
//---------------------------------------------------------------------------------------
//
TChar CBrowserTelService::RipWaitChar( TDes* aNumber )
    {
    TELSERVICE_ENTERFN( "RipWaitChar()" )
    TChar waitChar = NULL;
    if( aNumber )
        {
        TInt length = aNumber->Length();
        if( length )
            {
            TChar tempWaitChar = ( *aNumber )[0];
            if( tempWaitChar == 'w' || tempWaitChar == '/' || 
                tempWaitChar == 'p' || tempWaitChar == ',')
                {
                waitChar = tempWaitChar;
                aNumber->Delete( 0, 1 );
                }
            }
        }
    TELSERVICE_LEAVEFN( "RipWaitChar()" )
    return waitChar;
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::SendDTMF()
//---------------------------------------------------------------------------------------
//
EXPORT_C TInt CBrowserTelService::SendDTMF( TDesC& aNumber, 
										    TBool aSendDTMFAlwaysConfirm )
	{
    TELSERVICE_ENTERFN( "SendDTMF() synchronous" )

    iRetVal = KErrNone;

    TRAPD( err, SendDTMFL( aNumber, aSendDTMFAlwaysConfirm ) );

    if( err )
        {
        ErrorHandler( err );
        iRetVal = KErrCancel;
        }

    TELSERVICE_LEAVEFN( "SendDTMF() synchronous" )
    return iRetVal;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::SendDTMFL()
//---------------------------------------------------------------------------------------
//
EXPORT_C void CBrowserTelService::SendDTMF( TRequestStatus& aStatus,
										    TDesC& aNumber, 
										    TBool aSendDTMFAlwaysConfirm )
	{
    TELSERVICE_ENTERFN( "SendDTMF() asynchronous" )

    iRetVal = KErrNone;

    TRAPD( err, SendDTMFL( aStatus, aNumber, aSendDTMFAlwaysConfirm ) );

    if( err )
        {
        TELSERVICE_WRITE_FORMAT( "Complete the request: %d", KErrInvocationError )

        ErrorHandler( err );

        User::RequestComplete( iUserRequestStatus, KErrInvocationError );
        }
    
    TELSERVICE_LEAVEFN( "SendDTMF() asynchronous" )    
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::CancelSendDTMFL()
//---------------------------------------------------------------------------------------
//
EXPORT_C void CBrowserTelService::CancelSendDTMFL()
	{
    TELSERVICE_ENTERFN( "CancelSendDTMF()" )

    if( State() == EDialingDTMF )
		{
        RCall::TStatus callStatus;
        User::LeaveIfError( iRCall.GetStatus( callStatus ) );

        if( callStatus == RCall::EStatusConnected )
			{
            if( State() == EDialingDTMF )
				{
                Cancel();
                SetState( EConnected );
				}
			}
		}

    TELSERVICE_LEAVEFN( "CancelSendDTMF()" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::AddPBEntryL()
//---------------------------------------------------------------------------------------
//
EXPORT_C TInt CBrowserTelService::AddPBEntryL(
								TDesC& aNumber, 
								TDesC& aName, 
								TDesC& aEmail )
    {
    TELSERVICE_ENTERFN( "AddPBEntryL()" )

/*    CPbkContactEngine* pbkEngine = CPbkContactEngine::NewL();
    CleanupStack::PushL( pbkEngine );

    // Make sure a resource file is available
    RPbkViewResourceFile pbkResourceFile( *CCoeEnv::Static() );
    pbkResourceFile.OpenL();
    CleanupClosePushL<RPbkViewResourceFile> ( pbkResourceFile );
	
	// Step 1: Search Pb for matches and open existing or create new
    TBool newContact;
    CPbkContactItem* contactItem = SearchPBItemLC( *pbkEngine
                                                   , aNumber
                                                   , aEmail
                                                   , newContact );

    TPtrC lastName;
    TPtrC firstName;
	TPtrC email;

	email.Set( aEmail );

	// Step 2: Set values to the correct lines
    // default focused dialog line 
    TInt formIndex = -1;

    if( JAPANESE )
        // Japanese version only. See Vodafone spec: 
        // http://developers.vodafone.jp/dp/tool_dl/download.php?docid=120
        {
        TPtrC lastNameReading;

        TInt yomiganaInd = aName.LocateReverse( TChar('-') );

        if( yomiganaInd != KErrNotFound )
            {
            lastNameReading.Set( aName.Right( aName.Length() - yomiganaInd - 1 ) );
            lastName.Set( aName.Left( yomiganaInd ) );
            }
        else
            {
            lastName.Set( aName );
            }

        SetPBEntryFieldL( EPbkFieldIdLastName, pbkEngine, contactItem, lastName, formIndex );
        SetPBEntryFieldL( EPbkFieldIdLastNameReading, pbkEngine, contactItem, lastNameReading, formIndex );
        }
    else
        {
        TInt lastNameInd = aName.LocateReverse( TChar(' ') );
	    TBool whichtag = EFalse;
	    if ( lastNameInd == KErrNotFound )
		    {
		    _LIT( tag, "%20" );
		    lastNameInd = aName.Find( tag );
		    whichtag = ETrue;
		    }
	    
        if( lastNameInd != KErrNotFound )
		    {
            if( lastNameInd )
			    {
			    if ( whichtag )
				    {
				    lastName.Set( aName.Right( aName.Length() - lastNameInd - 3 ) );
				    }
			    else
				    {
				    lastName.Set( aName.Right( aName.Length() - lastNameInd - 1 ) );
				    }            
                firstName.Set( aName.Left( lastNameInd ) );
			    }
            else
			    {
                lastName.Set( aName );
			    }
		    }
        else
		    {
            lastName.Set( aName );
		    }

        SetPBEntryFieldL( EPbkFieldIdLastName, pbkEngine, contactItem, lastName, formIndex );
        SetPBEntryFieldL( EPbkFieldIdFirstName, pbkEngine, contactItem, firstName, formIndex );
        }

    // common fields
    SetPBEntryFieldL( EPbkFieldIdPhoneNumberStandard, pbkEngine, contactItem, aNumber, formIndex );
    SetPBEntryFieldL( EPbkFieldIdEmailAddress, pbkEngine, contactItem, aEmail, formIndex );

    // Step 3: Open the editor dialog
    CPbkContactEditorDlg* dlg = CPbkContactEditorDlg::NewL(
		*pbkEngine, *contactItem, newContact, formIndex, formIndex >= 0 ); 
    
    // Execute phonebook editor
    dlg->ExecuteLD();

    CleanupStack::PopAndDestroy( 3 ); // contactItem, pbkResourceFile, pbkEngine

    TELSERVICE_LEAVEFN( "AddPBEntryL()" )
*/
    return KErrNone;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::State()
//---------------------------------------------------------------------------------------
//
EXPORT_C TBrowserTelServiceState CBrowserTelService::State() const
	{
    TELSERVICE_WRITE_FORMAT( "State(): %d", iState )
    return iState;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::SetState()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::SetState( TBrowserTelServiceState aState )
	{
    TELSERVICE_WRITE( "SetState()" )
    iState = aState;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::AddObserver()
//---------------------------------------------------------------------------------------
//
EXPORT_C void CBrowserTelService::AddObserver(
								MBrowserTelServiceObserver* aObserver )
	{
    TELSERVICE_ENTERFN( "AddObserver()" )

    if( aObserver )
        {
        iObserverList.Append( aObserver );
		}
    TELSERVICE_LEAVEFN( "AddObserver()" )
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::RemoveObserver()
//---------------------------------------------------------------------------------------

EXPORT_C void CBrowserTelService::RemoveObserver(
								MBrowserTelServiceObserver* aObserver )
	{
    TELSERVICE_ENTERFN( "RemoveObserver()" )

    TInt count = iObserverList.Count();
    if( count )
	    {
        TInt index = iObserverList.Find( aObserver );
        if( index != KErrNotFound )
		    {
            iObserverList.Remove( index );
            iObserverList.GranularCompress();
		    }
		}
    TELSERVICE_LEAVEFN( "AddObserver()" )
	}
//---------------------------------------------------------------------------------------
// CBrowserTelService::SendEmailMessageL()
//---------------------------------------------------------------------------------------
//
EXPORT_C TInt CBrowserTelService::SendEmailMessageL(
												const TDesC& aToRecipients,
												const TDesC& aCcRecipients, 
                                                const TDesC& aParams,
												const TDesC& aBody )
	{ 	
	return this->SendEmailMessageL( aToRecipients, aCcRecipients,
									aParams, aBody, 0 );
	}
//---------------------------------------------------------------------------------------
// CBrowserTelService::SendEmailMessageL()
//---------------------------------------------------------------------------------------
//
EXPORT_C TInt CBrowserTelService::SendEmailMessageL( 
												const TDesC& aToRecipients,
												const TDesC& aCcRecipients, 
                                                const TDesC& aParams,
												const TDesC& aBody,
												TBool aLaunchEmbedded )
	{ 
    TELSERVICE_ENTERFN( "SendEmailMessageL()" )
	
    ///////////////////////////////////////////////////////////////
    // Determine if mailto handled via MMS from variation mechanism
    TBool sendMms( EFalse );
    TInt keyVal( 0 );
    CRepository* repository = CRepository::NewL( KCRUidBrowserUiLV );
    CleanupStack::PushL( repository );
    if(repository->Get( KBrowserMailtoSendingApp, keyVal ) == KErrNone )
        {
        sendMms = ( keyVal == 1 );
        }
    CleanupStack::PopAndDestroy( repository );    

    //Search for @ and .(dot) characters
    TInt at  = aToRecipients.Locate('@');
    TInt dot = aToRecipients.Locate('.');
    TInt len = aToRecipients.Length();
    if(len > 0 && KErrNotFound == at && KErrNotFound == dot)
    {
        TELSERVICE_WRITE("CBrowserTelService::SendEmailMessageL chars @ and .(dot) are not present");
        sendMms = ETrue;
        //Make sure all chars are numbers only except first char '+'
        TInt loopCounter = 0;
        const TUint16 *ptr = aToRecipients.Ptr();
        
        if(*ptr == '+')
        {
           ptr = ptr + 1;
           loopCounter = 1;
        }
                   
        for(loopCounter; loopCounter<len; loopCounter++)
        {
            if( (*ptr >= '0' && *ptr <= '9') || *ptr == ',')
            {
                TELSERVICE_WRITE_FORMAT("CBrowserTelService::SendEmailMessageL character: %c", *ptr );
                ptr = ptr + 1;
                continue;
            }
            else
            {
               TELSERVICE_WRITE_FORMAT("CBrowserTelService::SendEmailMessageL found invalid character: %c", *ptr );
               TELSERVICE_WRITE("CBrowserTelService::SendEmailMessageL chars otherthan numbers are present");
               sendMms = EFalse;
               break;
            }
        }
    }
    
    if ( sendMms )
        {
        TELSERVICE_WRITE("CBrowserTelService::SendEmailMessageL Sending MMS message");
        return SendMmsMessageL( aToRecipients, aCcRecipients, aParams, aBody, aLaunchEmbedded );
        }

    /////////////////////////////////
    // Email editor is launched.
    TInt cleanUpCount = 0;
    
    CParaFormatLayer* paraFormat = CParaFormatLayer::NewL();
    CleanupStack::PushL( paraFormat );
    ++cleanUpCount;

    CCharFormatLayer* charFormat = CCharFormatLayer::NewL(); 
    CleanupStack::PushL( charFormat );
    ++cleanUpCount;

    CRichText* body = CRichText::NewL( paraFormat, charFormat );
    CleanupStack::PushL( body );
    ++cleanUpCount;

    if( aBody.Length() )
		{
        body->InsertL( 0, aBody );
		}
    else
		{
        body->InsertL( 0, KNullDesC );
		}
		
    CPtrC16Array* recipients = ParseRecipientsLC( aToRecipients );
    ++cleanUpCount;
    CPtrC16Array* ccrecipients = ParseRecipientsLC( aCcRecipients );
    ++cleanUpCount;		
    
    CSendUi* sendUi = CSendUi::NewL();
    CleanupStack::PushL( sendUi );
    ++cleanUpCount;
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    ++cleanUpCount;
    messageData->SetSubjectL( &aParams );
    messageData->SetBodyTextL( body );
    for( TInt i = 0; i < recipients->Count(); i++ )
    	{
    	messageData->AppendToAddressL( (*recipients)[i] );
    	}
    for( TInt j = 0; j < ccrecipients->Count(); j++ )
    	{
    	messageData->AppendCcAddressL( (*ccrecipients)[j] );
    	}
    	    // check first that the selected mtm supports bodytext
/*
    TSendingCapabilities caps( aBody.Size(), 
                               aBody.Size(), 
                               TSendingCapabilities::ESupportsBodyText );    	    
    if( sendUi->ValidateServiceL( KUidMsgTypeSMTP, caps  ) )
		{
    	sendUi->CreateAndSendMessageL( KUidMsgTypeSMTP, messageData );
		} */
	sendUi->CreateAndSendMessageL( KUidMsgTypeSMTP, messageData );	  
    
    CleanupStack::PopAndDestroy( cleanUpCount );

    TELSERVICE_LEAVEFN( "SendEmailMessageL()" )
    return KErrNone;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::SendMmsMessageL()
//---------------------------------------------------------------------------------------
//
TInt CBrowserTelService::SendMmsMessageL( 
										const TDesC& aToRecipients,
										const TDesC& aCcRecipients, 
                                        const TDesC& aParams,
										const TDesC& aBody,
										TBool aLaunchEmbedded )
    {
    TELSERVICE_ENTERFN( "SendMmsMessageL()" )
    
    TInt cleanUpCount = 0;
    // We give NULL body for the SendUi.
    CRichText* body = NULL;
    
    // We need to write body to file in order to give as a attachment
    // for the SendUi.
    CDesCArrayFlat* attachments = NULL;
    TFileName fileName;
    TFileName tempPath;
    CCoeEnv* coeEnv = CCoeEnv::Static();
	RFs& fs = coeEnv->FsSession();

    attachments = new (ELeave) CDesCArrayFlat(1);
    CleanupStack::PushL(attachments);
    ++cleanUpCount;
    if ( aBody.Length() > 0 )
        {
        // Create plaintext object from body
        CPlainText* text = CPlainText::NewL( CEditableText::EFlatStorage );
        CleanupStack::PushL( text );
        text->InsertL( 0, aBody );

        TInt err = KErrNone;
        RFileWriteStream writer;
        tempPath.Copy(KTempDrive);  
  		tempPath.Append(KTempPath);  
  		err = fs.MkDirAll( tempPath );  
		if ( err != KErrNone && err != KErrAlreadyExists )
            {
            User::Leave( err );
            }
        writer.PushL();
        User::LeaveIfError( writer.Temp( fs, tempPath, fileName, EFileWrite ) );
        writer.WriteUint16L(CEditableText::EByteOrderMark);
        text->ExportTextL( 0, writer, 
            CPlainText::EOrganiseByParagraph );
        CleanupStack::PopAndDestroy( 2 ); // text, writer
        attachments->AppendL( fileName );
        }

    // Create address arrays
    CPtrC16Array* recipients = ParseRecipientsLC( aToRecipients );
    ++cleanUpCount;
    CPtrC16Array* ccrecipients = ParseRecipientsLC( aCcRecipients );
    ++cleanUpCount;
    TInt count = ccrecipients->Count();
    if ( count > 0 )
        {
        for (TInt i = 0; i < count; i++)
            {
            recipients->AppendL( ccrecipients->At(i) );
            }
        }
    CleanupStack::PopAndDestroy(ccrecipients);
    --cleanUpCount;
    ccrecipients = NULL;

    CSendUi* sendUi = CSendUi::NewL();
    CleanupStack::PushL( sendUi );
    ++cleanUpCount;
    CMessageData* messageData = CMessageData::NewL();
    CleanupStack::PushL( messageData );
    ++cleanUpCount;    
    messageData->SetSubjectL( &aParams );
    messageData->SetBodyTextL( body );
    for( TInt i = 0; i < recipients->Count(); i++ )
    	{
    	messageData->AppendToAddressL( (*recipients)[i] );
    	}
    for( TInt j = 0; j < attachments->Count(); j++ )
    	{
    	messageData->AppendAttachmentL( (*attachments)[j] );
    	}    	
    sendUi->CreateAndSendMessageL( KSenduiMtmMmsUid, messageData, 
                                   KNullUid, aLaunchEmbedded );

    fs.Delete( fileName );
    CleanupStack::PopAndDestroy( cleanUpCount );	

    TELSERVICE_LEAVEFN( "SendMmsMessageL()" )
    return KErrNone;
    }
//---------------------------------------------------------------------------------------
// CBrowserTelService::DisconnectActiveCSDConnectionL()
//---------------------------------------------------------------------------------------
//
EXPORT_C void CBrowserTelService::DisconnectActiveCSDConnectionL()
	{
    TELSERVICE_ENTERFN( "DisconnectActiveCSDConnectionL()" )

    RSocketServ server;
    RConnection connection;
    TInt err( KErrNone );

    User::LeaveIfError( server.Connect() );
    CleanupClosePushL<RSocketServ>( server );

    User::LeaveIfError( connection.Open( server, KAfInet ) );
    CleanupClosePushL<RConnection>( connection );

    TUint conns( 0 );
    connection.EnumerateConnections( conns );
    if( conns )
        // we need to check if there is any (HS)CSD conneciton
        {
        TPckgBuf<TConnectionInfo> connInfo;
        TBool found = EFalse;
        TUint index = 1;
        for( index = 1; index <= conns; ++index )
            {
            err = connection.GetConnectionInfo( index, connInfo );
            if( err == KErrNone )
                {						
                TApBearerType bearerType = EApBearerTypeGPRS;

                // Fixing SLAA-6k52P which only happens in MAGNUM. Since
                // we only need to find if there is CSD or HSCSD, we should
                // not leave here 
                TRAP_IGNORE( bearerType = ConnectionTypeL( connInfo().iIapId ) );
		        
		        if( bearerType == EApBearerTypeCSD || 
					bearerType == EApBearerTypeHSCSD )
			        {
                    found = ETrue;
                    }
                }
            }

        if( found )
            // there is active (HS)CSD. We need to close it.
            {
            if( ( err = connection.Attach( connInfo, 
				RConnection::EAttachTypeNormal ) ) == KErrNone )
                {
                // disconnect
                connection.Stop();
                }
            TELSERVICE_WRITE( "Found active (HS)CSD connection" )
            }
        }

    CleanupStack::PopAndDestroy( 2 ); // connection, server

    TELSERVICE_LEAVEFN( "DisconnectActiveCSDConnectionL()" )

    User::LeaveIfError( err );
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::ShowDialogL()
//---------------------------------------------------------------------------------------
//
EXPORT_C TInt CBrowserTelService::ShowDialogL(
								const TDesC& aDynamicText, 
								const TBrowserTelServiceDlgType& aDlgType)
	{
    TELSERVICE_ENTERFN( "ShowDialogL()" )

	TInt retVal = 0;

    switch( aDlgType )
        {
        case EConfirmMakeCall:
		    {
		    HBufC* number = NULL;
            HBufC* valNumber = NULL;
		    HBufC* text = NULL;
            number = aDynamicText.AllocLC();            
            
            valNumber = ValidateNumberL( *number );
            CleanupStack::PopAndDestroy( number );  // number;

            CleanupStack::PushL( valNumber );
		    
            HBufC* name = SearchPbForMatchL( *valNumber );
			if( name  )
				{
				CleanupStack::PushL( name );
				text = StringLoader::LoadL(
					R_QTN_WML_WTAI_CALL_NO, *name );
				CleanupStack::PopAndDestroy();	// name
				CleanupStack::PushL( text );
				}
			else
				{
				text = StringLoader::LoadLC(
    				R_QTN_WML_WTAI_CALL_NO, *valNumber );
                }
			
			retVal = TDialerDialogs::ConfirmationDlgL(
				*text, R_CONFIRMATION_DIALOG );

			CleanupStack::PopAndDestroy( 2 );	// valNumber, text
		    }
            break;

        case EConfirmSendDTMF:
		    {
		    HBufC* text = StringLoader::LoadLC(
				R_QTN_WML_WTAI_SEND_DTMF, aDynamicText, CEikonEnv::Static() );
		    retVal = TDialerDialogs::ConfirmationDlgL(
				*text, R_CONFIRMATION_DIALOG );
		    CleanupStack::PopAndDestroy();	// text
		    }
            break;

        case EConfirmAddToPb:
		    {
		    TInt index = 0;
			CAknListQueryDialog* dlg = 
				new( ELeave ) CAknListQueryDialog( &index );
			if ( dlg->ExecuteLD( R_ADD_TO_CONTACTS_DIALOG ) )
				{
				return index;
				}
			else
				{
				User::Leave( KErrCancel );
				}
		    }
            break;

        case ESendingDTMF:
            {
			HBufC* text = StringLoader::LoadLC(
			R_QTN_DTMF_SENDING_WAIT_NOTE, 
			aDynamicText, CEikonEnv::Static() );
			if( !iWaitDlg )
				{
				TELSERVICE_WRITE( "Create Sending DTMF wait dialog" )
				iWaitDlg = new ( ELeave ) CAknWaitDialog(
				reinterpret_cast<CEikDialog**>( &iWaitDlg ), ETrue );
				iWaitDlg->SetCallback( this );
				iWaitDlg->PrepareLC( R_SENDDTMF_DIALOG );
				iWaitDlg->SetTextL( *text );
				iWaitDlg->RunLD();
				}
			else
				{
				TELSERVICE_WRITE( "Update Sending DTMF wait dialog" )
					iWaitDlg->SetTextL( *text ); // Just update the text
				}
            CleanupStack::PopAndDestroy();  // text
            }
            break;
        default:
            break;
        }
    TELSERVICE_LEAVEFN( "ShowDialogL()" )
	return retVal;
	}



//---------------------------------------------------------------------------------------
// CBrowserTelService::MakeVOIPCall()
//---------------------------------------------------------------------------------------
//
EXPORT_C TInt CBrowserTelService::MakeVOIPCall( TDesC& aNumber, 
										    TBool aSendDTMFAlwaysConfirm )
	{
    TELSERVICE_ENTERFN( "MakeVOIPCall() synchronous" )
       
    TInt retVal( KErrNone );

    iRetVal = KErrNone;

    // Flag to indicate that synchronous method was called
    iSynch = ETrue;
    TBool voipCall = ETrue;
    TRAP( retVal, MakeCallL( aNumber, aSendDTMFAlwaysConfirm, voipCall ) );

    if( retVal != KErrNone )
		{
        ErrorHandler( retVal );

        retVal = KErrCancel;
        }
    iSynch = EFalse;

    TELSERVICE_LEAVEFN( "MakeVOIPCall() synchronous" )
    return retVal;
	}
	
	
//---------------------------------------------------------------------------------------
// CBrowserTelService::DoCancel()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::DoCancel()
	{
    TELSERVICE_ENTERFN( "DoCancel()" )

    switch( State() )
	    {
        case EDialing:
			{
            iRCall.DialCancel();
			}
            break;

        case EDialingDTMF:
			{			
            iRAdvGsmPhone.StopDTMFTone();
            if( iInternalDTMFWaitStarted )
				{
				// We have to hold on a bit so that the CSendAppUi is 
				// properly initialized
				iIdle->Start( TCallBack( IdleCallback,this ) );
				}
			}
            break;
			
        case EConnected:
			{
			}
            break;

		case EIdle:
        default:
            break;
		}

    TELSERVICE_LEAVEFN( "DoCancel()" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::RunL()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::RunL()
	{
    TELSERVICE_ENTERFN( "CBrowserTelService::RunL()" )

    RCall::TStatus callStatus;
    callStatus = RCall::EStatusUnknown;
        
    iRCall.GetStatus( callStatus );

    TInt status = iStatus.Int();
    TELSERVICE_WRITE_FORMAT( "status: %d", status );
    TELSERVICE_WRITE_FORMAT( "callStatus: %d", callStatus );

    if ( status != KErrNone )
		{
        TELSERVICE_WRITE_FORMAT( "RunL() - iStatus error: %d", status )
        HandleError( status );
        return;
		}
    
    if( status == KErrNone )
		{
        switch( State() )
			{
            // the call is now active
            case EDialing:
                {
                TELSERVICE_WRITE( "RunL - KDialing" )

                BringBrowserToForeground();

				if( callStatus == RCall::EStatusConnected )
					{
					SetState( EConnected );
					// Notify the observer
					NotifyObservers();

					if( iSynch )
						{
						iWait.AsyncStop();  // stop the wait loop. 
											// Now DoMakeCallL will return
						}
					else    
						{   // async variant was called
                        if( HandleInternalSendDTMFL() == EFalse )
                            { // nothing to handle anymore

						    // Complete the request
                            TELSERVICE_WRITE_FORMAT(
								"Complete the request: %d",KErrNone )

						    User::RequestComplete( 
							iUserRequestStatus, KErrNone );
                            }
						}
					}
                }
                break;

            // the DTMF tones have been sent
            case EDialingDTMF:
                {
                TELSERVICE_WRITE( "RunL - KDialingDTMF" )

				iRAdvGsmPhone.Close();

                if( HandleInternalSendDTMFL() == EFalse )
                    { // nothing to handle anymore
				    SetState( EConnected );
				    // Notify the observer
				    NotifyObservers();

				    if( iSynch || iInternalDTMFWaitStarted )
						{
					    iWait.AsyncStop();  // stop the wait loop. 
											//Now SendDTMFL() will return
						}
				    else    
					    {
					    // Asynchronous method was called.
                        if( iWaitDlg )
							{
                            iWaitDlg->ProcessFinishedL();
							}
					    // Complete the request
                        TELSERVICE_WRITE_FORMAT(
							"Complete the request: %d",KErrNone )

					    User::RequestComplete( iUserRequestStatus, KErrNone );
					    }
                    }
                }
                break;

            // the call has been active, but 
            // the caller has hanged up.
            case EConnected:
                {
                TELSERVICE_WRITE( "RunL - EPhoneAnswered,EConnected" )

				if( iCallStatus == RCall::EStatusIdle || 
					iCallStatus == RCall::EStatusHangingUp )
					{
					SetStateIdleAndNotifyObservers();                       
					}
                }
                break;

			case EIdle:
            default:
                {
                TELSERVICE_WRITE( "RunL - Default" )
                break;
				}
			}
		}
	else
		{
		if ( State() == EDialing )
			{
			BringBrowserToForeground();
			// If there is not activate voice call on
			// changes state to EIdle.
			CheckIsThereActivateVoiceCallOnL();
			// MakeCall must return error code Canceled in this case.
			iRetVal = KErrCancel;

			if( iSynch )
			    {
				iWait.AsyncStop();  // stop the wait loop. 
				// Now DoMakeCallL will return
			    }
			else    
			    {   // async variant was called
				if( HandleInternalSendDTMFL() == EFalse )
				    { // nothing to handle anymore
					
					// Complete the request
					TELSERVICE_WRITE_FORMAT(
						"Complete the request: %d",KErrNone )
						
						User::RequestComplete( 
						iUserRequestStatus, KErrNone );
				    }
			    }
			}
		}
    TELSERVICE_LEAVEFN("CBrowserTelService::RunL()")
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::CloseAllSessions()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::CloseAllSessions()
	{
    TELSERVICE_ENTERFN( "CloseAllSessions()" )

    iRCall.Close();
    iRLine.Close();
    iRPhone.Close();
    iRTelServer.Close();
    TELSERVICE_LEAVEFN( "CloseAllSessions()" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::SetStateIdleAndNotifyObserver()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::SetStateIdleAndNotifyObservers()
	{
    TELSERVICE_ENTERFN("SetStateIdleAndNotifyObservers()")

    SetState( EIdle );
    CloseAllSessions();
    // Notify the observers
    NotifyObservers();
    TELSERVICE_LEAVEFN( "SetStateIdleAndNotifyObservers()" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::NotifyObservers()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::NotifyObservers()
	{
    TELSERVICE_ENTERFN( "NotifyObservers()" )

    TInt count = iObserverList.Count();

    for( TInt i = 0; i < count ;i++ )
		{
        iObserverList[i]->BrowserTelServiceEvent( State() );
		}
    TELSERVICE_LEAVEFN( "NotifyObservers()" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::NotifyObservers()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::NotifyObservers( TBrowserTelServiceError aError )
	{
    TELSERVICE_ENTERFN( "NotifyObservers( TBrowserTelServiceError )" )

    TInt count = iObserverList.Count();
	
    for( TInt i = 0; i < count; i++)
		{
        iObserverList[i]->BrowserTelServiceError( aError );
		}
    TELSERVICE_LEAVEFN( "NotifyObservers(TBrowserTelServiceError)" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::HandleError()
//
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::HandleError( TInt aError )
	{
    TELSERVICE_ENTERFN( "HandleError()" )

    switch( State() )
		{
        case EDialingDTMF:
			{
            iRAdvGsmPhone.Close();  // close RAdvGsmPhone session.
            NotifyObservers( ESendDTMFFailed );
            SetState( EConnected );
            NotifyObservers();
			}
            break;
		
		case EDialing:
        case EConnected:
			{
            Cancel();
			}
            break;

		case EIdle:
        default:
            break;
		}

    iRetVal = aError;

    if( iSynch || iInternalDTMFWaitStarted )
		{
        iWait.AsyncStop();  // stop the wait loop. 
							// Now synchronous method will return
		}
    else    
		{
        // Asynchronous method was called.
        // Complete the request
        TELSERVICE_WRITE_FORMAT( "Complete the request: %d", aError )
        User::RequestComplete( iUserRequestStatus, aError );
		}
    TELSERVICE_LEAVEFN( "HandleError()" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::EtelWatcherEvent()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::BrowserTelServiceEtelWatcherEvent(
								  RCall::TStatus& aStatus )
	{
    TELSERVICE_ENTERFN( "EtelWatcherEvent()" )

    if( aStatus == RCall::EStatusIdle )
		{   
		// the call recepient has hung up without answering the call,
        // so lets reset our state and notify observers
        
        Cancel();
        SetStateIdleAndNotifyObservers();       

        iRetVal = KErrUserBusy;

        if( iSynch )
			{
            iWait.AsyncStop();  // stop the wait loop. 
								// Now DoMakeCallL will return
			}
        else    
			{
            // Asynchronous method was called.
            // Complete the request
            TELSERVICE_WRITE_FORMAT( "Complete the request: %d", KErrUserBusy )
            User::RequestComplete( iUserRequestStatus, KErrUserBusy );
			}
		}
    TELSERVICE_LEAVEFN( "EtelWatcherEvent()" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::ConnectionStageAchievedL()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::ConnectionStageAchievedL()
	{
    TELSERVICE_ENTERFN( "ConnectionStageAchievedL()" )
		iAgentDisconnected = ETrue;
		if( iWait.IsStarted() )
			{
			iWait.AsyncStop();
			}
    TELSERVICE_LEAVEFN( "ConnectionStageAchievedL()" )
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::SearchPbForMatchL()
//---------------------------------------------------------------------------------------
//
HBufC* CBrowserTelService::SearchPbForMatchL( 
									const TDesC& aPhoneNumber )
	{
    TELSERVICE_ENTERFN( "SearchPbForMatchL()" )

    HBufC* matchingName = NULL;
	
    // Create phonebook engine
/*    CPbkContactEngine* pbkEngine = CPbkContactEngine::NewL();
    CleanupStack::PushL( pbkEngine );

    // Make sure a resource file is available
    RPbkViewResourceFile pbkResourceFile( *CCoeEnv::Static() );
    pbkResourceFile.OpenL();
    CleanupClosePushL<RPbkViewResourceFile> ( pbkResourceFile );
	
	CContactIdArray* idArray = SearchPbForMatchLC( *pbkEngine
                                                   ,aPhoneNumber
                                                   ,EPhoneNumber 
                                                  );

    if( idArray->Count() )
        {
        // retrive contact's full matching name
        CPbkContactItem* item = pbkEngine->ReadMinimalContactLC( (*idArray)[0] );

        matchingName = item->GetContactTitleOrNullL();

        CleanupStack::PopAndDestroy( item );
        }

    CleanupStack::PopAndDestroy( 3 );   // idArray, pbkResourceFile, pbkEngine
*/
    TELSERVICE_LEAVEFN("SearchPbForMatchL()")

    return matchingName;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::SearchPbForMatchLC()
//---------------------------------------------------------------------------------------
//
CContactIdArray* CBrowserTelService::SearchPbForMatchLC( 
                                    CPbkContactEngine& aPbkEngine,
									const TDesC& aToMatch,
									const TMatchType& aMatchType )
	{
    TELSERVICE_ENTERFN( "SearchPbForMatchLC()" )

    CContactIdArray* idArray = NULL;

/*	if( aMatchType == EPhoneNumber )
		{
		idArray = aPbkEngine.MatchPhoneNumberL( aToMatch, aToMatch.Length() );
		}
	else if( aMatchType == EEmail )
		{
        CPbkFieldIdArray* findFrom = new( ELeave ) CPbkFieldIdArray;
        CleanupStack::PushL( findFrom );

		findFrom->AppendL( EPbkFieldIdEmailAddress );
		idArray = aPbkEngine.FindLC( aToMatch, findFrom );

        CleanupStack::Pop( idArray );
        CleanupStack::PopAndDestroy( findFrom );
		}

	CleanupStack::PushL( idArray ); */
    TELSERVICE_LEAVEFN( "SearchPbForMatchLC()" )
	return idArray;
	}

//---------------------------------------------------------------------------------------
// CBrowserTelService::IdleCallback()
//---------------------------------------------------------------------------------------
//
TInt CBrowserTelService::IdleCallback( TAny* aTelService )
    {
    TELSERVICE_ENTERFN( "IdleCallback()" )
	( ( CBrowserTelService* ) aTelService )->DoIdleCallback();
    TELSERVICE_LEAVEFN( "IdleCallback()" )
	return EFalse;	// return false since we don't want to run idle again
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::IdleCallback()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::DoIdleCallback()
    {
    TELSERVICE_WRITE( "DoIdleCallback()" )
	iWait.AsyncStop();
    }

// ---------------------------------------------------------
// CBrowserTelService::DialogDismissedL
// ---------------------------------------------------------
//
void CBrowserTelService::DialogDismissedL( TInt aButtonId )
    {
    TELSERVICE_ENTERFN( "DialogDismissedL()" )
    
    if ( aButtonId == EAknSoftkeyCancel )
        {
        Cancel();
        }
	iWaitDlg = NULL;   // Dialog will delete itself; prevent DoCancel
	// deleting it beforehand.
    TELSERVICE_LEAVEFN( "DialogDismissedL()" )
    }

// ---------------------------------------------------------
// CBrowserTelService::HandleInternalSendDTMFL
// ---------------------------------------------------------
//
TBool CBrowserTelService::HandleInternalSendDTMFL() 
    {
    TELSERVICE_ENTERFN( "HandleInternalSendDTMFL()" )
    
    iSynch = EFalse;    // Reset to EFalse since the HandleInternalSendDTMFL()
                        // is asynchronous!

    if( !(iSendableDTMFNumbers && iDlgDTMFNumbers) )
        {
        return EFalse;
        }

	TBool retVal = EFalse;

    CRepository* repository = CRepository::NewL( KCRUidBrowser );
    CleanupStack::PushL (repository );
    repository->Get( KBrowserConfirmedDTMFOnce, iConfirmedOnce );  

    if( iSendableDTMFNumbers->Count() > 0 && iDlgDTMFNumbers->Count() > 0 )
        {
        TELSERVICE_WRITE( "we have dtmf numbers to send" )
        TInt sendDTMF = 1;
        TPtr sendableNumberPtr = iSendableDTMFNumbers->At(0)->Des();
        TPtr dlgNumberPtr = iDlgDTMFNumbers->At(0)->Des();

        if( DisplaySendDTMFDialog( &dlgNumberPtr ) )
            {
            if( dlgNumberPtr.Length() > 0 )
                {
                TELSERVICE_WRITE( "display send dtmf dialog" )
                if( iDTMFAlwaysConfirm )
                    {
                    sendDTMF = ShowDialogL( dlgNumberPtr,EConfirmSendDTMF );
                    }
                else if( iDTMFAlwaysConfirm == EFalse && 
                         iConfirmedOnce == EFalse )
                    {
                    sendDTMF = ShowDialogL( dlgNumberPtr, 
											EConfirmSendDTMF );
                    iConfirmedOnce = ETrue;
                    repository->Set( KBrowserConfirmedDTMFOnce, iConfirmedOnce );
                    }
                else
                    {
                    sendDTMF = 1;
                    }
                }
            else
                {
                TELSERVICE_WRITE("don't display send dtmf dialog dlgNumberPtr.Length() < 0")
                sendDTMF = 0;
				}
            }
        else
            {
            TELSERVICE_WRITE( "don't display send dtmf dialog" )

            if( dlgNumberPtr.Length() > 0 )
                {
                TELSERVICE_WRITE( "start dtmf wait timer" )

                RTimer timer; 
                TRequestStatus timerStatus; // request status 
											// associated with timer
                timer.CreateLocal(); // create for this thread
                timer.After( timerStatus, KTimerDelay ); // pause for 2,5 
														 // seconds
                User::WaitForRequest( timerStatus );
                timer.Close();
                }
            else
				{
                sendDTMF = 0;
				}
            }

        if( sendDTMF )
            {
            TELSERVICE_WRITE( "Send the DTMF" )

            RipWaitChar( &sendableNumberPtr );
            ValidateDTMFNumberL( &sendableNumberPtr );
            // Send DTMF tones
            DoSendDTMFL( sendableNumberPtr );

            TELSERVICE_WRITE( "DoSendDTMFL() ok!" )

            ShowDialogL( dlgNumberPtr, ESendingDTMF );
            retVal = ETrue;
            }

        iSendableDTMFNumbers->Delete(0);
        iSendableDTMFNumbers->Compress();
        iDlgDTMFNumbers->Delete(0);
        iDlgDTMFNumbers->Compress();
        }

    CleanupStack::PopAndDestroy( repository );

    TELSERVICE_LEAVEFN( "HandleInternalSendDTMFL()" )
    return retVal;
    }

// ---------------------------------------------------------------------------
// CBrowserTelService::BringBrowserToForeground
// ---------------------------------------------------------------------------
//
void CBrowserTelService::BringBrowserToForeground()
    {
    TELSERVICE_ENTERFN( "BringBrowserToForeground()" )
    TApaTaskList taskList( CEikonEnv::Static()->WsSession() );
    TUid wapUid = KUidBrowserApplication;
    TApaTask task = taskList.FindApp( wapUid );
    task.BringToForeground();
    TELSERVICE_LEAVEFN( "BringBrowserToForeground()" )
    }

// ---------------------------------------------------------------------------
// CBrowserTelService::CheckIsThereActivateVoiceCallOnL
// ---------------------------------------------------------------------------
//
void CBrowserTelService::CheckIsThereActivateVoiceCallOnL()
	{
	RTelServer::TPhoneInfo info;
    RPhone::TLineInfo lineInfo;
	RLine::TLineInfo callInfo;
	
    // Connect to the telephony server and load the TSY.
    User::LeaveIfError( iRTelServer.Connect() );
    User::LeaveIfError( iRTelServer.LoadPhoneModule( KMmTsyModuleName ) );
    // Get the details for the first (and only) phone.
    User::LeaveIfError( iRTelServer.GetPhoneInfo( 0, info ) );
    // Open the phone.
	iRPhone.Close();
    User::LeaveIfError( iRPhone.Open( iRTelServer, info.iName ) );
    // Get the information for the voice line, line 0.
    User::LeaveIfError( iRPhone.GetLineInfo( 0, lineInfo ) );
 
    if( lineInfo.iStatus > RCall::EStatusIdle )
        {
        TELSERVICE_WRITE( "There's an active call" );
        // Open the line. iName will now be "VoiceLine1".
	    iRLine.Close();
        User::LeaveIfError( iRLine.Open( iRPhone, KMmTsyVoice1LineName ) );     
	    TInt count;
	    // Gets the number of calls opened from a line.	
	    User::LeaveIfError( iRLine.EnumerateCall( count ) );
	    if ( count >= 1 )
		    {
		    SetState( EConnected );
		    User::LeaveIfError( iRLine.GetInfo( callInfo ) );
		    iRCall.Close();

            User::LeaveIfError( iRCall.OpenExistingCall( iRLine, 
			    callInfo.iNameOfLastCallAdded ) );
		    }
	    else
		    {
		    SetState( EIdle );
		    }
        }
	else
		{
        TELSERVICE_WRITE( "No active call" );
		SetState( EIdle );
		}
	}

// ---------------------------------------------------------------------------
// CBrowserTelService::CheckDTMFNumber
// ---------------------------------------------------------------------------
//
void CBrowserTelService::CheckDTMFNumberL( TDesC& aNumber )
	{
	TBool multiw = EFalse;
	TBool multikeno = EFalse;

	for ( TInt i = 0; i < aNumber.Length(); i++ )
		{
        if( (KValidDTMFChars().Locate( aNumber[i] ) ) == KErrNotFound )
            {
            User::Leave( KErrArgument );
            }
        else
            {
            if( aNumber[i] == 'w' )
                {
				if( multiw )
					{
					User::Leave( KErrArgument );
					}
				multiw = ETrue;
                }
            else if( aNumber[i] == '/' )
                {
				if( multikeno )
					{
                    User::Leave( KErrArgument );
					}
				multikeno = ETrue;
                }
            }
        }
	}

// ---------------------------------------------------------------------------
// CBrowserTelService::CheckDTMFNumber
// ---------------------------------------------------------------------------
//
CPbkContactItem* CBrowserTelService::SearchPBItemLC( CPbkContactEngine& aPbkEngine,
                                                     TDesC& aNumber, 
                                                     TDesC& aEmail,
                                                     TBool& aNewContact )
    {
	CPbkContactItem* contactItem = NULL;
	CContactIdArray* idArrayPhoneNr = SearchPbForMatchLC( aPbkEngine, aNumber, EPhoneNumber );
	CContactIdArray* idArrayEmail = SearchPbForMatchLC( aPbkEngine, aEmail, EEmail );

    aNewContact = ETrue;

/*	if( idArrayPhoneNr->Count() && aNumber.Length() )
		{
		// Open existing
		if ( ShowDialogL( aNumber, EConfirmAddToPb ) )
			{
			contactItem = aPbkEngine.OpenContactL( ( *idArrayPhoneNr )[0] );
			aNewContact = EFalse;
			}
		else
			{
			// Create a new contact
			contactItem = aPbkEngine.CreateEmptyContactL();
			}
		
		}
	else if( idArrayEmail->Count() && aEmail.Length() )
		{
		if ( ShowDialogL( aEmail, EConfirmAddToPb ) )
			{
			// Open existing
			contactItem = aPbkEngine.OpenContactL( ( *idArrayEmail )[0] );
			aNewContact = EFalse;
			}
		else
			{
			// Create a new contact
			contactItem = aPbkEngine.CreateEmptyContactL();
			}		
		}
	else
		{
		// Create a new contact
		contactItem = aPbkEngine.CreateEmptyContactL();
		}

	CleanupStack::PopAndDestroy( 2 );		// idArrayEmail,idArrayPhoneNr

	CleanupStack::PushL( contactItem );
*/
    return contactItem;
    }

// ---------------------------------------------------------------------------
// CBrowserTelService::SetPBEntryField()
// ---------------------------------------------------------------------------
//
void CBrowserTelService::SetPBEntryFieldL( TInt aField,
                                           CPbkContactEngine* aPbkEngine,
                                           CPbkContactItem* aContactItem,
                                           const TPtrC& aFieldValue,
                                           TInt& aFormIndex )
    {
    if( aFieldValue.Length() )
		{
  /*      TPbkContactItemField *dataField = aContactItem->FindField( aField );

        if( !dataField )
			{
            CPbkFieldInfo* newFieldType = NULL;

            for ( TInt i = 0; !newFieldType &&
			          i < aPbkEngine->FieldsInfo().Count(); ++i ) 
		        {
                CPbkFieldInfo* fieldInfo = aPbkEngine->FieldsInfo()[i];
                if( fieldInfo->FieldId() == aField )
			        {
                    // we found the proper field. Yeah!
                    newFieldType = fieldInfo;
			        }
		        }
            // add new field
            dataField = aContactItem->AddOrReturnUnusedFieldL( *newFieldType );
            }

        if( dataField )
            {
            // Put the data to the field
            dataField->TextStorage()->SetTextL( aFieldValue );
            if( aFormIndex == -1 )
			    {
                aFormIndex = aContactItem->FindFieldIndex( *dataField );
			    }
            }*/
		}
    }

// ---------------------------------------------------------------------------
// CBrowserTelService::ParseRecipientsLC
// ---------------------------------------------------------------------------
//
CPtrC16Array* CBrowserTelService::ParseRecipientsLC( const TDesC& aRecipients )
    {
    CPtrC16Array* recips = new( ELeave ) CPtrC16Array( 2 );

    CleanupStack::PushL( recips );

    if( aRecipients.Length() )
        {
        TPtrC tempRec( aRecipients );

        do
            {
            int posComma = tempRec.Find( _L(",") );


            if( posComma != KErrNotFound )
                {
                recips->AppendL( tempRec.Left( posComma ) );
                tempRec.Set( tempRec.Right( tempRec.Length() - posComma - 1 ) );
                }
            else
                {
                recips->AppendL( tempRec );
                tempRec.Set( KNullDesC );
                }

            }while( tempRec.Length() );
        }

    return recips;
    }
   
// ---------------------------------------------------------------------------
// CBrowserTelService::ConnectionTypeL
// ---------------------------------------------------------------------------
//
TApBearerType CBrowserTelService::ConnectionTypeL( TUint aApId )
    {
	CCommsDatabase* commsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
    CleanupStack::PushL( commsDb );

	CApDataHandler* apDataHandler = CApDataHandler::NewLC( *commsDb );
	TUint32 apId = 0;
	CApUtils* apUtils = CApUtils::NewLC( *commsDb );

	apId = apUtils->WapIdFromIapIdL( aApId );

	CleanupStack::PopAndDestroy(); //apUtils
    apUtils = NULL;

	CApAccessPointItem* apItem = CApAccessPointItem::NewLC();
	
	apDataHandler->AccessPointDataL( apId, *apItem );

    TApBearerType bearerType = apItem->BearerTypeL();

    CleanupStack::PopAndDestroy( 3 );    // apItem, apDataHandler, commsDb

    return bearerType;
    }

//---------------------------------------------------------------------------------------
// CBrowserTelService::ErrorHandler()
//---------------------------------------------------------------------------------------
//
void CBrowserTelService::ErrorHandler( TInt aErrorId )
    {
    TELSERVICE_WRITE_FORMAT( "Error handler: %d", aErrorId );
    
    if( aErrorId == KErrCancel )
        {
        return;
        }

    TInt i;
    for( i = 0; KErrorConvTable[i][0] && KErrorConvTable[i][0] != aErrorId; ++i ){};

    if( KErrorConvTable[i][0] )
        {
        // If KErrorConvTable[i][1] is 0 it's already handled by the system
        if( KErrorConvTable[i][1] )
            {
            TRAP_IGNORE( iErrorUi->ShowGlobalErrorNoteL( -(KErrorConvTable[i][1] + KKimonoBase )) );
            }
        }
    else
        // global error id is received
        {
        TRAP_IGNORE( iErrorUi->ShowGlobalErrorNoteL( aErrorId ) );
        }
    }

#ifdef __BROWSER_TEL_SERVICES_CALLUI__

//---------------------------------------------------------------------------------------
// CBrowserTelService::HandleNotifyL()
//---------------------------------------------------------------------------------------
//
TInt CBrowserTelService::HandleNotifyL( TInt /*aCmdId*/,
                                        TInt /*aEventId*/,
                                        CAiwGenericParamList& /*aEventParamList*/,
                                        const CAiwGenericParamList& /*aInParamList*/ )
    {
    TELSERVICE_ENTERFN("CBrowserTelService::HandleNotifyL");

 	if( iSynch && iWait.IsStarted() )
		{
		iWait.AsyncStop();  // stop the wait loop. 
							// Now DoMakeCallL will return
		}
    else
        {
        // The client is not interested what happened!
        iSynch = EFalse; // Just in case not to start iWait after ExecuteServiceCmdL
        if( iUserRequestStatus )
            {
            User::RequestComplete( iUserRequestStatus, KErrNone );
            }
        }

    TELSERVICE_LEAVEFN( "CBrowserTelService::HandleNotifyL" );
    return KErrNone;
    }

#endif // __BROWSER_TEL_SERVICES_CALLUI__

// ================= OTHER EXPORTED FUNCTIONS ==============

