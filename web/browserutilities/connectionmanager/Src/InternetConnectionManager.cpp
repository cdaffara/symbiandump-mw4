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
*     This class provides connection/session related information.
*	  Both the requested and the stored AP's are here.
*
*
*/


//INCLUDE FILES

	//System Includes
#include <bldvariant.hrh>

#include <ApAccessPointItem.h>
#include <VpnApEngine.h>
#include <AknNotifyStd.h>
#include <AknGlobalNote.h>
#include <e32std.h>
#include <ApUtils.h>
#include <ApDataHandler.h>
#include <cdbstore.h>
#include <coemain.h>
#include <bautils.h>
#include <connectionmanager.rsg>
#include <barsread.h>
#include <Avkon.rsg>
#include <ErrorUI.h>
#include <AknGlobalConfirmationQuery.h>
#include <AknQueryDialog.h>
#include <AknWaitDialog.h>
#include <cdblen.h>
#include <StringLoader.h>
#include <connectprog.h>
#include <nd_err.h>
#include <commdbconnpref.h>
#include <mmtsy_names.h>
#include <etelmm.h>
#include <data_caging_path_literals.hrh>
#include <AknsUtils.h> 
#include <avkon.mbg>
#include <ConnMan.mbg>
#include <rconnmon.h>
#include <agentdialog.h>
#include <ConnectionUiUtilities.h>
#include <AknQueryDialog.h>
#include <WlanCdbCols.h>
#include <etelpckt.h>
#include <featmgr.h>
#include <cmmanagerext.h>
#include <cmdestinationext.h>
#include <commsdat.h>
#include <commsdattypeinfov1_1.h>

	//User Includes
#include "InternetConnectionManager.h"
#include "ConnMan.hrh"
#include "ConnectionManagerLogger.h"
#include "ConnectionObservers.h"
#include "ConnManActiveConnector.h"

using namespace CMManager;

// CONSTANTS
_LIT(KConnectionResourceFile, "\\resource\\ConnectionManager.rsc");

// ============================ MEMBER FUNCTIONS ===============================
//--------------------------------------------------------------------------
//CInternetConnectionManager::ConnectL( TUint32 aIAPId1, TUint32 aIAPId2 )
//--------------------------------------------------------------------------
EXPORT_C TInt CInternetConnectionManager::ConnectL(  TUint32 /*aIAPId1*/, TUint32 /*aIAPId2*/ )
	{
    User::Panic( KNullDesC, KErrNotSupported );

	return KErrGeneral;
	}

//--------------------------------------------------------------------------
//CInternetConnectionManager::ConnectL( TUint32 aIAPId1 )
//--------------------------------------------------------------------------
EXPORT_C TInt CInternetConnectionManager::ConnectL( TUint32 aIAPId )
	{
	//1) Check whether AP exists
	//2) Check CSD coverage
	//3) Check GPRS coverage
	//4) Check Whether a connection already exists

    CLOG_WRITE_1( "CInternetConnectionManager::ConnectL - %d", aIAPId );

	// GPRS available
	iInternalError = KErrNone;
	iNewConnectionEstablished = EFalse;
	TInt connErr( KErrNone );

	connErr = ConnectWithoutCheckL( aIAPId );
	
    DisplayConnErrorL();

	return connErr;
	}


//---------------------------------------------------------------------------------------
//CInternetConnectionManager::ConnectWithoutCheckL( TUint32 aIAPId1 )
//-----------------------------------------------------------------------------------------
TInt CInternetConnectionManager::ConnectWithoutCheckL( TUint32 aIAPId )
	{
    CLOG_ENTERFN( "ConnectWithoutCheckL" );

	iInternalError = KErrNone;
	TInt connErr;
	//Do the connection
    TCommDbConnPref pref;
	TBool gprsCovered( ETrue );
	if( !CheckNetworkL( gprsCovered ) )
		{
		//show that there is no coverage!
		ShowGlobalNoteL( EAknGlobalErrorNote, R_EXT_ERR_NO_NETWORK_COVER );
		connErr = KErrGeneral;
		}
	else //there is coverage			
		{
        TBool IsApGprs( EFalse );
		IsApGprs = BearerTypeL( aIAPId ) == EApBearerTypeGPRS;
		if( !iRequestedAPIds.iFirstPreference )
		    {
		    pref.SetDialogPreference( ECommDbDialogPrefPrompt );
		    }
		else
		    {
            pref.SetDialogPreference( ECommDbDialogPrefDoNotPrompt );
		    }

		if( gprsCovered || !IsApGprs  )
			{
            pref.SetIapId( aIAPId );
            if( !iSilentMode && iRequestedAPIds.iFirstPreference )

                // Temp fix for CDMA 
                {
                if( IsApGprs )
                    {
                    pref.SetBearerSet( ECommDbBearerGPRS );
                    }
                else
                    {
                    pref.SetBearerSet( ECommDbBearerCSD );
                    }
                }

            if (BearerTypeL( aIAPId ) == EApBearerTypeAllBearers)
            	{
            		pref.SetBearerSet( ECommDbBearerCSD |
									   ECommDbBearerPSD | // includes both KCommDbBearerWcdma and KCommDbBearerCdma2000. ECommDbBearerGPRS is ECommDbBearerWcdma
									   ECommDbBearerLAN |
									   ECommDbBearerVirtual |
									   ECommDbBearerPAN |
									   ECommDbBearerWLAN );
            	}

			connErr = ConnectL( pref );
			}
		else //there is no gprs coverage and the AP needs it!!!
			{
			//show that there is no coverage!!!
			ShowGlobalNoteL( EAknGlobalErrorNote, R_EXT_ERR_NO_NETWORK_COVER );
			connErr = KErrGeneral;
			}
		}

    if( connErr == KErrNone )
        {
        iConnected = ETrue;

        // Enable connection cloning        
        _LIT_SECURITY_POLICY_C2(KProcPolicy1, ECapabilityNetworkServices, ECapabilityNetworkControl);

        TSecurityPolicyBuf secPolBuf;
        secPolBuf().Set(KProcPolicy1().Package());

        iConnection.Control(KCOLConnection, KCoEnableCloneOpen, secPolBuf);
        
        if( iSilentMode )
            // Temp fix for CDMA
            {
            TUint32 iapId;
            TBuf<20> query;
            TBuf<40> val;

            query.Format( _L("%s\\%s"), IAP, COMMDB_ID );
            User::LeaveIfError( iConnection.GetIntSetting( query, iapId ) );
            iCurrentAP = (CApAccessPointItem*)iapId;

            query.Format(_L("%s\\%s"), IAP, COMMDB_NAME);
            User::LeaveIfError( iConnection.GetDesSetting( query, val ) );

            iConnName = val.AllocL();
            }
        else if( !iRequestedAPIds.iFirstPreference )
            {
            TBuf<20> query;
            TBuf<40> val;

            query.Format( _L("%s\\%s"), IAP, COMMDB_ID );
            User::LeaveIfError( iConnection.GetIntSetting( query, iRequestedAPIds.iFirstPreference ) );
#ifndef __WINS__
            CApAccessPointItem* ap = APItemFromIAPIdLC( iRequestedAPIds.iFirstPreference );
		    UpdateCurrentAPL( *ap, EFalse );
		    CleanupStack::PopAndDestroy();  // ap
#endif
            query.Format(_L("%s\\%s"), IAP, COMMDB_NAME);
            User::LeaveIfError( iConnection.GetDesSetting( query, val ) );

            iConnName = val.AllocL();
            }
        else
            {
#ifndef __WINS__
            CApAccessPointItem* ap = APItemFromIAPIdLC( aIAPId );
		    UpdateCurrentAPL( *ap, EFalse );
		    CleanupStack::PopAndDestroy();  // ap

#endif
            }

        StartConnectionObservingL();
        }

	return connErr;
	}
	
//--------------------------------------------------------------------------
//CInternetConnectionManager::ConnectWithoutCheckL( TUint32 aIAPId1 )
//--------------------------------------------------------------------------
TInt CInternetConnectionManager::ConnectWithoutCheckL( TUint32 /*aIAPId1*/, TUint32 /*aIAPId2*/ )
	{
    User::Panic( KNullDesC, KErrNotSupported );

	return KErrGeneral;
	}

