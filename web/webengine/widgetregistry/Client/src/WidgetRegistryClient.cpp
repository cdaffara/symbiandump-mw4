/*
* Copyright (c) 2003, 2004, 2005 Nokia Corporation and/or its subsidiary(-ies).
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
*
*/

#include <e32base.h>
#include <f32file.h>
#include <s32mem.h>
#include "WidgetRegistryClient.h"


// ============================================================================
// Starts the widget registry server
//
// @since 3.1
// ============================================================================
//
TInt StartServer()
    {
    const TUidType KServerUidType(
        KNullUid, KNullUid, KWidgetRegistryServerUid );

    RProcess server;
    TInt ret = server.Create(
        KWidgetRegistryImage, KNullDesC, KServerUidType );

    // Did we manage to create the thread/process?
    if (ret == KErrNone)
        {
        // Wait to see if the thread/process died during construction
        TRequestStatus serverDiedRequestStatus;
        server.Rendezvous( serverDiedRequestStatus );

        // do we have to abort?
        if  ( serverDiedRequestStatus != KRequestPending )
            {
            // abort the startup here
            server.Kill(0);
            }
        else
            {
            // start server
            server.Resume();
            }

        User::WaitForRequest( serverDiedRequestStatus );

        // Determine the reason for the server exit.
        const TInt exitReason = ( server.ExitType() == EExitPanic ) ?
            KErrGeneral : serverDiedRequestStatus.Int();
        ret = exitReason;

        // Finished with process handle
        server.Close();
        }

    return ret;
    }

// ============================================================================
// RWidgetRegistryClientSession::RWidgetRegistryClientSession()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
EXPORT_C RWidgetRegistryClientSession::RWidgetRegistryClientSession()
    {
    }

// ============================================================================
// RWidgetRegistryClientSession::Connect()
// Connect to widgetregistry server
//
// @since 3.1
// ============================================================================
//
EXPORT_C TInt RWidgetRegistryClientSession::Connect()
    {
    TInt startupAttempts = KWidgetRegistryServerStartupAttempts;

    for ( ;; )
        {
        TInt ret = CreateSession( KWidgetRegistryName, Version(),
            KWidgetRegistryServerAsynchronousSlotCount );

        if ( ret != KErrNotFound && ret != KErrServerTerminated )
            {
            return ret;
            }

        if ( startupAttempts-- == 0 )
            {
            return ret;
            }

        ret = StartServer();
        if ( ret != KErrNone && ret != KErrAlreadyExists )
            {
            return ret;
            }
        }
    }

// ============================================================================
// RWidgetRegistryClientSession::Version()
// Returns the version
//
// @since 3.1
// ============================================================================
//
EXPORT_C TVersion RWidgetRegistryClientSession::Version() const
    {
    const TVersion version(
        KWidgetRegistryClientVersionMajor,
        KWidgetRegistryClientVersionMinor,
        KWidgetRegistryClientVersionBuild );

    return version;
    }


// ============================================================================
// RWidgetRegistryClientSession::Disconnect()
// Disconnect from widgetregistry server
//
// @since 3.1
// ============================================================================
//
EXPORT_C TInt RWidgetRegistryClientSession::Disconnect()
    {
    SendReceive( EOpCodeWidgetRegistryDisconnect, TIpcArgs() );
    RSessionBase::Close();
    return KErrNone;
    }

// ============================================================================
// RWidgetRegistryClientSession::IsWidget()
// Returns true if the Uid falls within the range specified for widgets.
//
// @since 3.1
// ============================================================================
//
EXPORT_C TBool RWidgetRegistryClientSession::IsWidget( const TUid& aUid )
    {
    TBool retVal = EFalse;
    iMesgArgs = TIpcArgs( aUid.iUid );
    retVal = SendReceive( EOpCodeIsWidget, iMesgArgs );

    return retVal;
    }

// ============================================================================
// RWidgetRegistryClientSession::WidgetExists()
// Returns true if the widget is installed
//
// @since 3.1
// ============================================================================
//
EXPORT_C TBool RWidgetRegistryClientSession::WidgetExistsL(
    const TDesC& aBundleId )
    {
    TBool retVal = EFalse;
    TInt len = aBundleId.Length();
    iMesgArgs = TIpcArgs( &aBundleId, len );

    retVal = SendReceive( EOpCodeWidgetExists, iMesgArgs );
    return retVal;
    }


