/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  CWidgetRegistrySTIF class member functions
*
*/


// INCLUDE FILES
#include <Stiftestinterface.h>
#include "WidgetRegistrySTIF.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT( KPassed , "Test case passed");
_LIT( KFailed , "Test case failed");
_LIT( KDriveC, "C:");
_LIT( KBundleIdentifier, "BundleIdentifier" );
_LIT( KLong, "XXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXXX");
_LIT( KValidBundle, "com.nokia.widget.sapi.landmarks");
const TInt KUidTestValue = 0x2000DEB9 - 50;
const TUid KInvalidUid = TUid::Uid(0);


// MACROS
// Function pointer related internal definitions
// Rounding known bug in GCC

#ifdef __VC32__
#define GETPTR
#else
#define GETPTR &
#endif
#define ENTRY(str,func) {_S(str), GETPTR func,0,0,0}
#define FUNCENTRY(func) {_S(#func), GETPTR func,0,0,0}
#define OOM_ENTRY(str,func,a,b,c) {_S(str), GETPTR func,a,b,c}
#define OOM_FUNCENTRY(func,a,b,c) {_S(#func), GETPTR func,a,b,c}

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWidgetRegistrySTIF::CWidgetRegistrySTIF
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CWidgetRegistrySTIF::CWidgetRegistrySTIF()
    {
    }

// -----------------------------------------------------------------------------
// CWidgetRegistrySTIF::ConstructL
// Symbian 2nd phase constructor can leave.
// Note: If OOM test case uses STIF Logger, then STIF Logger must be created
// with static buffer size parameter (aStaticBufferSize). Otherwise Logger
// allocates memory from heap and therefore causes error situations with OOM
// testing. For more information about STIF Logger construction, see STIF Users
// Guide.
// -----------------------------------------------------------------------------
//
void CWidgetRegistrySTIF::ConstructL()
    {
    iLog = CStifLogger::NewL( KWidgetRegistrySTIFLogPath,
                              KWidgetRegistrySTIFLogFile );

    User::LeaveIfError( iFileSystem.Connect() );
    
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        RWidgetInfoArray widgetInfoArray;
        error = registryClient->InstalledWidgetsL( widgetInfoArray );
        CWidgetInfo *myInfo = widgetInfoArray[0];
        iValidUid = myInfo->iUid;
        }
     CleanupStack::PopAndDestroy( registryClient ); 
    }

