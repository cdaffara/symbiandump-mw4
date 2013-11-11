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
* Description:  This file contains the header file of the CMidletUI class.
*
*                This class implements the ECom SWInstUIPluginAPI interface
*                for midlet installation.
*
*
*/

#include <f32file.h>
#include <bautils.h>
#include <e32cmn.h>

#include <libxml2_globals.h>
#include <libc/stdlib.h>
#include <libxml2_parser.h>
#include <libxml2_tree.h>

#include "Browser_platform_variant.hrh"

#ifdef BRDO_SYMBIAN_LIBXML_FF
#include <xmlengxestd.h>
#endif

#include "WidgetInstaller.h"
#include "WidgetConfigHandler.h" // info.plist parser
#include "WidgetRegistrationManager.h" // interface to "shell"
#include "IconConverter.h"
#include "WidgetBackupRegistryXml.h" 

// CONSTANTS
_LIT( KInfoPList,"Info.plist" );
_LIT( KIconFile, "Icon.png" );
_LIT( KMBMExt, ".mbm");
_LIT( KLprojExt, ".lproj" );
_LIT( KInfoPlistStrings, "InfoPlist.strings" );
_LIT( KWidgetPropFile, "\\private\\10282822\\WidgetEntryStore.xml" );
	
// For parsing backedup registration file 
_LIT8( KWidgetRegistry, "widgetregistry" );
_LIT8( KEntry, "entry" );
_LIT8( KXmlPropTag, "prop" );
_LIT8( KXmlValTag, "val" );
_LIT8( KXmlTypeTag, "type" );
_LIT( KXmlDataTypeBool, "bool" );
_LIT( KXmlDataTypeInt, "int" );
_LIT( KXmlDataTypeString, "string" );
_LIT( KXmlDataTypeUid, "uid" );

// TODO MW has a hard dependency to APP domain. Not very good...
// TODO Hard-coded UID.
_LIT( KWidgetAppDir, "\\private\\10282822\\" );
_LIT( KBackSlash, "\\" );
// todo: other keystring.dat for preference
_LIT(KWidgetPref, "prefs.dat");


// =========================== MEMBER FUNCTIONS ===============================


