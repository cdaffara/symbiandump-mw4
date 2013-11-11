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
* Description:  Manages list of widget entries.
*
*/


#include <e32base.h>
#include <f32file.h>
#include <s32file.h>
#include <s32mem.h>
#include <bautils.h>

#include "WidgetEntry.h"
#include "WidgetRegistry.h"
#include "WidgetInstaller.h"
#include <libxml2_globals.h>
#include <libc/stdlib.h>
#include <libxml2_parser.h>
#include <libxml2_tree.h>
#include <xmlengxestd.h>
#include "UidAllocator.h"
#if defined( BRDO_WRT_SECURITY_MGR_FF )
#include <rtsecmanager.h>
#endif
#include <e32property.h>

// CONSTANTS

static const TInt KDefaultWidgetCount = 20; // used for initial RArray size
static const TInt KAppArchTimeout = 10000000; // 10 seconds in microseconds
static const TInt KAppArchDelayInterval = 500000; // 500 milliseonds in microseconds

enum
    {
    EAppListFlagEntry = 1
    };

enum
    {
    EInstallListFlagEntry = 1
    };

// Be sure that length of literal pathname strings is less than
// KWidgetRegistryMaxPathName in header file.
//
// drive letter a: is used as a placeholder, it will be set as needed
_LIT( KWidgetInstallPath,
      "a:\\private\\10282822\\" );
_LIT( KWidgetEntryStoreBinaryFile,
      "a:\\private\\10282f06\\WidgetEntryStore.dat" );
_LIT( KWidgetEntryStoreXmlFile,
      "a:\\private\\10282f06\\WidgetEntryStore.xml" );
_LIT( KWidgetEntryStoreXmlTempFile,
      "a:\\private\\10282f06\\WidgetEntryStoreTemp.xml" );
_LIT( KWidgetDirFile, "widget_lproj.xml" );
_LIT( KWidgetAccessPolicy, "WidgetAccessPolicy.xml" );
_LIT( KWidgetPolicyIdFile, "WidgetPolicyId.dat" );

_LIT( KWidgetDefaultLangDir, "en" );
_LIT8( KLangID, "LangID" );
_LIT8( KLangDir, "LangDir" );

// for exteranlize
_LIT( KXmlHeader,
      "<?xml version=\"1.0\" encoding=\"UTF-16\" standalone=\"yes\" ?>" );
_LIT( KXmlRootStart, "<widgetregistry>" );
_LIT( KXmlRootEnd, "</widgetregistry>" );
_LIT( KWidgetEntryStart, "<entry>" );
_LIT( KWidgetEntryEnd, "</entry>" );
_LIT( KXmlNewline, "\x0D\x0A" ); // DOS/Symbian style works with XML parsers

// for internalize
_LIT8( KWidgetRegistry, "widgetregistry" );
_LIT8( KEntry, "entry" );

LOG_NAMES( "widreg", "widreg.txt" )

void XmlDocFree( TAny* aPtr )
    {
    __ASSERT_DEBUG( aPtr, User::Invariant() );
    xmlDocPtr ptr( (xmlDocPtr)(aPtr) );
    xmlFreeDoc( ptr );
    }

// ============================================================================
// Traverse to the next Node
//
// @param aNode: current node
// @since 3.1
// @return next node
// ============================================================================
//
xmlNode* TraverseNextNode( xmlNode* n )
    {
    // depth first
    if ( n->children )
        {
        n = n->children;
        }
    else
        {
        // go up while no sibling
        while ( n->parent && !n->next )
            {
            n = n->parent;
            }
        // sibling?
        if ( n->next )
            {
            n = n->next;
            }
        else // done
            {
            n = NULL;
            }
        }
    return n;
    }
// ============================================================================
// Changes the Publish & Subscribe key value
// ============================================================================
//

static void NotifyWidgetAltered()
    {
    const TUid KMyPropertyCat = { 0x10282E5A };
    enum TMyPropertyKeys { EMyPropertyAltered = 110 };
    TInt altered( 1 );
    RProperty::Set( KMyPropertyCat, EMyPropertyAltered , altered );
    }
// ============================================================================
// CWidgetRegistry::NewL()
// two-phase constructor
//
// @since 3.1
// ============================================================================
//
CWidgetRegistry* CWidgetRegistry::NewL( RFs& aFs )
    {
    CWidgetRegistry* widgetRegistry = new ( ELeave ) CWidgetRegistry( aFs );
    CleanupStack::PushL( widgetRegistry );
    TRAP_IGNORE( widgetRegistry->ConstructL() );
    CleanupStack::Pop( widgetRegistry );
    return widgetRegistry;
    }

// ============================================================================
// CWidgetRegistry::CWidgetRegistry()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
CWidgetRegistry::CWidgetRegistry( RFs& aFs ):
    iFs( aFs ),
    iWidgetInstallPath( KWidgetInstallPath ),
    iRegistryBinaryFileName( KWidgetEntryStoreBinaryFile ),
    iRegistryXmlFileName( KWidgetEntryStoreXmlFile ),
    iRegistryXmlTempFileName( KWidgetEntryStoreXmlTempFile ),
    iPolicyId( 0 )
    {
    }

// ============================================================================
// CWidgetRegistry::~CWidgetRegistry()
// destructor
//
// @since 3.1
// ============================================================================
//
CWidgetRegistry::~CWidgetRegistry()
    {
    iEntries.ResetAndDestroy();
    iUsedUids.Close();
    // iFs not owned
    iAppArch.Close();
    delete iInstaller;
    iLangDirList.ResetAndDestroy();
    delete iXmlProcessor;
	delete iApaAppListNotifier;
    iFs.Close();
    LOG_DESTRUCT;
    }

// ============================================================================
// CWidgetRegistry::ConstructL()
// Symbian second phase constructor
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::ConstructL()
    {
    LOG_CONSTRUCTL;
    LOG_OPEN;
    LOG( "CWidgetRegistry::ConstructL internalize" );

    User::LeaveIfError( iFs.Connect() );
    iFs.CreatePrivatePath( EDriveC );
    iFs.SetSessionToPrivate( EDriveC );
    User::LeaveIfError( iAppArch.Connect() );
    iInstaller = CWidgetInstaller::NewL();

    // If internalizing leaves, then the registry will be out-of-sync
    // with the installed widgets and apparc.  But that's okay since
    // it should be detected and corrected once the resource limit
    // that caused the leave is removed.
    TBool dirtyFlag = EFalse;
    TInt parseError = KErrNone;
    iXmlProcessor = CWidgetRegistryXml::NewL();
    // For first attempt assume usual case that things are consistent
    // and the registry entry file can be parsed and used.
    TRAP_IGNORE( InternalizeL( EFalse, // aDoConsistency,
                               EFalse, // aIgnoreParseError
                               dirtyFlag,
                               parseError ) );
    if ( parseError )
        {
        // Special case where we need to do consistency because
        // the widget registry entry file is corrupt.
        TRAP_IGNORE( InternalizeL( ETrue, // aDoConsistency,
                                   ETrue, // aIgnoreParseError
                                   dirtyFlag,
                                   parseError ) );
        }
    if ( dirtyFlag )
        {
        // Basically same reason to ignore leaves here.
        TRAP_IGNORE( ExternalizeL() );
        }

    LOG( " ctor internalize done" );

#if defined( BRDO_WRT_SECURITY_MGR_FF )
    FetchSecurityPolicyIdL();
#endif

    LOG1( "ConstructL internalize done, registry count %d",
              iEntries.Count() );
    LOG_CLOSE;
    iApaAppListNotifier = CApaAppListNotifier::NewL(this,CActive::EPriorityStandard);
    }