// ============================================================================
// RWidgetRegistryClientSession::IsWidgetRunning()
// Returns true if the widget is running.
//
// @since 3.1
// ============================================================================
//
EXPORT_C TBool RWidgetRegistryClientSession::IsWidgetRunning( const TUid& aUid )
    {
    TBool retVal = EFalse;
    iMesgArgs = TIpcArgs( aUid.iUid );
    retVal = SendReceive( EOpCodeIsWidgetRunning, iMesgArgs );

    return retVal;
    }

// ============================================================================
// RWidgetRegistryClientSession::WidgetSapiAccessState()
// Returns whether sapi access is prompted, promptless or denied.
//
// @since 5.0
// ============================================================================
//
EXPORT_C TInt RWidgetRegistryClientSession::WidgetSapiAccessState( const TUid& aUid )
    {
    TInt retVal = -1;
    iMesgArgs = TIpcArgs( aUid.iUid );
    retVal = SendReceive( EOpWidgetSapiAccessState, iMesgArgs );

    return retVal;
    }

// ============================================================================
// RWidgetRegistryClientSession::IsWidgetInMiniView()
// Returns true if the widget is in MiniView.
//
// @since 
// ============================================================================
//
EXPORT_C TBool RWidgetRegistryClientSession::IsWidgetInMiniView( const TUid& aUid )
    {
    TBool retVal = EFalse;
    iMesgArgs = TIpcArgs( aUid.iUid );
    retVal = SendReceive( EOpCodeIsWidgetInMiniView, iMesgArgs );

    return retVal;
    }

// ============================================================================
// RWidgetRegistryClientSession::IsWidgetInFullView()
// Returns true if the widget is in FullView.
//
// @since 
// ============================================================================
//
EXPORT_C TBool RWidgetRegistryClientSession::IsWidgetInFullView( const TUid& aUid )
    {
    TBool retVal = EFalse;
    iMesgArgs = TIpcArgs( aUid.iUid );
    retVal = SendReceive( EOpCodeIsWidgetInFullView, iMesgArgs );

    return retVal;
    }

// ============================================================================
// RWidgetRegistryClientSession::IsBlanketPermGranted()
// Returns true if the widget is granted blanket permission.
//
// @since 
// ============================================================================
EXPORT_C TBool RWidgetRegistryClientSession::IsBlanketPermGranted( const TUid& aUid )
    {
    TBool retVal = EFalse;
    iMesgArgs = TIpcArgs( aUid.iUid );
    retVal = SendReceive( EOpCodeIsWidgetPromptNeeded, iMesgArgs );

    return retVal;
    }

// ============================================================================
// RWidgetRegistryClientSession::WidgetCount()
// Returns count of widgets installed.
//
// @since 3.1
// ============================================================================
//
EXPORT_C TInt RWidgetRegistryClientSession::WidgetCount( TInt& aCount )
    {
    TInt ret = 0;
    iMesgArgs = TIpcArgs();
    ret = SendReceive( EOpCodeWidgetCount, iMesgArgs );
    aCount = ret;

    return ret;
    }

// ============================================================================
// RWidgetRegistryClientSession::GetWidgetPath()
// Returns path of the widget with a particular UId
//
// @since 3.1
// ============================================================================
//
EXPORT_C void RWidgetRegistryClientSession::GetWidgetPath(
    const TUid& aUid,
    TDes& aWidgetPath )
    {
    iMesgArgs = TIpcArgs( &aWidgetPath, aWidgetPath.Length(), aUid.iUid );
    TInt len = SendReceive( EOpCodeGetWidgetPath, iMesgArgs );

    if ( len >= 0 )
        {
        aWidgetPath.SetLength( len );
        }
    }

// ============================================================================
// RWidgetRegistryClientSession::GetWidgetUid()
// Returns uid of the widget with a particular bundle identifier
//
// @since 3.1
// ============================================================================
//
EXPORT_C TInt RWidgetRegistryClientSession::GetWidgetUidL(
    const TDesC& aBundleId )
    {
    TInt uid = 0;
    iMesgArgs = TIpcArgs( &aBundleId,  aBundleId.Length() );

    uid = SendReceive( EOpCodeGetWidgetUid, iMesgArgs );
    return uid;
    }