// -----------------------------------------------------------------------------
// CWidgetRegistrySTIF::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CWidgetRegistrySTIF* CWidgetRegistrySTIF::NewL()
    {
    CWidgetRegistrySTIF* self = new (ELeave) CWidgetRegistrySTIF;
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// Destructor
CWidgetRegistrySTIF::~CWidgetRegistrySTIF()
    {
    iFileSystem.Close();
    delete iLog;
    }

// -----------------------------------------------------------------------------
// CWidgetRegistrySTIF::InitL
// InitL is used to initialize the Test Module.
// -----------------------------------------------------------------------------
//
TInt CWidgetRegistrySTIF::InitL( TFileName& /*aIniFile*/,
                                 TBool /*aFirstTime*/ )
    {
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionCreateL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    aResult.SetResult( KErrNone, KPassed );
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
    TInt CWidgetRegistrySTIF::ClientSessionDisconnectFirstL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
        TInt error = registryClient->Disconnect();
        if ( KErrNone == error )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    

TInt CWidgetRegistrySTIF::ClientSessionConnectL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        aResult.SetResult( KErrNone, KPassed );
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
TInt CWidgetRegistrySTIF::ClientSessionTwoConnectL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    registryClient->Connect();
    RWidgetRegistryClientSession* registryClient2;
    registryClient2 = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient2 );
    TInt error = registryClient2->Connect();
    if ( KErrNone == error )
        {
        aResult.SetResult( KErrNone, KPassed );
        }
    registryClient2->Disconnect();
    registryClient->Disconnect();
    CleanupStack::PopAndDestroy( registryClient2 );
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
    
TInt CWidgetRegistrySTIF::ClientSessionDisconnectL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        error = registryClient->Disconnect();
        if ( KErrNone == error )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionVersionL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TVersion version = registryClient->Version();
        if ( 0 < (version.iMajor + version.iMinor + version.iBuild) )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionIsWidgetNullL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TBool iswidget = registryClient->IsWidget( KNullUid );
        if ( EFalse == iswidget )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
    TInt CWidgetRegistrySTIF::ClientSessionIsWidgetValidL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TBool iswidget = registryClient->IsWidget( iValidUid );
        if ( iswidget == TRUE )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

    TInt CWidgetRegistrySTIF::ClientSessionIsWidgetInvalidL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TBool iswidget = registryClient->IsWidget( KInvalidUid );
        if ( EFalse == iswidget )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
TInt CWidgetRegistrySTIF::ClientSessionWidgetExistsL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TBool b = registryClient->WidgetExistsL( KNullDesC );
        if ( EFalse == b )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
    TInt CWidgetRegistrySTIF::ClientSessionWidgetExistsOverflowL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TBool b = registryClient->WidgetExistsL( KLong );
        if ( EFalse == b )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
    TInt CWidgetRegistrySTIF::ClientSessionWidgetExistsValidL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TBool b = registryClient->WidgetExistsL( KValidBundle );
        if ( b != EFalse )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionIsWidgetRunningNullL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TBool b = registryClient->IsWidgetRunning( KNullUid );
        if ( EFalse == b )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
    TInt CWidgetRegistrySTIF::ClientSessionIsWidgetRunningValidL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TBool b = registryClient->IsWidgetRunning( iValidUid );
        if ( b == TRUE )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
    TInt CWidgetRegistrySTIF::ClientSessionIsWidgetRunningInvalidL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TBool b = registryClient->IsWidgetRunning( KInvalidUid );
        if ( b == EFalse )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    

TInt CWidgetRegistrySTIF::ClientSessionWidgetCountL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TInt count = -1;
        TInt count2 = count - 1;
        count2 = registryClient->WidgetCount( count );
        if ( -1 < count && count == count2 )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    


TInt CWidgetRegistrySTIF::ClientSessionGetWidgetPathL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TFileName fn;
        fn.SetLength( 0 );
        registryClient->GetWidgetPath( iValidUid, fn );
        if ( 0 < fn.Length() )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
TInt CWidgetRegistrySTIF::ClientSessionGetWidgetUidL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TInt uid = registryClient->GetWidgetUidL( KNullDesC );
        if ( KNullUid == TUid::Uid( uid ) )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
    TInt CWidgetRegistrySTIF::ClientSessionGetWidgetUidValidL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TInt uid = registryClient->GetWidgetUidL( KValidBundle );
        if ( iValidUid == TUid::Uid( uid ) )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionGetWidgetUidForUrlL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TInt uid = registryClient->GetWidgetUidForUrl( KNullDesC );
        if ( KNullUid == TUid::Uid( uid ) )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionGetAvailableUidL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TRAP( error,
        TUid uid = registryClient->GetAvailableUidL( 'C' );
        if ( KNullUid != uid )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        )
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
    TInt CWidgetRegistrySTIF::ClientSessionGetAvailableUidNullL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
		TRAP(error,
		TUid uid = registryClient->GetAvailableUidL( 'Q' ) );
        if ( error == KErrNone )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionGetWidgetBundleIdL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TFileName fn;
        fn.SetLength( 0 );
        registryClient->GetWidgetBundleId( KNullUid, fn );

        {
        aResult.SetResult( KErrNone, KPassed );
        }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }	

TInt CWidgetRegistrySTIF::ClientSessionGetWidgetBundleNameL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TFileName fn;
        fn.SetLength( 0 );
        registryClient->GetWidgetBundleName( KNullUid, fn );
        if ( 0 == fn.Length() )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionGetWidgetPropertyValueL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        CWidgetPropertyValue* value = NULL;
        TRAP( error,
              value = registryClient->GetWidgetPropertyValueL( iValidUid, EUid ) );
        if ( KErrNone == error  )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        delete value;
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }


