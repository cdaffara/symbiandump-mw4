/*
* Copyright (c) 2006, 2008 Nokia Corporation and/or its subsidiary(-ies).
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


#include <s32mem.h>

// User includes
#include "WidgetRegistry.h"
#include "WidgetRegistryConstants.h"
#include "WidgetRegistryServer.h"
#include "WidgetRegistryConstants.h"
#include "WidgetRegistrySession.h"


CWidgetRegistry* CWidgetRegistryServerSession::iWidgetRegistry = 0;

// ============================================================================
// CWidgetRegistryServerSession::CWidgetRegistryServerSession()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
CWidgetRegistryServerSession::CWidgetRegistryServerSession()
    {
    }

// ============================================================================
// CWidgetRegistryServerSession::~CWidgetRegistryServerSession()
// Destructor
//
// @since 3.1
// ============================================================================
//
CWidgetRegistryServerSession::~CWidgetRegistryServerSession()
    {
    CWidgetRegistryServer* server( static_cast<CWidgetRegistryServer*>(
        const_cast<CServer2*>( Server() ) ) );

    if ( server && server->SessionCount() <= 0 )
        {
        delete iWidgetRegistry;
        iWidgetRegistry = NULL;
        iRfs.Close(); // after delete registry because registry/mmc refs it
        }
    }


// ============================================================================
// CWidgetRegistryServerSession::CreateL()
// Called by the CServer framework
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistryServerSession::CreateL()
    {
    CWidgetRegistryServer* server;

    server = static_cast<CWidgetRegistryServer*>(
        const_cast<CServer2*>( Server() ) );
    User::LeaveIfNull ( server );

    if ( !iWidgetRegistry )
        {
        User::LeaveIfError( iRfs.Connect() );
        iWidgetRegistry = CWidgetRegistry::NewL( iRfs );
        }

    server->AddSession();
    }

// ============================================================================
// CWidgetRegistryServerSession::ConstructL()
// ConstructL method
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistryServerSession::ConstructL(
    CWidgetRegistryServer& /*aServer*/,
    const RMessage2& /*aMessage*/ )
    {
    if ( !iWidgetRegistry )
        {
        User::LeaveIfError( iRfs.Connect() );
        iRfs.CreatePrivatePath( EDriveC );
        iRfs.SetSessionToPrivate( EDriveC );
        iRfs.ShareProtected();
        iWidgetRegistry = CWidgetRegistry::NewL( iRfs );
        }
    }

// ============================================================================
// CWidgetRegistryServerSession::NewL()
//
// @since 3.1
// ============================================================================
//
CWidgetRegistryServerSession* CWidgetRegistryServerSession::NewL(
    CWidgetRegistryServer& aServer, const RMessage2& aMessage )
    {
    CWidgetRegistryServerSession* self;

    self = new ( ELeave ) CWidgetRegistryServerSession();
    CleanupStack::PushL( self );
    self->ConstructL( aServer, aMessage );
    CleanupStack::Pop( self );
    return self;
    }

