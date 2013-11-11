/*
* Copyright (c) 2008-2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
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

// [INCLUDE FILES] - do not remove
#include <e32svr.h>
#include <StifParser.h>
#include <ecom/ecom.h>
#include <StifTestInterface.h>
#include "usbuinotifapitest.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES  

// CONSTANTS
#ifdef __WINS__
_LIT( KUsbAppProcPattern, "usb[*" );
#else
_LIT( KUsbAppProcPattern, "#USBSettingsApp*" );
#endif

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CUSBUiNotifApiTest::Delete
// Delete here all resources allocated and opened from test methods. 
// Called from destructor. 
// -----------------------------------------------------------------------------
//
void CUSBUiNotifApiTest::Delete() 
    {
    iNotifier -> Close();
    delete iNotifier;
    iNotifier = NULL;
    }

// -----------------------------------------------------------------------------
// CUSBUiNotifApiTest::RunMethodL
// Run specified method. Contains also table of test mothods and their names.
// -----------------------------------------------------------------------------
//
TInt CUSBUiNotifApiTest::RunMethodL( 
    CStifItemParser& aItem ) 
    {

    static TStifFunctionInfo const KFunctions[] =
        {  
        // Copy this line for every implemented function.
        // First string is the function name used in TestScripter script file.
        // Second is the actual implementation member function. 
        ENTRY( "CableConnectedNotifierTest", CUSBUiNotifApiTest::CableConnectedNotifierTest ),
        ENTRY( "FinishCableConnectedQuery", CUSBUiNotifApiTest::FinishCableConnectedQuery ),        
        ENTRY( "UsbQueriesNotifierTest", CUSBUiNotifApiTest::UsbQueriesNotifierTest ),
        ENTRY( "FinishQuery", CUSBUiNotifApiTest::FinishQuery ),
        ENTRY( "UsbOTGErrorNotifierTests", CUSBUiNotifApiTest::UsbOTGErrorNotifierTests ), 
        ENTRY( "UsbOTGWarningNotifierTests", CUSBUiNotifApiTest::UsbOTGWarningNotifierTests ),
        ENTRY( "LoadNotifiers", CUSBUiNotifApiTest::LoadNotifiersL ),
        ENTRY( "UnLoadNotifiers", CUSBUiNotifApiTest::UnLoadNotifiers ),
        ENTRY( "UsbMSMMNotifierTests", CUSBUiNotifApiTest::UsbMSMMNotifierTests ),
        ENTRY( "CancelMsmmNotifier", CUSBUiNotifApiTest::CancelMsmmNotifier ),
        ENTRY( "CancelQueryNotifier", CUSBUiNotifApiTest::CancelQueryNotifier ),
        ENTRY( "CancelOtgErrorNotifier", CUSBUiNotifApiTest::CancelOtgErrorNotifier ),
        ENTRY( "CancelOtgWarningNotifier", CUSBUiNotifApiTest::CancelOtgWarningNotifier ),
        ENTRY( "CancelCableConnectedNotifier", CUSBUiNotifApiTest::CancelCableConnectedNotifier ),
        ENTRY( "WaitForRequest", CUSBUiNotifApiTest::WaitForRequest ),
        ENTRY( "SynchStart", CUSBUiNotifApiTest::SynchStart ),	
        ENTRY( "Update", CUSBUiNotifApiTest::Update )
        //ADD NEW ENTRY HERE
        // [test cases entries] - Do not remove

        };

    const TInt count = sizeof( KFunctions ) / 
                        sizeof( TStifFunctionInfo );

    return RunInternalL( KFunctions, count, aItem );

    }

// -----------------------------------------------------------------------------
// CUSBUiNotifApiTest::CableConnectedNotifierTests
// -----------------------------------------------------------------------------

TInt CUSBUiNotifApiTest::CableConnectedNotifierTest( CStifItemParser& /*aItem*/ )
    {
    TInt res;
    TPtrC8 dummyPckg; //no parameters used
    
    res = iNotifier -> Connect();
    if ( res != KErrNone )
        {
        iLog -> Log( _L("Failed to connect RNotifier with value: %d"), res );
        return res;
        }   
    iLog -> Log( _L("RNotifier connected") );

    //kill the possibly running usb application
    FindAndKillProcess(KUsbAppProcPattern);
    
    iNotifier -> StartNotifierAndGetResponse( iReqStatus, KCableConnectedNotifierUid, 
            dummyPckg, iConnectionNotifierResponseParamsPckg );    
    
    return KErrNone;
    
    }