// ============================================================================
// CWidgetRegistry::FetchSecurityPolicyIdL()
// Get policyId from security manager
//
// @since 5.0
// ============================================================================
//
TInt CWidgetRegistry::FetchSecurityPolicyIdL()
    {
#if defined( BRDO_WRT_SECURITY_MGR_FF )
    if ( !iPolicyId )
        {
        if ( KErrNone == iFs.ShareProtected() )
            {
            TFileName secPolicyFileName;
            RFile secPolicyFile;
            iFs.PrivatePath( secPolicyFileName );
#ifdef __WINSCW__
            secPolicyFileName.Insert( 0, _L( "C:" ));
#else
            secPolicyFileName.Insert( 0, _L( "Z:" ));
#endif
            secPolicyFileName.Append( KWidgetAccessPolicy );

            if ( KErrNone == secPolicyFile.Open( iFs, secPolicyFileName, EFileShareAny ) )
                {
                CleanupClosePushL( secPolicyFile );
                CRTSecManager* secMgr = CRTSecManager::NewL();
                iPolicyId = secMgr->SetPolicy( secPolicyFile );
                CleanupStack::PopAndDestroy( &secPolicyFile );

                CleanupStack::PushL( secMgr );
                // read old policyId, unset old policy file; store new policyId
                RFile policyIdFile;
                TFileName policyIdFileName;
                iFs.PrivatePath( policyIdFileName );
                policyIdFileName.Insert( 0, _L( "C:" ));
                policyIdFileName.Append( KWidgetPolicyIdFile );

                TInt error = KErrNotFound;
                error = policyIdFile.Open( iFs, policyIdFileName, EFileShareAny );
                // policy Id file exists
                if ( error == KErrNone )
                    {
                    CleanupClosePushL( policyIdFile );
                    RFileReadStream readStream;
                    CleanupClosePushL( readStream );
                    readStream.Attach( policyIdFile );

                    TInt oldPolicyId = 0;
                    TRAP_IGNORE( oldPolicyId = readStream.ReadInt32L() );
                    // clean the old policy file
                    if ( oldPolicyId > 0 )
                        {
                        secMgr->UnSetPolicy( oldPolicyId );
                        }

                    CleanupStack::PopAndDestroy( &readStream );
                    }
                // create policy Id file
                else if ( error == KErrNotFound )
                    {
                    User::LeaveIfError( policyIdFile.Create( iFs, policyIdFileName, EFileShareAny ) );
                        CleanupClosePushL( policyIdFile );
                    }

                // we should have a good policyIdFile by now, store iPolicyId there
                    RFileWriteStream writeStream;
                    CleanupClosePushL( writeStream );
                    writeStream.Replace( iFs, policyIdFileName, EFileShareAny );

                    TRAP_IGNORE( writeStream.WriteInt32L( iPolicyId ) );

                CleanupStack::PopAndDestroy( 2, &policyIdFile );// writeStream, policyIdFile

                CleanupStack::PopAndDestroy( secMgr );
                }
            }
        }
#else
	iPolicyId = KErrNotSupported;
#endif    
    return iPolicyId;
    }

// ============================================================================
// CWidgetRegistry::GetWidgetEntryL()
// Get widget entry by UID or leave with KErrNotFound
//
// @since 3.1
// ============================================================================
//
CWidgetEntry& CWidgetRegistry::GetWidgetEntryL( const TUid& aUid ) const
    {
    for( TInt i = 0; i < iEntries.Count(); i++)
        {
        CWidgetEntry* entry = iEntries[i];
        if ( TUid::Uid( (*entry)[EUid] ) == aUid )
            {
            return *entry;
            }
        }
    User::Leave( KErrNotFound );
    return *iEntries[0];  // just for compiler
    }

// ============================================================================
// CWidgetRegistry::GetWidgetEntry()
// Get the widget entry
//
// @since 3.1
// ============================================================================
//
// TODO use leaving version GetWidgetEntryL everywhere and delete this version
TInt CWidgetRegistry::GetWidgetEntry(
    const TUid& aUid,
    CWidgetEntry*& aEntry) const
    {
    for(TInt i = 0;i < iEntries.Count();i++)
        {
        CWidgetEntry* entry = iEntries[i];
        if ( TUid::Uid( (*entry)[EUid] ) == aUid )
            {
            aEntry = entry;
            return i;
            }
        }
    return -1;
    }

// ============================================================================
// CWidgetRegistry::GetWidgetEntry()
// Get the widget entry
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistry::GetWidgetEntry(
    const TDesC& aBundleId,
    CWidgetEntry*& aEntry) const
    {
    for(TInt i = 0;i < iEntries.Count();i++)
        {
        CWidgetEntry* entry = iEntries[i];
        const TDesC& widgetBundleId = (*entry)[EBundleIdentifier];
        if ( widgetBundleId.CompareF( aBundleId ) == 0 )
            {
            aEntry = entry;
            return i;
            }
        }
    return -1;
    }

// ============================================================================
// CWidgetRegistry::GetWidgetEntry()
// Returns the WidgetEntry at a particular index
//
// @since 3.1
// ============================================================================
//
const CWidgetEntry* CWidgetRegistry::GetWidgetEntry( const TInt& aPos )
    {
    return iEntries[aPos];
    }


// ============================================================================
// CWidgetRegistry::InsertL()
// Insert the widget entry into the list if BundleID is not already in
// the list.
//
// @since 3.1
// ============================================================================
//
TInt CWidgetRegistry::InsertL( CWidgetEntry* aEntry )
    {
    CWidgetEntry* entry = NULL;
    TDesC bundleID = (*aEntry)[EBundleIdentifier];
    TInt pos  = GetWidgetEntry( bundleID, entry );
    if ( pos == KErrNotFound )
        {
        iUsedUids.AppendL( TUid::Uid( (*aEntry)[EUid] ) );
        TInt error = iEntries.Append( aEntry );
        if ( KErrNone != error )
            {
            TInt upos = iUsedUids.FindReverse( TUid::Uid( (*aEntry)[EUid] ) );
            iUsedUids.Remove( upos );
            User::Leave( error );
            }
        }
    return pos;
    }

// ============================================================================
// CWidgetRegistry::Remove()
// Remove the widget entry from the list
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::Remove( const TDesC& aBundleId )
    {
    CWidgetEntry* entry = NULL;
    TInt pos  = GetWidgetEntry( aBundleId, entry );

    if ( pos != -1 )
        {
        TInt upos = iUsedUids.Find( TUid::Uid( (*entry)[EUid] ) );
        if ( upos >= 0 )
            {
            iUsedUids.Remove( upos );
            }

        iEntries.Remove( pos );

        delete entry;
        }
    }

// ============================================================================
// CWidgetRegistry::Remove()
// Remove the widget entry from the list
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::Remove( const TUid& aUid )
    {
    CWidgetEntry* entry = NULL;
    TInt pos  = GetWidgetEntry( aUid, entry );

    if ( pos != -1 )
        {
        TInt upos = iUsedUids.Find( TUid::Uid( (*entry)[EUid] ) );
        if ( upos >= 0 )
            {
            iUsedUids.Remove( upos );
            }

        iEntries.Remove( pos );
        delete entry;
        }

    }