// ============================================================================
// CWidgetRegistryServerSession::ServiceL()
// Handles messages from client
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistryServerSession::ServiceL( const RMessage2& aMessage )
    {
    TBool retBool( EFalse );
    TInt len = 0;
    TInt uid = 0;
    TInt count = 0;
    TInt policyId = 0;
    TInt error;
    const TInt function = aMessage.Function();

    // TBD seems like we should trap leaves and return error codes so
    // as not to exit the server
    switch ( function )
        {
        case EOpCodeRegisterWidget:
            OpRegisterWidgetL( aMessage );
            aMessage.Complete( KErrNone );
            break;
        case EOpCodeWidgetExists:
            retBool = OpWidgetExists( aMessage );
            aMessage.Complete( retBool );
            break;
        case EOpCodeIsWidget:
            retBool = OpIsWidget( aMessage );
            aMessage.Complete( retBool );
            break;
        case EOpCodeIsWidgetRunning:
            retBool = OpIsWidgetRunning( aMessage );
            aMessage.Complete( retBool );
            break;
        case EOpWidgetSapiAccessState:
            retBool = OpWidgetSapiAccessState( aMessage );
            aMessage.Complete( retBool );
            break;
        case EOpCodeIsWidgetInMiniView:
            retBool = OpIsWidgetInMiniView( aMessage );
            aMessage.Complete( retBool );
            break;
        case EOpCodeIsWidgetInFullView:
            retBool = OpIsWidgetInFullView( aMessage );
            aMessage.Complete( retBool );
            break;
        case EOpCodeIsWidgetPromptNeeded:
            retBool = OpIsWidgetPromptNeeded( aMessage );
            aMessage.Complete( retBool );
            break;
        case EOpCodeWidgetCount:
            count = OpWidgetCount( aMessage );
            aMessage.Complete( count );
            break;
        case EOpCodeInstalledWidgetsPhase1:
            len = OpInstalledWidgetsPhase1L( aMessage );
            aMessage.Complete( len );
            break;
        case EOpCodeInstalledWidgetsPhase2:
            len = OpInstalledWidgetsPhase2L( aMessage );
            aMessage.Complete( len );
            break;
        case EOpCodeRunningWidgetsPhase1:
            len = OpRunningWidgetsPhase1L( aMessage );
            aMessage.Complete( len );
            break;
        case EOpCodeRunningWidgetsPhase2:
            len = OpRunningWidgetsPhase2L( aMessage );
            aMessage.Complete( len );
            break;
        case EOpCodeGetWidgetBundleId:
            len = OpGetWidgetBundleId( aMessage );
            aMessage.Complete( len );
            break;
        case EOpCodeGetWidgetBundleName:
            len = OpGetWidgetBundleName( aMessage );
            aMessage.Complete( len );
            break;
        case EOpCodeGetWidgetPropertyValue:
            error = OpGetWidgetPropertyValueL( aMessage );
            aMessage.Complete( error );
            break;
        case EOpCodeGetWidgetPath:
            len = OpGetWidgetPath( aMessage );
            aMessage.Complete( len );
            break;
        case EOpCodeGetWidgetUid:
            uid = OpGetWidgetUid( aMessage );
            aMessage.Complete( uid );
            break;
        case EOpCodeGetWidgetUidForUrl:
            uid = OpGetWidgetUidForUrl( aMessage );
            aMessage.Complete( uid );
            break;
        case EOpCodeDeRegisterWidget:
            OpDeRegisterWidgetL( aMessage );
            aMessage.Complete( KErrNone );
            break;
        case EOpCodeGetAvailableUid:
            uid = OpGetAvailableUidL( aMessage );
            aMessage.Complete( uid );
            break;
        case EOpCodeSetActive:
            OpSetActive( aMessage );
            aMessage.Complete( KErrNone );
            break;
        case EOpCodeWidgetRegistryDisconnect:
            OpDisconnect( aMessage );
            aMessage.Complete( KErrNone );
            break;
        case EOpCodeGetLprojName:
            len = OpGetLprojNameL( aMessage );
            aMessage.Complete( len );
            break;
        case EOpCodeSecurityPolicyId:
            policyId = OpSecurityPolicyId( aMessage );
            aMessage.Complete( policyId );
            break;        
        case EOpCodeSetWidgetInMiniView:
            OpSetMiniView( aMessage );
            aMessage.Complete( KErrNone );
            break;	
        case EOpCodeSetWidgetInFullView:
            OpSetFullView( aMessage );
            aMessage.Complete( KErrNone );
            break;
        case EOpCodeSetWidgetPromptNeeded:
            OpSetPromptNeeded( aMessage );
            aMessage.Complete( KErrNone );
            break;
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    }

// ============================================================================
// CWidgetRegistryServerSession::ServiceError()
// Handles leaves from ServiceL()
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistryServerSession::ServiceError( const RMessage2& aMessage,
                                                 TInt aError )
    {
    const TInt function = aMessage.Function();

    // supply the correct error return message params for each opcode
    switch ( function )
        {
        // TODO many of these opcodes have no provision for proper
        // error signalling and need to be reworked to communicate
        // error codes
        case EOpCodeRegisterWidget:
            aMessage.Complete( aError );
            break;
        case EOpCodeWidgetExists:
            aMessage.Complete( EFalse );
            break;
        case EOpCodeIsWidget:
            aMessage.Complete( EFalse );
            break;
        case EOpCodeIsWidgetRunning:
            aMessage.Complete( EFalse );
            break;
        case EOpCodeIsWidgetInMiniView:
            aMessage.Complete( EFalse );
            break;
        case EOpCodeIsWidgetInFullView:
            aMessage.Complete( EFalse );
            break;
        case EOpCodeIsWidgetPromptNeeded:
            aMessage.Complete( EFalse );
            break;
        case EOpCodeWidgetCount:
            aMessage.Complete( 0 );
            break;
        case EOpCodeInstalledWidgetsPhase1:
            aMessage.Complete( 0 );
            break;
        case EOpCodeInstalledWidgetsPhase2:
            aMessage.Complete( 0 );
            break;
        case EOpCodeRunningWidgetsPhase1:
            aMessage.Complete( 0 );
            break;
        case EOpCodeRunningWidgetsPhase2:
            aMessage.Complete( 0 );
            break;
        case EOpCodeGetWidgetBundleId:
            aMessage.Complete( 0 );
            break;
        case EOpCodeGetWidgetBundleName:
            aMessage.Complete( 0 );
            break;
        case EOpCodeGetWidgetPropertyValue:
            aMessage.Complete( aError );
            break;
        case EOpCodeGetWidgetPath:
            aMessage.Complete( 0 );
            break;
        case EOpCodeGetWidgetUid:
            aMessage.Complete( KNullUid.iUid );
            break;
        case EOpCodeGetWidgetUidForUrl:
            aMessage.Complete( KNullUid.iUid );
            break;
        case EOpCodeDeRegisterWidget:
            aMessage.Complete( aError );
            break;
        case EOpCodeGetAvailableUid:
            aMessage.Complete( KNullUid.iUid );
            break;
        case EOpCodeSetActive:
            aMessage.Complete( aError );
            break;
        case EOpCodeWidgetRegistryDisconnect:
            aMessage.Complete( aError );
            break;
        case EOpCodeGetLprojName:
            aMessage.Complete( 0 );
            break;
        case EOpCodeSecurityPolicyId:
            aMessage.Complete( 0 );
            break;  
        case EOpCodeSetWidgetInMiniView:
            aMessage.Complete( EFalse );
            break;
        case EOpCodeSetWidgetInFullView:
            aMessage.Complete( EFalse );
            break;
        case EOpCodeSetWidgetPromptNeeded:
            aMessage.Complete( EFalse );
            break;
        case EOpWidgetSapiAccessState:
            aMessage.Complete( EFalse );
            break;        
        default:
            aMessage.Complete( aError );
            break;
        }
    }


// ============================================================================
// CWidgetRegistryServerSession::OpRegisterWidgetL()
// Registers the widget
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpRegisterWidgetL(
    const RMessage2& aMessage )
    {
    // ARGS: 0 -> descriptor to buf of serialized property values
    TInt l = aMessage.GetDesMaxLength( 0 );
    HBufC8* msgData = HBufC8::NewLC( l );
    TPtr8 readPtr( msgData->Des() );
    aMessage.ReadL( 0, readPtr );
    RDesReadStream stream( *msgData );
    CleanupClosePushL( stream );

    iWidgetRegistry->RegisterWidgetL( stream );

    CleanupStack::PopAndDestroy( 2 ); //stream, msgData,
    return KErrNone;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpIsWidget()
// Returns true if the given UId falls within the range allocated for widgets
//
// @since 3.1
// ============================================================================
//
TBool CWidgetRegistryServerSession::OpIsWidget(
    const RMessage2& aMessage )
    {
    TInt uid = 0;
    TBool retVal( EFalse );

    uid = aMessage.Int0();
    retVal = iWidgetRegistry->IsWidget( TUid::Uid( uid ) );

    return retVal;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpWidgetExists()
// Returns true if the widget with the given UId has been installed.
//
// @since 3.1
// ============================================================================
//
TBool CWidgetRegistryServerSession::OpWidgetExists(
    const RMessage2& aMessage )
    {
    TBool retVal( EFalse );
    TBuf<KWidgetRegistryVal> bundleId;
    TInt len = aMessage.Int1();

    if ( len < 1 || len > KWidgetRegistryVal )
        {
        return retVal;
        }

    bundleId.SetLength( len );
    TRAP_IGNORE( aMessage.ReadL( 0, bundleId ) )

    retVal = iWidgetRegistry->WidgetExists( bundleId );

    return retVal;
    }


// ============================================================================
// CWidgetRegistryServerSession::OpIsWidgetRunning()
// Returns running status of the widget with the given UId.
//
// @since 3.1
// ============================================================================
//
TBool CWidgetRegistryServerSession::OpIsWidgetRunning(
    const RMessage2& aMessage )
    {
    TInt uid = 0;
    TBool retVal( EFalse );

    uid = aMessage.Int0();
    retVal = iWidgetRegistry->IsWidgetRunning( TUid::Uid( uid ) );

    return retVal;
    }
// ============================================================================
// CWidgetRegistryServerSession::OpWidgetSapiAccessState()
// Returns widget sapi access state is prompted, promptless or denied.
//
// @since 5.0
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpWidgetSapiAccessState(
    const RMessage2& aMessage )
    {
    TInt uid = 0;
    TInt retVal( -1 );

    uid = aMessage.Int0();
    retVal = iWidgetRegistry->WidgetSapiAccessState( TUid::Uid( uid ) );

    return retVal;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpIsWidgetInMiniView()
// Returns status whether widget is started in miniview or not.
//
// @since 
// ============================================================================
//
TBool CWidgetRegistryServerSession::OpIsWidgetInMiniView(
    const RMessage2& aMessage )
    {
    TInt uid = 0;
    TBool retVal( EFalse );

    uid = aMessage.Int0();
    retVal = iWidgetRegistry->IsWidgetInMiniView( TUid::Uid( uid ) );

    return retVal;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpIsWidgetInFullView()
// Returns status whether widget is started in full view or not.
//
// @since 
// ============================================================================
//
TBool CWidgetRegistryServerSession::OpIsWidgetInFullView(
    const RMessage2& aMessage )
    {
    TInt uid = 0;
    TBool retVal( EFalse );

    uid = aMessage.Int0();
    retVal = iWidgetRegistry->IsWidgetInFullView( TUid::Uid( uid ) );

    return retVal;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpIsWidgetPromptNeeded()
// Returns status whether miniview platform access prompt is needed or not.
//
// @since  5.0
// ============================================================================
//
TBool CWidgetRegistryServerSession::OpIsWidgetPromptNeeded(
    const RMessage2& aMessage )
    {
    TInt uid = 0;
    TBool retVal( EFalse );

    uid = aMessage.Int0();
    retVal = iWidgetRegistry->IsBlanketPermGranted( TUid::Uid( uid ) );

    return retVal;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpWidgetCount()
// Returns count of installed widgets
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpWidgetCount(
    const RMessage2& /*aMessage*/ )
    {
    return iWidgetRegistry->WidgetCount();
    }

// ============================================================================
// CWidgetRegistryServerSession::OpInstalledWidgetsPhase1L()
// gets total size of all elements in the widgetinfo array in terms of
// buffer length; negative value means one of the system-wide error
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpInstalledWidgetsPhase1L(
    const RMessage2& /*aMessage*/ )
    {
    TInt ret = 0;

    RWidgetInfoArray widgetInfoArr;
    CleanupStack::PushL( TCleanupItem( 
            CWidgetRegistryServerSession::ResetAndDestroy, &widgetInfoArr ) );
    
    iWidgetRegistry->InstalledWidgetsL( widgetInfoArr );
        
    if ( widgetInfoArr.Count() > 0 )
        {
        ret = widgetInfoArr.Count() * ( 4 * sizeof( TInt ) +
            KWidgetRegistryVal + 1 + KMaxDriveName + 1 + 1 );
        }

    CleanupStack::PopAndDestroy(&widgetInfoArr);
    
    return ret;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpInstalledWidgetsPhase2L()
// Sets widgetinfo array of the installed widgets in the message itself
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpInstalledWidgetsPhase2L(
    const RMessage2& aMessage )
    {
    TInt ret = 0;

    RWidgetInfoArray widgetInfoArr;
    CleanupStack::PushL( TCleanupItem(
            CWidgetRegistryServerSession::ResetAndDestroy, &widgetInfoArr) );
    iWidgetRegistry->InstalledWidgetsL( widgetInfoArr );

    TInt error = KErrNone;
    HBufC8* widgetInfoDes = NULL;

    TRAP( error, widgetInfoDes = MarshalDataL( widgetInfoArr ) );
    CleanupStack::PushL( widgetInfoDes );

    if ( error == KErrNone )
        {
        ret = widgetInfoArr.Count();
        aMessage.WriteL( 0, *widgetInfoDes );
        }

    
    CleanupStack::PopAndDestroy( 2 );

    return ret;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpRunningWidgetsPhase1L()
// gets total size of all elements in the widgetinfo array in terms of
// buffer length; negative value means one of the system-wide error
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpRunningWidgetsPhase1L(
    const RMessage2& /*aMessage*/ )
    {
    TInt ret = 0;

    RWidgetInfoArray widgetInfoArr;
    CleanupStack::PushL( TCleanupItem(
            CWidgetRegistryServerSession::ResetAndDestroy, &widgetInfoArr ));
    iWidgetRegistry->RunningWidgetsL( widgetInfoArr );

    if ( widgetInfoArr.Count() > 0 )
        {
        ret = widgetInfoArr.Count() * ( 4 * sizeof( TInt ) +
            KWidgetRegistryVal + 1 + KMaxDriveName + 1 + 1 );
        }

    
    CleanupStack::PopAndDestroy();

    return ret;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpRunningWidgetsPhase2L()
// Sets widgetinfo array of the running widgets in the message itself
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpRunningWidgetsPhase2L(
    const RMessage2& aMessage )
    {
    TInt ret = 0;

    RWidgetInfoArray widgetInfoArr;
    CleanupStack::PushL( TCleanupItem(
            CWidgetRegistryServerSession::ResetAndDestroy, &widgetInfoArr) );
    iWidgetRegistry->RunningWidgetsL( widgetInfoArr );

    TInt error = KErrNone;
    HBufC8* widgetInfoDes = NULL;

    TRAP( error, widgetInfoDes = MarshalDataL( widgetInfoArr ) );
    CleanupStack::PushL( widgetInfoDes );

    if ( error == KErrNone )
        {
        ret = widgetInfoArr.Count();
        aMessage.WriteL( 0, *widgetInfoDes );
        }

    CleanupStack::PopAndDestroy( 2 );

    return ret;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpGetWidgetBundleId()
// Returns bundleId of the widget with the given UId.
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpGetWidgetBundleId(
    const RMessage2& aMessage )
    {
    TInt ret = 0;
    TInt uid = aMessage.Int2();
    TBuf<KWidgetRegistryVal> buf;

    buf.SetLength( aMessage.Int1() );
    TRAP_IGNORE( aMessage.ReadL( 0, buf ) )
    iWidgetRegistry->GetWidgetBundleId( TUid::Uid( uid ), buf );
    TRAP_IGNORE( aMessage.WriteL( 0, buf ) )
    ret = buf.Length();

    return ret;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpGetWidgetBundleName()
// Returns bundle display name of the widget with the given UId.
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpGetWidgetBundleName(
    const RMessage2& aMessage )
    {
    TInt ret = 0;
    TInt uid = aMessage.Int2();
    TBuf<KWidgetRegistryVal> buf;
    buf.SetLength( aMessage.Int1() );
    iWidgetRegistry->GetWidgetBundleName( TUid::Uid( uid ), buf );
    TRAP_IGNORE( aMessage.WriteL( 0, buf ) )
    ret = buf.Length();

    return ret;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpGetWidgetPropertyValue()
// Produces serialized value and error code
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpGetWidgetPropertyValueL(
    const RMessage2& aMessage )
    {
    // ARGS( 0 -> UID, 1 -> TWidgetPropertyId,
    //       2 -> descriptor for buffer to hold serialization of value )
    // returns error code
    TRAPD( error,
    {
    TUid uid( TUid::Uid( aMessage.Int0() ) );
    TWidgetPropertyId propId = static_cast<TWidgetPropertyId>(aMessage.Int1());
    TInt maxLength = aMessage.GetDesMaxLength( 2 );

    CBufFlat* buf = iWidgetRegistry->GetWidgetPropertyValueL(
        uid, propId, maxLength );

    CleanupStack::PushL( buf );
    aMessage.WriteL( 2, buf->Ptr(0) );
    CleanupStack::PopAndDestroy(); // buf
    }
        );
    return error;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpGetWidgetPath()
// Returns path of the widget with the given UId.
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpGetWidgetPath(
    const RMessage2& aMessage )
    {
    TInt ret = 0;
    TInt uid = aMessage.Int2();
    TBuf<KWidgetRegistryVal> path;

    path.SetLength( aMessage.Int1() );
    TRAP_IGNORE( aMessage.ReadL( 0, path ) )
    iWidgetRegistry->GetWidgetPath( TUid::Uid( uid ), path );
    TRAP_IGNORE( aMessage.WriteL( 0, path ) )
    ret = path.Length();

    return ret;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpGetWidgetUid()
// Returns UId of the widget with the given bundle Id.
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpGetWidgetUid(
    const RMessage2& aMessage )
    {
    TUid uid = TUid::Uid( 0 );
    TBuf<KWidgetRegistryVal> bundleId;
    TInt len = aMessage.Int1();

    if ( len < 1 || len > KWidgetRegistryVal )
        {
        return 0;
        }

    bundleId.SetLength( len );
    TRAP_IGNORE( aMessage.ReadL( 0, bundleId ) )
    uid = iWidgetRegistry->GetWidgetUid( bundleId );

    return uid.iUid;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpGetWidgetUidForUrl()
// Returns UId of the widget with the given Url
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpGetWidgetUidForUrl(
    const RMessage2& aMessage )
    {
    TBuf<KWidgetRegistryVal> url;
    TInt len = aMessage.Int1();

    if ( len < 1 || len > KWidgetRegistryVal )
        {
        return 0;
        }

    url.SetLength( len );
    TRAP_IGNORE( aMessage.ReadL( 0, url ) )
    TInt uid = ( iWidgetRegistry->GetWidgetUidForUrl( url ) ).iUid;

    return uid;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpDeRegisterWidgetL()
// Deregister the widget
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpDeRegisterWidgetL(
    const RMessage2& aMessage )
    {
    TInt uid;
    TInt ret = KErrNone;

    uid = aMessage.Int0();
    iWidgetRegistry->DeRegisterWidgetL( TUid::Uid( uid ) );

    return ret;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpGetAvailableUidL()
// Returns next available UId
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpGetAvailableUidL(
    const RMessage2& aMessage )
    {
    TUid uid = TUid::Uid( 0 );
    uid = iWidgetRegistry->GetAvailableUidL( aMessage.Int0() );

    return uid.iUid;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpGetLprojName()
// Returns language project name for directory which holds the localized content
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpGetLprojNameL(
    const RMessage2& aMessage )
    {
    TBuf<KWidgetRegistryVal> aLprojName;

    aLprojName.SetLength( aMessage.Int1() );
    aMessage.ReadL( 0, aLprojName );
    iWidgetRegistry->GetLprojNameL( aLprojName );
    aMessage.WriteL( 0, aLprojName );

    return aLprojName.Length();
    }

// ============================================================================
// CWidgetRegistryServerSession::OpSetActive()
// Set/Reset active status of the widget
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistryServerSession::OpSetActive( const RMessage2& aMessage )
    {
    TInt uid = aMessage.Int0();
    TInt status = aMessage.Int1();
    iWidgetRegistry->SetActive( TUid::Uid( uid ), status );
    }

// ============================================================================
// CWidgetRegistryServerSession::OpSetMiniView()
// Set/Reset Widget status for launched in MiniView
//
// @since 
// ============================================================================
//
void CWidgetRegistryServerSession::OpSetMiniView( const RMessage2& aMessage )
    {
    TInt uid = aMessage.Int0();
    TInt status = aMessage.Int1();
    iWidgetRegistry->SetMiniView( TUid::Uid( uid ), status );
    }

// ============================================================================
// CWidgetRegistryServerSession::OpSetFullView()
// Set/Reset Widget status in FullView
//
// @since 
// ============================================================================
//
void CWidgetRegistryServerSession::OpSetFullView( const RMessage2& aMessage )
    {
    TInt uid = aMessage.Int0();
    TInt status = aMessage.Int1();
    iWidgetRegistry->SetFullView( TUid::Uid( uid ), status );
    }

// ============================================================================
// CWidgetRegistryServerSession::OpSetPromptNeeded()
// Set/Reset Widget miniview platform access prompt is needed
//
// @since 
// ============================================================================
//
void CWidgetRegistryServerSession::OpSetPromptNeeded( const RMessage2& aMessage )
    {
    TInt uid = aMessage.Int0();
    TInt status = aMessage.Int1();
    TRAP_IGNORE(iWidgetRegistry->SetBlanketPermissionL( TUid::Uid( uid ), status ));
    }

// ============================================================================
// CWidgetRegistryServerSession::OpDisconnect()
// Disconnects session with server
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpDisconnect(
    const RMessage2& /*aMessage*/ )
    {
    TInt ret = KErrNone;
    CWidgetRegistryServer* server;

    server = static_cast<CWidgetRegistryServer*>(
        const_cast<CServer2*>( Server() ) );

    if ( server )
        {
        server->RemoveSession();
        }

    return ret;
    }

// ============================================================================
// CWidgetRegistryServerSession::OpSecurityPolicyId()
// Returns security policy Id
//
// @since 5.0
// ============================================================================
//
TInt CWidgetRegistryServerSession::OpSecurityPolicyId(
    const RMessage2& /*aMessage*/ )
    {
    return iWidgetRegistry->SecurityPolicyId();
    }

// ============================================================================
// CWidgetRegistryServerSession::WidgetRegistryServer()
// Returns server object
//
// @since 3.1
// ============================================================================
//
CWidgetRegistryServer& CWidgetRegistryServerSession::WidgetRegistryServer()
    {
    return static_cast<CWidgetRegistryServer&>(
        *const_cast<CServer2*>( Server() ) );
    }

// ============================================================================
// CWidgetRegistryServerSession::MarshalDataL()
// Creates and returns heap descriptor which holds contents of RWidgetInfoArray
// @since 3.1
// ============================================================================
//
HBufC8* CWidgetRegistryServerSession::MarshalDataL(
    RWidgetInfoArray& aWidgetInfoArr )
    {
    // Create a dynamic flat buffer to hold the data data
    const TInt KExpandSize = 128; // "Granularity" of dynamic buffer
    CBufFlat* buf = CBufFlat::NewL( KExpandSize );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf ); // Stream over the buffer
    CleanupClosePushL( stream );
    ExternalizeL( stream, aWidgetInfoArr );
    CleanupStack::PopAndDestroy( &stream );

    // Create a heap descriptor from the buffer
    HBufC8* des = HBufC8::NewL( buf->Size() );
    TPtr8 ptr( des->Des() );
    buf->Read( 0, ptr, buf->Size() );

    CleanupStack::PopAndDestroy( buf ); // Finished with the buffer
    return ( des );
    }

// ============================================================================
// CWidgetRegistryServerSession::ExternalizeL()
// Write data to aStream
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistryServerSession::ExternalizeL(
    RWriteStream& aStream,
    RWidgetInfoArray& aWidgetInfoArr )
    {
    TInt len = 0;

    for ( TInt i = 0; i < aWidgetInfoArr.Count(); i++ )
        {
        aStream.WriteInt32L( aWidgetInfoArr[i]->iUid.iUid );
        aStream.WriteInt32L( aWidgetInfoArr[i]->iFileSize );

        len = aWidgetInfoArr[i]->iBundleName->Length();
        aStream.WriteInt32L( len );
        aStream.WriteL( *(aWidgetInfoArr[i]->iBundleName), len );

        len = aWidgetInfoArr[i]->iDriveName->Length();
        aStream.WriteInt32L( len );
        aStream.WriteL( *(aWidgetInfoArr[i]->iDriveName), len );
        }
    }

// ============================================================================
// CWidgetRegistryServerSession::ResetWidgetInfo()
// delete each item in RWidgetInfoArray before closing it
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistryServerSession::ResetWidgetInfo(
    RWidgetInfoArray& aWidgetInfoArr )
    {
    for ( TInt i = 0; i < aWidgetInfoArr.Count(); i++ )
        {
        CWidgetInfo *item = aWidgetInfoArr[i];
        delete item ;
        }
    }

// End of File