// ============================================================================
// RWidgetRegistryClientSession::GetWidgetUidForUrl()
// Returns UId of the widget with a patricular html path
//
// @since 3.1
// ============================================================================
//
EXPORT_C TInt RWidgetRegistryClientSession::GetWidgetUidForUrl(
    const TDesC& aUrl )
    {
    TInt uid = 0;
    iMesgArgs = TIpcArgs( &aUrl, aUrl.Length() );

    uid = SendReceive( EOpCodeGetWidgetUidForUrl, iMesgArgs );
    return uid;
    }

// ============================================================================
// RWidgetRegistryClientSession::GetAvailableUidL()
// Return the next available UId
//
// @since 3.1
// ============================================================================
//
EXPORT_C TUid RWidgetRegistryClientSession::GetAvailableUidL( TUint aDriveLetter )
    {
    TInt uid = 0;
    iMesgArgs = TIpcArgs( aDriveLetter );

    uid = SendReceive( EOpCodeGetAvailableUid, iMesgArgs );
    if ( KNullUid.iUid == uid )
      {
      User::Leave( KErrNotFound );
      }
    return TUid::Uid( uid );
    }

// ============================================================================
// RWidgetRegistryClientSession::GetWidgetBundleId()
// Returns bundle Id of the widget with a particular UId
//
// @since 3.1
// ============================================================================
//
EXPORT_C void RWidgetRegistryClientSession::GetWidgetBundleId(
    const TUid& aUid,
    TDes& aWidgetBundleId )
    {
    iMesgArgs = TIpcArgs(
        &aWidgetBundleId, aWidgetBundleId.Length(), aUid.iUid );
    TInt len = SendReceive( EOpCodeGetWidgetBundleId, iMesgArgs );

    if ( len >= 0 )
        {
        aWidgetBundleId.SetLength( len );
        }
    }

// ============================================================================
// RWidgetRegistryClientSession::GetWidgetBundleName()
// Returns bundle display name of the widget with a particular UId
//
// @since 3.1
// ============================================================================
//
EXPORT_C void RWidgetRegistryClientSession::GetWidgetBundleName(
    const TUid& aUid,
    TDes& aWidgetBundleName )
    {
    iMesgArgs = TIpcArgs(
        &aWidgetBundleName, aWidgetBundleName.Length(), aUid.iUid );
    TInt len = SendReceive( EOpCodeGetWidgetBundleName, iMesgArgs );

    if ( len >= 0 )
        {
        aWidgetBundleName.SetLength( len );
        }
    }

// ============================================================================
// RWidgetRegistryClientSession::GetWidgetPropertyValue()
// Returns info.plist key value string for the widget with a particular UId
//
// @since 3.1
// ============================================================================
//
EXPORT_C
CWidgetPropertyValue* RWidgetRegistryClientSession::GetWidgetPropertyValueL(
    const TUid& aUid,
    TWidgetPropertyId aPropertyId)
    {
    const TInt maxSize = 2*KWidgetPropertyValSerializeMaxLength; // need 16 bit chars
    CBufFlat* buf = CBufFlat::NewL( maxSize );
    CleanupStack::PushL( buf );
    buf->ExpandL( 0, maxSize );
    TPtr8 p( buf->Ptr(0) );
    User::LeaveIfError(

        SendReceive( EOpCodeGetWidgetPropertyValue,
                     TIpcArgs( aUid.iUid, aPropertyId, &p ) )
        );

    // deserialize
    RDesReadStream stream( p );
    CleanupClosePushL( stream );

    CWidgetPropertyValue* value = CWidgetPropertyValue::NewL();
    CleanupStack::PushL( value );

    value->DeserializeL( stream );

    CleanupStack::Pop(); // value
    CleanupStack::PopAndDestroy( 2, buf ); // stream, buf
    return value;
    }