// ============================================================================
// CWidgetRegistry::InternalizeL()
// Read entry info from data file into memory
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::InternalizeL( TBool doConsistency, // in param
                                    TBool aIgnoreParseError, // in param
                                    TBool& aDirtyFlag, // out param
                                    TInt& aParseError )// out param
    {
    LOG_OPEN;
    LOG( "Internalize" );

    // prepare for consistency enforcement
    RArray<TUid> appArchList( KDefaultWidgetCount );
    RArray<TInt> appArchListFlags( KDefaultWidgetCount );
    // default is internalization just imports files without
    // modification, if dirty flag gets set to true, do externalize
    // after internalize to bring files into sync with modified
    // registry contents
    aDirtyFlag = EFalse;

    // internal dirty flag, will be copied to arg dirty flag at
    // end if no leave occurs
    TBool dirtyFlag = EFalse;

    // empty the registry
    iEntries.ResetAndDestroy();
    iUsedUids.Reset();

    CleanupClosePushL( appArchList );
    CleanupClosePushL( appArchListFlags );
    if ( doConsistency )
        {
        doConsistency = AppArchWidgets( appArchList, appArchListFlags );
        }
    if ( doConsistency )
        {

        // UIDs are the key differentiator of apps in app arch.  The
        // widget implementation used BundleID to differentiate
        // widgets on installation (overwrite is keyed by BundleID).
        // But once widgets are installed, they are assigned a unique
        // UID.  For UID allocation purposes, it is important that
        // UIDs already known to app arch be reserved.
        for ( TInt i = 0; i < appArchList.Count(); i++ )
            {
            if ( KErrNone != iUsedUids.Append( (appArchList)[i] ) )
                {
                // no recovery possible
                doConsistency = EFalse;
                break;
                }
            }
        }
    LOG2( " iUsedUids %d and doConsistency %d",
          (TInt)(iUsedUids.Count()), (TInt)doConsistency );

    TRAPD( error, GetLprojNameL( iLprojName ) );
    if ( KErrNone != error )
        {
        // on error use english
        iLprojName = _L("en");
        }

    // List all drives in the system
    TDriveList driveList;
    User::LeaveIfError( iFs.DriveList(driveList) );

    // Check all drives but Z and D.  Scan from Y to A because that is
    // the order that the loader and AppArch follow when "shadowing"
    // UID identical apps. Drive Z is ignored because it is used for
    // the phone ROM image and is not an installation location for
    // widgets. Drive D is ignored because it is a temporary RAM disk
    // and not a widget install location.
    for ( TInt driveNumber = EDriveY; driveNumber >= EDriveA; driveNumber-- )
        {
        // The drives that will be filtered out are the same ones that
        // WidgetInstaller filters out in CWidgetUIHandler::SelectDriveL()
        if ( (EDriveD == driveNumber)
             || !driveList[driveNumber] )
            {
            // EDriveD is a temporary drive usually a RAM disk
            continue;
            }

        TVolumeInfo volInfo;
        if ( iFs.Volume( volInfo, driveNumber ) != KErrNone )
            {
            // volume is not usable (e.g. no media card inserted)
            continue;
            }
        if ( (volInfo.iDrive.iType == EMediaNotPresent) ||
             (volInfo.iDrive.iType == EMediaRom) ||
             (volInfo.iDrive.iType == EMediaRemote) ||
             (volInfo.iDrive.iDriveAtt & KDriveAttRom) ||
             (volInfo.iDrive.iDriveAtt & KDriveAttSubsted) )
            {
            // not a suitable widget install drive
            continue;
            }

        // found a usable drive
        TDriveUnit driveUnit( driveNumber );
        LOG1( " Drive %c", (TUint)(driveUnit.Name()[0]) );

        // prepare for consistency enforcement
        CDir* installedListForDrive = NULL;
        RArray<TInt> installedListForDriveFlags;
        if ( doConsistency )
            {
            doConsistency = InstallDirWidgets( driveUnit,
                                               installedListForDrive,
                                               installedListForDriveFlags );
            }
        LOG1( " after InstallDirWidgets doConsistency %d",
                  (TInt)doConsistency );

        // direct install path to this drive
        iWidgetInstallPath[0] = driveUnit.Name()[0];

        // which persistent data files exist?
        iRegistryBinaryFileName[0] = driveUnit.Name()[0];
        TBool binaryExists = BaflUtils::FileExists( iFs,
                                                    iRegistryBinaryFileName );

        iRegistryXmlFileName[0] = driveUnit.Name()[0];
        TBool xmlExists = BaflUtils::FileExists( iFs,
                                                 iRegistryXmlFileName );

        // Here internalizing a single drive.  If internalizing
        // leaves, then the registry will be out-of-sync with the
        // installed widgets and apparc.  But that's okay since it
        // should be detected and corrected once the resource limit
        // that caused the leave is removed.
        if ( xmlExists )
            {
            TRAP( aParseError,
                  InternalizeXmlL( iRegistryXmlFileName,
                                   driveUnit,
                                   doConsistency,
                                   appArchList,
                                   appArchListFlags,
                                   installedListForDrive,
                                   installedListForDriveFlags,
                                   dirtyFlag ) );
            }
        else if ( binaryExists )
            {
            TRAP( aParseError,
                  InternalizeBinaryL( iRegistryBinaryFileName,
                                      driveUnit,
                                      doConsistency,
                                      appArchList,
                                      appArchListFlags,
                                      installedListForDrive,
                                      installedListForDriveFlags,
                                      dirtyFlag ) );
            }
        if ( !aIgnoreParseError && aParseError )
            {
            User::Leave( aParseError );
            }

        if ( doConsistency )
            {
            InstallDirConsistency( installedListForDrive,
                                   installedListForDriveFlags,
                                   dirtyFlag );
            }
        delete installedListForDrive;
        installedListForDriveFlags.Close();
        } // for
    if ( doConsistency )
        {
        AppArchListConsistency( appArchList, appArchListFlags );
        }    
    CleanupStack::PopAndDestroy( 2, &appArchList );//appArchListFlags, appArchList

    aDirtyFlag = dirtyFlag;
    LOG1( "Internalize done, dirty flag %d", (TInt)dirtyFlag );
    LOG_CLOSE;
    }

// ============================================================================
// CWidgetRegistry::InternalizeBinaryL()
// Read entry info from data file into memory
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::InternalizeBinaryL( const TDesC& aFileName,
                                          const TDriveUnit& aDriveUnit,
                                          TBool aDoConsistency,
                                          RArray<TUid>& aAppArchList,
                                          RArray<TInt>& aAppArchListFlags,
                                          const CDir* aInstalledListForDrive,
                                          RArray<TInt>& aInstalledListForDriveFlags,
                                          TBool& aDirtyFlag )
    {
    LOG( "InternalizeBinaryL" );
    RFile file;
    User::LeaveIfError( file.Open( iFs, aFileName, EFileRead ) );
    CleanupClosePushL( file );

    RFileReadStream readStream;
    CleanupClosePushL( readStream );
    readStream.Attach( file );

    TInt error = KErrNone;
    TInt entryCount = 0;
    TRAP( error, entryCount = readStream.ReadInt32L() );
    // TODO should limit entryCount to something like 1024
    // for each entry in the registry file
    for ( TInt i = 0 ; i < entryCount; i++ )
        {
        CWidgetEntry* entry = CWidgetEntry::NewL();
        CleanupStack::PushL( entry );

        // extract one entry
        TRAP( error,
              entry->InternalizeBinaryL( readStream ) );
        if ( KErrNone == error )
            {
            CWidgetEntry* res( NULL );
            if ( aDoConsistency )
                {
                res = EntryConsistency( entry,
                                          aAppArchList,
                                          aAppArchListFlags,
                                          aInstalledListForDrive,
                                          aInstalledListForDriveFlags,
                                          aDriveUnit,
                                          aDirtyFlag );
                }
            if ( NULL != res )
                {
                TRAP( error, InsertL( entry ) );
                if ( KErrNone != error )
                    {
                    CleanupStack::PopAndDestroy( entry );
                    }
                else
                    {
                    __ASSERT_DEBUG( res == entry, User::Invariant() );
                    // Entry was inserted successfully.
                    CleanupStack::Pop( entry );
                    // add uid to AppArchList if not there,
                    // this can happend due to UID
                    // reallocation for UID collision resolution
                    TInt uidInt = (*entry)[EUid];
                    if ( aDoConsistency &&
                         ( KErrNotFound
                           == aAppArchList.Find(TUid::Uid(uidInt)) ) )
                        {
                        User::LeaveIfError( aAppArchList.Append( TUid::Uid(uidInt) ) );
                        User::LeaveIfError( aAppArchListFlags.Append( EAppListFlagEntry ) );
                        }
                    LOG2( " entry 0x%x (%d) added to registry",
                              uidInt, uidInt );
                    }
                }
            }
        else
            {
            // entry error
            CleanupStack::PopAndDestroy( entry );
            }
        } // for

    CleanupStack::PopAndDestroy( 2, &file ); // readStream, file
    }

// ============================================================================
// CWidgetRegistry::InternalizeXmlL()
// Read entry info from data file into memory
//
// @since 5.0
// ============================================================================
//
void CWidgetRegistry::InternalizeXmlL( const TDesC& aFileName,
                                       const TDriveUnit& aDriveUnit,
                                       TBool aDoConsistency,
                                       RArray<TUid>& aAppArchList,
                                       RArray<TInt>& aAppArchListFlags,
                                       const CDir* aInstalledListForDrive,
                                       RArray<TInt>& aInstalledListForDriveFlags,
                                       TBool& aDirtyFlag )
    {
    LOG( "InternalizeXmlL" );
    RFile file;
    User::LeaveIfError( file.Open( iFs, aFileName, EFileRead ) );
    CleanupClosePushL( file );

    TInt size;
    User::LeaveIfError( file.Size( size ) );
    HBufC8* buf = HBufC8::NewLC( size );
    TPtr8 bufPtr( buf->Des() );
    User::LeaveIfError( file.Read( bufPtr ) );

    // initialize the parser and check compiled code matches lib version
    LIBXML_TEST_VERSION

    xmlDocPtr doc; // resulting document tree

    doc = xmlReadMemory( (const char *)bufPtr.Ptr(), bufPtr.Length(),
                         NULL, // no base URL
                         NULL, // get encoding from doc
                         0); // options

    if ( !doc )
        {
        LOG( " leaving: parse failed XML corrupt" );
        User::Leave( KErrCorrupt );
        }
    
    //TCleanupItem item( XmlDocFree, doc );
    //CleanupStack::PushL( item );
    xmlNode* rootElement = xmlDocGetRootElement( doc );
    TPtrC8 rootTag( rootElement->name );
    if ( 0 != rootTag.Compare( KWidgetRegistry() ) )
    {
        LOG( " leaving: XML root element mismatch" );
        User::Leave( KErrCorrupt );
    }

    for ( xmlNode* n = rootElement->children;
          n;
          n = n->next )
        {
        if ( n->type == XML_ELEMENT_NODE )
            {
            TPtrC8 element( n->name );

            if ( 0 == element.Compare( KEntry() ) )
                {
                if ( NULL == n->children )
                    {
                    // malformed? should we require entry to have
                    // some minimal info?
                    continue;
                    }
                CWidgetEntry* entry = CWidgetEntry::NewL();
                CleanupStack::PushL( entry );

                // extract one entry
                TRAPD( error,
                       entry->InternalizeXmlL( iFs, doc, n->children,
                                               iXmlProcessor ) );
                LOG2( " entry 0x%x read from XML with error %d",
                          (error == KErrNone)? (*entry)[EUid] : 0,
                          error );
                if ( KErrNone == error )
                    {
                    if ( aDoConsistency )
                        {
                        entry = EntryConsistency( entry,
                                                  aAppArchList,
                                                  aAppArchListFlags,
                                                  aInstalledListForDrive,
                                                  aInstalledListForDriveFlags,
                                                  aDriveUnit,
                                                  aDirtyFlag );
                        }
                    if ( NULL != entry )
                        {
                        TRAP( error, InsertL( entry ) );
                        if ( KErrNone != error )
                            {
                            delete entry;
                            }
                        else
                            {
                            entry->SetBlanketPermission((*entry)[EBlanketPermGranted] );
                            // add uid to AppArchList if not there,
                            // this can happend due to UID
                            // reallocation for UID collision resolution
                            TInt uidInt = (*entry)[EUid];
                            if ( aDoConsistency &&
                                 ( KErrNotFound
                                   == aAppArchList.Find(TUid::Uid(uidInt)) ) )
                                {
                                aAppArchList.Append( TUid::Uid(uidInt) );
                                aAppArchListFlags.Append( EAppListFlagEntry );
                                }
                            LOG2( " entry 0x%x (%d) added to registry",
                                      uidInt, uidInt );
                            }
                        }
                    }
                else
                    {
                    // entry error
                    delete entry;
                    LOG( " entry internalize failed" );
                    }

                CleanupStack::Pop(); //entry
                } // if <entry>
            } // if n is element
        } // for

    xmlFreeDoc( doc );
    xmlCleanupParser();

    CleanupStack::PopAndDestroy( 2, &file ); // buf, file
    }