//--------------------------------------------------------------------------
//CInternetConnectionManager::DisplayConnErrorL
//--------------------------------------------------------------------------
void CInternetConnectionManager::DisplayConnErrorL()
	{
	CLOG_ENTERFN("DisplayConnErrorL");
	if ( iInternalError && !iSilentMode && iInternalError!= KErrCancel && IsConnectionAllowedL( EFalse ) )
		{
		CLOG_WRITE("Some Error should be shown");
		CLOG_WRITE_1("Error %d", iInternalError);
		//filtering out errors that are shown by the phone app
        if( iInternalError == KErrEtelCallAlreadyActive ||
            iInternalError == KErrEtelBusyDetected )
            {
            ShowErrorNoteL( iInternalError );
            }
        else if ( !InIntArray( KNumsOfDontShowErrors, KADontShowErrors, iInternalError ) )
			{
			CLOG_WRITE("Error is shown by ConnMan");
			if ( !InIntArray( KNumsOfExcludeFromConverting , 
							  KAExcludeFromConverting, 
							  iInternalError )
				&&	( iInternalError == KErrGeneral || iInternalError == KErrCommsLineFail 
				|| KEtelExtErrIntervalLow < iInternalError && iInternalError < KEtelExtErrIntervalHigh 
				|| KGsmErrIntervalLow < iInternalError && iInternalError < KGsmErrIntervalHigh 
				|| KEtelCoreErrIntervalLow < iInternalError && iInternalError < KEtelCoreErrIntervalHigh
				)  )
				{
				CLOG_WRITE("KErrExitModemError");
				ShowErrorNoteL( KErrExitModemError );
				}
			else
				{
                    if (iInternalError == KErrNotFound ) 
				    {
				        // Since -1 is an error from bottom layer with no more information
				        // The good match using "Extended Error Handling" of UI spec would 
				        // be "connection not available". 
                        ShowErrorNoteL(NW_STAT_WPS_ERROR, CTextResolver::ECtxNoCtxNoSeparator);                         
				    }
				    else 
				    {
					    ShowErrorNoteL( iInternalError );
				    }
				}
			}
		}
    else
        {
        if( !IsConnectionAllowedL( EFalse ) )
            {
            CLOG_WRITE("Operation is not allowed in off-line mode");
            ShowGlobalNoteL( EAknGlobalInformationNote, R_QTN_OFFLINE_NOT_POSSIBLE );
            iInternalError = KErrGeneral;
            }
        }
    }
	
	
//--------------------------------------------------------------------------
//CInternetConnectionManager::NewL
//--------------------------------------------------------------------------

EXPORT_C CInternetConnectionManager* CInternetConnectionManager::NewL( TBool aSilentMode )
	{
    CInternetConnectionManager* self = CInternetConnectionManager::NewLC( aSilentMode );
	CleanupStack::Pop();
	return self;
	}

//--------------------------------------------------------------------------
//CInternetConnectionManager::NewL
//--------------------------------------------------------------------------

EXPORT_C CInternetConnectionManager* CInternetConnectionManager::NewL( CCommsDatabase* aCommsDb, TBool aSilentMode )
	{
	CInternetConnectionManager* self = CInternetConnectionManager::NewLC( aCommsDb, aSilentMode );
	CleanupStack::Pop();
	return self;
	}


//--------------------------------------------------------------------------
//CInternetConnectionManager::NewLC
//--------------------------------------------------------------------------
EXPORT_C CInternetConnectionManager* CInternetConnectionManager::NewLC( TBool aSilentMode )
	{
	CInternetConnectionManager*  self = new( ELeave )CInternetConnectionManager( aSilentMode );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

//--------------------------------------------------------------------------
//CInternetConnectionManager::NewLC
//--------------------------------------------------------------------------
EXPORT_C CInternetConnectionManager* CInternetConnectionManager::NewLC( CCommsDatabase* aCommsDb, TBool aSilentMode )
	{
    CInternetConnectionManager*  self = new( ELeave )CInternetConnectionManager( aCommsDb, aSilentMode );
	CleanupStack::PushL( self );
	self->ConstructL();
	return self;
	}

//--------------------------------------------------------------------------
//CInternetConnectionManager::ConnectL
//--------------------------------------------------------------------------

TInt CInternetConnectionManager::ConnectL
	(
    TCommDbConnPref& /*aPref1*/,
    TCommDbConnPref& /*aPref2*/
	)
	{
    User::Panic( KNullDesC, KErrNotSupported );

	return KErrGeneral;
	}

//--------------------------------------------------------------------------
//CInternetConnectionManager::ConnectL
//--------------------------------------------------------------------------

TInt CInternetConnectionManager::ConnectL
	(
    TCommDbConnPref& aPref1
	)
	{
	if ( iAlreadyInUse )
		{
		CLOG_WRITE( "Reentering happend, calling User::Leave!!!" );
		User::Leave( KErrAlreadyExists );
		}
	iAlreadyInUse = ETrue;
	iInternalError = KErrNone;
	iCalledFromProcessFinish = EFalse;
	TInt connErr( KErrNone );
	
	CLOG_WRITE( "ConnMan: Setting overides" );
		//Leave point
    TCommDbConnPref *overrides = new (ELeave) TCommDbConnPref;
	CleanupStack::PushL( overrides );

    // there's no copy constructor thus 
    // I need to make a copy of this object manually
    overrides->SetDialogPreference( aPref1.DialogPreference() );
    // Because from 2.0 there is only browser who integrates
    // connection manager there's no need for prompt
    // browser always sets valid access point id.
//    __ASSERT_DEBUG( aPref1.IapId(), User::Panic( KNullDesC, KErrArgument ) ); // Temp for CDMA fix
    // set access point id in overrides
    overrides->SetIapId( aPref1.IapId() );
    overrides->SetBearerSet( aPref1.BearerSet() );

    TInt connAllowed( ETrue );
    
    TInt err = 
        iRepository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed);
    
    CLOG_WRITE_1( "ConnMan connAllowed: %d", connAllowed );
    if( !connAllowed && ( KErrNone == err ) )
        {
        CLOG_WRITE( "ConnMan SetBearerSet: ECommDbBearerWLAN"  );
        overrides->SetBearerSet( ECommDbBearerWLAN );
        }


	//TUint32 bearer = aPref1.BearerSet();
	CLOG_WRITE( "ConnMan Overrides set"  );
//>>>>>>>>>>>
    if ( !iSilentMode && !connErr )
		{
		CLOG_WRITE( "ConnMan: Starting Wait dlg with name" );
		HBufC* buf = NULL;
		if( aPref1.IapId() )
		    {
        	buf = StringLoader::LoadL
        				( 
        				R_ICS_WAIT_CONNECTING, 
        				*APNameLC( aPref1.IapId() )
        				);
        	CleanupStack::PushL( buf );
		    }
		CLOG_WRITE( "Calling StartWaitDialogL" );
		StartWaitDialogL( buf, EWaitGPRSIcon );
		CLOG_WRITE( "StartWaitDialogL returned with no leave" );
        if( aPref1.IapId() )
		    {		
		    CleanupStack::PopAndDestroy( 2 );//buf, HBuf* returned by APNameLC	
		    }
		}
//>>>>>>>>>>>
	if ( !connErr )
		{
		TBool alreadyCancelled = ( !iSilentMode && !iWaitDialog );
		if ( alreadyCancelled )
			{
			connErr = KErrCancel;
			}
		else
			{               
		        connErr = iConnection.Open( iServ, KAfInet );
		        CLOG_WRITE_1( "RConnection: %d", connErr );
		        if( connErr == KErrNone )
		            {
		            // Always pass the IAP Id to RConnection even in silent mode
		            connErr = iSyncConnector->Connect( overrides );
		            }
		        if( connErr != KErrNone )
		            {
		            CLOG_WRITE( "Closing all" );
		            iConnection.Close();
		            }
			}
		}
	if ( iWaitDialog )
		{
		CLOG_WRITE(  "Killing wait dialog..." );
		iCalledFromProcessFinish = ETrue;
		iWaitDialog->ProcessFinishedL();
		CLOG_WRITE(  "ProgressFinished is called" );
		}
	CleanupStack::PopAndDestroy();//overrides
    	iInternalError = connErr;

	CLOG_WRITE_1( "ConnectL returned with %d", connErr );
	iNewConnectionEstablished = ( connErr == KErrNone );
	iAlreadyInUse = EFalse;
	return connErr;
	}


//--------------------------------------------------------------------------
//CInternetConnectionManager::~CInternetConnectionManager
//--------------------------------------------------------------------------
			
CInternetConnectionManager::~CInternetConnectionManager()
	{
	CLOG_WRITE("Destructor");
	
    Disconnect();
    
    delete iStageNotifier;

    iMobilePhone.Close();
    iTelServer.Close();
    
    delete iRepository;

	if( !iSilentMode )
		{
		CCoeEnv::Static()->DeleteResourceFile( iResOffset );
		}
    delete iVpnEngine;
    delete iApDataHandler;
	delete iApUtils;
	delete iNote;
	if ( iCommsDbOwned )
		{
		delete iCommsDb;
		}

    if( iConnected )
        {
        iConnection.Close();
        }

    if( !iSilentMode )
        // Temp fix for CDMA
        {
        delete iCurrentAP;
        }

    delete iVpnItem;

    // Temp fix for CDMA
    delete iConnName;
	delete iWaitDialog;
	delete iNoteDialog;
	delete iSyncConnector;
	iRFs.Close();

    iServ.Close();
    
	CLOG_CLOSE;
	}
	