// ============================================================================
// RWidgetRegistryClientSession::InstalledWidgets()
// Returns widget info for all the installed widgets.
//
// @since 3.1
// ============================================================================
//
EXPORT_C TInt RWidgetRegistryClientSession::InstalledWidgetsL(
    RWidgetInfoArray& aWidgetInfoArr )
    {
    TInt status = KErrNone;
    // gets total size of all elements in the widgetinfo array in terms of
    // buffer length; negative value means one of the system-wide error
    TInt ret = SendReceive( EOpCodeInstalledWidgetsPhase1, TIpcArgs() );
    if ( ret > 0 )
        {
        CBufFlat* responseBuff = NULL;
        TPtr8 responsePtr( NULL, 0 );
        responseBuff = CBufFlat::NewL( ret );
        CleanupStack::PushL( responseBuff );
        responseBuff->ExpandL( 0, ret );
        responsePtr.Set( ( TUint8* ) responseBuff->Ptr( 0 ).Ptr(), ret, ret );

        iMesgArgs = TIpcArgs( &responsePtr );
        TInt count = SendReceive( EOpCodeInstalledWidgetsPhase2, iMesgArgs );

        if ( count > 0 )
            {
            RBufReadStream stream;
            // Unpack the response.
            stream.Open( *responseBuff, 0 );
            CleanupClosePushL( stream );
            // Read the widget info.
            DeserializeWidgetInfoL( stream, aWidgetInfoArr, count );
            CleanupStack::PopAndDestroy( &stream );
            }

        CleanupStack::PopAndDestroy( responseBuff );
        }

   return status;
   }

// ============================================================================
// RWidgetRegistryClientSession::RunningWidgets()
// Returns widget info for all the running widgets.
//
// @since 3.1
// ============================================================================
//
EXPORT_C TInt RWidgetRegistryClientSession::RunningWidgetsL(
    RWidgetInfoArray& widgetInfoArr )
    {
    TInt status = KErrNone;
    // gets total size of all elements in the widgetinfo array in terms of
    // buffer length; negative value means one of the system-wide error
    TInt ret = SendReceive( EOpCodeRunningWidgetsPhase1, TIpcArgs() );

    if ( ret > 0 )
        {
        CBufFlat* responseBuff = NULL;
        TPtr8 responsePtr( NULL, 0 );
        responseBuff = CBufFlat::NewL( ret );
        CleanupStack::PushL( responseBuff );
        responseBuff->ExpandL( 0, ret );
        responsePtr.Set( ( TUint8* ) responseBuff->Ptr( 0 ).Ptr(), ret, ret );

        iMesgArgs = TIpcArgs( &responsePtr );
        TInt count = SendReceive( EOpCodeRunningWidgetsPhase2, iMesgArgs );

        if ( count > 0 )
            {
            RBufReadStream stream;
            // Unpack the response.
            stream.Open( *responseBuff, 0 );
            CleanupClosePushL( stream );
            // Read the widget info.
            DeserializeWidgetInfoL( stream, widgetInfoArr, count );
            CleanupStack::PopAndDestroy( &stream );
            }

        CleanupStack::PopAndDestroy( responseBuff );
        }

    return status;
    }

// ============================================================================
// RWidgetRegistryClientSession::RegisterWidget()
// Registers the widget
//
// @since 3.1
// ============================================================================
//
EXPORT_C void RWidgetRegistryClientSession::RegisterWidgetL(
    const RPointerArray<CWidgetPropertyValue>&  aPropertyValues )
    {
    CBufFlat* buf = MarshalPropertyValuesL( aPropertyValues );

    CleanupStack::PushL( buf );
    TPtr8 p( buf->Ptr(0) );
    User::LeaveIfError(

        SendReceive( EOpCodeRegisterWidget, TIpcArgs( &p ) )

        );
    CleanupStack::PopAndDestroy( buf ); 
    }

// ============================================================================
// RWidgetRegistryClientSession::DeRegisterWidget()
// Deregister the widget
//
// @since 3.1
// ============================================================================
//
EXPORT_C void RWidgetRegistryClientSession::DeRegisterWidgetL(
    const TUid& aUid )
    {
    iMesgArgs = TIpcArgs( aUid.iUid );

    TInt error = SendReceive( EOpCodeDeRegisterWidget, iMesgArgs );
    User::LeaveIfError( error );
    }

// ============================================================================
// RWidgetRegistryClientSession::SetActive()
// Set/Reset active status of the widget
//
// @since 3.1
// ============================================================================
//
EXPORT_C void RWidgetRegistryClientSession::SetActive(
    const TUid& aUid, TInt aStatus )
    {
    iMesgArgs = TIpcArgs( aUid.iUid, aStatus );

    TInt error = SendReceive( EOpCodeSetActive, iMesgArgs );
    User::LeaveIfError( error );
    }
// ============================================================================
// RWidgetRegistryClientSession::SetMiniViewL()
// Sets when widget is launched in miniview
//
// @since 5.0
// ============================================================================
//
EXPORT_C void RWidgetRegistryClientSession::SetMiniViewL(
    const TUid& aUid, TInt aStatus )
    {
    iMesgArgs = TIpcArgs( aUid.iUid, aStatus );
    TInt error = SendReceive( EOpCodeSetWidgetInMiniView, iMesgArgs );
    User::LeaveIfError( error );
    }