TInt CUSBUiNotifApiTest::FinishCableConnectedQuery( CStifItemParser& aItem )
    {
    //give time for application to start, in microseconds
    const TInt KAppLaunchDelay = 5000000; 
    TInt res;
    TPtrC optionName( KNullDesC );
    TTestOption option;
    TPtrC expectedUsbPersonalityName( KNullDesC );
    TInt expectedErrorCode;

    iLog -> Log( _L("FinishCableConnectedQuery") );
    
    res = aItem.GetString( _L( "FinishCableConnectedQuery" ), optionName );
        
    if ( res != KErrNone )
        {
        iLog -> Log( _L("GetString failed with value: %d"), res );
        iNotifier -> Close();
        return res;
        }
    
    iLog -> Log( _L("FinishCableConnectedQuery getting option") );
    res = GetTestOption( optionName, option );
    if ( res != KErrNone )
        {
        iLog -> Log( _L("GetTestOption failed with value: %d"), res );
        iUsbWatcher -> Close();
        iNotifier -> Close();
        return res;
        }
 
    iLog -> Log( _L("FinishCableConnectedQuery wait for request") );
    User::WaitForRequest( iReqStatus );
    expectedErrorCode = iReqStatus.Int();

    iLog -> Log( _L("FinishCableConnectedQuery request complete") );
    
    iNotifier -> Close();  

    iLog -> Log( _L("FinishCableConnectedQuery close") );
    
    TInt ret = KErrNone;
    switch( option )
        {
        case EQueryAccepted:
            iLog -> Log( _L("EQueryAccepted"));
            User::After(KAppLaunchDelay); //let the app start and let the tester person to see that
            ret = FindAndKillProcess(KUsbAppProcPattern);
            if (ret != KErrNone)
                {
                iLog -> Log( _L("Process start failed: %d"), ret );
                return ret;
                }
            iLog -> Log( _L("Request status value: %d, expected: %d"), expectedErrorCode, KErrCancel );
            //the notifier returns KErrCancel when clicked
            STIF_ASSERT_EQUALS( KErrCancel, expectedErrorCode );
            break;
        case EQueryCanceled:
            iLog -> Log( _L("FinishCableConnectedQuery canceled") );
            iLog -> Log( _L("Request status value: %d, expected: %d"), expectedErrorCode, KErrCancel );
            STIF_ASSERT_EQUALS( KErrCancel, expectedErrorCode );
            break;
        default:
            iLog -> Log( _L("FinishCableConnectedQuery default - not found") );
            return KErrNotFound;
        }

    iLog -> Log( _L("Test case passed!") );
    return KErrNone;
    }


// -----------------------------------------------------------------------------
// CBtNotifApiTest::UsbQueriesNotifierTests
// -----------------------------------------------------------------------------