TInt CWidgetRegistrySTIF::ClientSessionInstalledWidgetsL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        RWidgetInfoArray widgetInfoArray;
        error = registryClient->InstalledWidgetsL( widgetInfoArray );
        CWidgetInfo *myInfo = widgetInfoArray[0];
        if ( KErrNone == error  )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionRunningWidgetsL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        RWidgetInfoArray widgetInfoArray;
        error = registryClient->RunningWidgetsL( widgetInfoArray );
        if ( KErrNone == error  )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionRegisterWidgetL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        RPointerArray<CWidgetPropertyValue> propertyValues;
        TInt i = 0;
        // empty values
        for ( ; i < EWidgetPropertyIdCount; ++i )
            {
            CWidgetPropertyValue* value = CWidgetPropertyValue::NewL();
            CleanupStack::PushL( value );
            User::LeaveIfError( propertyValues.Insert( value, i ) );
            CleanupStack::Pop( value );
            }
        *(propertyValues[EWidgetPropertyListVersion]) = WIDGETPROPERTYLISTVERSION;
        TFileName buf;
        buf.Copy( KDriveC );
        *(propertyValues[EDriveName]) = buf;
        buf.Copy( KBundleIdentifier );
        *(propertyValues[EBundleIdentifier]) = buf;
        *(propertyValues[EUid]) = KUidTestValue /* arbitrary value */;
        TRAP( error,
              registryClient->RegisterWidgetL( propertyValues ) );
        if ( KErrNone == error  )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionDeRegisterWidgetL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TRAP( error,
              registryClient->DeRegisterWidgetL( iValidUid ));
        if ( KErrNone == error  )
            {
            aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionSetActiveL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        registryClient->SetActive( KNullUid, 0 );
        aResult.SetResult( KErrNone, KPassed );
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }

TInt CWidgetRegistrySTIF::ClientSessionGetLprojNameL( TTestResult& aResult )
    {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
        TFileName buf;
        registryClient->GetLprojName( buf );
        aResult.SetResult( KErrNone, KPassed );
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
    }
    
 TInt CWidgetRegistrySTIF::ClientSessionSecurityPolicyId( TTestResult& aResult )
 {
    aResult.SetResult( KErrGeneral, KFailed );
    RWidgetRegistryClientSession* registryClient;
    registryClient = new (ELeave) RWidgetRegistryClientSession;
    CleanupStack::PushL( registryClient );
    TInt error = registryClient->Connect();
    if ( KErrNone == error )
        {
			TInt SecurityPolicyId = registryClient->SecurityPolicyId();
			if( 0 != SecurityPolicyId )
			{
            	aResult.SetResult( KErrNone, KPassed );
            }
        registryClient->Disconnect();
        }
    CleanupStack::PopAndDestroy( registryClient );
    return KErrNone;
 }