// ============================================================================
// RWidgetRegistryClientSession::SetFullViewL()
// Sets when widget is launched in fullview
//
// @since 5.0
// ============================================================================
//
EXPORT_C void RWidgetRegistryClientSession::SetFullViewL(
    const TUid& aUid, TInt aStatus )
    {
    iMesgArgs = TIpcArgs( aUid.iUid, aStatus );
    TInt error = SendReceive( EOpCodeSetWidgetInFullView, iMesgArgs );
    User::LeaveIfError( error );
    }

// ============================================================================
// RWidgetRegistryClientSession::SetBlanketPermissionL()
// Set/Reset blanket permission for widget
//
// @since 5.0
// ============================================================================
//
EXPORT_C void RWidgetRegistryClientSession::SetBlanketPermissionL(
    const TUid& aUid, TInt aStatus )
    {
    iMesgArgs = TIpcArgs( aUid.iUid, aStatus );
    TInt error = SendReceive( EOpCodeSetWidgetPromptNeeded, iMesgArgs );
    User::LeaveIfError( error );
    }

// ============================================================================
// RWidgetRegistryClientSession::MarshalPropertyValuesL()
// Creates and returns heap descriptor which holds contents of property values
//
// @since 3.1
// ============================================================================
//
CBufFlat* RWidgetRegistryClientSession::MarshalPropertyValuesL(
    const RPointerArray<CWidgetPropertyValue>&  aPropertyValues ) const
    {
    CBufFlat* buf = CBufFlat::NewL( 512 );
    CleanupStack::PushL( buf );

    RBufWriteStream stream( *buf );
    CleanupClosePushL( stream );

    TInt i = 0;
    for ( ; i < EWidgetPropertyIdCount; ++i )
        {
        aPropertyValues[i]->SerializeL( stream );
        }

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::Pop( buf ); 

    return buf;
    }

// ============================================================================
// RWidgetRegistryClientSession::DeserializeWidgetInfoL()
// Initializes widgetinfo array with the contents of aStream
//
// @since 3.1
// ============================================================================
//
void RWidgetRegistryClientSession::DeserializeWidgetInfoL(
    RReadStream& aStream,
    RWidgetInfoArray& aWidgetInfoArr,
    TInt aCount )
    {

    for ( TInt i = 0; i < aCount; i++ )
        {
        CWidgetInfo* tempInfo = new ( ELeave ) CWidgetInfo();
        CleanupStack::PushL( tempInfo );
        tempInfo->iUid.iUid = aStream.ReadInt32L();
        tempInfo->iFileSize = aStream.ReadInt32L();

        TInt len = 0;
        TBuf<KWidgetRegistryVal> tempNameBuf;
        TBuf<KMaxDriveName+1> tempDriveBuf;

        len = aStream.ReadInt32L();
        aStream.ReadL( tempNameBuf, len );
        *(tempInfo->iBundleName) = tempNameBuf;

        len = aStream.ReadInt32L();
        aStream.ReadL( tempDriveBuf, len );
        *(tempInfo->iDriveName) = tempDriveBuf;

        aWidgetInfoArr.AppendL( tempInfo );
        CleanupStack::Pop( tempInfo );
        }
    }

// ============================================================================
// RWidgetRegistryClientSession::GetLprojName()
// Get the language project name which is the directory to hold localized
// resources
//
// @since 3.1
// ============================================================================
//
EXPORT_C void RWidgetRegistryClientSession::GetLprojName( TDes& aLprojName )
    {
    iMesgArgs = TIpcArgs( &aLprojName, aLprojName.Length() );
    TInt len = SendReceive( EOpCodeGetLprojName, iMesgArgs );

    if ( len >= 0 )
        {
        aLprojName.SetLength( len );
        }
    }

// ============================================================================
// RWidgetRegistryClientSession::SecurityPolicyId()
// Returns security policyId.
//
// @since 5.0
// ============================================================================
//
EXPORT_C TInt RWidgetRegistryClientSession::SecurityPolicyId()
    {
    TInt ret = 0;
    iMesgArgs = TIpcArgs();
    ret = SendReceive( EOpCodeSecurityPolicyId, iMesgArgs );
    return ret;
    }

// End of File