// ============================================================================
// CWidgetRegistry::ExternalizeL()
// Write entry info in memory into data file
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::ExternalizeL()
    {
    // in order to have a list of all entries for a particular drive,
    // create a hash map from the entries with the key as drive name
    // and value as an array of entry indices for that drive
    RPtrHashMap< TInt, CArrayFixFlat<TInt> > driveEntryHashMap;
    CleanupClosePushL( driveEntryHashMap );

    for (TInt i = 0 ;i < iEntries.Count(); i++)
        {
        CWidgetEntry* entry = iEntries[i];
        const TDesC& driveName = (*entry)[EDriveName];
        TDriveUnit driveUnit( driveName );
        CArrayFixFlat<TInt>* array =
            driveEntryHashMap.Find( driveUnit );
        if( !array )
           {
           TInt* driveNo = new (ELeave) TInt( driveUnit );
           CArrayFixFlat<TInt>* drArray = new (ELeave) CArrayFixFlat<TInt>(1);
            CleanupStack::PushL( drArray );
           drArray->AppendL(i);
           driveEntryHashMap.Insert(driveNo, drArray);
           }
        else
           {
           array->AppendL(i);
           }
        }

    // List all drives in the system
    TDriveList driveList;
    User::LeaveIfError( iFs.DriveList(driveList) );

    // Check all drives
    for ( TInt driveNumber = EDriveA; driveNumber <= EDriveZ; driveNumber++ )
        {
        // The drives that will be filtered out are the same ones that
        // WidgetInstaller filters out in CWidgetUIHandler::SelectDriveL()
        if ( (EDriveD == driveNumber)
             || !driveList[driveNumber] )
            {
            // EDriveD is a temporary drive, usually a RAM disk
            continue;
            }

        TVolumeInfo volInfo;
        if ( iFs.Volume( volInfo, driveNumber ) != KErrNone )
            {
            // The volume is not usable (e.g. no media card inserted)
            continue;
            }
        if ( (volInfo.iDrive.iType == EMediaNotPresent) ||
             (volInfo.iDrive.iType == EMediaRom) ||
             (volInfo.iDrive.iType == EMediaRemote) ||
             (volInfo.iDrive.iDriveAtt & KDriveAttRom) ||
             (volInfo.iDrive.iDriveAtt & KDriveAttSubsted) )
            {
            continue;
            }

        // found a usable drive
        TDriveUnit driveUnit( driveNumber );

        // redirect paths to this drive
        iRegistryBinaryFileName[0] = driveUnit.Name()[0];
        iRegistryXmlFileName[0] = iRegistryBinaryFileName[0];
        iRegistryXmlTempFileName[0] = iRegistryBinaryFileName[0];

        const CArrayFixFlat<TInt>* indices =
            driveEntryHashMap.Find( driveNumber );
        if ( NULL == indices )
            {
            // not in hash map, delete any existing versions
            BaflUtils::DeleteFile( iFs, iRegistryBinaryFileName );
            BaflUtils::DeleteFile( iFs, iRegistryXmlFileName );
            BaflUtils::DeleteFile( iFs, iRegistryXmlTempFileName );
            }
        else
            {
            iFs.CreatePrivatePath( driveUnit );

            // a transactional file update to protect against
            // disk full, etc: overwrite temp then rename temp to original

            TRAPD( error,
                   ExternalizeXmlL( iRegistryXmlTempFileName, indices ) );
            if ( KErrNone == error )
                {
                // last steps in transactional update
                BaflUtils::DeleteFile( iFs, iRegistryXmlFileName );
                BaflUtils::RenameFile( iFs,
                                       iRegistryXmlTempFileName,
                                       iRegistryXmlFileName );
                }
            else // handle leave by deleting temp file
                {
                BaflUtils::DeleteFile( iFs, iRegistryXmlTempFileName );
                }
            }
        }

    for ( TInt i = 0; i < driveEntryHashMap.Count(); i++ )
        {
        CleanupStack::Pop();
        }
    CleanupStack::Pop( &driveEntryHashMap );
    driveEntryHashMap.ResetAndDestroy();
    driveEntryHashMap.Close();
    }

// ============================================================================
// CWidgetRegistry::ExternalizeBinaryL
// Externalize Binary file
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::ExternalizeBinaryL( const TDesC& aFilename,
                                          const CArrayFixFlat<TInt>* aIndices )
    {
    RFileWriteStream writeStream;
    CleanupClosePushL( writeStream );

    RFile file;
    User::LeaveIfError( file.Replace( iFs, aFilename, EFileWrite ) );
    CleanupClosePushL( file );
    writeStream.Attach( file );
    writeStream.WriteInt32L( aIndices->Count() );
    for ( TInt i = 0; i < aIndices->Count() ; i++ )
        {
        TInt pos = (*aIndices)[i];
        CWidgetEntry* entry = iEntries[pos];
        if(entry)
            {
            TRAPD( error, entry->ExternalizeBinaryL( writeStream ) );
            if ( KErrNone != error )
                {
                // TODO how to recover from error?
                continue;
                }
            }
        }
    writeStream.CommitL();
    CleanupStack::PopAndDestroy(2);
    }

// ============================================================================
// CWidgetRegistry::ExternalizeXmlL
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::ExternalizeXmlL( const TDesC& aFilename,
                                       const CArrayFixFlat<TInt>* aIndices )
    {
    // sequence: write XML header, write each entry including
    // unrecognized XML, write closing XML

    RFileWriteStream writeStream;
    CleanupClosePushL( writeStream );

    RFile file;
    User::LeaveIfError( file.Replace( iFs, aFilename, EFileWrite ) );
    CleanupClosePushL( file );
    writeStream.Attach( file );

    // write XML header
    TInt bom = 0xfeff; // byte-order mark
    writeStream.WriteInt16L( bom );
    writeStream.WriteL( KXmlHeader );
    writeStream.WriteL( KXmlNewline );
    writeStream.WriteL( KXmlRootStart );
    writeStream.WriteL( KXmlNewline );

    for ( TInt i = 0; i < aIndices->Count() ; i++ )
        {
        TInt pos = (*aIndices)[i];
        CWidgetEntry* entry = iEntries[pos];
        if ( entry )
            {
            writeStream.WriteL( KWidgetEntryStart );
            writeStream.WriteL( KXmlNewline );
            // TODO handle unrecognized XML from internalize
            TRAPD( error, entry->ExternalizeXmlL( writeStream,
                                                  iXmlProcessor,
                                                  iFs) );
            writeStream.WriteL( KWidgetEntryEnd );
            writeStream.WriteL( KXmlNewline );
            if ( KErrNone != error )
                {
                // TODO how to recover from error?
                continue;
                }
            }
        }

    writeStream.WriteL( KXmlRootEnd );
    writeStream.WriteL( KXmlNewline );
    writeStream.CommitL();

    CleanupStack::PopAndDestroy( 2 ); // file, writeStream
    }