//--------------------------------------------------------------------------
//CInternetConnectionManager::CInternetConnectionManager
//--------------------------------------------------------------------------

CInternetConnectionManager::CInternetConnectionManager( TBool aSilentMode )
    : iSilentMode( aSilentMode )
	,iRequestedAPIds()
	{
	CLOG_CREATE;
	CLOG_NAME_1( _L("IntConnMan_%x"), this );
	}
		
//--------------------------------------------------------------------------
//CInternetConnectionManager::CInternetConnectionManager
//--------------------------------------------------------------------------
CInternetConnectionManager::CInternetConnectionManager( CCommsDatabase* aCommsDb, TBool aSilentMode ): iCommsDb( aCommsDb ), iSilentMode( aSilentMode )
	{
	CLOG_CREATE;
	CLOG_NAME_1( _L("IntConnMan_%x"), this );
	}

//--------------------------------------------------------------------------
//CInternetConnectionManager::ConstructL
//--------------------------------------------------------------------------
void CInternetConnectionManager::ConstructL()
	{
	CLOG_ENTERFN( "ConstructL" );
	}

//--------------------------------------------------------------------------
//CInternetConnectionManager::CheckNetworkL
//--------------------------------------------------------------------------
TBool CInternetConnectionManager::CheckNetworkL( TBool& aGPRSAvailable )
	{
    aGPRSAvailable = ETrue;
    return ETrue;
	}

//--------------------------------------------------------------------------
//CInternetConnectionManager::BearerTypeL
//--------------------------------------------------------------------------
TApBearerType CInternetConnectionManager::BearerTypeL( TUint32 aIAPId )
	{
	TApBearerType apbearerType = EApBearerTypeAllBearers;
    if( iSilentMode || !iRequestedAPIds.iFirstPreference )
        // Temp fix for CDMA 
        {
        return EApBearerTypeAllBearers;
        }

#ifndef __WINS__
    CApAccessPointItem* ap = APItemFromIAPIdLC( aIAPId );
    TUint32 wapAPId = ap->WapUid();
    CleanupStack::PopAndDestroy( ap );  // ap
    TRAPD(err, apbearerType = iApUtils->BearerTypeL( wapAPId ));
    if( err == KErrNone)
        {
    	return(apbearerType);
    	}
    else
    	{
     	return (EApBearerTypeAllBearers);
    	}
#else
    aIAPId = 0;
    return EApBearerTypeGPRS;
#endif
	}

//---------------------------------------------------------
// CInternetConnectionManager::ShowGeneralConnectionErrorL
//---------------------------------------------------------

EXPORT_C void CInternetConnectionManager::ShowGeneralConnectionErrorL()
	{
	ShowErrorNoteL( KErrGeneralConnection );
	}

//---------------------------------------------------------
// CInternetConnectionManager::ShowErrorNoteL
//---------------------------------------------------------
void CInternetConnectionManager::ShowErrorNoteL( 
		    TInt aErrorStat,
		    CTextResolver::TErrorContext aContext )
	{
	if ( !iSilentMode )
		{
		CLOG_WRITE_1(  "Showing Error: %d", aErrorStat );
		CErrorUI* errorUI = CErrorUI::NewLC( );
        if ( !errorUI->ShowGlobalErrorNoteL( aErrorStat, aContext) )
            {
            errorUI->ShowGlobalErrorNoteL( KErrGeneralConnection, aContext);
            }
        CleanupStack::PopAndDestroy();//errorUI
		}
	}

//---------------------------------------------------------
// CInternetConnectionManager::StopConnectionL
//---------------------------------------------------------
EXPORT_C void CInternetConnectionManager::StopConnectionL()
	{
    CLOG_ENTERFN( "StopConnectionL()" );

    StopConnectionObserving();
    iConnection.Close();
//    iServ.Close();
    iConnected = EFalse;
    iEasyWlan = EFalse;

    if( !iSilentMode )
        {
        delete iCurrentAP;
        }
    iCurrentAP = NULL;
    }

//-------------------------------------------------------------------------
// CInternetConnectionManager::NewConnectionEstablished()
//-------------------------------------------------------------------------
EXPORT_C TBool CInternetConnectionManager::NewConnectionEstablished(  ) const
	{
	return iNewConnectionEstablished;	
	}

//---------------------------------------------------------------
//CInternetConnectionManager::ShowConfQueryNoteL
//---------------------------------------------------------------
TInt CInternetConnectionManager::ShowConfQueryNoteL( TInt aTextResId )
	{
	TInt retVal(  EAknSoftkeyCancel );
	if ( !iSilentMode )
		{
        HBufC* buf = StringLoader::LoadLC( aTextResId );
		CAknQueryDialog* dialog =  CAknQueryDialog::NewL();
		dialog->PrepareLC( R_CONNMAN_CONF_QUERY );
		dialog->SetPromptL( buf->Des()  );
		retVal = dialog->RunLD();
		CleanupStack::PopAndDestroy();//buf
		}
	return retVal;
	}

//---------------------------------------------------------------
//CInternetConnectionManager::APNameLC
//---------------------------------------------------------------
HBufC* CInternetConnectionManager::APNameLC( TUint32 aIAPId ) const
	{
	TBuf <KCommsDbSvrMaxFieldLength> apName;
#ifndef __WINS__
    TUint32 wapAPId = iApUtils->WapIdFromIapIdL( aIAPId );

	iApUtils->NameL( wapAPId, apName );
#else
    aIAPId = 0;
#endif
	return ( apName.AllocLC() );
	}

//---------------------------------------------------------------
//CInternetConnectionManager::DialogDismissedL
//---------------------------------------------------------------
void CInternetConnectionManager::DialogDismissedL( TInt /*aButtonId*/ )
	{
	if ( !iCalledFromProcessFinish )
		{
		CLOG_WRITE( "Dialog Dismissed called" );
		iSyncConnector->Cancel();
		CLOG_WRITE( "Stopping connection" ); 
		}
	}
//---------------------------------------------------------------
//CInternetConnectionManager::ShowGlobalNoteL
//---------------------------------------------------------------
void CInternetConnectionManager::ShowGlobalNoteL( TAknGlobalNoteType aNoteType, TInt aTextResId )
	{
	if (!iSilentMode )
		{
			HBufC* buf;
			buf = StringLoader::LoadLC( aTextResId );
			iNote->ShowNoteL( aNoteType, buf->Des() );
			CleanupStack::PopAndDestroy();//Cleanup the buf
		}
	}


//---------------------------------------------------------------
//CInternetConnectionManager::ShowInformationNoteL
//---------------------------------------------------------------
void CInternetConnectionManager::ShowInformationNoteL( TInt aTextResId )
	{
	if (!iSilentMode )
		{
		if ( iNoteDialog )
			{
			delete iNoteDialog ;
			iNoteDialog = NULL;
			}
		iNoteDialog = new (ELeave) CAknNoteDialog
			( REINTERPRET_CAST( CEikDialog**, &iNoteDialog ),
            CAknNoteDialog::EConfirmationTone,CAknNoteDialog::ELongTimeout );
		HBufC* buf = StringLoader::LoadLC( aTextResId );
		iNoteDialog->PrepareLC( R_CONNMAN_INFORMATION_NOTE );
		iNoteDialog->SetTextL( buf->Des() );
		iNoteDialog->RunLD();
		CleanupStack::PopAndDestroy();//buf
		}
	}


//------------------------------------------------------------------------
//CInternetConnectionManager::InIntArray
//------------------------------------------------------------------------
TBool CInternetConnectionManager::InIntArray( TInt aUpperBound, const TInt* aArray, TInt aElement )
	{
	TBool found( EFalse );
	for (TInt i = 0; i< aUpperBound ; i++ )
		{
		if ( aArray[i] == aElement )
			{
			found = ETrue;
			break;	
			}
		}
	return found;
	}