const TCaseInfo CWidgetRegistrySTIF::Case ( const TInt aCaseNumber ) const
    {
    static TCaseInfoInternal const KCases[] =
        {
        // To add new test cases, add new items to this array

        // NOTE: When compiled to GCCE, there must be Classname::
        // declaration in front of the method name, e.g.
        // CDemoModule::PrintTest. Otherwise the compiler
        // gives errors.

        ENTRY( "ClientSession New",
               CWidgetRegistrySTIF::ClientSessionCreateL ),
        ENTRY( "ClientSession DisconnectFirst",
               CWidgetRegistrySTIF::ClientSessionDisconnectFirstL ),
        ENTRY( "ClientSession Connect",
               CWidgetRegistrySTIF::ClientSessionConnectL ),
        ENTRY( "ClientSession ConnectAgain",
               CWidgetRegistrySTIF::ClientSessionTwoConnectL ),
        ENTRY( "ClientSession Disconnect",
               CWidgetRegistrySTIF::ClientSessionDisconnectL ),
        ENTRY( "ClientSession Version",
               CWidgetRegistrySTIF::ClientSessionVersionL ),
        ENTRY( "ClientSession IsWidgetNull",
               CWidgetRegistrySTIF::ClientSessionIsWidgetNullL ),
        ENTRY( "ClientSession IsWidgetInvalid",
               CWidgetRegistrySTIF::ClientSessionIsWidgetInvalidL ),
        ENTRY( "ClientSession IsWidgetValid",
               CWidgetRegistrySTIF::ClientSessionIsWidgetValidL ),
        ENTRY( "ClientSession WidgetExists",
               CWidgetRegistrySTIF::ClientSessionWidgetExistsL ),
        ENTRY( "ClientSession WidgetExistsOverflow",
  				CWidgetRegistrySTIF::ClientSessionWidgetExistsOverflowL),
  		ENTRY( "ClientSession WidgetExistsValid",
  				CWidgetRegistrySTIF::ClientSessionWidgetExistsValidL),
        ENTRY( "ClientSession IsWidgetRunningNull",
               CWidgetRegistrySTIF::ClientSessionIsWidgetRunningNullL ),
        ENTRY( "ClientSession IsWidgetRunningValid",
               CWidgetRegistrySTIF::ClientSessionIsWidgetRunningValidL ),
		ENTRY( "ClientSession IsWidgetRunningInvalid",
               CWidgetRegistrySTIF::ClientSessionIsWidgetRunningInvalidL ),
        ENTRY( "ClientSession WidgetCount",
               CWidgetRegistrySTIF::ClientSessionWidgetCountL ),
        ENTRY( "ClientSession GetWidgetPath",
               CWidgetRegistrySTIF::ClientSessionGetWidgetPathL ),
        ENTRY( "ClientSession GetWidgetUid",
               CWidgetRegistrySTIF::ClientSessionGetWidgetUidL ),
        ENTRY( "ClientSession GetWidgetUidValid",
               CWidgetRegistrySTIF::ClientSessionGetWidgetUidValidL ),
        ENTRY( "ClientSession GetWidgetUidForUrl",
               CWidgetRegistrySTIF::ClientSessionGetWidgetUidForUrlL ),
        ENTRY( "ClientSession GetAvailableUid",
               CWidgetRegistrySTIF::ClientSessionGetAvailableUidL ),
        ENTRY( "ClientSession GetAvailableUidNull",
               CWidgetRegistrySTIF::ClientSessionGetAvailableUidNullL ),
        ENTRY( "ClientSession GetWidgetBundleId",
               CWidgetRegistrySTIF::ClientSessionGetWidgetBundleIdL ),
        ENTRY( "ClientSession GetWidgetBundleName",
               CWidgetRegistrySTIF::ClientSessionGetWidgetBundleNameL ),
        ENTRY( "ClientSession GetWidgetPropertyValue",
               CWidgetRegistrySTIF::ClientSessionGetWidgetPropertyValueL ),
        ENTRY( "ClientSession InstalledWidgets",
               CWidgetRegistrySTIF::ClientSessionInstalledWidgetsL ),
        ENTRY( "ClientSession RunningWidgets",
               CWidgetRegistrySTIF::ClientSessionRunningWidgetsL ),
        ENTRY( "ClientSession RegisterWidget",
               CWidgetRegistrySTIF::ClientSessionRegisterWidgetL ),
        ENTRY( "ClientSession DeRegisterWidget",
               CWidgetRegistrySTIF::ClientSessionDeRegisterWidgetL ),
        ENTRY( "ClientSession SetActive",
               CWidgetRegistrySTIF::ClientSessionSetActiveL ),
        ENTRY( "ClientSession GetLprojName",
               CWidgetRegistrySTIF::ClientSessionGetLprojNameL ),
        ENTRY( "ClientSession SecurityPolicyId",
               CWidgetRegistrySTIF::ClientSessionSecurityPolicyId ),
        
        };
    /*
     * To add new test cases, implement new test case function and add new
     * line to KCases array specify the name of the case and the function
     * doing the test case
     * In practice, do following
     *
     * 1) Make copy of existing test case function and change its name
     *    and functionality. Note that the function must be added to
     *    OOMHard.cpp file and to OOMHard.h
     *    header file.
     *
     * 2) Add entry to following KCases array either by using:
     *
     * 2.1: FUNCENTRY or ENTRY macro
     * ENTRY macro takes two parameters: test case name and test case
     * function name.
     *
     * FUNCENTRY macro takes only test case function name as a parameter and
     * uses that as a test case name and test case function name.
     *
     * Or
     *
     * 2.2: OOM_FUNCENTRY or OOM_ENTRY macro. Note that these macros are used
     * only with OOM (Out-Of-Memory) testing!
     *
     * OOM_ENTRY macro takes five parameters: test case name, test case
     * function name, TBool which specifies is method supposed to be run using
     * OOM conditions, TInt value for first heap memory allocation failure and
     * TInt value for last heap memory allocation failure.
     *
     * OOM_FUNCENTRY macro takes test case function name as a parameter and uses
     * that as a test case name, TBool which specifies is method supposed to be
     * run using OOM conditions, TInt value for first heap memory allocation
     * failure and TInt value for last heap memory allocation failure.
     */

    // Verify that case number is valid
    if( (TUint) aCaseNumber >= sizeof( KCases ) /
        sizeof( TCaseInfoInternal ) )
        {

        // Invalid case, construct empty object
        TCaseInfo null( (const TText*) L"" );
        null.iMethod = NULL;
        null.iIsOOMTest = EFalse;
        null.iFirstMemoryAllocation = 0;
        null.iLastMemoryAllocation = 0;
        return null;
        }

    // Construct TCaseInfo object and return it
    TCaseInfo tmp ( KCases[ aCaseNumber ].iCaseName );
    tmp.iMethod = KCases[ aCaseNumber ].iMethod;
    tmp.iIsOOMTest = KCases[ aCaseNumber ].iIsOOMTest;
    tmp.iFirstMemoryAllocation = KCases[ aCaseNumber ].iFirstMemoryAllocation;
    tmp.iLastMemoryAllocation = KCases[ aCaseNumber ].iLastMemoryAllocation;
    return tmp;
    }