// ============================================================================
// CWidgetRegistry::RegisterWidget()
// Creates CWidgetEntry
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::RegisterWidgetL( RReadStream& aStream )
    {
    CWidgetEntry* entry = CWidgetEntry::NewL();
    CleanupStack::PushL( entry );

    entry->InternalizeBinaryL( aStream );
    InsertL( entry );

    CleanupStack::Pop(); //entry
    ExternalizeL();

    //Notify WRT Harvester that widget registry has changed
    NotifyWidgetAltered();
    }

// ============================================================================
// CWidgetRegistry::IsWidget()
// Returns true if the Uid falls within the range specified for widgets
//
// @since 3.1
// ============================================================================
//
TBool CWidgetRegistry::IsWidget( const TUid& aUid ) const
    {
    if ( ( aUid.iUid >= KWidgetUidLowerBound ) &&
        ( aUid.iUid <= KWidgetUidUpperBound ) )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }

    }

// ============================================================================
// CWidgetRegistry::WidgetExists()
// Returns true if the widget is installed
//
// @since 3.1
// ============================================================================
//
TBool CWidgetRegistry::WidgetExists( const TDesC& aWidgetId ) const
    {
    CWidgetEntry* entry = NULL;
    TInt pos = GetWidgetEntry( TPtrC( aWidgetId ), entry );
    return ( pos != -1 )?  ETrue : EFalse;
    }

// ============================================================================
// CWidgetRegistry::IsWidgetRunning()
// Returns true if the widget is running
//
// @since 3.1
// ============================================================================
//
TBool CWidgetRegistry::IsWidgetRunning( const TUid& aUid ) const
    {
    TBool active = EFalse;
    CWidgetEntry* entry = NULL;

    GetWidgetEntry( aUid, entry );
    if ( entry && entry->ActiveL() )
        {
        active = ETrue;
        }
    return active;
    }

// ============================================================================
// CWidgetRegistry::WidgetSapiAccessState()
// Returns sapi widget access state
//
// @since 5.0
// ============================================================================
//
TInt CWidgetRegistry::WidgetSapiAccessState( const TUid& aUid ) const
    {
    CWidgetEntry* entry = NULL;

    GetWidgetEntry( aUid, entry );
    if ( entry)
        {
        return entry->SapiAccessState();
        }
    return -1;
    }

// ============================================================================
// CWidgetRegistry::IsWidgetInMiniView()
// Returns true if the widget is in miniview
//
// @since 5.0
// ============================================================================
//
TBool CWidgetRegistry::IsWidgetInMiniView( const TUid& aUid ) const
    {
    TBool state = EFalse;
    CWidgetEntry* entry = NULL;

    GetWidgetEntry( aUid, entry );
    if ( entry && entry->GetMiniViewState())
        {
        state = ETrue;
        }
    return state;
    }

// ============================================================================
// CWidgetRegistry::IsWidgetInFullView()
// Returns true if the widget is in miniview
//
// @since 5.0
// ============================================================================
//
TBool CWidgetRegistry::IsWidgetInFullView( const TUid& aUid ) const
    {
    TBool state = EFalse;
    CWidgetEntry* entry = NULL;

    GetWidgetEntry( aUid, entry );
    if ( entry && entry->GetFullViewState())
        {
        state = ETrue;
        }
    return state;
    }

TBool CWidgetRegistry::IsBlanketPermGranted( const TUid& aUid ) const
    {
    TBool state = EFalse;
    CWidgetEntry* entry = NULL;

    GetWidgetEntry( aUid, entry );
    if ( entry && entry->GetBlanketPermGranted())
        {
        state = ETrue;
        }
    return state;
    }

// ============================================================================
// CWidgetRegistry::InstalledWidgets()
// Returns widget info for all the installed widgets
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::InstalledWidgetsL( RWidgetInfoArray& aWidgetInfoArr )
    {
    for ( TInt pos = 0; pos < iEntries.Count(); pos++ )
        {
        CWidgetEntry* entry = iEntries[pos];
        if(entry)
            {
            CWidgetInfo *info = new ( ELeave ) CWidgetInfo();
            info->iUid = TUid::Uid( (*entry)[EUid] );
            info->iFileSize = (*entry)[EFileSize];
            *(info->iBundleName) = (*entry)[EBundleName];
            *(info->iDriveName) = (*entry)[EDriveName];
            aWidgetInfoArr.AppendL( info );
            }
        }
    }

// ============================================================================
// CWidgetRegistry::RunningWidgetsL()
// Returns widget info for all the running widgets
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::RunningWidgetsL( RWidgetInfoArray& aWidgetInfoArr )
    {
    for ( TInt pos = 0; pos < iEntries.Count(); pos++ )
        {
        CWidgetEntry* entry = iEntries[pos];

        if ( entry && entry->ActiveL() )
            {
            CWidgetInfo* info = new ( ELeave ) CWidgetInfo();
            info->iUid = TUid::Uid( (*entry)[EUid] );
            info->iFileSize = (*entry)[EFileSize];
            *(info->iBundleName) = (*entry)[EBundleName];
            *(info->iDriveName) = (*entry)[EDriveName];
            aWidgetInfoArr.AppendL( info );
            }
        }
    }

// ============================================================================
// CWidgetRegistry::DeRegisterWidgetL()
// Deregister the widget
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::DeRegisterWidgetL( const TUid& aUid )
    {
    Remove( aUid );
    ExternalizeL( );
    //Notify WRT Harvester that widget registry has changed
    NotifyWidgetAltered();
    }

// ============================================================================
// CWidgetRegistry::GetWidgetBundleId()
// Returns bundleId of the widget with a particular UId.
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::GetWidgetBundleId( const TUid& aUid, TDes& aBundleId )
    {
    CWidgetEntry* entry = NULL;
    TInt pos = GetWidgetEntry( aUid, entry );

    if ( pos != -1 )
        {
        const TDesC& widgetBundleId = (*entry)[EBundleIdentifier];
        aBundleId.Copy( widgetBundleId );
        aBundleId.SetLength( widgetBundleId.Length() );
        }
    else
        {
        aBundleId.SetLength( 0 );
        }
    }

// ============================================================================
// CWidgetRegistry::GetWidgetBundleName()
// Returns bundle display name of the widget with a particular UId.
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::GetWidgetBundleName( const TUid& aUid, TDes& aBundleName )
    {
    CWidgetEntry* entry = NULL;
    TInt pos = GetWidgetEntry( aUid, entry );

    if ( pos != -1 )
        {
        const TDesC& widgetBundleName = (*entry)[EBundleName];
        aBundleName.Copy( widgetBundleName );
        aBundleName.SetLength( widgetBundleName.Length() );
        }
    else
        {
        aBundleName.SetLength( 0 );
        }
    }

// ============================================================================
// CWidgetRegistry::GetWidgetPropertyValueL()
// return serialized value of property aId for the widget aUid or leave
//
// @since 3.1
// ============================================================================
//
CBufFlat* CWidgetRegistry::GetWidgetPropertyValueL(
    const TUid& aUid,
    TWidgetPropertyId aId,
    TInt aMaxLength )
    {
    CWidgetEntry& entry = GetWidgetEntryL( aUid );
    CBufFlat* buf = CBufFlat::NewL( aMaxLength );
    CleanupStack::PushL( buf );
    RBufWriteStream stream( *buf ); // stream over the buffer
    CleanupClosePushL( stream );

    // TBD safe array indexing (leave if out of range)
    entry[aId].SerializeL( stream ); // serialize

    CleanupStack::PopAndDestroy( &stream );
    CleanupStack::Pop(); // buf
    return buf;
    }

// ============================================================================
// CWidgetRegistry::GetWidgetPath()
// Returns path of the widget with a particular UId.
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::GetWidgetPath( const TUid& aUid, TDes& aPath )
    {
    CWidgetEntry* entry = NULL;
    TInt pos = GetWidgetEntry( aUid, entry );

    if ( pos != -1 )
        {
        const TDesC& widgetBasePath = (*entry)[EBasePath];
        aPath.Copy( widgetBasePath );
        aPath.SetLength( widgetBasePath.Length() );
        }
    else
        {
        aPath.SetLength( 0 );
        }
    }

// ============================================================================
// CWidgetRegistry::GetLprojName()
// Returns lproj name
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::GetLprojNameL( TDes& aPath )
    {
    if ( !iLangDirList.Count() )
        {
        CreateLangDirListL();
        }
    HBufC8* lprojName = iLangDirList.Find( User::Language() );
    if ( lprojName )
        {
        TPtr8 lprojNamePtr = lprojName->Des();
        aPath.Copy( lprojNamePtr );
        }
    else
        {
        aPath.Copy( KWidgetDefaultLangDir );
        }
    }