// ============================================================================
// CWidgetInstaller::NewL()
// two-phase constructor
//
// @since 3.1
// @return pointer to CWidgetInstaller
// ============================================================================
//
EXPORT_C CWidgetInstaller* CWidgetInstaller::NewL()
    {
    CWidgetInstaller* self
        = new (ELeave) CWidgetInstaller();
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

// ============================================================================
// CWidgetInstaller::CWidgetInstaller()
// C++ constructor
//
// @since 3.1
// ============================================================================
//
CWidgetInstaller::CWidgetInstaller()
    : iPropertyValues( EWidgetPropertyIdCount )
    {
    }

// ============================================================================
// CWidgetInstaller::ConstructL()
// Symbian second phase constructor
//
// @since 3.1
// ============================================================================
//
void CWidgetInstaller::ConstructL()
    {
    User::LeaveIfError( iRfs.Connect() );
    User::LeaveIfError( iRfs.ShareProtected() );
    iFileMgr = CFileMan::NewL( iRfs );

    iAppManager = CWidgetRegistrationManager::NewL( iRfs );
    iWidgetConfigHandler = CWidgetConfigHandler::NewL();

    // delay registry connect because registry is a client and that
    // produces a circular dependency
    iServerConnected = EFalse;

    // empty values
    for ( TInt i( 0 ); i < EWidgetPropertyIdCount; ++i )
        {
        CWidgetPropertyValue* value = CWidgetPropertyValue::NewL();
        User::LeaveIfError( iPropertyValues.Insert( value, i ) );
        }
    *(iPropertyValues[EWidgetPropertyListVersion]) = WIDGETPROPERTYLISTVERSION;

#ifdef _DEBUG
    _LIT(KDir, "WidgetBUR");
    _LIT(KFile, "WidgetBURInstaller.log");
    TInt err( 0 );

    err = iFileLogger.Connect();
    if ( err == KErrNone )
        {
        iFileLogger.CreateLog( KDir(), KFile(), EFileLoggingModeOverwrite );
        iCanLog = ETrue;
        }
#endif
    }

// ============================================================================
// CWidgetInstaller::~CWidgetInstaller()
// destructor
//
// @since 3.1
// ============================================================================
//
CWidgetInstaller::~CWidgetInstaller()
    {
    TInt i = 0;
    for ( ; i < EWidgetPropertyIdCount; ++i )
        {
        delete iPropertyValues[i];
        }
    iPropertyValues.Close();

    delete iFileMgr;
    iRfs.Close();

    delete iWidgetConfigHandler;
    delete iAppManager;

    if ( iServerConnected )
        {
        iRegistry.Disconnect();
        }
#ifdef _DEBUG
    iFileLogger.Close();
#endif
    }

// ============================================================================
// CWidgetInstaller::InstallL()
// NORMAL INSTALL STEPS:
// 2. get metadata file (e.g. Info.plist) and parse it
// 3. if needed, replace identically named existing widget)
// 4. do free space check
// 5. if replacing existing widget then move currently installed files
//    to backup for possible restore on install error
//
// @since 3.1
// ============================================================================
//
EXPORT_C void CWidgetInstaller::InstallL(
    TDesC& aRestoreDir )
    {
    if ( EFalse == iServerConnected )
        {
        User::LeaveIfError( iRegistry.Connect() );
        iServerConnected = ETrue;
        }

    // The bool will be flase until doing something that
    // needs to be cleaned up on error/cancel.
    iOverwriting = EFalse;

    TRAPD( error,
        {
        // do initial bundle processing (parse and validate, total size,
        // check if replacing existing widget with same bundle ID, etc.)
        //
        // this step doesn't do anything that would need to be undone
        TBool replaceExisting = PreprocessWidgetBundleL( aRestoreDir );
        SetDriveAndCheckSpaceL();

        if ( replaceExisting )
            {
            BackupL(); // nothing need be undone if backup leaves
            }
        }
        ); // TRAPD
    if ( error == KErrNone )
        {
        ProcessRestoreDirL( aRestoreDir );
        }
    else if ( error == KErrAlreadyExists )
        {
        // max number of uid reached, to avoid SBE undo previous widget restore, don't leave with error
        return;
        }
    else
        {
        User::Leave( error );
        }
    }

// ============================================================================
// CWidgetInstaller::SearchWidgetRootL
//
// @since 3.2
// ============================================================================
//
TInt CWidgetInstaller::SearchWidgetRootL( const TDesC& aDir, TFileName& aFoundName )
    {
    CDir* entryList = NULL;
    TBool found = EFalse;
    RFile rFile;

    // aDir = "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID\"
    TInt err = iRfs.GetDir( aDir, KEntryAttMatchMask, EDirsFirst, entryList );
    if ( err == KErrNone )
        {
        CleanupStack::PushL(entryList);

        // check for directory entries
        TInt cnt = entryList->Count();
        for (TInt i=0; i<cnt; i++)
            {
            const TEntry& fileEntry = (*entryList)[i];
            TFileName tempFile( aDir );
            tempFile.Append( fileEntry.iName );
            tempFile.Append( KBackSlash );
            TInt64 dirSize = GetDirSizeL( tempFile );
            if ( 0 == dirSize )
                continue;
            tempFile.Append( KInfoPList );
            TInt error = rFile.Open( iRfs, tempFile, EFileRead );
            CleanupClosePushL( rFile );
            CleanupStack::PopAndDestroy();  // rFile
            if ( KErrNone == error )
                {
                aFoundName.Copy( fileEntry.iName );
                found = ETrue;
                break;
                }
            }  // end of for

            CleanupStack::PopAndDestroy();  // entryList
        }  // end of if  iRfs

    return found;
    }
	
// ============================================================================
// CWidgetInstaller::FixWidgetPropsL()
// Handles preprocessing of widget properties 
//
// @since 5.0
// ============================================================================
//
void CWidgetInstaller::FixWidgetPropsL()
    {
	TInt  backupBlanketPerm = -1; 
    
    CWidgetBackupRegistryXml* aXmlProcessor = CWidgetBackupRegistryXml::NewL();
    RPointerArray<CWidgetPropertyValue>  backupPropertyValues;
   
    TInt i = 0;
    // empty values
    for ( ; i < EWidgetPropertyIdCount; ++i )
        {
        CWidgetPropertyValue* value = CWidgetPropertyValue::NewL();
        CleanupStack::PushL( value );
        User::LeaveIfError( backupPropertyValues.Insert( value, i ) );
        CleanupStack::Pop( value );
        }
               
// Algorithm
// 1. Look for the entry 
// 2. Check if the EBundleIdentifier is what we are looking for
// 3. If so, looking for the value of EBlanketPermGranted
// 4. If exist, then we fix it 
    // hardcode the filename first
    RFile file;
    TFileName propFile( KWidgetPropFile );
    User::LeaveIfError( file.Open( iRfs, propFile, EFileRead )); 
    CleanupClosePushL( file ); 
	
    TInt size;
    User::LeaveIfError ( file.Size ( size )); 
    HBufC8* buf = HBufC8::NewLC ( size ); 
    TPtr8 bufPtr ( buf->Des() );
    User::LeaveIfError( file.Read( bufPtr ) );

    // initialize the parser and check compiled code matches lib version
    LIBXML_TEST_VERSION

    xmlDocPtr doc; 
    doc = xmlReadMemory( (const char *)bufPtr.Ptr(), bufPtr.Length(),
                         NULL, 
                         NULL, 
                         0); 
    if ( !doc )
        {
        User::Leave( KErrCorrupt );
        }

    xmlNode* rootElement = xmlDocGetRootElement( doc );
    TPtrC8 rootTag( rootElement->name );
    if ( 0 != rootTag.Compare( KWidgetRegistry() ) )
    {
        User::Leave( KErrCorrupt );
    }

    for ( xmlNode* m = rootElement->children;
          m;
          m = m->next )
        {
        if ( m->type == XML_ELEMENT_NODE )
            {
            TPtrC8 element( m->name );

            if ( 0 == element.Compare( KEntry() ) )
                {
                backupBlanketPerm = -1;  // reset this value for every entry 	
                if ( NULL == m->children )
                    {
                    // malformed? should we require entry to have
                    // some minimal info?
                    continue;
                    }

                // extract one entry
                xmlNode* n;
                n = m->children;  
                for ( ; 
                      n; 
                    )
                    { 
                    while ( n && ( n->type != XML_ELEMENT_NODE )) 
                        {
                        n = n->next; 
                        }
                    if ( NULL == n ) 
                        {
                        break; 
                        }

                TPtrC8 propTag( n->name );
                if ( 0 != propTag.Compare( KXmlPropTag() ) )
                    {
                    // unrecognized subtree?
                    break;
                    }
                // validate n->children != NULL and type XML_TEXT_NODE
                HBufC* name;
                aXmlProcessor->GetContentL( iRfs, doc, n->children, &name );

                // get value array index (TWidgetPropertyId) for name
                TPtr namePtr( name->Des() );
                TInt propId =
                    aXmlProcessor->GetPropertyId( namePtr );
                delete name;
                name = NULL;
                if ( EWidgetPropertyIdInvalid == propId )
                    {
                    User::Leave( KErrNoMemory );
                    }
					
                n = n->children->next; // down to val
                if ( NULL == n )
                {
                    User::Leave( KErrCorrupt );
                }
                TPtrC8 valTag( n->name );
                if ( 0 != valTag.Compare( KXmlValTag() ) )
                    {
                    User::Leave( KErrCorrupt );
                    }
                if (propId >= EWidgetPropertyIdCount) // unsupported property
                    {
                    HBufC* value = NULL;
                    if (n->children)
                        {
                        aXmlProcessor->GetTextContentAsStringL( iRfs, doc, n->children, &value );
                        }
                    else
                       {
                       value = KNullDesC().AllocL();
                       }

                n = (n->parent)->next; // up two and next sibling
                continue;
                }
                HBufC* value;
                aXmlProcessor->GetContentL( iRfs, doc, n->children, &value );
                CleanupStack::PushL( value );

                n = n->children->next; // down to type
                if ( NULL == n )
                {
                User::Leave( KErrCorrupt );
                }
                TPtrC8 typeTag( n->name );
                if ( 0 != typeTag.Compare( KXmlTypeTag() ) )
                {
                    User::Leave( KErrCorrupt );
                }
                // validate n->children != NULL and type XML_TEXT_NODE
                HBufC* type;
                aXmlProcessor->GetContentL( iRfs, doc, n->children, &type );
                CleanupStack::PushL( type );
                //
                // assume void/unknown is not put in XML format so anything
                // not recognized should be handled like other unrecognized
                // subtree
                TWidgetPropertyType typeEnum = EWidgetPropTypeUnknown;
                if ( 0 == type->Des().Compare( KXmlDataTypeBool() ) )
                    {
                    typeEnum = EWidgetPropTypeBool;
                    }
                else if ( 0 == type->Des().Compare( KXmlDataTypeInt() ) )
                    {
                    typeEnum = EWidgetPropTypeInt;
                    }
                else if ( 0 == type->Des().Compare( KXmlDataTypeString() ) )
                    {
                    typeEnum = EWidgetPropTypeString;
                    }
                else if ( 0 == type->Des().Compare( KXmlDataTypeUid() ) )
                    {
                    typeEnum = EWidgetPropTypeUid;
                    }
                CleanupStack::PopAndDestroy( type );

                // set prop according to type
                switch ( typeEnum )
                    {
                    case EWidgetPropTypeBool:
                    if ( 0 == value->Des().Compare( _L("0") ) )
                        {
                        *backupPropertyValues[propId] = 0;
                        }
                    else
                        {
                        *(backupPropertyValues[propId]) = 1;
                        }        
                    break;
                    
                    case EWidgetPropTypeInt:
                    TLex toInt( value->Des() );
                    TInt k;
                    if ( KErrNone != toInt.Val( k ) )
                       {
                       User::Leave( KErrCorrupt );
                       }
                    if ( propId ==  EBlanketPermGranted )
                        backupBlanketPerm = k;        
                    break;                    
					          
					          case EWidgetPropTypeString:
                    *(backupPropertyValues[propId]) = *value; 
                    break;
                    
                    case EWidgetPropTypeUid:
                        {
                        TLex toUid( value->Des() );
                        TInt u;
                        if ( KErrNone != toUid.Val( u ) )
                            {
                            User::Leave( KErrCorrupt );
                            }
                       *(backupPropertyValues[propId])  = TUid::Uid( u );
                       }
                    break;        
                    };

                   CleanupStack::Pop( value );
                   if ( EWidgetPropTypeString != typeEnum )
                       {
                       delete value;
                       }
        
                   n = ((n->parent)->parent)->next; // up two and next sibling
                   }                     
                   // Compare to see if it's this widget 
                   if ( 0 == (iPropertyValues[EBundleIdentifier]->iValue.s)->Compare(*backupPropertyValues[EBundleIdentifier]->iValue.s) )
                       {
                       if ( backupBlanketPerm != -1 ) 
                           {                       	
                           (*iPropertyValues[EBlanketPermGranted]) = backupBlanketPerm; 
                           }
                       break;
                       }
                } // if <entry>
            } // if n is element
        } // for

 
    TInt j = 0; 
    for ( ; j < EWidgetPropertyIdCount; ++j )
        {
        delete backupPropertyValues[j];
        }
    backupPropertyValues.Close();
           
    // cleanup 
    xmlFreeDoc(doc);
    xmlCleanupParser();
    
#ifdef BRDO_SYMBIAN_LIBXML_FF
    XmlEngineCleanup();
#else    
    xmlCleanupGlobalData();
#endif
    CleanupStack::PopAndDestroy( 2, &file ); // buf, file    

    delete aXmlProcessor; 
	}

// ============================================================================
// CWidgetInstaller::PreprocessWidgetBundleL()
// Handles parsing and creating widget
// Widget data are still in "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID\
//
// @since 3.2
// ============================================================================
//
TBool CWidgetInstaller::PreprocessWidgetBundleL( TDesC& aRestoreDir )
    {
    TBool found = EFalse;
    RFile rFile;
    
    // Reset the PropertyValues array, so as to erase the previous widget values, 
    // otherwise widget installation can fail due to the check for duplicate keys 
    // in ParseValidateBundleMetadataL(). Starting from the second element in the 
    // array, since the first element (EWidgetPropertyListVersion) is already
    // initialized to WIDGETPROPERTYLISTVERSION.
    for (TInt i=1; i<EWidgetPropertyIdCount; i++)
        {
        iPropertyValues[i]->Reset();
        }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // find the top-level directory under which to get metadata
    // aRestoreDir = "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID"
    // tempFile = "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID\"
    // iBundleRootName = "WeatherBug.wdgt"
    TFileName tempFile( aRestoreDir );
    tempFile.Append( KBackSlash );
    // searching for a directory which contains info.plist (under tempFile), 
    // rather than return the first directory found.
    TInt foundRoot = SearchWidgetRootL( tempFile, iBundleRootName );
    if( !foundRoot )
        {
        User::Leave( KErrNotSupported );
        }

    // root dir in widget bundle (not install root) of WidgetBackupRestore
    // e.g. bundleRoot = "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID\WeatherBug.wdgt\"
    TFileName bundleRoot( tempFile );
    bundleRoot.Append( iBundleRootName );
    bundleRoot.Append( KBackSlash );

    TInt64 bundleRootSize = GetDirSizeL( bundleRoot );
    if ( 0 == bundleRootSize )
        {
         User::Leave( KErrNotSupported );
        }
    *(iPropertyValues[EFileSize]) = bundleRootSize;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // metadata file: "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID\WeatherBug.wdgt\Info.plist"
    tempFile.Copy( bundleRoot );
    tempFile.Append( KInfoPList );
    TInt err = rFile.Open( iRfs, tempFile, EFileRead );
    if ( KErrNone != err )
        {
        // missing metadata file
        User::Leave( KErrNotSupported );
        }
    CleanupClosePushL( rFile );

    TInt size = 0;
    rFile.Size( size );
    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 bufferPtr( buffer->Des() );
    User::LeaveIfError( rFile.Read( bufferPtr ) );

    // METADATA PARSE
    iWidgetConfigHandler->ParseValidateBundleMetadataL(
        bufferPtr, iPropertyValues, iRfs );

    CleanupStack::PopAndDestroy( 2, &rFile ); // rFile, buffer

    // Fix the widget properties from restored file 
    TRAP_IGNORE (FixWidgetPropsL());  // Even the fixing leaves, it should not stop the process going 

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // EXISTING WIDGET?
    if( iRegistry.WidgetExistsL( *(iPropertyValues[EBundleIdentifier]) ) )
        {
        // replacement for currently installed widget
        *(iPropertyValues[EUid]) = iRegistry.GetWidgetUidL(
            *(iPropertyValues[EBundleIdentifier]));
        found = ETrue;
        // get original install dir from registry in case user
        // decides to "overrite" to another memory location
        // e.g. iOriginalDir = "\private\[WidgetUIUid]\bundleID\"
        iRegistry.GetWidgetPath( TUid::Uid( *(iPropertyValues[EUid]) ),
                                 iOriginalDir );
        }
    else
        {
        // new widget, get an unused UID for this widget
        TDriveUnit phoneMemDriveUnit( EDriveC );
        const TDesC& drive = phoneMemDriveUnit.Name();
        *(iPropertyValues[EUid]) = iRegistry.GetAvailableUidL(drive[0]).iUid;
        }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // see if main.html exists
    // main.html file: "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID\WeatherBug.wdgt\YahooTraffic.html"
    tempFile.Copy( bundleRoot );
    tempFile.Append( *(iPropertyValues[EMainHTML]) );
    if( !BaflUtils::FileExists( iRfs, tempFile ) )
        {
        User::Leave( KErrCorrupt );
        }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // infoLoc file: "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID\WeatherBug.wdgt\en.lproj\InfoPlist.strings"
    TBuf<32> lproj;
    iRegistry.GetLprojName( lproj ); // e.g. "en", "fr", "zh_Hans" ...
    TFileName infoLocFile( bundleRoot );
    infoLocFile.Append( lproj );
    infoLocFile.Append( KLprojExt );
    infoLocFile.Append( KBackSlash );
    infoLocFile.Append( KInfoPlistStrings );

    err = rFile.Open( iRfs, infoLocFile, EFileRead );

    CleanupClosePushL( rFile );
    if ( err == KErrNone )
        {
        TInt size = 0;
        rFile.Size( size );
        HBufC8* buffer = HBufC8::NewLC( size );
        TPtr8 bufferPtr( buffer->Des() );
        err = rFile.Read( bufferPtr );
        User::LeaveIfError( err );

        // parse the l10n file and localize the bundle display name
        iWidgetConfigHandler->ParseInfoLocL(
            bufferPtr, iRfs, *(iPropertyValues[EBundleDisplayName]) );
        CleanupStack::PopAndDestroy( buffer ); // buffer
        }
    CleanupStack::PopAndDestroy( &rFile ); // rFile

    return found;
    }

void CWidgetInstaller::ProcessRestoreDirL( TDesC& aRestoreDir )
    {
    TInt err = KErrNone;
    HBufC* newDir( HBufC::NewLC( KMaxFileName ) );
    HBufC* tmpDir( HBufC::NewLC( KMaxFileName ) );
    //////////////////////////////////////////////////////////////////////////////////////////////////
    // widgetAppDir = "\private\[WidgetUIUid]\"
    // If the path is missing create it.
    if ( !BaflUtils::FolderExists( iRfs, KWidgetAppDir ) )
        {
        User::LeaveIfError( iRfs.MkDir( KWidgetAppDir ) );
        }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // newDir = "\private\[WidgetUIUid]\bundleID\"    
    *newDir = *(iPropertyValues[EBasePath]);
    // oldDir = "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID"
    *tmpDir =  aRestoreDir;
    // oldDir = "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID\"
    tmpDir->Des().Append( KBackSlash );

    TInt renameError = iRfs.Rename( *tmpDir, *newDir );
    if (renameError)
        {
        // copy if different drive or KErrAlreadyExists
        CFileMan* fileManager = CFileMan::NewL( iRfs );
        CleanupStack::PushL( fileManager );
        User::LeaveIfError( fileManager->Copy( *tmpDir, *newDir, CFileMan::ERecurse ) );
        User::LeaveIfError( fileManager->Attribs(*newDir, KEntryAttNormal, KEntryAttReadOnly,
                            TTime(0), CFileMan::ERecurse) );
        CleanupStack::PopAndDestroy( fileManager );  // fileMananger
        }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // e.g. bundleRootWithDrive = "C:\private\[WidgetUIUid]\bundleID\WeatherBug.wdgt\"
    // e.g. iBundleRootName = "WeatherBug.wdgt"
    TFileName bundleRootWithDrive( *newDir );
    bundleRootWithDrive.Append( iBundleRootName );
    bundleRootWithDrive.Append( KBackSlash );

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // update MainHTML info
    TFileName mainHtml( bundleRootWithDrive );
    mainHtml.Append( *(iPropertyValues[EMainHTML]) );
    *(iPropertyValues[EMainHTML]) = mainHtml;

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // parse the Icon.png file
    // e.g. iconPath = "C:\private\[WidgetUIUid]\bundleID\WeatherBug.wdgt\"
    TFileName iconPath( bundleRootWithDrive );
    TFileName iconFile( iconPath );
    iconFile.Append( KIconFile );
    if ( BaflUtils::FileExists( iRfs, iconFile ) )
        {
        *(iPropertyValues[EIconPath]) = iconPath;

        // scan the bundleRootWithDrive directory
        TFileName oldMbmIconTemp( KNullDesC );
        TBool mbmIconFound = SearchByExtL( bundleRootWithDrive, EDirsLast | EDescending, KMBMExt, oldMbmIconTemp );
        // skip converting to .mbm file, simply rename oldUid.mbm to newUid.mbm
        // where oldUid comes from backup widget,
        // newUid comes from widget registration or reuse of existing widget uid
        if( mbmIconFound )
            {
            TUid uid = TUid::Uid( *(iPropertyValues[EUid]) );
            // e.g. newMbmIcon = "C:\private\[WidgetUIUid]\bundleID\WeatherBug.wdgt\[newUid].mbm"
            TFileName newMbmIcon( iconPath );
            newMbmIcon.Append( uid.Name() );
            newMbmIcon.Append( KMBMExt() );

            // e.g. oldMbmIcon = "C:\private\[WidgetUIUid]\bundleID\WeatherBug.wdgt\[oldUid].mbm"
            TFileName oldMbmIcon( iconPath );
            oldMbmIcon.Append( oldMbmIconTemp );
            err = iRfs.Rename( oldMbmIcon, newMbmIcon );
            // it's ok that oldMbm and newMbm share the same uid
            if( err != KErrNone && err != KErrAlreadyExists )
                {
                User::Leave( err );
                }
            }
         else
            {
            // This case is specific to the WidgetPreInstaller.
            // Once the mbm file can be generated by the script this won't be needed.
            // Create the mbm file from the icon file
            TUid uid = TUid::Uid( *(iPropertyValues[EUid]) );
            ConvertIconL(uid, iconPath);
            }
        }

    //////////////////////////////////////////////////////////////////////////////////////////////////
    // delete "\private\[WidgetUIUid]\bundleID\prefs.dat"
    TFileName   widgetPref( *newDir );
    widgetPref.Append(KWidgetPref);
    err = iRfs.Delete( widgetPref );
    // it's ok not to have pref.dat
    if( err != KErrNone && err != KErrNotFound )
        {
        User::Leave( err );
        }

    CleanupStack::PopAndDestroy( 2, newDir );    
    //////////////////////////////////////////////////////////////////////////////////////////////////
    FinishInstallL( KErrNone );
    }

// ============================================================================
// CWidgetInstaller::FinishInstallL()
//
// @since 3.1
// ============================================================================
//
EXPORT_C void CWidgetInstaller::FinishInstallL( TInt aErr )
    {
#ifdef _DEBUG
    _LIT(KText, "FinishInstallL, aErr = %d, iOverwriting=%d");

    if ( iCanLog )
        {
        iFileLogger.WriteFormat( KText, aErr, iOverwriting );
        }
#endif
    if ( EFalse == iServerConnected )
        {
        User::LeaveIfError( iRegistry.Connect() );
        iServerConnected = ETrue;
        }

    if ( !aErr )
       {
        if ( iOverwriting )
            {
            TUid uid = TUid::Uid( *(iPropertyValues[EUid]) );
            iRegistry.DeRegisterWidgetL( uid );
            iAppManager->DeregisterWidgetL( uid );
            }
        iRegistry.RegisterWidgetL( iPropertyValues );
        iAppManager->RegisterWidgetL( *(iPropertyValues[EMainHTML]),
                                        *(iPropertyValues[EBundleDisplayName]),
                                        *(iPropertyValues[EIconPath]),
                                        *(iPropertyValues[EDriveName]),
                                        TUid::Uid( *(iPropertyValues[EUid]) ) );

        if ( iOverwriting )
            {
            // delete backup
            TInt err = iFileMgr->RmDir( iBackupDir );
            }
       }
    else
        {
        if ( iOverwriting )
            {
            // delete what was being installed, and restore previous
            iFileMgr->RmDir( *(iPropertyValues[EBasePath]) );
            RestoreL();
            }
        }

    iOverwriting = EFalse;
    }

    /**
     * @since 5.0
     */
EXPORT_C void CWidgetInstaller::RegisterWidgetL( const TDesC& aMainHTML,
                                                 const TDesC& aBundleDisplayName,
                                                 const TDesC& aIconPath,
                                                 const TDesC& aDriveName,
                                                 const TUid& aUid )
    {
    iAppManager->RegisterWidgetL( aMainHTML, aBundleDisplayName,
                                  aIconPath, aDriveName, aUid );
    }

/**
 * @since 5.0
 */
EXPORT_C void CWidgetInstaller::DeregisterWidgetL( const TUid& aUid )
    {
    iAppManager->DeregisterWidgetL( aUid );
    }

// ============================================================================
// CWidgetInstaller::RunError()
// It is called by WidgetActiveCallback when InstallL leaves.
//
// @since 3.2
// ============================================================================
//
EXPORT_C TInt CWidgetInstaller::RunError( TInt aError )
    {
    TRAP_IGNORE( FinishInstallL( aError ) );

    return KErrNone; // indicates error was handled
    }

// ============================================================================
// CWidgetInstaller::SetDriveAndCheckSpaceL()
//
// @since 3.2
// ============================================================================
//
TBool CWidgetInstaller::SetDriveAndCheckSpaceL()
    {
    TDriveUnit phoneMemDriveUnit( EDriveC );
    TInt64 driveSpace( 0 );

    TInt error = DriveInfo( (TInt)phoneMemDriveUnit, driveSpace );
    if ( *(iPropertyValues[EFileSize]) > driveSpace )
        {
        User::Leave( KErrNoMemory );
        }

    UpdateWidgetBasePathL( phoneMemDriveUnit );
    *(iPropertyValues[EDriveName]) = phoneMemDriveUnit.Name();

    return ETrue;
    }

// ============================================================================
// CWidgetInstaller::UpdateWidgetBasePathL()
//
// @since 3.1
// ============================================================================
//
void CWidgetInstaller::UpdateWidgetBasePathL( TDriveUnit& aDrive )
    {
    const TDesC& bundleId = *(iPropertyValues[EBundleIdentifier]);
    HBufC* basePath = HBufC::NewL(
        aDrive.Name().Length() + KWidgetAppDir().Length()
        + bundleId.Length() + 1 );
    basePath->Des().Append( aDrive.Name() );
    basePath->Des().Append( KWidgetAppDir );
    basePath->Des().Append( *(iPropertyValues[EBundleIdentifier]) );
    basePath->Des().Append( KBackSlash );

    *(iPropertyValues[EBasePath]) = *basePath;
    delete basePath;
    }

// ============================================================================
// CWidgetInstaller::BackupL()
// When overwriting an installed widget, backup so can restore on error.
//
// @since 3.1
// ============================================================================
//
void CWidgetInstaller::BackupL()
    {
#ifdef _DEBUG
    _LIT(KText1, "inside BackupL, iOriginalDir=%S, iBackupDir=%S");

    if ( iCanLog )
        {
        iFileLogger.WriteFormat( KText1, &iOriginalDir, &iBackupDir );
        }
#endif
    // if backup leaves, there is nothing caller needs to do to clean up

    // move to backup (by renaming) the existing install dir for the widget

    // first create a "unique" backup dir on the original dir drive
    // (same drive required since can't "rename" across drives)
    TParse p;
    p.Set( iOriginalDir, NULL, NULL );
    TFileName path;
    path.Copy( p.Drive() );
    path.Append( KWidgetAppDir );
    // NOT A GOOD WAY TO CREATE A UNIQUE DIRECTORY First we create a
    // unique file and then delete it and then create a directory with
    // same name.
    RFile file;
    CleanupClosePushL( file );
    User::LeaveIfError( file.Temp( iRfs, path, iBackupDir, EFileWrite ) );
    CleanupStack::PopAndDestroy(); // file
    // delete the temp file and...
    iFileMgr->Delete( iBackupDir );
    // ...convert to dir name
    iBackupDir.Append( KBackSlash );

    // make the backup
    User::LeaveIfError( iFileMgr->Rename( iOriginalDir, iBackupDir ) );
    // overwriting boolean is only true when backup is successful
    iOverwriting = ETrue;
    }

// ============================================================================
// CWidgetInstaller::RestoreL()
//
// @since 3.1
// ============================================================================
//
void CWidgetInstaller::RestoreL()
    {
    // move (by renaming) the backup copy to the install dir for the widget

    // to assure a clean destination, try deleting install dir first
    (void)iFileMgr->RmDir( iOriginalDir );

    User::LeaveIfError( iFileMgr->Rename( iBackupDir, iOriginalDir ) );

    // restore complete
    iOverwriting = EFalse;
    }

// ============================================================================
// CWidgetInstaller::DriveInfo
// Indicates if the given drive is available and ready to use.
// (other items were commented in a header).
//
// @since 3.2
// ============================================================================
//
TInt CWidgetInstaller::DriveInfo( TInt aDrive, TInt64& aDiskSpace )
    {
    TInt error = KErrNone;
    TVolumeInfo volumeInfo;

    error = iRfs.Volume( volumeInfo, aDrive );
    if( !error )
        {
        aDiskSpace = volumeInfo.iFree;
        }

    return error;
    }

// ============================================================================
// CWidgetInstaller::SearchByExtL
//
// @since 3.2
// ============================================================================
//
TInt CWidgetInstaller::SearchByExtL( const TDesC& aDir, TUint aSortKey, const TDesC& aExt, TFileName& aFoundName )
    {
    CDir* entryList = NULL;
    TBool found = EFalse;

    // aDir = "\private\[WidgetBackupRestoreUid]\WidgetBURTemp\bundleID\"
    TInt err = iRfs.GetDir( aDir, KEntryAttMatchMask, aSortKey, entryList );
    if ( err == KErrNone )
        {
        CleanupStack::PushL(entryList);

        // check for directory entries
        TInt extLen = aExt.Length();
        TInt cnt = entryList->Count();
        for (TInt i=0; i<cnt; i++)
            {
            const TEntry& fileEntry = (*entryList)[i];
            // when looking for root folder, we don't pass aExt in this function; the first directory will be chosen
            if ( ( aExt.Length() == 0 ) ||
                 ( fileEntry.iName.Right( extLen ).Compare( aExt ) == 0 ) )
                {
                aFoundName.Copy( fileEntry.iName );
                found = ETrue;
                break;
                }
            }  // end of for

            CleanupStack::PopAndDestroy();  // entryList
        }  // end of if  iRfs

    return found;
    }

// ============================================================================
// CWidgetInstaller::GetDirSizeL
//
// @since 3.2
// ============================================================================
//
TInt CWidgetInstaller::GetDirSizeL( const TDesC& aDir )
    {
    CDirScan* scanner = CDirScan::NewLC( iRfs );
    scanner->SetScanDataL( aDir, KEntryAttMatchMask, EDirsLast );

    TInt64 dirSize = 0;
    CDir* results = NULL;
    scanner->NextL( results );
    while( results )
        {
        CleanupStack::PushL( results );

        // Prescan to get the dir file size.
        TInt64 fileSize = 0;
        const TInt count = results->Count();
        TInt i = 0;
        for( i=0; i<count; i++ )
            {
            const TEntry& entry = (*results)[ i ];
            if  ( !entry.IsDir() )
                {
                fileSize += entry.iSize;
                }
            else
                {
                // sort by EDirLast
                break;
                }
            }

        dirSize += fileSize;
        const TPtrC path( scanner->FullPath() );
        CleanupStack::PopAndDestroy( results );

        scanner->NextL( results );
        }

    CleanupStack::PopAndDestroy();  // scanner

    return dirSize;
    }

// ============================================================================
// CWidgetUIOperationsWatcher::ConvertIconL()
// Convert icon.png into mbm format for widget
//
// @since 5.0
// ============================================================================
//
void CWidgetInstaller::ConvertIconL(
    TUid& aUid,
    const TDesC& aIconPath )
    {
    TFileName pngIcon;
    pngIcon.Copy( aIconPath );

    if ( pngIcon.Length() )
        {
        pngIcon.Append( KIconFile );
        TFileName mbmIcon;
        mbmIcon.Copy( aIconPath );
        mbmIcon.Append( aUid.Name() );
        mbmIcon.Append( KMBMExt() );

        CIconConverter* iconConverter = CIconConverter::NewL( this, iRfs );
        CleanupStack::PushL(iconConverter);

        iconConverter->StartToDecodeL( pngIcon, mbmIcon );
        CActiveScheduler::Start(); // wait until the converter process finishes
        CleanupStack::PopAndDestroy(); // iconConverter
        }
    }

// ============================================================================
// CWidgetInstaller::NotifyCompletionL()
// Icon conversion calls this when it is complete.
//
// @since 5.0
// ============================================================================
//
void CWidgetInstaller::NotifyCompletionL( TInt /*aErr*/ )
    {
    CActiveScheduler::Stop();
    }


// ============================================================================
// CWidgetInstaller::WidgetPropertiesFromInstalledWidgetL()
// Generates the widget property values from an already installed widget.
//
// @since 5.0
// ============================================================================
//
EXPORT_C RPointerArray<CWidgetPropertyValue>*
CWidgetInstaller::WidgetPropertiesFromInstalledWidgetL(
    RFs &aFs,
    const TDesC& aWidgetPath, /* should end with backslash */
    const TDesC& aLproj,
    TUid aUid )
    {
    // output, ownership will be handed back to caller
    RPointerArray<CWidgetPropertyValue>* propertyValues = new (ELeave)
        RPointerArray<CWidgetPropertyValue>(
            EWidgetPropertyIdCount );
    CleanupClosePushL( *propertyValues );

    TInt i = 0;
    // empty values
    for ( ; i < EWidgetPropertyIdCount; ++i )
        {
        CWidgetPropertyValue* value = CWidgetPropertyValue::NewL();
        User::LeaveIfError( propertyValues->Insert( value, i ) );
        }
    *(*propertyValues)[EWidgetPropertyListVersion] = WIDGETPROPERTYLISTVERSION;
    // UID
    *(*propertyValues)[EUid] = aUid.iUid;
    // size
    TInt64 bundleRootSize = GetDirSizeL( aWidgetPath );
    if ( 0 == bundleRootSize )
        {
         User::Leave( KErrNotSupported );
        }
    *(*propertyValues)[EFileSize] = bundleRootSize;
    // Info.plist
    TFileName manifest ( aWidgetPath );
    manifest.Append( KInfoPList );
    RFile rFile;
    User::LeaveIfError( rFile.Open( aFs, manifest, EFileRead ) );
    CleanupClosePushL( rFile );
    TInt size = 0;
    rFile.Size( size );
    HBufC8* buffer = HBufC8::NewLC( size );
    TPtr8 bufferPtr( buffer->Des() );
    User::LeaveIfError( rFile.Read( bufferPtr ) );
    iWidgetConfigHandler->ParseValidateBundleMetadataL(
        bufferPtr, *propertyValues, aFs );

    CleanupStack::PopAndDestroy(  ) ; // buffer
    CleanupStack::Pop();
    rFile.Close();
    // update base path from metadata
    *(*propertyValues)[EBasePath] = aWidgetPath;
    // update drive name
    *(*propertyValues)[EDriveName] = aWidgetPath.Left( 2 );
    // update main HTML
    TFileName mainHtml( aWidgetPath );
    mainHtml.Append( *(*propertyValues)[EMainHTML] );
    *(*propertyValues)[EMainHTML] = mainHtml;
    // update icon path, depends on Icon.png and mbm versions existing
    TFileName iconFile( aWidgetPath );
    iconFile.Append( KIconFile );
    if ( BaflUtils::FileExists( aFs, iconFile ) )
        {
        // scan the directory for mbm extension file with processed icon images
        TFileName mbmFilename;
        TBool mbmFound = SearchByExtL( aWidgetPath,
                                       EDirsLast | EDescending,
                                       KMBMExt,
                                       mbmFilename );
        if ( mbmFound )
            {
            // rename mbm file using supplied UID
            // [A-Z]:\private\[WidgetUIUid]\[bundleID]\[zip-root]\[UID].mbm"
            TFileName oldMbmFilename( aWidgetPath );
            oldMbmFilename.Append( mbmFilename );
            TFileName newMbmFilename( aWidgetPath );
            newMbmFilename.Append( aUid.Name() );
            newMbmFilename.Append( KMBMExt() );

            TInt error = iRfs.Rename( oldMbmFilename, newMbmFilename );
            // it's ok that oldMbm and newMbm share the same uid
            if( error != KErrNone && error != KErrAlreadyExists )
                {
                User::Leave( error );
                }
            *(*propertyValues)[EIconPath] = aWidgetPath;
            }
        }

    // localization of display name
    TFileName infoLocFile( aWidgetPath );
    infoLocFile.Append( aLproj );
    infoLocFile.Append( KLprojExt );
    infoLocFile.Append( KBackSlash );
    infoLocFile.Append( KInfoPlistStrings );
    TInt error = rFile.Open( aFs, infoLocFile, EFileRead );
    CleanupClosePushL( rFile );
    if ( error == KErrNone )
        {
        TInt size = 0;
        rFile.Size( size );
        HBufC8* buffer = HBufC8::NewLC( size );
        TPtr8 bufferPtr( buffer->Des() );
        error = rFile.Read( bufferPtr );
        User::LeaveIfError( error );
        // parse the l10n file and localize the bundle display name
        iWidgetConfigHandler->ParseInfoLocL(
            bufferPtr, aFs, *(*propertyValues)[EBundleDisplayName] );
        CleanupStack::PopAndDestroy(  ); // buffer
        }


    CleanupStack::Pop( 2 ); // rfile, propertyValues
    rFile.Close();
    return propertyValues;
    }

EXPORT_C TInt CWidgetInstaller::RenameIconFile(
    RFs &aFs,
    RPointerArray<CWidgetPropertyValue>* aEntry )
    {
    // update icon path, depends on Icon.png and mbm versions existing
    const TDesC& mainHtml = *(*aEntry)[EMainHTML];
    TInt mainHtmlPathEnd = mainHtml.LocateReverse( TChar( '\\' ) );
    if ( KErrNotFound == mainHtmlPathEnd )
        {
        return KErrNotFound;
        }
    TFileName mainPath(  mainHtml.Left( mainHtmlPathEnd + 1 ) );
    TFileName iconFile( mainPath );
    iconFile.Append( KIconFile );
    if ( BaflUtils::FileExists( aFs, iconFile ) )
        {
        // scan the directory for mbm extension file with processed icon images
        TFileName mbmFilename;
        TBool mbmFound = SearchByExtL( mainPath,
                                       EDirsLast | EDescending,
                                       KMBMExt,
                                       mbmFilename );
        if ( mbmFound )
            {
            // rename mbm file using supplied UID
            // [A-Z]:\private\[WidgetUIUid]\[bundleID]\[zip-root]\[UID].mbm"
            TFileName oldMbmFilename( mainPath );
            oldMbmFilename.Append( mbmFilename );
            TFileName newMbmFilename( mainPath );
            TInt uid = *(*aEntry)[EUid];
            newMbmFilename.Append( TUid::Uid(uid).Name() );
            newMbmFilename.Append( KMBMExt() );

            // it's ok that oldMbm and newMbm share the same uid
            TInt error = iRfs.Rename( oldMbmFilename, newMbmFilename );
            if( error != KErrNone && error != KErrAlreadyExists )
                {
                return error;
                }
            *(*aEntry)[EIconPath] = mainPath;
            }
        }
    return KErrNone;
    }

//  End of File