//-------------------------------------------------------------------
//CInternetConnectionManager::Connection
//-------------------------------------------------------------------
//
EXPORT_C RConnection& CInternetConnectionManager::Connection()
    {
    return iConnection;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::ConnectionNameL
//-------------------------------------------------------------------
//
EXPORT_C TName* CInternetConnectionManager::ConnectionNameL()
    {
    if( !iConnected )
        {
        User::Leave( KErrGeneral );
        }

    TName *name = new (ELeave) TName;
    CleanupStack::PushL( name );
    
    User::LeaveIfError( iConnection.Name( *name ) );

    CleanupStack::Pop( name );

    return name;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::SocketServer
//-------------------------------------------------------------------
//
EXPORT_C RSocketServ& CInternetConnectionManager::SocketServer()
    {
    return iServ;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::SetRequestedAPs
//-------------------------------------------------------------------
//
EXPORT_C void CInternetConnectionManager::SetRequestedAPs( TIdPair aRequestedAPIds )
	{
	iRequestedAPIds = aRequestedAPIds;
	iNoSecondPreference = EFalse;
    }
    

//-------------------------------------------------------------------
//CInternetConnectionManager::SetRequestedAP
//-------------------------------------------------------------------
//
EXPORT_C void CInternetConnectionManager::SetRequestedAP( TUint32 aRequestedAPId, TBool /*aDefault*/ )
	{
	iRequestedAPIds.iFirstPreference = aRequestedAPId;
	iNoSecondPreference = ETrue;
	}

//-------------------------------------------------------------------
//CInternetConnectionManager::RequestedAPValidityL
//-------------------------------------------------------------------
//
EXPORT_C TAPValidity CInternetConnectionManager::RequestedAPValidityL()
	{
    return EBoth;
	}

// ---------------------------------------------------------
// CInternetConnectionManager::Connected
// ---------------------------------------------------------
//
EXPORT_C TBool CInternetConnectionManager::Connected()
	{
    CLOG_WRITE_1( "Int::Connected: %d", iConnected );
    return iConnected;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::CurrentAccessPoint
//-------------------------------------------------------------------
//
EXPORT_C const CApAccessPointItem*
    CInternetConnectionManager::CurrentAccessPoint() const
	{
	User::Panic( KNullDesC, KErrNotSupported );

    return NULL;
	}

// ---------------------------------------------------------
// CInternetConnectionManager::CurrentAPId
// ---------------------------------------------------------
//  
EXPORT_C TUint32 CInternetConnectionManager::CurrentAPId() const
	{
    if( !iSilentMode )
        {
	    if ( iCurrentAP )
		    {
		    TUint32 ret( 0 );
		    TRAPD( err, ret = iApUtils->IapIdFromWapIdL( iCurrentAP->WapUid() ) );
		    ret = ( KErrNone != err ) ? KPrompt : ret;
		    return ret;
		    }
	    else
		    {
		    return KPrompt;
		    }
        }
    else
        // Temp fix for CDMA
        {
        return (TUint32)iCurrentAP;
        }
	}

// ---------------------------------------------------------
// CInternetConnectionManager::CurrentSessionSecure
// ---------------------------------------------------------
//
EXPORT_C TBool CInternetConnectionManager::CurrentSessionSecure() const
	{
	User::Panic( KNullDesC, KErrNotSupported );

    return EFalse;
	}

// --------------------------------------------------------------
// CInternetConnectionManager::CurrentSessionConnectionOriented
// --------------------------------------------------------------
//
EXPORT_C TBool CInternetConnectionManager::CurrentSessionConnectionOriented() const
	{
	User::Panic( KNullDesC, KErrNotSupported );

	return EFalse;
	}

// ---------------------------------------------------------
// CInternetConnectionManager::CurrentGatewayLC
// ---------------------------------------------------------
//
EXPORT_C HBufC* CInternetConnectionManager::CurrentGatewayLC() const
	{
	User::Panic( KNullDesC, KErrNotSupported );

	return NULL;
	}

// ---------------------------------------------------------
// CInternetConnectionManager::CurrentStartPageLC
// ---------------------------------------------------------
//
EXPORT_C HBufC* CInternetConnectionManager::CurrentStartPageLC() const
	{
	User::Panic( KNullDesC, KErrNotSupported );

	return NULL;
	}

// ---------------------------------------------------------
// CInternetConnectionManager::CurrentApNameLC
// ---------------------------------------------------------
//
EXPORT_C HBufC* CInternetConnectionManager::CurrentApNameLC() const
	{
    if( iSilentMode )
        // Temp fix for CDMA
        {
        return iConnName->AllocLC();
        }

#ifndef __WINS__
    if( iCurrentAP )
        {
	    return APNameLC( iApUtils->IapIdFromWapIdL( iCurrentAP->WapUid() ) );
        }
    else
        {
        User::Panic( KNullDesC, KErrGeneral );

        // to avoid compiler warning
        return NULL;
        }
#else
    return KNullDesC().AllocLC();
#endif
	}

// --------------------------------------------------------------
// CInternetConnectionManager::CurrentConnectionSpeed
// --------------------------------------------------------------
//
EXPORT_C TApCallSpeed CInternetConnectionManager::CurrentConnectionSpeed() const
	{
	TApCallSpeed  connectionSpeed( KSpeedAutobaud );
	if ( iCurrentAP && !iSilentMode )
        // Temp fix for CDMA
		{
		iCurrentAP->ReadUint( EApIspBearerSpeed, (TUint32&)connectionSpeed );
		}

    return connectionSpeed;
	}

// ---------------------------------------------------------
// CInternetConnectionManager::CurrentBearerTypeL
// ---------------------------------------------------------
//

EXPORT_C TApBearerType CInternetConnectionManager::CurrentBearerTypeL() const
	{
#ifndef __WINS__
	if ( iCurrentAP && !iSilentMode )
        // Temp fix for CDMA
		{
        CLOG_WRITE_1("BT: %d", iCurrentAP->BearerTypeL() );
		return iCurrentAP->BearerTypeL();
		}

    CLOG_WRITE( "Unknown BT" );
	return EApBearerTypeAll;
#else
    return EApBearerTypeGPRS;
#endif
	}

// ---------------------------------------------------------
// CInternetConnectionManager::Disconnect
// ---------------------------------------------------------
//
EXPORT_C void CInternetConnectionManager::Disconnect()
	{
	TRAP_IGNORE( StopConnectionL() );
	}

// ---------------------------------------------------------
// CInternetConnectionManager::SetApChangeObserver
// ---------------------------------------------------------
//
EXPORT_C void CInternetConnectionManager::SetApChangeObserver( MApChangeObserver& aObserver )
	{
	__ASSERT_DEBUG ( iApObserver == NULL, User::Panic( KNullDesC, KErrInUse ) );
	iApObserver = &aObserver;
	}

// ---------------------------------------------------------
// CInternetConnectionManager::UnsetApChangeObserver
// ---------------------------------------------------------
//
EXPORT_C void CInternetConnectionManager::UnsetApChangeObserver()
	{
	iApObserver = 0;
	}

// ---------------------------------------------------------
// CInternetConnectionManager::StartConnectionL
// ---------------------------------------------------------
//
EXPORT_C TInt CInternetConnectionManager::StartConnectionL( TBool aDisableConnNeeded, TBool /*aIgnoreSecureAttribute*/ )
	{
	CLOG_ENTERFN( "StartConnectionL" );
	CLOG_WRITE_1( "IAPid: [%d]", iRequestedAPIds.iFirstPreference );
	
		TUint32 newIapId;
	    TBool doConnect( EFalse );
	    TInt err( KErrNone );

    InitializeL();
    
    //Connect with Snap Id if the connection type is Destination Nw
    
    if (iConnectionType == EDestination)
    	{
    	err = ConnectWithSnapId(iRequestedSnapId);	
    	return err;
    	}
    	else
    		{
    			
			TApBearerType bearerType( EApBearerTypeAllBearers );
			
			if( iRequestedAPIds.iFirstPreference )
			    {
			    bearerType = BearerTypeL( iRequestedAPIds.iFirstPreference );
			    }
		    	
		    CLOG_WRITE_1("Bearer type: %d", bearerType );
		    
			 /* Connecting to VPN tunnel on the same real IAP using which VPN configurations were made then
	    		the existing connection would not be disconnected but a new VPN tunnel will be created on top of it.
	    		so iConnected would be true which was making it to leave with only if(iConnected), it should not leave 
	    		incase of VPN tunnel so that VPN tunnel could be created further. To avoid leaving from this condition
	   		  	incase of VPN connection added a new connection for checking if iConnected is true + its a non VPN connection 
	    		incase of VPN this condition will be ignored and following connectL function will be called to create a VPN connection.*/

	    	if( iConnected && !iVpnItem)
	        	{
	        	CLOG_WRITE("Already connected");
	        	return KErrNone;
	        	}



			    if( iRequestedAPIds.iFirstPreference && CheckIfAlreadyConnected( newIapId ) )
			        // there's connection with any of the requested AP point
			        // try to attach to the that
			        {
			        CLOG_WRITE("Already connected with this IAPId");
			        err = ConnectL( newIapId );
			        if( err != KErrNone && err != KErrCancel )
			            // we couldn't attach but give me one more chance
			            {
			            CLOG_WRITE_1("Attach failed:%d", err);
			            doConnect = ETrue;
			            }
			        }
			    else
			        {
			        if( !IsConnectionAllowedL( ETrue ) )
			            // In offline mode it's not possible to start connection
			            {
			            return KErrGeneral;
			            }


			        TBool nw3G = Check3GNetworkL();
			        // In 3G network or if bearer type is WLAN 
			        // we don't have to check if there is (HS)CSD connection
			        // opened in the phone
			        TBool isThere = nw3G || bearerType == EApBearerTypeWLAN ? EFalse : IsThereActiveHSCSDConnectionL();

			        if( !isThere )
			            {
			            // In 3G network or if bearer type is WLAN 
			            // we don't have to check if there is active voice
			            // call in the phone
			            isThere = nw3G || bearerType == EApBearerTypeWLAN ? EFalse : IsThereActiveVoiceCall();

			            if( !isThere )
			                {
			                if ( !iSilentMode && !aDisableConnNeeded )
					            {
					            if ( ShowConfQueryNoteL ( R_WML_CONF_CONN_NEED ) )
						            {
			                        if( IsConnectionAllowedL( ETrue ) )
			                            {
			                            doConnect = ETrue;
			                            }
			                        else
			                            {
			                            err = KErrGeneral;                                
			                            }
						            }
			                    else
			                        {
						            err = KErrCancel;	
			                        }
					            }
			                else
			                    // we are in silent mode, conn query is not needed.
			                    // in this case connection has to be started automatically.
			                    {
			                    doConnect = ETrue;
			                    }
			                }
			            else
			                {
			                err = iInternalError = KErrEtelCallAlreadyActive;
			                DisplayConnErrorL();
			                }
			            }
			        else
			            {
			            err = KErrGeneral;
			            ShowInformationNoteL( R_ICS_NOTE_CLOSE_CONNECTION );
			            }
			        }

			    if( !err )
			        {
			        if( doConnect )
			            {
			            TUint32 usedAp( iRequestedAPIds.iFirstPreference );

			            if( IsConnectionAllowedL( ETrue ) )
			                {
			                err = ConnectL( usedAp );
			                }
			            }
			        }
			        
			    if( err == KErrNone )
			        {
			        CLOG_WRITE( "No error in connection" );
			#if !defined( __WINS__ )
			        // it's not a critical problem if this flag
			        // cannot be updated.
			        TRAP_IGNORE( UpdateEasyWlanFlagL() );
			#endif        
			        }
			    else if( err == KErrEtelCallNotActive )
			        // this is a special case. User pressed end key
			        // while (HS)CSD connection was coming up.
			        {
			        err = KErrCancel;
			        }
			    else if( err != KErrCancel )
			        // to make it compatible with old connman.
			        {
			        CLOG_WRITE_1( "Error: %d", err );
			        err = KErrGeneral;
			        }

			    return err;
	    	} //if iConnection
	}

// ---------------------------------------------------------
// CInternetConnectionManager::CurrentAP
// ---------------------------------------------------------
//
EXPORT_C CApAccessPointItem* CInternetConnectionManager::CurrentAP()
    {
    if( iSilentMode )
        // Temp fix for CDMA
        {
        User::Panic( KNullDesC, KErrGeneral );
        }
    CLOG_WRITE_1( "CurrentAP(): %d", (TUint32)iCurrentAP );
    return iCurrentAP;
    }

// ---------------------------------------------------------
// CInternetConnectionManager::UpdateCurrentAPL
// ---------------------------------------------------------
//
void CInternetConnectionManager::UpdateCurrentAPL( CApAccessPointItem& aBaseAP,TBool aOnlyWAPpart )
    {
    if( iSilentMode )
        // Temp fix for CDMA
        {
        User::Panic( KNullDesC, KErrGeneral );
        }

    CLOG_WRITE( "CIntConnMan::UpdateCurrentAPL" );
    if ( !aOnlyWAPpart || !iCurrentAP )
        {
        delete iCurrentAP;
        iCurrentAP = NULL;
        iCurrentAP = CApAccessPointItem::NewLC();
        CleanupStack::Pop();//Don't leave it on cleanup stack because it is a member
        iCurrentAP->CopyFromL( aBaseAP );
        TUint32 wapId;
        aBaseAP.ReadUint( EApWapAccessPointID, wapId );
        iCurrentAP->WriteUint( EApWapAccessPointID, wapId );
        }
    else
        {
        TUint32 length( 0 );
        length = aBaseAP.ReadTextLengthL( EApWapGatewayAddress );
        HBufC* gw = HBufC::NewLC( length );//C:1
        TPtr addressPtr( gw->Des() );
        aBaseAP.ReadTextL( EApWapGatewayAddress, addressPtr );

        const HBufC* sp = aBaseAP.ReadConstLongTextL( EApWapStartPage );//Not owned!

        length = aBaseAP.ReadTextLengthL( EApWapAccessPointName );
        HBufC* apn = HBufC::NewLC( length );//C:2
        TPtr apnPtr( apn->Des() );
        aBaseAP.ReadTextL( EApWapAccessPointName, apnPtr );
	        
        TBool sec;
        aBaseAP.ReadBool( EApWapSecurity, sec );

        TUint32 ct;
        aBaseAP.ReadUint( EApWapWspOption, ct );

        TUint32 wapUid;
        aBaseAP.ReadUint( EApWapAccessPointID, wapUid );

        TPtrC temp = ( const_cast<HBufC*>(sp) )->Des();
        iCurrentAP->WriteLongTextL(	EApWapStartPage, temp );
        iCurrentAP->WriteTextL( EApWapGatewayAddress, gw->Des() );
        iCurrentAP->WriteTextL( EApWapAccessPointName, apn->Des() );
        iCurrentAP->WriteBool( EApWapSecurity, sec );
        iCurrentAP->WriteUint( EApWapWspOption, ct );
        iCurrentAP->WriteUint( EApWapAccessPointID, wapUid );//change the wapUid of the current one
        CleanupStack::PopAndDestroy( 2 );//apn, gw
        }
    CLOG_WRITE( "CIntConnMan::UpdateCurrentAPL done" );
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::RequestedIdPair
//-------------------------------------------------------------------
//
TIdPair& CInternetConnectionManager::RequestedIdPair()
    {
    return iRequestedAPIds;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::NoSecondPreference
//-------------------------------------------------------------------
//
TBool CInternetConnectionManager::NoSecondPreference() const
    {
    return iNoSecondPreference;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::SetNoSecondPreference
//-------------------------------------------------------------------
//
void CInternetConnectionManager::SetNoSecondPreference( TBool aNoSecondPreference )
    {
    iNoSecondPreference = aNoSecondPreference;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::SilentMode
//-------------------------------------------------------------------
//
TBool CInternetConnectionManager::SilentMode() const
    {
    return iSilentMode;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::ApUtils
//-------------------------------------------------------------------
//
CApUtils* CInternetConnectionManager::ApUtils()
    {
    return iApUtils;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::ApObserver
//-------------------------------------------------------------------
//
MApChangeObserver* CInternetConnectionManager::ApObserver()
    {
    return iApObserver;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::StartConnectionObservingL
//-------------------------------------------------------------------
//
void CInternetConnectionManager::StartConnectionObservingL()
    {
    CLOG_ENTERFN( "StartConnectionObservingL()" );

    TName* name = ConnectionNameL();
    __ASSERT_DEBUG( name, User::Panic( KNullDesC, KErrCorrupt ) );
    CleanupStack::PushL( name );
    
    iSatges[0] = KConnectionUninitialised;
    iSatges[1] = KConnectionClosed;
    iSatges[2] = KLinkLayerClosed;

    iStageNotifier->StartNotificationL( name,iSatges,KMaxStages,this, ETrue );

    CleanupStack::PopAndDestroy( name );
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::StopConnectionObserving
//-------------------------------------------------------------------
//
void CInternetConnectionManager::StopConnectionObserving()
    {
    CLOG_ENTERFN( "StopConnectionObserving()" );
    if ( iStageNotifier )
        {
        iStageNotifier->Cancel();
        }
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::ConnectionStageAchievedL
//-------------------------------------------------------------------
//
void CInternetConnectionManager::ConnectionStageAchievedL(TInt /*aStage*/)
    {
    CLOG_ENTERFN( "ConnectionStageAchievedL()" );
 //   CLOG_WRITE_1( "ConnectionStageAchievedL() Stage = %d ", aStage);
     if(iConnected) 
     {
     	// this is a connection closed event
        CLOG_WRITE( "ConnectionStageAchievedL() Stoping the connection instead of closing" );
        iConnection.Stop();
    	iConnected = EFalse;

    	if( !iSilentMode )
        // Temp fix for CDMA
        {
        	delete iCurrentAP;
        }
    	iCurrentAP = NULL;
     }
    
    }

// ---------------------------------------------------------
// CInternetConnectionManager::APItemFromAPIdLC
// ---------------------------------------------------------
//
CApAccessPointItem* CInternetConnectionManager::APItemFromAPIdLC( TUint32 aAPId )
	{
	CLOG_ENTERFN( "APItemFromAPIdLC" );
    CLOG_WRITE_1( "iapId: %d", aAPId );
	CApAccessPointItem* apItem = CApAccessPointItem::NewLC();

    if( iVpnEngine->IsVpnApL( aAPId ) )
        {
        delete iVpnItem;
        iVpnItem = NULL;

        iVpnItem = CVpnApItem::NewLC();
        CleanupStack::Pop();

        TRAP_IGNORE(iVpnEngine->VpnDataL( aAPId, *iVpnItem ));

        // get real WAP id
        if( NULL != iVpnItem )
          iVpnItem->ReadUint( EApVpnRealWapID, aAPId );
        }

    TRAP_IGNORE(iApDataHandler->AccessPointDataL( aAPId, *apItem ));

	return apItem;
	}

//-------------------------------------------------------------------
//CInternetConnectionManager::APItemFromIAPIdLC
//-------------------------------------------------------------------
//
CApAccessPointItem* CInternetConnectionManager::APItemFromIAPIdLC( TUint32 aIAPId )
	{
	CLOG_ENTERFN( "APItemFromIAPIdLC" );
	TUint32 wapAPId = ApUtils()->WapIdFromIapIdL( aIAPId );
	return APItemFromAPIdLC( wapAPId );
	}

//-------------------------------------------------------------------
//CInternetConnectionManager::ApDataHandler
//-------------------------------------------------------------------
//
CApDataHandler* CInternetConnectionManager::ApDataHandler()
    {
    return iApDataHandler;
    }

//------------------------------------------------------------------------
//CInternetConnectionManager::IsThereActiveCSDConnection
//------------------------------------------------------------------------
EXPORT_C TBool CInternetConnectionManager::IsThereActiveHSCSDConnectionL()
    {
    return EFalse;
    }

//------------------------------------------------------------------------
//CInternetConnectionManager::CheckIfAlreadyConnected
//------------------------------------------------------------------------
TBool CInternetConnectionManager::CheckIfAlreadyConnected( TUint32& aNewIapId )
    {
    CLOG_ENTERFN( "CheckIfAlreadyConnected" );

    TBool retVal( EFalse );
    RSocketServ serv;
    RConnection connection;
    TUint   count;

    if( serv.Connect() == KErrNone )
        {
        if( connection.Open( serv, KAfInet ) == KErrNone )
            {
            TUint32 reqId1( iRequestedAPIds.iFirstPreference );
    
            if( connection.EnumerateConnections( count ) == KErrNone )
                {
                TPckgBuf<TConnectionInfo> connInfo;

                CLOG_WRITE_1( "Conns: %d", count );

                for( TUint i = 1; i <= count; ++i )
                    {
                    connection.GetConnectionInfo( i, connInfo );

                    if( connInfo().iIapId == reqId1 )
                        {
                        CLOG_WRITE( "Found request1(Int)" );
                        aNewIapId = reqId1;
                        retVal = ETrue;
                        break;
                        }
                    }
                }

            connection.Close();
            }

        serv.Close();
        }

    CLOG_WRITE_1( "CheckIfAlreadyConnected(Int): %d", retVal );
    return retVal;
    }

//------------------------------------------------------------------------
//CInternetConnectionManager::Check3GNetworkL
//------------------------------------------------------------------------
TBool CInternetConnectionManager::Check3GNetworkL()
    {
    // This method must not be called if 2G/3G distinction is not supported;
    // the RMobilePhone-related classes are not open in that case.
    TBool is3g( EFalse );
#if !defined( __WINS__ )
    RMobilePhone::TMobilePhoneNetworkMode networkMode;
    User::LeaveIfError( iMobilePhone.GetCurrentMode( networkMode ) );

    CLOG_WRITE_1( "Network mode: %d", networkMode );

    if( networkMode == RMobilePhone::ENetworkModeCdma2000 ||
        networkMode == RMobilePhone::ENetworkModeWcdma )
        {
        is3g = ETrue;
        }
#endif
    return is3g;
    }

//------------------------------------------------------------------------
//CInternetConnectionManager::IsThereActiveVoiceCall
//------------------------------------------------------------------------
TBool CInternetConnectionManager::IsThereActiveVoiceCall()
    {
    return EFalse;
    }

//------------------------------------------------------------------------
//CInternetConnectionManager::SetInternalError
//------------------------------------------------------------------------
void CInternetConnectionManager::SetInternalError( TInt aInternalError )
    {
    iInternalError = aInternalError;
    }

//------------------------------------------------------------------------
//CInternetConnectionManager::OfflineModeChanged
//------------------------------------------------------------------------
void CInternetConnectionManager::OfflineModeChanged( TBool /*aAllowed*/ )
    {
    // This function is not used anymore
    }
    
//------------------------------------------------------------------------
//CInternetConnectionManager::CheckForWLAN
//------------------------------------------------------------------------
TBool CInternetConnectionManager::CheckForWLAN()
	{
    TBool IsWLANSupported(EFalse);

    TRAPD(err, FeatureManager::InitializeLibL());
    
    if(err == KErrNone && FeatureManager::FeatureSupported( KFeatureIdProtocolWlan ))
    	IsWLANSupported = ETrue;
    
    FeatureManager::UnInitializeLib();
	return IsWLANSupported;		
	}

//------------------------------------------------------------------------
//CInternetConnectionManager::IsConnectionAllowedL
//------------------------------------------------------------------------
TBool CInternetConnectionManager::IsConnectionAllowedL( TBool aDisplayError )
    {
    CLOG_ENTERFN("IsConnectionAllowedL");
    CLOG_WRITE_1("Disp: %d", aDisplayError );
    
    if( iRequestedAPIds.iFirstPreference )
        {
        if( BearerTypeL( iRequestedAPIds.iFirstPreference ) == EApBearerTypeWLAN )
            // WLAN is enabled in off-line mode, too.
            {
            return ETrue;
            }
        }
    else
        {
        // Always ask - enable connection because of WLAN
        TBool IsWLANSupported(ETrue);

#if !defined( __WINS__ )

		TInt OfflineModeConnAllowed( ETrue );

		iRepository->Get( KCoreAppUIsNetworkConnectionAllowed, OfflineModeConnAllowed);
		
		//Check whether WLAN is supported in offline mode
		if( !OfflineModeConnAllowed )
			{
				
		    IsWLANSupported = CheckForWLAN();

	        if(!IsWLANSupported && aDisplayError)
	        	DisplayConnErrorL();
			}
#endif
		return IsWLANSupported;
        }

    TInt connAllowed( ETrue );
    
    iRepository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed);
    
    if( !connAllowed && aDisplayError )
        {
        DisplayConnErrorL();        
        }
    
    return connAllowed;
    }

//------------------------------------------------------------------------
//CInternetConnectionManager::ChangeIapL
//------------------------------------------------------------------------
EXPORT_C void CInternetConnectionManager::ChangeIapL(  
                                         TConManChangeConn& aChangeConn,
                                         TUint32& aNewIap  )
    {
    CLOG_ENTERFN("ChangeIapL");
    
    CLOG_WRITE_2( "aNewAp: [%d], old: [%d]", aNewIap, iRequestedAPIds.iFirstPreference );
    
    aChangeConn = EConManDoNothing;

    if( !DoChangeIapL( aNewIap ) )
        {
        CLOG_WRITE("Do nothing1");
        return; // Do nothing
        }

    if( iRequestedAPIds.iFirstPreference == aNewIap && !iEasyWlan )
        // Do nothing if the the selected AP is the same as we
        // already have, but it's not EasyWlan.
        // In case of EasyWlan we always disconnects
        {
        CLOG_WRITE("Do nothing2");
        return; // Do nothing
        }
    else
        {
        TRAPD( err, CheckVPNL(aNewIap,aChangeConn) );
        CLOG_WRITE_2( "CheckVPNL - err: [%d], aChangeConn: [%d]", err, aChangeConn );
        
        if( err )
            {
            aChangeConn = EConManCloseAndStart;
            }
        }
    }

//------------------------------------------------------------------------
//CInternetConnectionManager::ShowConnectionChangedDlg
//------------------------------------------------------------------------
EXPORT_C void CInternetConnectionManager::ShowConnectionChangedDlg()
    {
    TRAP_IGNORE( DoShowConnectionChangedDlgL() );
    }
    
//------------------------------------------------------------------------
//CInternetConnectionManager::AskIap
//------------------------------------------------------------------------
EXPORT_C TInt CInternetConnectionManager::AskIap( TUint32& aNewIap )
    {
    CLOG_ENTERFN("AskIap");
    TInt retVal( KErrNone );
    RGenConAgentDialogServer dlgSv;

    retVal = dlgSv.Connect();
    if ( retVal == KErrNone ) 
        {
	    TConnectionPrefs prefs;
	    prefs.iRank = 1;
	    prefs.iDirection = ECommDbConnectionDirectionOutgoing;
	    prefs.iBearerSet = ECommDbBearerCSD | ECommDbBearerWcdma | ECommDbBearerVirtual | ECommDbBearerWLAN;

        TRequestStatus status;
        dlgSv.IapConnection( aNewIap, prefs, status );
        User::WaitForRequest( status );
        retVal = status.Int();
        }

    dlgSv.Close();

    return retVal;
    }

//-------------------------------------------------------------------
//CInternetConnectionManager::SetConnectionType
//-------------------------------------------------------------------
//    
EXPORT_C void CInternetConnectionManager::SetConnectionType( TCmSettingSelectionMode aConnectionType )
	{
	CLOG_ENTERFN("CInternetConnectionManager::SetConnectionType");
	CLOG_WRITE_1( "CInternetConnectionManager::SetConnectionType - %d", aConnectionType );
	iConnectionType = aConnectionType;
	}
    
//-------------------------------------------------------------------
//CInternetConnectionManager::SetRequestedSnap
//-------------------------------------------------------------------
//    
EXPORT_C void CInternetConnectionManager::SetRequestedSnap (TUint32 aRequestedSnapId)
	{
	CLOG_ENTERFN("CInternetConnectionManager::SetRequestedSnap");
	iRequestedSnapId = aRequestedSnapId;
	}
//------------------------------------------------------------------------
//CInternetConnectionManager::DoChangeIapL
//------------------------------------------------------------------------
TBool CInternetConnectionManager::DoChangeIapL( TUint32& aNewAp )
	{
	CLOG_ENTERFN("DoChangeIapL");
    
    
	TBool returnValue;
	TUint32 elementID;        
    
	TConnectionPrefs prefs;
    prefs.iRank = 1;
    prefs.iDirection = ECommDbConnectionDirectionOutgoing;
    prefs.iBearerSet = ECommDbBearerCSD | ECommDbBearerWcdma | ECommDbBearerWLAN |ECommDbBearerVirtual;
    
	TRequestStatus status;
	TUint32 dummy( 0 );

	RCmManagerExt cmManagerExt;
	cmManagerExt.OpenL();
    CleanupClosePushL( cmManagerExt );
    
    RCmDestinationExt cmDestExt;
    RCmConnectionMethodExt cmConnMethodExt;
		
	// Getting elementID in both Edestination and ConnectionMethod cases as new NW APIs require elementID as 1 of its parameter.
	if ( EDestination == iConnectionType ) 
		{ 
		cmDestExt = cmManagerExt.DestinationL( iRequestedSnapId ); 	
		elementID = cmDestExt.ElementId(); 
		cmDestExt.Close();
		} 
	else 
		{ 
		cmConnMethodExt = cmManagerExt.ConnectionMethodL( iRequestedAPIds.iFirstPreference );		
		CleanupClosePushL( cmConnMethodExt ); 
		elementID = cmConnMethodExt.GetIntAttributeL( ECmElementID ); 
		CleanupStack::PopAndDestroy(); //cmConnMethodExt
		} 

	RGenConAgentDialogServer dlgSv;
	User::LeaveIfError( dlgSv.Connect() );
	CleanupClosePushL( dlgSv );
	//Changed NW API which will take elementID of existing IAP and return elementID of selected IAP after selecting new connection from the list displayed
	dlgSv.AccessPointConnection( dummy, dummy, elementID, prefs.iBearerSet, status );
  User::WaitForRequest( status );
        
   if(KErrNone == status.Int())
	    {
	    
	    TMDBElementId tableId = elementID & KCDMaskShowRecordType;
	    TUint32 destId;
	    const TInt ID( 0 );
	    
	    /*If tableid is KCDTIdNetworkRecord then the result of ( elementID & KCDMaskShowRecordId ) >> 8 will be SnapId otherwise its IAP
		 This is dependent on User Selection, while change connection user gets a list of SNAPs available for changing connection, 
		 if user go into the options of a particular SNAP and selects a IAP
		 within that SNAP then the below condition will fall in else part and connection type/mode will change to EConnectionMethod*/
		 
	    if ( KCDTIdNetworkRecord == tableId )
	        {
	        destId = ( elementID & KCDMaskShowRecordId ) >> 8;
	        SetRequestedSnap( destId );							// if SnapId recovered then setting snapid for creating new Connection further
	        
	        cmDestExt = cmManagerExt.DestinationL( destId );
	        CleanupClosePushL( cmDestExt );

	        cmConnMethodExt = cmDestExt.ConnectionMethodL( ID );
	        
            CleanupClosePushL( cmConnMethodExt );
            aNewAp = cmConnMethodExt.GetIntAttributeL( ECmIapId );
            CleanupStack::PopAndDestroy();//cmConnMethodExt
	        CleanupStack::PopAndDestroy();//cmDestExt
                 
	        }
	     else
	     	{
	     	aNewAp = ( elementID & KCDMaskShowRecordId ) >> 8;
	     	/* if Iap recovered then now connection type/mode changed to EConnectionMethod because in this case
	        user has selected a IAP instead of SNAP.*/
	      	SetConnectionType( EConnectionMethod );
	     	}
	  	returnValue = ETrue;	    
	    }
	else
		{
		dlgSv.CancelAccessPointConnection();
		returnValue = EFalse;
		}
	    
    CLOG_WRITE_1( "New IAP: [%d]", aNewAp );
    CLOG_WRITE_1( "retVal: %d", returnValue );
    
    CleanupStack::PopAndDestroy();//dlgSv
	  CleanupStack::PopAndDestroy();//cmManagerExt
    return returnValue;
	}

//------------------------------------------------------------------------
//CInternetConnectionManager::CheckVPN
//------------------------------------------------------------------------
void CInternetConnectionManager::CheckVPNL( TUint32 aNewIap, 
                                            TConManChangeConn& aChangeConn )
    {
    CLOG_ENTERFN( "CheckVPNL" );
    
    TInt wap( 0 );
    wap = ApUtils()->WapIdFromIapIdL( aNewIap );
    if( iVpnEngine->IsVpnApL( wap ) )
        {
        delete iVpnItem;
        iVpnItem = NULL;

        iVpnItem = CVpnApItem::NewLC();
        CleanupStack::Pop();

        iVpnEngine->VpnDataL( aNewIap, *iVpnItem );
        
        // Fix for the TSW error JSIN-738BLA for making VPN work on changing connection.
        // get real IAP id
        iVpnItem->ReadUint( EApVpnRealIapID, aNewIap );      
        if( iRequestedAPIds.iFirstPreference == aNewIap )
            {
            aChangeConn = EConManStartAgain;
            }
        else
            {
            aChangeConn = EConManCloseAndStart;
            }    
        }
    else
        {
        aChangeConn = EConManCloseAndStart;
        }
    }
	
//------------------------------------------------------------------------
//CInternetConnectionManager::UpdateEasyWlanFlagL
//------------------------------------------------------------------------
void CInternetConnectionManager::UpdateEasyWlanFlagL()
    {
    CLOG_ENTERFN( "UpdateEasyWlanFlagL" );

    iEasyWlan = EFalse;
    
    if( iSilentMode )
        // This is always CDMA
        {
        return;
        }
    
    __ASSERT_DEBUG( iCommsDb, User::Panic( KNullDesC, KErrNone ) );

    TUint32 serviceId;
    
    CLOG_WRITE( "Getting servideId" );
    User::LeaveIfError( iCurrentAP->ReadUint( EApIapServiceId, serviceId ) );
    
    CCommsDbTableView* wLanServiceTable = NULL;
        
    CLOG_WRITE( "Opening view" );
    wLanServiceTable = iCommsDb->OpenViewMatchingUintLC( 
                                            TPtrC( WLAN_SERVICE ),
                                            TPtrC( WLAN_SERVICE_ID ),
                                            serviceId );

    CLOG_WRITE_1( "ServiceId: %d", serviceId );

    TInt errorCode = wLanServiceTable->GotoFirstRecord();

    CLOG_WRITE_1( "GotoFirstRecord returned %d", errorCode );
    TWlanSsid sSID;                                

    if ( errorCode == KErrNone )
        {
        wLanServiceTable->ReadTextL( TPtrC( WLAN_SSID ), sSID );
        CLOG_WRITE8_1( "SSID: [%S]", &sSID );
        // this is Easy WLan if string is empty.
        iEasyWlan = sSID.Length() == 0;
        CLOG_WRITE_1( "iEasyWlan: %d", iEasyWlan );
        }

    CleanupStack::PopAndDestroy();  // wLanServiceTable
    }

//------------------------------------------------------------------------
//CInternetConnectionManager::DoShowConnectionChangedDlgL
//------------------------------------------------------------------------
void CInternetConnectionManager::DoShowConnectionChangedDlgL()
    {
    CConnectionUiUtilities* connUiUtils = CConnectionUiUtilities::NewL();
    CleanupStack::PushL( connUiUtils );

    connUiUtils->ShowConnectionChangedNoteL( iRequestedAPIds.iFirstPreference );
    
    CleanupStack::PopAndDestroy( connUiUtils ); 
    }

//------------------------------------------------------------------------
//CInternetConnectionManager::InitializeL
//------------------------------------------------------------------------
void CInternetConnectionManager::InitializeL()
    {
    CLOG_ENTERFN( "InitializeL" );
    if( iInitialized )
        {
        return;
        }
        
	//Constructing commsdb
	if ( !iCommsDb )
		{
		iCommsDb = CCommsDatabase::NewL( EDatabaseTypeIAP );
		iCommsDbOwned = ETrue;
		}
		
    if( !iNote )
        {
    	iNote = CAknGlobalNote::NewL( );
    	iNote->SetPriority( CActive::EPriorityHigh );
        }
       
    if( !iRFs.Handle() )
        {
    	User::LeaveIfError( iRFs.Connect() );
        }
        
	if( !iSilentMode && !iResOffset )
		{
		TFileName fileName;
        TParse parse;
        
        Dll::FileName (fileName);
        parse.Set(KConnectionResourceFile, &fileName, NULL);
        fileName = parse.FullName();
        BaflUtils::NearestLanguageFile(CCoeEnv::Static()->FsSession(), fileName);
        if ( !BaflUtils::FileExists( CCoeEnv::Static()->FsSession(), fileName ) )        
            {
            // Use the Z drive one
            fileName.Format( KConnectionResourceFileFormatter, &KDC_RESOURCE_FILES_DIR );
            BaflUtils::NearestLanguageFile( CCoeEnv::Static()->FsSession(),fileName );
            }
      	iResOffset = CCoeEnv::Static()->AddResourceFileL( fileName );
		}
	
	if( !iApUtils )
	    {
        iApUtils = CApUtils::NewLC( *iCommsDb );
    	CleanupStack::Pop();
	    }
	    
	if( !iApDataHandler )
	    {
    	iApDataHandler = CApDataHandler::NewLC( *iCommsDb );
    	CleanupStack::Pop();
	    }

    if( !iVpnEngine )
        {
        iVpnEngine = CVpnApEngine::NewLC( iCommsDb );
        CleanupStack::Pop();
        }

    if( !iSyncConnector )
        {
        iSyncConnector = CActiveConnectorSyncWrapper::NewL( iConnection );
        CLOG_ATTACH( iSyncConnector, this );
        }

    if( !iRepository )
        {
    	iRepository = CRepository::NewL( KCRUidCoreApplicationUIs );
        }

    if( !iTelServer.Handle() )	
        {
        // this initialization needed for identifying 3G networks
        //
        User::LeaveIfError( iTelServer.Connect() );
        User::LeaveIfError( iTelServer.LoadPhoneModule( KMmTsyModuleName ) );

        TInt numPhones;
        User::LeaveIfError( iTelServer.EnumeratePhones( numPhones ) );
        if( numPhones <= 0 )
            {
            // Huh???
            User::Leave( KErrCorrupt );
            }

        RTelServer::TPhoneInfo phoneInfo;
        User::LeaveIfError( iTelServer.GetPhoneInfo( 0, phoneInfo ) );
        User::LeaveIfError( iMobilePhone.Open( iTelServer, phoneInfo.iName ) );
        User::LeaveIfError( iMobilePhone.Initialise() );
        }

    if( !iStageNotifier )
        {
        iStageNotifier = CConnectionStageNotifierWCB::NewL();
        }

    if( !iServ.Handle() )
        {
        User::LeaveIfError( iServ.Connect() );
        }

    CLOG_WRITE( "Fully initialized" );
    iInitialized = ETrue;    
    }
    
//------------------------------------------------------------------------
//CInternetConnectionManager::ConnectWithSnapId
//------------------------------------------------------------------------    
TInt CInternetConnectionManager::ConnectWithSnapId(TUint32 aRequestedSnapId)
	{

    CLOG_WRITE_1( "CInternetConnectionManager::ConnectWithSnapId - %d", aRequestedSnapId );

	iInternalError = KErrNone;
	iNewConnectionEstablished = EFalse;
	TInt connErr( KErrNone );

	connErr = ConnectSnapWithoutCheckL( aRequestedSnapId );
	
    DisplayConnErrorL();
#ifndef __WINS__
    if( KErrNone == connErr )
        {
        TUint32 iIapID;
        TBuf<20> query;
        query.Format( _L("%s\\%s"), IAP, COMMDB_ID );
        if( iConnection.GetIntSetting( query, iIapID ) == KErrNone )
            {
            CLOG_WRITE_1( "ConnectWithSnapId::AccessPoint - %d", iIapID );
            CApAccessPointItem* ap = APItemFromIAPIdLC( iIapID );
            UpdateCurrentAPL( *ap, EFalse );
            CleanupStack::PopAndDestroy();  // ap
            }
        }
            
#endif

	return connErr;
	
	}

//------------------------------------------------------------------------
//CInternetConnectionManager::ConnectSnapWithoutCheckL
//------------------------------------------------------------------------    

    
 TInt CInternetConnectionManager::ConnectSnapWithoutCheckL(TUint32 aRequestedSnapId)
 	{
 	    CLOG_ENTERFN( "ConnectSnapWithoutCheckL" );

	iInternalError = KErrNone;
	TInt connErr;
	//Do the connection
    TConnSnapPref pref;
			   
    pref.SetSnap(aRequestedSnapId);
          
   	connErr = ConnectSnapL( pref );

    if( connErr == KErrNone )
        {
        iConnected = ETrue;

        // Enable connection cloning        
        _LIT_SECURITY_POLICY_C2(KProcPolicy1, ECapabilityNetworkServices, ECapabilityNetworkControl);

        TSecurityPolicyBuf secPolBuf;
        secPolBuf().Set(KProcPolicy1().Package());

        iConnection.Control(KCOLConnection, KCoEnableCloneOpen, secPolBuf);

        StartConnectionObservingL();
        }

	return connErr;	
 	}

//------------------------------------------------------------------------
//CInternetConnectionManager::ConnectSnapL
//------------------------------------------------------------------------    
 	
 	TInt CInternetConnectionManager::ConnectSnapL
	(
    TConnSnapPref& aPref1
	)
	{
	if ( iAlreadyInUse )
		{
		CLOG_WRITE( "Reentering happend, calling User::Leave!!!" );
		User::Leave( KErrAlreadyExists );
		}
	iAlreadyInUse = ETrue;
	iInternalError = KErrNone;
	TInt connErr( KErrNone );
	
	CLOG_WRITE( "ConnMan: Setting overides" );
		//Leave point
    TConnSnapPref *overrides = new (ELeave) TConnSnapPref;
	CleanupStack::PushL( overrides );

    // set access snap id in overrides
    overrides->SetSnap( aPref1.Snap() );

	CLOG_WRITE( "ConnMan Overrides set"  );

	if ( !connErr )
		{
        connErr = iConnection.Open( iServ, KAfInet );
        CLOG_WRITE_1( "RConnection: %d", connErr );
        if( connErr == KErrNone )
            {
           //connect with snap id
            connErr = iSyncConnector->ConnectSnap( overrides );
            }
        if( connErr != KErrNone )
            {
            CLOG_WRITE( "Closing all" );
            iConnection.Close();
            }
		}
	
	CleanupStack::PopAndDestroy();//overrides
    iInternalError = connErr;

	CLOG_WRITE_1( "ConnectSnapL returned with %d", connErr );
	iNewConnectionEstablished = ( connErr == KErrNone );
	iAlreadyInUse = EFalse;
	return connErr;
	}

//---------------------------------------------------------------
//CInternetConnectionManager::StartWaitDialogL
//---------------------------------------------------------------
void CInternetConnectionManager::StartWaitDialogL( HBufC* aLabel, TWaitIconType aType )
    {
    delete iWaitDialog;
    
    iWaitDialog = NULL;
    iWaitDialog = new ( ELeave )CAknWaitDialog( REINTERPRET_CAST(CEikDialog**,&iWaitDialog), ETrue );
    
    switch( aType )
        {
            case EWaitDataIcon:
                iWaitDialog->PrepareLC( R_CONNMAN_WAIT_NOTE_CSD );
                break;
            case EWaitGPRSIcon:
                iWaitDialog->PrepareLC( R_CONNMAN_WAIT_NOTE_GPRS );
                break;
            case EWaitNoIcon:
            default:
                iWaitDialog->PrepareLC( R_CONNMAN_WAIT_NOTE );
        }
    
    iWaitDialog->SetCallback( this );
    if( aLabel )
        {
        iWaitDialog->SetTextL( aLabel->Des() );
        }
    iWaitDialog->RunLD();
    }
    
// End of File