// ============================================================================
// CWidgetRegistry::GetWidgetUid()
// Returns UId of the widget with a particular bundle identifier
//
// @since 3.1
// ============================================================================
//
TUid CWidgetRegistry::GetWidgetUid( const TDesC& aBundleId ) const
    {
    CWidgetEntry* entry = NULL;
    TInt pos = GetWidgetEntry( aBundleId, entry );

    if ( pos != -1 )
        {
        return TUid::Uid( (*entry)[EUid] );
        }
    else
        {
        return KNullUid;
        }
    }

// ============================================================================
// CWidgetRegistry::GetWidgetUidForUrl()
// Returns uid of the widget with a patricular html path
//
// @since 3.1
// ============================================================================
//
TUid CWidgetRegistry::GetWidgetUidForUrl( const TDesC& aUrl ) const
    {
    for( TInt i = 0; i < iEntries.Count(); i++)
        {
        CWidgetEntry* entry = iEntries[i];

        if ( aUrl.CompareF( (*entry)[EMainHTML] ) == 0 )
            {
            return TUid::Uid( (*entry)[EUid] );
            }
        }
    return KNullUid;
    }

// ============================================================================
// CWidgetRegistry::UsedUids()
// Returns array of used UIds
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::UsedUidsL( RUidArray& aUsedUids )
    {
    for( TInt i = 0; i < iUsedUids.Count(); i++ )
        {
        aUsedUids.AppendL( iUsedUids[i] );
        }
    }

// ============================================================================
// CWidgetRegistry::GetAvailableUidL()
// Get next availble uid from the pool using a random generation
//
// @since 3.1
// ============================================================================
//
TUid CWidgetRegistry::GetAvailableUidL( TInt aDriveLetter )
    {
    TUidAllocator uidAllocator;
    return TUid::Uid( uidAllocator.AllocateL( iUsedUids, aDriveLetter ) );
    }

// ============================================================================
// CWidgetRegistry::SetActive()
// Set/Reset active status of the widget with a particular UId
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::SetActive( TUid aUid, TInt aStatus )
    {
    CWidgetEntry* entry = NULL;
    TInt pos = GetWidgetEntry( aUid, entry );
    if ( pos != -1 )
      {
        entry->SetActive( aStatus );
      }
    }

// ============================================================================
// CWidgetRegistry::SetMiniView()
// Set/Reset Widget status for launched in MiniView
//
// @since 5.0
// ============================================================================
//
void CWidgetRegistry::SetMiniView( TUid aUid, TInt aStatus )
    {
    CWidgetEntry* entry = NULL;
    TInt pos = GetWidgetEntry( aUid, entry );
    if ( pos != -1 )
        {
        entry->SetMiniView( aStatus );
        }
    }

// ============================================================================
// CWidgetRegistry::SetFullView()
// Set/Reset Widget status in FullView
//
// @since 5.0
// ============================================================================
//
void CWidgetRegistry::SetFullView( TUid aUid, TInt aStatus )
    {
    CWidgetEntry* entry = NULL;
    TInt pos = GetWidgetEntry( aUid, entry );
    if ( pos != -1 )
        {
        entry->SetFullView( aStatus );
        }
    }

// ============================================================================
// CWidgetRegistry::SetBlanketPermissionL()
// Set/Reset Widget Blanket Permission
//
// @since 5.0
// ============================================================================
//
void CWidgetRegistry::SetBlanketPermissionL( TUid aUid, TInt aStatus )
    {
    CWidgetEntry* entry = NULL;
    TInt pos = GetWidgetEntry( aUid, entry );
    if ( pos != -1 )
        {
        entry->SetBlanketPermission( aStatus );
        }
    ExternalizeL();
    }

// ============================================================================
// CWidgetRegistry::CreateLangDirListL()
// create the mapping table for language and lproj dir
//
// @since 3.1
// ============================================================================
//
void CWidgetRegistry::CreateLangDirListL()
    {
    TLanguage defaultLanguage = User::Language();
    RFile file;
    TFileName langDirFile;
    iFs.PrivatePath( langDirFile );
#ifdef __WINSCW__
    langDirFile.Insert( 0, _L( "C:" ));
#else
    langDirFile.Insert( 0, _L( "Z:" ));
#endif
    langDirFile.Append( KWidgetDirFile );
    User::LeaveIfError( file.Open( iFs, langDirFile, EFileRead ) );
    CleanupClosePushL( file );

    TInt size;
    User::LeaveIfError( file.Size( size ) );
    HBufC8* buf = HBufC8::NewLC( size );
    TPtr8 bufPtr( buf->Des() );
    User::LeaveIfError( file.Read( bufPtr ) );
    CleanupStack::Pop( buf );
    CleanupStack::PopAndDestroy( &file );
    CleanupStack::PushL( buf );

    // initialize the parser and check compiled code matches lib version
    LIBXML_TEST_VERSION

    xmlDocPtr doc; // resulting document tree

    doc = xmlReadMemory( (const char *)bufPtr.Ptr(), bufPtr.Length(),
                         NULL, // no base URL
                         NULL, // get encoding from doc
                         0); // options

    if ( !doc )
          {
          User::Leave( KErrCorrupt );
          }

    xmlNode* rootElement = xmlDocGetRootElement( doc );

    TInt* langID = NULL;
    xmlNode* n;

    for ( n = rootElement; n; n = TraverseNextNode( n ) )
        {
        HBufC8* langDir;
        if ( n->type == XML_ELEMENT_NODE )
            {
            TPtrC8 element( n->name );

            if ( element.Compare( KLangID() ) == 0 )
                {
                if ( n->children && n->children->type == XML_TEXT_NODE )
                    {
                    TPtrC8 content( n->children->content );
                    langID = new TInt( atoi( (const char *)content.Ptr() ) );
                    CleanupStack::PushL( langID );
                    }
                }
            if ( element.Compare( KLangDir() ) == 0 )
                {
                if ( n->children && n->children->type == XML_TEXT_NODE )
                    {
                        TPtrC8 content( n->children->content );
                    langDir = HBufC8::NewLC( content.Length() );
                    langDir->Des().Copy( content );
                    if ( *langID <= 0 )
                        {
                        User::Leave( KErrCorrupt );
                        }
                    TInt* currentLangID = new TInt(*langID);
                    CleanupStack::Pop(langDir);
                    iLangDirList.Insert( currentLangID, langDir );
                    *langID = 0;
                    CleanupStack::PopAndDestroy(langID);
                    langID = NULL;
                    }
                }
            }   // if n is element

        }   // for

    xmlFreeDoc(doc);
    xmlCleanupParser();
    XmlEngineCleanup();
    CleanupStack::PopAndDestroy( buf );
    }

TBool CWidgetRegistry::InstallDirWidgets(
    const TDriveUnit& aDriveUnit,
    CDir*& aInstalledListForDrive,
    RArray<TInt>& aInstalledListForDriveFlags )
    {
    LOG( "InstallDirWidgets" );
    TBool doConsistency = ETrue;
    aInstalledListForDrive = NULL;
    aInstalledListForDriveFlags.Reset();
    iWidgetInstallPath[0] = aDriveUnit.Name()[0];
    TInt error = iFs.GetDir( iWidgetInstallPath,
                             KEntryAttDir | KEntryAttMatchExclusive,
                             ESortNone,
                             aInstalledListForDrive );
    if ( KErrPathNotFound == error )
        {
        LOG( "install path not found" );
        delete aInstalledListForDrive;
        aInstalledListForDrive = NULL;
        return doConsistency;
        }
    if ( KErrNone != error || NULL == aInstalledListForDrive )
        {
        delete aInstalledListForDrive;
        aInstalledListForDrive = NULL;
        doConsistency = EFalse;
        LOG( "error listing install directory, doConsistency 0" );
        }
    else
        {
        TInt count =
            ( (NULL == aInstalledListForDrive) ?
              0 : aInstalledListForDrive->Count() );
        if ( count )
            {
                for ( TInt i = 0; i < aInstalledListForDrive->Count(); i++ )
                    {
                error = aInstalledListForDriveFlags.Append( 0 );
                    if ( KErrNone != error )
                        {
                        break;
                        }
                    }
            if ( KErrNone != error )
                {
                doConsistency = EFalse;
                aInstalledListForDriveFlags.Reset();
                
                delete aInstalledListForDrive;
                aInstalledListForDrive = NULL;
                }
            }
        }
    LOG_CODE( if ( aInstalledListForDrive ) )
    LOG2( "InstallDirWidgets done count %d doConsistency %d",
              aInstalledListForDrive->Count(),
              (TInt)doConsistency );
    LOG_CODE( else )
    LOG1( "InstallDirWidgets return NULL list doConsistency %d",
              (TInt)doConsistency );
    return doConsistency;
    }