TInt CUSBUiNotifApiTest::UsbQueriesNotifierTest( CStifItemParser& aItem )
    {
    TInt res;    
    TPtrC usbQueryName( KNullDesC );
    TUSBUIQueries usbQueryType;
    
    res = aItem.GetString( _L( "UsbQueriesNotifierTest" ), usbQueryName );   
    if ( res != KErrNone )
        {
        iLog -> Log( _L("GetString failed with value: %d"), res );
        iNotifier -> Close();
        return res;
        }
    
    res = GetQueryType( usbQueryName, usbQueryType );
    if ( res != KErrNone )
        {
        iLog -> Log( _L("GetQueryType failed with value: %d"), res );
        return res;
        }
    
    TUSBQueriesNotiferParams notifierParams;
    notifierParams.iDrive = 'c';
    notifierParams.iQuery = usbQueryType;
    TUSBQueriesNotifierParamsPckg notifierParamsPckg( notifierParams );
    TPckgBuf<TBool> output;
    
    if (!iNotifierConnected)
        {
        res = iNotifier -> Connect();
        iNotifierConnected = ETrue;
        }
    if ( res != KErrNone )
        {
        iLog -> Log( _L("Failed to connect RNotifier with value: %d"), res );
        return res;
        }   
    iLog -> Log( _L("RNotifier connected") );
    
    switch( usbQueryType )
        {
        case EUSBNoMemoryCard:
            iCompleteQuery = ETrue;
            break;
        case EUSBStorageMediaFailure:
        case EUSBChangeFromMassStorage:
            iCompleteQuery = EFalse;
            break;
        default:
            break; 
        }
    
    iNotifier -> StartNotifierAndGetResponse( iReqStatus, KQueriesNotifier, 
            notifierParamsPckg, output ); 
    iLog -> Log( _L("StartNotifierAndGetResponse ready") );
    
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CBtNotifApiTest::UsbOTGErrorNotifierTests
// -----------------------------------------------------------------------------

TInt CUSBUiNotifApiTest::UsbOTGErrorNotifierTests( CStifItemParser& aItem )
    {
    TInt res;    
    TPtrC usbQueryName( KNullDesC );
    TUsbUiNotifOtgError usbOTGErrorType;
    
    res = aItem.GetString( _L( "UsbOTGErrorNotifierTests" ), usbQueryName );   
    if ( res != KErrNone )
        {
        iLog -> Log( _L("GetString failed with value: %d"), res );
        iNotifier -> Close();
        return res;
        }
    
    res = GetOTGErrorType( usbQueryName, usbOTGErrorType );
    if ( res != KErrNone )
        {
        iLog -> Log( _L("usbOTGErrorType failed with value: %d"), res );
        return res;
        }    
    
    if (!iNotifierConnected)
        {
        res = iNotifier -> Connect();
        iNotifierConnected = ETrue;
        }
    if ( res != KErrNone )
        {
        iLog -> Log( _L("Failed to connect RNotifier with value: %d"), res );
        return res;
        }   
    iLog -> Log( _L("RNotifier connected") );
    
    TPckgBuf<TInt> notifierParamsPckg;
    notifierParamsPckg() = usbOTGErrorType;
    iCompleteQuery = EFalse;

    iNotifier -> StartNotifierAndGetResponse( iReqStatus, 
            KUsbUiNotifOtgError, notifierParamsPckg, iRes ); 
    
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBtNotifApiTest::UsbOTGErrorNotifierTests
// -----------------------------------------------------------------------------

TInt CUSBUiNotifApiTest::UsbOTGWarningNotifierTests( CStifItemParser& aItem )
    {        
    TInt res;    
    TPtrC usbQueryName( KNullDesC );
    TUsbUiNotifOtgWarning usbOTGWarningType;
    
    res = aItem.GetString( _L( "UsbOTGWarningNotifierTests" ), usbQueryName );   
    if ( res != KErrNone )
        {
        iLog -> Log( _L("GetString failed with value: %d"), res );
        iNotifier -> Close();
        return res;
        }
    
    res = GetOTGWarningType( usbQueryName, usbOTGWarningType );
    if ( res != KErrNone )
        {
        iLog -> Log( _L("usbOTGErrorType failed with value: %d"), res );
        return res;
        }    
    
    if (!iNotifierConnected)
        {
        res = iNotifier -> Connect();
        iNotifierConnected = ETrue;
        }
    if ( res != KErrNone )
        {
        iLog -> Log( _L("Failed to connect RNotifier with value: %d"), res );
        return res;
        }   
    iLog -> Log( _L("RNotifier connected") );
    
    TPckgBuf<TInt> notifierParamsPckg;
    notifierParamsPckg() = usbOTGWarningType;
    iCompleteQuery = EFalse;

    iNotifier -> StartNotifierAndGetResponse( iReqStatus, 
            KUsbUiNotifOtgWarning, notifierParamsPckg, iRes ); 
    
    return KErrNone;    
    }

// -----------------------------------------------------------------------------
// CBtNotifApiTest::UsbMSMMNotifierTests
// -----------------------------------------------------------------------------

TInt CUSBUiNotifApiTest::UsbMSMMNotifierTests( CStifItemParser& aItem )
    {        
    TInt res;    
    TPtrC usbQueryName( KNullDesC );
    THostMsErrCode usbMSMMNErrorType;
    
    iLog -> Log( _L("------msmm------") );
    
    res = aItem.GetString( _L( "UsbMSMMNotifierTests" ), usbQueryName );   
    if ( res != KErrNone )
        {
        iLog -> Log( _L("GetString failed with value: %d"), res );
        iNotifier -> Close();
        return res;
        }
    
    res = GetMSMMrrorType( usbQueryName, usbMSMMNErrorType );
    if ( res != KErrNone )
        {
        iLog -> Log( _L("MSMMrrorType failed with value: %d"), res );
        return res;
        }    
    iErrData.iError=usbMSMMNErrorType;
    iErrPckg = iErrData;
    if (!iNotifierConnected)
        {
        res = iNotifier -> Connect();
        iNotifierConnected = ETrue;
        }
    if ( res != KErrNone )
        {
        iLog -> Log( _L("Failed to connect RNotifier with value: %d"), res );
        return res;
        }   
    iLog -> Log( _L("RNotifier connected") );
    
    TPckgBuf<TInt> notifierParamsPckg;
    notifierParamsPckg() = usbMSMMNErrorType;
    iCompleteQuery = EFalse;

    iNotifier -> StartNotifierAndGetResponse( iReqStatus, KUsbUiNotifMsmmError, iErrPckg, iRes ); 
    
    return KErrNone;    
    }


TInt CUSBUiNotifApiTest::CancelMsmmNotifier( CStifItemParser& /*aItem*/ ) 
    {
    return ( iNotifier->CancelNotifier(KUsbUiNotifMsmmError) );
    }


TInt CUSBUiNotifApiTest::CancelQueryNotifier( CStifItemParser& /*aItem*/ ) 
    {
    iLog -> Log( _L("CancelQueryNotifier") );
    return ( iNotifier->CancelNotifier(KQueriesNotifier) );
    }


TInt CUSBUiNotifApiTest::CancelOtgErrorNotifier( CStifItemParser& /*aItem*/ ) 
    {
    return ( iNotifier->CancelNotifier(KUsbUiNotifOtgError) );
    }

TInt CUSBUiNotifApiTest::CancelOtgWarningNotifier( CStifItemParser& /*aItem*/ ) 
    {
    return ( iNotifier->CancelNotifier(KUsbUiNotifOtgWarning) );
    }

TInt CUSBUiNotifApiTest::CancelCableConnectedNotifier( CStifItemParser& /*aItem*/ ) 
    {
    return ( iNotifier->CancelNotifier(KCableConnectedNotifierUid) );
    }

TInt CUSBUiNotifApiTest::WaitForRequest( CStifItemParser& /*aItem*/ ) 
    {
    User::WaitForRequest( iReqStatus );
    return KErrNone;
    }

TInt CUSBUiNotifApiTest::SynchStart(CStifItemParser& aItem )
{
    TInt res;    
    TPtrC usbQueryName( KNullDesC );
    TUsbUiNotifOtgWarning usbOTGWarningType;
    
    res = aItem.GetString( _L( "SynchStart" ), usbQueryName );   
    if ( res != KErrNone )
        {
        iLog -> Log( _L("GetString failed with value: %d"), res );
        iNotifier -> Close();
        return res;
        }
    
    res = GetOTGWarningType( usbQueryName, usbOTGWarningType );
    if ( res != KErrNone )
        {
        iLog -> Log( _L("usbOTGErrorType failed with value: %d"), res );
        return res;
        }    
 
 
    res = iNotifier -> Connect();
    if ( res != KErrNone )
        {
        iLog -> Log( _L("Failed to connect RNotifier with value: %d"), res );
        return res;
        }   
    iLog -> Log( _L("RNotifier connected") );
 
    TPckgBuf<TInt> notifierParamsPckg;
    notifierParamsPckg() = usbOTGWarningType;
    iCompleteQuery = EFalse;

    TInt retVal = iNotifier -> StartNotifier( KUsbUiNotifOtgWarning, notifierParamsPckg );
    iLog -> Log( _L("StartNotifier returned with value: %d"), retVal );
    
    iNotifier->Close();
    return retVal;    
}

TInt CUSBUiNotifApiTest::Update(CStifItemParser& /*aItem*/ )
{
    //delay in microseconds before updating the notifier
    const TInt KUpdateDelay = 2000000;
    TInt res;    
    TPtrC usbQueryName( KNullDesC );
    
    res = iNotifier -> Connect();
    if ( res != KErrNone )
        {
        iLog -> Log( _L("Failed to connect RNotifier with value: %d"), res );
        return res;
        }   
    iLog -> Log( _L("RNotifier connected") );
 
    TPckgBuf<TInt> notifierParamsPckg;
    notifierParamsPckg() = EUsbOtgUnsupportedDevice;
    iCompleteQuery = EFalse;
    TPckgBuf<TInt> response ;
    iNotifier -> StartNotifierAndGetResponse( iReqStatus, KUsbUiNotifOtgError, 
            notifierParamsPckg, iRes );
		
    User::After(KUpdateDelay); //simulate update after a delay
    TPckgBuf<TInt> notifierParamsPckgUpdate;
    notifierParamsPckgUpdate() = EUsbOtgErrorNoMemory;    
  	TInt retVal = iNotifier -> UpdateNotifier( KUsbUiNotifOtgError, 
  	        notifierParamsPckgUpdate, response);
  	
    iNotifier->Close();  	
    return retVal;    
}


TInt CUSBUiNotifApiTest::FinishQuery( CStifItemParser& aItem )
    {
    iLog -> Log( _L("FinishQuery") );
    TInt res;
    TPtrC optionName( KNullDesC );
    TTestOption option;
    
    if ( iCompleteQuery )
        {
        iLog -> Log( _L("FinishQuery completing request") );
        TRequestStatus* statPtr = &iReqStatus;
        User::RequestComplete( statPtr, KErrNone );
        }
    
    iLog -> Log( _L("FinishQuery getting options") );
    res = aItem.GetString( _L( "FinishQuery" ), optionName );   
    if ( res != KErrNone )
        {
        iLog -> Log( _L("GetString failed with value: %d"), res );
        iNotifier -> Close();
        return res;
        }
    
    res = GetTestOption( optionName, option );
    if ( res != KErrNone )
        {
        iLog -> Log( _L("GetTestOption failed with value: %d"), res );
        iNotifier -> Close();
        return res;
        }
    
    iLog -> Log( _L("FinishQuery waiting for request") );
    User::WaitForRequest( iReqStatus );
    iLog -> Log( _L("FinishQuery closing notifier") );
    iNotifier -> Close();
    iNotifierConnected = EFalse;
    
    switch( option )
        {
        case EQueryAccepted:
            iLog -> Log( _L("Request status value: %d, expected: %d"), iReqStatus.Int(), KErrNone );
            STIF_ASSERT_EQUALS( KErrNone, iReqStatus.Int() );
            break;
        case EQueryCanceled:
            iLog -> Log( _L("Request status value: %d, expected: %d"), iReqStatus.Int(), KErrCancel );
            STIF_ASSERT_EQUALS( KErrCancel, iReqStatus.Int() );
            break;
        default:
            return KErrNotFound;
        }
    
    iLog -> Log( _L("Test case passed!") );
    return KErrNone;
    }

TInt CUSBUiNotifApiTest::LoadNotifiersL( CStifItemParser& /*aItem*/ )
    {
    iNotifierArray = reinterpret_cast<CArrayPtr<MEikSrvNotifierBase2>*>(
												REComSession::CreateImplementationL(TUid::Uid(0x10281F23), iInstanceUid));
	return KErrNone;	
    }
    
TInt CUSBUiNotifApiTest::UnLoadNotifiers ( CStifItemParser& /*aItem*/ )
    {
    TInt count = iNotifierArray->Count();
    TInt i=0;
    for (i=0; i<count; i++)
        {
        iNotifierArray->At(i)->Release();        
        }
    iNotifierArray->Reset();    
    delete iNotifierArray;
    iNotifierArray = NULL;
    REComSession::DestroyedImplementation(iInstanceUid);    
    return KErrNone;
    }
                       
TInt CUSBUiNotifApiTest::GetQueryType( TPtrC aTypeString, TUSBUIQueries& aQueryType )
    {
    if ( !aTypeString.Compare( _L( "EUSBNoMemoryCard" ) ) )
        {
        aQueryType = EUSBNoMemoryCard;
        iLog -> Log( _L("Query type: EUSBNoMemoryCard") );
        }
    else if ( !aTypeString.Compare( _L( "EUSBStorageMediaFailure" ) ) )
        {
        aQueryType = EUSBStorageMediaFailure;
        iLog -> Log( _L("Query type: EUSBStorageMediaFailure") );
        }
    else if ( !aTypeString.Compare( _L( "EUSBDiskFull" ) ) )
        {
        aQueryType = EUSBDiskFull;
        iLog -> Log( _L("Query type: EUSBDiskFull") );
        }    
    else if ( !aTypeString.Compare( _L( "EUSBNotEnoughRam" ) ) )
        {
        aQueryType = EUSBNotEnoughRam;
        iLog -> Log( _L("Query type: EUSBNotEnoughRam") );
        }    
    else
        {
        iLog -> Log( _L("Query type: not supported") );
        return KErrNotFound;
        }
 
    return KErrNone;
    }

TInt CUSBUiNotifApiTest::GetOTGErrorType( TPtrC aTypeString, TUsbUiNotifOtgError& aQueryType )
    {
    if ( !aTypeString.Compare( _L( "EUsbOtgTooMuchPower" ) ) )
        {
        aQueryType = EUsbOtgTooMuchPower;
        iLog -> Log( _L("Query type: EUsbOtgTooMuchPower") );
        }    
    else if ( !aTypeString.Compare( _L( "EUsbOtgTooMuchPowerRequired" ) ) )
        {
        aQueryType = EUsbOtgTooMuchPowerRequired;
        iLog -> Log( _L("Query type: EUsbOtgTooMuchPowerRequired") );
        }    
    else if ( !aTypeString.Compare( _L( "EUsbOtgUnsupportedDevice" ) ) )
        {
        aQueryType = EUsbOtgUnsupportedDevice;
        iLog -> Log( _L("Query type: EUsbOtgUnsupportedDevice") );
        }        
    else if ( !aTypeString.Compare( _L( "EUsbOtgHubUnsupported" ) ) )
        {
        aQueryType = EUsbOtgHubUnsupported;
        iLog -> Log( _L("Query type: EUsbOtgHubUnsupported") );
        }            
    else if ( !aTypeString.Compare( _L( "EUsbOtgErrorInConnection" ) ) )
        {
        aQueryType = EUsbOtgErrorInConnection;
        iLog -> Log( _L("Query type: EUsbOtgErrorInConnection") );
        }                
    else if ( !aTypeString.Compare( _L( "EUsbOtgErrorAttachTimedOut" ) ) )
        {
        aQueryType = EUsbOtgErrorAttachTimedOut;
        iLog -> Log( _L("Query type: EUsbOtgErrorAttachTimedOut") );
        }                    
    else if ( !aTypeString.Compare( _L( "EUsbOtgErrorNoMemory" ) ) )
        {
        aQueryType = EUsbOtgErrorNoMemory;
        iLog -> Log( _L("Query type: EUsbOtgErrorNoMemory") );
        }                    
    else
        {
        iLog -> Log( _L("Query type: not supported") );
        return KErrNotFound;
        } 
    return KErrNone;    
    }


TInt CUSBUiNotifApiTest::GetOTGWarningType( TPtrC aTypeString, TUsbUiNotifOtgWarning& aQueryType )
    {        
    if ( !aTypeString.Compare( _L( "EUsbOtgPartiallySupportedDevice" ) ) )
        {            
        aQueryType = EUsbOtgPartiallySupportedDevice;
        iLog -> Log( _L("Query type: EUsbOtgPartiallySupportedDevice;") );
        }    
    else
        {
        iLog -> Log( _L("Query type: not supported") );
        return KErrNotFound;
        }
 
    return KErrNone;
    }

TInt CUSBUiNotifApiTest::GetMSMMrrorType( TPtrC aTypeString, THostMsErrCode& aQueryType )
    {        
    if ( !aTypeString.Compare( _L( "EUsbMSMMGeneralError" ) ) )
        {            
        aQueryType = EHostMsErrGeneral;
        iLog -> Log( _L("Query type: EUsbOtgPartiallySupportedDevice;") );
        }    
    else if ( !aTypeString.Compare( _L( "EUsbMSMMUnknownFileSystem" ) ) )
        {
        aQueryType = EHostMsErrUnknownFileSystem;
        iLog -> Log( _L("Query type: EUsbMSMMUnknownFileSystem") );
        }         
    else if ( !aTypeString.Compare( _L( "EUsbMSMMOutOfMemory" ) ) )
        {
        aQueryType = EHostMsErrOutOfMemory;
        iLog -> Log( _L("Query type: EUsbMSMMOutOfMemory") );
        }     
    else if ( !aTypeString.Compare( _L( "EUsbMSMMSafeToRemove" ) ) )
        {
        aQueryType = EHostMsErrNone;
        iLog -> Log( _L("Query type: EUsbMSMMSafeToRemove") );
        }     
    else if ( !aTypeString.Compare( _L( "EUsbMSMMUnableToEject" ) ) )
        {
        aQueryType = EHostMsErrInUse;
        iLog -> Log( _L("Query type: EUsbMSMMUnableToEject") );
        }     
    else
        {
        iLog -> Log( _L("Query type: not supported") );
        return KErrNotFound;
        }
 
    return KErrNone;
    }
    
// -----------------------------------------------------------------------------
// CBtNotifApiTest::GetTestOption
// -----------------------------------------------------------------------------

TInt CUSBUiNotifApiTest::GetTestOption( TPtrC aOptionString, TTestOption& aOption )
    {    
    if ( !aOptionString.Compare( _L( "DISCARD" ) ) )
        {
        aOption = EQueryDiscarded;
        iLog -> Log( _L("Test type: EQueryDiscarded") );
        }
    else if ( !aOptionString.Compare( _L( "ACCEPT" ) ) )
        {
        aOption = EQueryAccepted;
        iLog -> Log( _L("Test type: EQueryAccepted") );
        }
    else if ( !aOptionString.Compare( _L( "CANCEL" ) ) )
        {
        aOption = EQueryCanceled;
        iLog -> Log( _L("Test type: EQueryCanceled") );
        }
    else
        {
        iLog -> Log( _L("Test type: not supported") );
        return KErrNotFound;
        }
 
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CBtNotifApiTest::GetPersonalityIdFromString
// -----------------------------------------------------------------------------

TInt CUSBUiNotifApiTest::GetPersonalityIdFromString( TPtrC aOptionString, TInt& aPersonalityId )
    {    
    if ( !aOptionString.Compare( _L( "MS" ) ) )
        {
        aPersonalityId = KUsbPersonalityIdMS;
        iLog -> Log( _L("Usb personality type: KUsbPersonalityIdMS") );
        }
    else if ( !aOptionString.Compare( _L( "PTP" ) ) )
        {
        aPersonalityId = KUsbPersonalityIdPTP;
        iLog -> Log( _L("Usb personality type: KUsbPersonalityIdPTP") );
        }
    else if ( !aOptionString.Compare( _L( "MTP" ) ) )
        {
        aPersonalityId = KUsbPersonalityIdMTP;
        iLog -> Log( _L("Usb personality type: KUsbPersonalityIdMTP") );
        }
    else if ( !aOptionString.Compare( _L( "PCSUITEMTP" ) ) )
        {
        aPersonalityId = KUsbPersonalityIdPCSuiteMTP;
        iLog -> Log( _L("Usb personality type: KUsbPersonalityIdPCSuiteMTP") );
        }
    else
        {
        iLog -> Log( _L("Usb personality type: not found") );
        return KErrNotFound;  
        }
 
    return KErrNone;
    }


TInt CUSBUiNotifApiTest::FindAndKillProcess(const TDesC& aProcessName)
    {
    iLog -> Log( _L("FindAndKillProcess") );
    TInt ret = KErrNotFound;
    RProcess process;            
    TFindProcess findProc;
    findProc.Find(aProcessName);
    TFullName procName;
    ret = findProc.Next(procName);
    iLog -> Log( _L("FindAndKillProcess findProc returned: %d"), ret );
    if (ret == KErrNone)
        {
        ret = process.Open(procName, EOwnerThread);
        iLog -> Log( _L("FindAndKillProcess process open returned: %d"), ret );
        if (ret == KErrNone)
            {
            process.Terminate(KErrNone);
            process.Close();
            }
        }
    return ret;
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================
// None

//  [End of File] - Do not remove