// -----------------------------------------------------------------------------
// CWidgetRegistrySTIF::GetTestCasesL
// GetTestCases is used to aquire test cases from the Test Module. Test
// cases are stored to array of test cases. The Test Framework will be
// the owner of the data in the RPointerArray after GetTestCases return
// and it does the memory deallocation.
// -----------------------------------------------------------------------------
//
TInt
CWidgetRegistrySTIF::GetTestCasesL( const TFileName& /*aConfig*/,
                                    RPointerArray<TTestCaseInfo>& aTestCases )
    {
    // copy name and number for all the tests--to be used in UI
    for( TInt i = 0; Case(i).iMethod; i++ )
        {
        TTestCaseInfo* newCase = new (ELeave) TTestCaseInfo();
        CleanupStack::PushL( newCase );
        newCase->iCaseNumber = i; // will be a parameter to RunTestCaseL
        newCase->iTitle.Copy( Case(i).iCaseName ); // for display
        // transfer ownership
        User::LeaveIfError( aTestCases.Append( newCase ) );
        CleanupStack::Pop( newCase );
        }
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// CWidgetRegistrySTIF::RunTestCaseL
// RunTestCaseL is used to run an individual test case specified
// by aTestCase. Test cases that can be run may be requested from
// Test Module by GetTestCases method before calling RunTestCase.
// -----------------------------------------------------------------------------
//
TInt CWidgetRegistrySTIF::RunTestCaseL( const TInt aCaseNumber,
                                        const TFileName& /*aConfig*/,
                                        TTestResult& aResult )
    {
    TInt result = KErrNotFound;
    TCaseInfo tmp = Case( aCaseNumber );
    if ( NULL != tmp.iMethod )
        {
        _LIT( KLogInfo, "Starting testcase [%S]" );
        iLog->Log( KLogInfo, &tmp.iCaseName );
        result = (this->*tmp.iMethod)( aResult );
        }
    return result;
    }

// -----------------------------------------------------------------------------
// CWidgetRegistrySTIF::OOMTestQueryL
// Used to check if a particular test case should be run in OOM conditions and
// which memory allocations should fail.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.
// -----------------------------------------------------------------------------
//
TBool CWidgetRegistrySTIF::OOMTestQueryL( const TFileName& /* aTestCaseFile */,
                                          const TInt /* aCaseNumber */,
                                          TOOMFailureType& /* aFailureType */,
                                          TInt& /* aFirstMemFailure */,
                                          TInt& /* aLastMemFailure */ )
    {
    _LIT( KOOMTestQueryL, "CWidgetRegistrySTIF::OOMTestQueryL" );
    iLog->Log( KOOMTestQueryL );
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWidgetRegistrySTIF::OOMTestInitializeL
// Used to perform the test environment setup for a particular OOM test case.
// Test Modules may use the initialization file to read parameters for Test
// Module initialization but they can also have their own configure file or
// some other routine to initialize themselves.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.
// -----------------------------------------------------------------------------
//
void CWidgetRegistrySTIF::OOMTestInitializeL(
    const TFileName& /* aTestCaseFile */,
    const TInt /* aCaseNumber */ )
    {
    }

// -----------------------------------------------------------------------------
// CWidgetRegistrySTIF::OOMHandleWarningL
// In some cases the heap memory allocation should be skipped, either due to
// problems in the OS code or components used by the code being tested, or even
// inside the tested components which are implemented this way on purpose (by
// design), so it is important to give the tester a way to bypass allocation
// failures.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.
// -----------------------------------------------------------------------------
//
void CWidgetRegistrySTIF::OOMHandleWarningL(
    const TFileName& /* aTestCaseFile */,
    const TInt /* aCaseNumber */,
    TInt& /* aFailNextValue */ )
    {
    }

// -----------------------------------------------------------------------------
// CWidgetRegistrySTIF::OOMTestFinalizeL
// Used to perform the test environment cleanup for a particular OOM test case.
//
// NOTE: This method is virtual and must be implemented only if test case
// should be executed using OOM conditions.
// -----------------------------------------------------------------------------
//
void CWidgetRegistrySTIF::OOMTestFinalizeL(
    const TFileName& /* aTestCaseFile */,
    const TInt /* aCaseNumber */ )
    {
    }

// ========================== OTHER EXPORTED FUNCTIONS =========================

// -----------------------------------------------------------------------------
// LibEntryL is a polymorphic Dll entry point
// Returns: CTestModuleBase*: Pointer to Test Module object
// -----------------------------------------------------------------------------
//
EXPORT_C CTestModuleBase* LibEntryL()
    {
    return CWidgetRegistrySTIF::NewL();
    }

// -----------------------------------------------------------------------------
// SetRequirements handles test module parameters(implements evolution
// version 1 for test module's heap and stack sizes configuring).
// Returns: TInt: Symbian error code.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt SetRequirements( CTestModuleParam*& /*aTestModuleParam*/,
                               TUint32& /*aParameterValid*/ )
    {
    /* --------------------------------- NOTE ---------------------------------
    USER PANICS occurs in test thread creation when:
    1) "The panic occurs when the value of the stack size is negative."
    2) "The panic occurs if the minimum heap size specified is less
       than KMinHeapSize".
       KMinHeapSize: "Functions that require a new heap to be allocated will
       either panic, or will reset the required heap size to this value if
       a smaller heap size is specified".
    3) "The panic occurs if the minimum heap size specified is greater than
       the maximum size to which the heap can grow".
    Other:
    1) Make sure that your hardware or Symbian OS is supporting given sizes.
       e.g. Hardware might support only sizes that are divisible by four.
    ------------------------------- NOTE end ------------------------------- */

    // Normally STIF uses default heap and stack sizes for test thread, see:
    // KTestThreadMinHeap, KTestThreadMinHeap and KStackSize.
    // If needed heap and stack sizes can be configured here by user. Remove
    // comments and define sizes.

/*
    aParameterValid = KStifTestModuleParameterChanged;

    CTestModuleParamVer01* param = CTestModuleParamVer01::NewL();
    // Stack size
    param->iTestThreadStackSize= 16384; // 16K stack
    // Heap sizes
    param->iTestThreadMinHeap = 4096;   // 4K heap min
    param->iTestThreadMaxHeap = 1048576;// 1M heap max

    aTestModuleParam = param;
*/
    return KErrNone;
    }

// -----------------------------------------------------------------------------
// E32Dll is a DLL entry point function
// Returns: KErrNone: No error
// -----------------------------------------------------------------------------
//
#ifndef EKA2 // Hide Dll entry point to EKA2
GLDEF_C TInt E32Dll(
    TDllReason /*aReason*/) // Reason
    {
    return(KErrNone);
    }
#endif // EKA2

//  End of File