// a routine to build an array of widgets according to app arch app list
// returns NULL on error, otherwise a UID list (possibly with no entries)
TInt CWidgetRegistry::AppArchWidgetUids( RArray< TUid >& aUids )
    {
    LOG( "AppArchWidgetUids" );
    TInt error = iAppArch.GetAllApps();
    aUids.Reset();
    if ( KErrNone != error )
        {
        LOG ( "AppArchWidgetUids done error NULL list" );
        
        return error;
        }
    TInt appArchWaitTotal = 0;
    TApaAppInfo info;
    do
        {
        error = iAppArch.GetNextApp( info );
        if ( KErrNone == error )
            {
            if ( TUidAllocator::IsWidget( info.iUid ) )
                {
                LOG2( " widget uid 0x%x (%d)",
                          (TUint)(info.iUid.iUid), info.iUid.iUid );
                error = aUids.Append( info.iUid );
                if ( KErrNone !=  error )
                    {
                    break;
                    }
                }
            continue;
            }
        else if ( RApaLsSession::EAppListInvalid == error )
            {
            if ( appArchWaitTotal > KAppArchTimeout )
                {
                LOG( " appArchWaitTotal > KAppArchTimeout" );
                break;
                }
            appArchWaitTotal += KAppArchDelayInterval;
            LOG1( " appArchWaitTotal %d", appArchWaitTotal );
            User::After( KAppArchDelayInterval );
            continue;
            }
        } while ( RApaLsSession::ENoMoreAppsInList != error );
    if ( RApaLsSession::ENoMoreAppsInList != error )
        {
        aUids.Reset();
        }
	else 
		{
		error = KErrNone;
		}
    LOG_CODE( if ( aUids.Count() ) )
    LOG1( "AppArchWidgetUids done widget count %d",
              aUids.Count() );
    LOG_CODE( else )
    LOG( "AppArchWidgetUids done error NULL list" );

    return error;
    }

// if primary return bool is T then arg lists will be non NUll, else
// return bool will be F and arg lists will be NULL (error condition)
TBool CWidgetRegistry::AppArchWidgets( RArray<TUid>& aAppArchList,
                                       RArray<TInt>& aAppArchListFlags )
    {
    TBool doConsistency = ETrue;
    TInt error = AppArchWidgetUids( aAppArchList );
    aAppArchListFlags.Reset();
    // NULL means AppArchWidgetUids encountered an error
    if ( error )
        {
        // doConsistency flase means there is an unrecoverable error
        // in consistency logic
        doConsistency = EFalse;
        }
    else
        {
        TInt error = KErrNone;
        for ( TInt i = 0 ; i < aAppArchList.Count(); i++ )
                {
            error = aAppArchListFlags.Append( 0 );
                if ( KErrNone != error )
                    {
                    break;
                    }
                }
    
        if ( KErrNone != error )
            {
            doConsistency = EFalse;
            aAppArchListFlags.Reset();
            aAppArchList.Reset();
            }
        }
    LOG_CODE( if ( aAppArchList.Count() ) )
    LOG2( "AppArchWidgets done widgets count %d doConsistency %d",
              aAppArchListFlags.Count(), (TInt)doConsistency );
    LOG_CODE( else )
    LOG1( "AppArchWidgets done error doConsistency %d",
              (TInt)doConsistency );
    return doConsistency;
    }

// when called: aAppArchList and Flags must not be
// NULL. aInstalledListForDrive and Flags may be NULL
CWidgetEntry* CWidgetRegistry::EntryConsistency(
    CWidgetEntry* aEntry,
    const RArray<TUid>& aAppArchList,
    RArray<TInt>& aAppArchListFlags,
    const CDir* aInstalledListForDrive,
    RArray<TInt>& aInstalledListForDriveFlags,
    const TDriveUnit& aDriveUnit,
    TBool& aDirtyFlag )
    {
    LOG( "EntryConsistency" );
    TInt error;
    TBool inAppList;
    TUid uid;

    if ( IsEntryInstalled( aEntry, aInstalledListForDrive,
                           aInstalledListForDriveFlags ) )
        {
        error = IsEntryInAppList( aEntry,
                                  aAppArchList,
                                  aAppArchListFlags,
                                  inAppList,
                                  uid );
        if ( ( inAppList == EFalse ) || ( KErrNone != error ) )
            {
            if ( KErrNone == error )
                {
                LOG( " entry not in AppArch" );
                // not in AppArch but may be using a UID already in
                // use (UID conflict)
                error =
                    InternalizeEntryNewUidIfNeeded( aEntry, aDriveUnit );
                if ( KErrNone == error )
                    {
                    iInstaller->RenameIconFile( iFs, aEntry->Properties() );
                    TInt uidInt = (*aEntry)[EUid];
                    TRAP( error,
                          iInstaller->RegisterWidgetL(
                              (*aEntry)[EMainHTML],
                              (*aEntry)[EBundleDisplayName],
                              (*aEntry)[EIconPath],
                              (*aEntry)[EDriveName],
                              TUid::Uid( uidInt ) ) );
                    if ( KErrNone == error )
                        {
                        LOG2( " registered widget 0x%x (%d)",
                                  uidInt, uidInt );
                        LOG( " registry dirty flag set" );
                        aDirtyFlag = ETrue;
                        }
                    }
                }
            // catches errors from several prior lines
            if ( KErrNone != error )
                {
                // allocated uid hasn't been added to iUsedUid yet so
                // just delete entry

                // on error don't put entry in registry
                LOG( " error entry deleted" );
                delete aEntry;
                aEntry = NULL;
                }
            }
        }
    else
        {
        LOG( " entry is not installed" );
        // entry is not installed, clean up app list if needed
        error = IsEntryInAppList( aEntry,
                                  aAppArchList,
                                  aAppArchListFlags,
                                  inAppList,
                                  uid );
        if ( ( (TInt)ETrue == inAppList ) && ( KErrNone == error ) )
            {
            LOG( " removing from AppArch" );
            // there is nothing useful to do on error
            TRAP_IGNORE( iInstaller->DeregisterWidgetL( uid ) );
            }

        // don't put entry in registry
        LOG( " entry deleted" );
        delete aEntry;
        aEntry = NULL;
        aDirtyFlag = ETrue;
        }
    LOG( "EntryConsistency done " );
    return aEntry;
    }

// aInstalledListForDrive and Flags may be NULL (meaning no installed widgets)
TBool CWidgetRegistry::IsEntryInstalled(
    CWidgetEntry* aEntry,
    const CDir* aInstalledListForDrive,
    RArray<TInt>& aInstalledListForDriveFlags )
    {
    TBool result = EFalse;
    // NULL is an expected input due to GetDir producing it for empty lists
    if ( NULL != aInstalledListForDrive )
        {
        const TDesC& bundleIdentifier = (*aEntry)[EBundleIdentifier];
        for ( TInt i = 0; i < aInstalledListForDrive->Count(); i++ )
            {
            if ( !((aInstalledListForDriveFlags)[i] & EInstallListFlagEntry ))
                {
                TEntry dirEntry = (*aInstalledListForDrive)[i];
                if ( 0 == bundleIdentifier.Compare( dirEntry.iName ) )
                    {
                    (aInstalledListForDriveFlags)[i] |= EInstallListFlagEntry;
                    result = ETrue;
                    break;
                    }
                }
            }
        }
    LOG1( "IsEntryInstalled result %d", (TInt)result );
    return result;
    }

// aAppArchList and Flags must not be NULL but may be empty (0 count)
TInt CWidgetRegistry::IsEntryInAppList( CWidgetEntry* aEntry,
                                        const RArray<TUid>& aAppArchList,
                                        RArray<TInt>& aAppArchListFlags,
                                        TBool& aInAppList,
                                        TUid& uid )
     {
     // entry has uid and EMainHTML, appInfo has uid and iFullName,
     // both should match exactly
     TInt error = KErrNone;
     aInAppList = EFalse;
     for ( TInt i = 0; i < aAppArchList.Count(); i++ )
         {
         if ( !( (aAppArchListFlags)[i] & EAppListFlagEntry ) )
             {
             uid = aAppArchList[i];
             TInt uidInt = (*aEntry)[EUid];
             TUid uid2 = TUid::Uid( uidInt );
             if ( uid == uid2 )
                 {
                 TApaAppInfo appInfo;
                 error = iAppArch.GetAppInfo( appInfo, uid );
                 if ( KErrNone == error )
                     {
                     const TDesC& mainHtml = (*aEntry)[EMainHTML];
                     if ( 0 == mainHtml.Compare( appInfo.iFullName ) )
                         {
                         aAppArchListFlags[i] |= EAppListFlagEntry;
                         aInAppList = ETrue;
                         break;
                         }
                     }
                 else
                     {
                     break;
                     }
                 }
             }
         }
     LOG2( "IsEntryInAppList done %d error %d",
           (TInt)aInAppList, error );
     return error;
     }

// This will assign a new UID if the UID in the entry appears either
// in iUsedUids or a call to GetAppInfo returns some info (something
// is using the UID).  The new UID will *not* be placed in iUsedUids and
// GetAppInfo using the new UID will return KErrNotFound.
TInt CWidgetRegistry::InternalizeEntryNewUidIfNeeded(
    CWidgetEntry* aEntry,
    const TDriveUnit& aDriveUnit )
    {
    LOG( "InternalizeEntryNewUidIfNeeded" );
    // entry has a uid but it may already be in use by another widget
    // according to app arch or iUsedUids
    TUidAllocator uidAllocator;
    TInt upos = iUsedUids.Find( TUid::Uid( (*aEntry)[EUid] ) );
    TApaAppInfo appInfo;
    TInt error = iAppArch.GetAppInfo( appInfo, TUid::Uid( (*aEntry)[EUid] ) );
    if ( KErrNotFound != upos || KErrNotFound != error )
        {
        LOG( " widget needs a new UID" );
        LOG_CLOSE;
        LOG_OPEN;
        do
            {
            TInt uidInt = 0;
            TRAP( error,
                  uidInt = uidAllocator.AllocateL( iUsedUids,
                                                   aDriveUnit.Name()[0] ) );
            if ( KErrNone != error )
                {
                LOG(
    "InternalizeEntryNewUidIfNeeded done error failed to allocate a new UID" );
                return KErrNotFound;
                }
            TUid uid = TUid::Uid( uidInt );
            error = iAppArch.GetAppInfo( appInfo, uid );
            if ( KErrNotFound == error )
                {
                // use new uid (do icon mbm file rename elsewhere) and
                // don't add it to iUsedUids yet as that will be done
                // when entry is finally inserted in registry
                (*aEntry)[EUid] = uidInt;
                LOG2(
  "InternalizeEntryNewUidIfNeeded done new uid is 0x%x (%d)", uidInt, uidInt );
                return KErrNone;
                }
            else if ( KErrNone == error )
                {
                // don't allocate this uid because AppArch thinks it
                // is in use
                error = iUsedUids.Append( uid );
                }
            } while ( KErrNone == error );
        LOG(
    "InternalizeEntryNewUidIfNeeded done error failed to allocate a new UID" );
        return KErrNotFound;
        }
    LOG( "InternalizeEntryNewUidIfNeeded done widget already has a unique ID" );    LOG_CLOSE;
    LOG_OPEN;
    return KErrNone;
    }


// this examines the installed widget list after the persistent
// registry entries have been processed for any installed widgets that
// don't yet have a registry entry.  An entry is generated and the
// widget is registered with app arch.
void CWidgetRegistry::InstallDirConsistency(
    const CDir* aInstalledListForDrive,
    RArray<TInt>& aInstalledListForDriveFlags,
    TBool& aDirtyFlag )
    {
    LOG( "InstallDirConsistency" );
    if ( NULL == aInstalledListForDrive
         || ( ( NULL != aInstalledListForDrive)
              && ( 0 == aInstalledListForDrive->Count() ) ) )
        {
        LOG( " nothing is installed" );
        return;
        }
    TInt error = KErrNone;
    TUidAllocator uidAllocator;
    for ( TInt i = 0; i < aInstalledListForDrive->Count(); i++ )
        {
        if ( !( aInstalledListForDriveFlags[i] & EInstallListFlagEntry ) )
            {
            TFileName widgetPath( iWidgetInstallPath );
            widgetPath.Append( (*aInstalledListForDrive)[i].iName );
            widgetPath.Append( _L("\\") );
            // assume directory under this is widget top-level dir
            CDir *listing = NULL;
            error = iFs.GetDir( widgetPath,
                                KEntryAttDir | KEntryAttMatchExclusive,
                                ESortNone,
                                listing );
            if ( ( KErrNone != error )
                 || ( 1 != listing->Count() ) )
                {
                // skip this widget
                LOG( " rejecting a directory that has unexpected contents" );
                delete listing;
                continue;
                }
            LOG( " found inconsistency" );
            widgetPath.Append( ((*listing)[0]).iName );
            widgetPath.Append( _L("\\") );
            delete listing;

            TInt uidInt = 0;
            TRAP( error, uidInt =
                  uidAllocator.AllocateL( iUsedUids,
                                          iWidgetInstallPath[0] ) );
            if ( KErrNone != error )
                {
                LOG1( " uid allocate error %d", error );
                continue; // skip this widget
                }

            LOG( " getting properties from installed widget" );
            LOG_CLOSE;
            LOG_OPEN;
            RPointerArray<CWidgetPropertyValue>* tmp;
            TRAP( error,
                  tmp =
                  iInstaller->WidgetPropertiesFromInstalledWidgetL(
                      iFs, widgetPath, iLprojName, TUid::Uid( uidInt ) ) );
            if ( KErrNone != error || !tmp )
                {
                LOG1( " props from installed widget error %d", error );
                continue; // skip this widget
                }
            LOG( " got properties from installed widget" );
            // transfer ownership of tmp
            CWidgetEntry* entry = NULL;
            TRAP( error, entry = CWidgetEntry::NewL( &tmp ) );
            if ( KErrNone != error )
                {
                LOG1( " new entry copy ctor error %d", error );
                if ( tmp )
                    {
                    tmp->ResetAndDestroy();
                    }
                continue; // skip this widget
                }
            // app arch may have this widget under another uid
            // but if we assign a new uid and register the
            // widget then the duplicate app arch entry will
            // be removed when app arch list is made
            // consistent by removing entries not in the
            // registry by uid

            // check that uid is unknown to app arch
            TDriveUnit driveUnit( widgetPath );
            error = InternalizeEntryNewUidIfNeeded( entry, driveUnit );
            if ( KErrNone != error )
                {
                LOG1( " new uid if needed error %d", error );
                delete entry;
                continue; // skip this widget
                }
            LOG_CODE( TInt uidLog = (*entry)[EUid] );
            LOG1( " uid 0x%x", uidLog );
            LOG_CODE( const TDesC& logDes = (*entry)[EMainHTML] );
            LOG1( " mainHTML %S", &logDes );
            LOG_CODE( const TDesC& logDes2 = (*entry)[EBundleDisplayName] );
            LOG1( " bundleDiplayname %S", &logDes2 );

            TRAP( error,
                  iInstaller->RegisterWidgetL(
                      (*entry)[EMainHTML],
                      (*entry)[EBundleDisplayName],
                      (*entry)[EIconPath],
                      (*entry)[EDriveName],
                      TUid::Uid( (*entry)[EUid]) ) );

            if ( KErrNone == error )
                {
                LOG2( " registered widget 0x%x (%d)",
                          uidLog, uidLog );
                TRAP( error,
                      InsertL( entry ) );
                if ( KErrNone != error )
                    {
                    LOG1( " insert entry error %d", error );
                    delete entry;
                    continue; // skip this widget
                    }
                LOG2( " entry 0x%x (%d) added to registry",
                          uidLog, uidLog );
                LOG( "registry dirty flag set" );
                aDirtyFlag = ETrue;
                }
            else
                {
                // allocated uid hasn't been added to iUsedUid yet so
                // just delete entry
                delete entry;
                LOG( "failed to recover an installed widget" );
                continue; // skip this widget
                }
            }
        }
    LOG( "InstallDirConsistency done" );
    }

void CWidgetRegistry::AppArchListConsistency( const RArray<TUid>& aAppArchList,
                                              RArray<TInt>& aAppArchListFlags )
    {
    LOG( "AppArchListConsistency" );
    for ( TInt i = 0; i < aAppArchList.Count(); i++ )
        {
        if ( !( aAppArchListFlags[i] & EAppListFlagEntry ) )
            {
            LOG_CODE( TInt uidIntLog = aAppArchList[i].iUid );
            LOG2( " deregistered widget 0x%x (%d)",
                      uidIntLog, uidIntLog );
            TRAP_IGNORE( iInstaller->DeregisterWidgetL( aAppArchList[i] ) );
            }
        }
    LOG( "AppArchListConsistency done" );
    }

void CWidgetRegistry::HandleAppListEvent(TInt aEvent)
    {
    TBool dirtyFlag = EFalse;
    TInt parseError = KErrNone;
    // Assume usual case and things are consistent
    // and the registry entry file can be parsed and used.
    TRAPD( error, InternalizeL( EFalse,
                                EFalse,
                                dirtyFlag,
                                parseError ) );
    if ( KErrNone == error )
        {
        // internalize consistency enforcement may have altered registry
        if ( dirtyFlag )
            {
            TRAP_IGNORE( ExternalizeL(); );
            }
        }
    }
// End of File
