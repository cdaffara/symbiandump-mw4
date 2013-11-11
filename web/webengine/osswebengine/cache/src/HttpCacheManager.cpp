/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Implementation of CHttpCacheManager
*
*/

// INCLUDE FILES
#include "HttpCacheManager.h"
#include "HttpCacheHandler.h"
#include "HttpCacheUtil.h"
#include "HttpCacheFileWriteHandler.h"
#include <cenrepnotifyhandler.h>
#include <httpcachemanagerinternalcrkeys.h>
#include <CoreApplicationUIsSDKCRKeys.h>
#include <centralrepository.h>
#include <featmgr.h>
#include <http/rhttptransaction.h>
#include <sysutildomaincrkeys.h>
#include <eikenv.h>
#include <f32file.h>
#include <e32hashtab.h>
#include <mmf/common/mmfcontrollerpluginresolver.h>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

// kbyte
const TUint KDefaultCacheSize = 1048576; // 1MB = 1024*1024
const TUint KDefaultOperatorCacheSize = 300*1024; // 300KB
_LIT( KDefaultCacheDrive, "c");
_LIT( KDefaultCacheDir, "c:\\system\\cache\\");
_LIT( KDefaultOperatorCacheDir, "c:\\system\\cache\\");
_LIT( KDefaultVssCacheDir, "c:\\system\\cache\\vss\\");
_LIT( KDefaultIndexFile, "index.dat" );
_LIT( KDefaultOpIndexFile, "index_op.dat" );
_LIT( KDefaultVSSIndexFile, "index_vss.dat" );
_LIT( KIndexFileExtension, ".dat" );
_LIT( KCrashCheck, "browser.val");
_LIT( KValidationFileExtension, ".val" );
_LIT8( KVSSHeaderFieldName, "X-Vodafone-Content" );
_LIT8( KVSSHeaderFieldValue, "Portal" );
// default override string, browser gets 2MB, widgetUI gets 1MB, OVI Store gets 1MB : max 4MB.
_LIT( KDefaultOverrideString, "10008D39;1;2048;C;10282822;1;1024;C;102829A0;1;1024;C" );

// MACROS
#define KUIDBROWSERNG 0x10008D39
#define KUIDWIDGETUI 0x10282822
#define KUIDBROWSERCACHECLEANER 0x20026777
#define KUIDOPERATORMENU 0x100039CE
#define KUIDOVISTORE 0x102829A0

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

TBool IsBrowser()
    {
    RProcess process;
    TSecureId secId = process.SecureId();
    TUint32 secIdInt = secId;
    
    return ( secIdInt == KUIDBROWSERNG );
    }


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CHttpCacheManager::CHttpCacheManager
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CHttpCacheManager::CHttpCacheManager():iCacheFolder( KDefaultCacheDir )
    {
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CHttpCacheManager::ConstructL()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L("-------------------------------------------"));
    HttpCacheUtil::WriteLog( 0, _L("| Creating new CHttpCacheManager Instance |"), (TInt)this);
    HttpCacheUtil::WriteLog( 0, _L("-------------------------------------------"));
#endif
    
    CreateCacheHandlersL();
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L("Created cache handlers"), (TInt)this);
#endif

    // read offline mode
    if( FeatureManager::FeatureSupported( KFeatureIdOfflineMode ) )
        {
        // check if connection is not allowed and
        // come up with a listerner on the offline setting
        TInt connAllowed( 1 );

        iOfflineRepository = CRepository::NewL( KCRUidCoreApplicationUIs );

        iOfflineRepository->Get( KCoreAppUIsNetworkConnectionAllowed, connAllowed );

        iOfflineMode = !connAllowed;

        iOfflineNotifyHandler = CCenRepNotifyHandler::NewL( *this, *iOfflineRepository,
            CCenRepNotifyHandler::EIntKey, (TUint32)KCoreAppUIsNetworkConnectionAllowed );
        iOfflineNotifyHandler->StartListeningL();
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
EXPORT_C CHttpCacheManager* CHttpCacheManager::NewL()
    {
    CHttpCacheManager* self = new( ELeave ) CHttpCacheManager();

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
CHttpCacheManager::~CHttpCacheManager()
    {
#ifdef __CACHELOG__
    HttpCacheUtil::WriteLog( 0, _L("-----------------------------------------"));
    HttpCacheUtil::WriteLog( 0, _L("| Destroying CHttpCacheManager Instance | "), (TInt)this);
    HttpCacheUtil::WriteLog( 0, _L("-----------------------------------------"));
#endif
    
    if( IsBrowser() )    // only enabled in browser
        {
        // delete crashcheck.dat file
        RFs rfs;
        rfs.Connect();
        rfs.SetSessionPath( iCacheFolder );
        rfs.Delete(KCrashCheck());
        rfs.Close();
        }
    
    delete iOfflineNotifyHandler;
    delete iOfflineRepository;
    delete iCache;
    delete iOperatorCache;
    delete iOpDomain;
    delete iphoneSpecificCache;
    delete iVSSWhiteList;
    delete iFileWriteHandler;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::VSSRequestCheck
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheManager::VSSRequestCheckL( const RHTTPTransaction& aTrans,
                                           const RHTTPHeaders& aHttpHeader,
                                           const TDesC8& aUrl )
    {
    TBool VSSTransaction ( EFalse );
    if ( iVSSCacheEnabled && HttpCacheUtil::VSSCacheContent( aUrl, iVSSWhiteList ) )
        {
        RStringPool strP = aTrans.Session().StringPool();

#ifdef __CACHELOG__
        RHTTPHeaders headers ( aHttpHeader );
        HBufC8* responseHeaderStr8 = HttpCacheUtil::HeadersToBufferLC( headers, strP );
        HBufC* responseHeaderStr;
        if( responseHeaderStr8 )
            {
            responseHeaderStr = HBufC::NewL( responseHeaderStr8->Length() );
            responseHeaderStr->Des().Copy( responseHeaderStr8->Des() );
            CleanupStack::PopAndDestroy(); //  responseHeaderStr8;
            TPtrC headerStr( responseHeaderStr->Des() );
            HttpCacheUtil::WriteLog( 0, _L("========== VSS Header Start =========\n") );
            HttpCacheUtil::WriteLog( 0, responseHeaderStr->Des() );
            HttpCacheUtil::WriteLog( 0, _L("========== VSS Header End =========\n") );
            delete responseHeaderStr;
            }
#endif

        TPtrC8 nameStr ( KVSSHeaderFieldName() );

        RStringF VSSnameStr = strP.OpenFStringL( nameStr );
        CleanupClosePushL<RStringF>( VSSnameStr);

        THTTPHdrVal tempVal;
        if ( aHttpHeader.GetField( VSSnameStr, 0, tempVal ) == KErrNone )
            {
            TPtrC8 valueStr ( KVSSHeaderFieldValue() );
            RStringF VSSValueStr = strP.OpenFStringL( valueStr );
            CleanupClosePushL<RStringF>( VSSValueStr );

            if ( tempVal == VSSValueStr )
                {
                VSSTransaction      = ETrue;
                }
            CleanupStack::PopAndDestroy(); // VSSValueStr
            }
        CleanupStack::PopAndDestroy(); // VSSnameStr
        } //end if( iVSSCacheEnabled && HttpCacheUtil::VSSCacheContent( aUrl, iVSSWhiteList ) )
    return VSSTransaction;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RequestL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpCacheManager::RequestL(
    RHTTPTransaction& aTrans,
    TBrCtlDefs::TBrCtlCacheMode aCacheMode,
    THttpCacheEntry& aCacheEntry )
    {
    //
    TInt status( KErrNotFound );
    if( iCacheEnabled || iVSSCacheEnabled )
        {
        CHttpCacheHandler* cache = CacheHandler( aTrans.Request().URI().UriDes(), NULL ) ;
        // adjust cache mode in case of offline operation
        if( iOfflineMode )
            {
            aCacheMode = TBrCtlDefs::ECacheModeOnlyCache;
            }
        //
        if( cache )
            {
            status = cache->RequestL( aTrans, aCacheMode, aCacheEntry );
            // save cache handler even if the entry is not in the cache
            // so that when the response comes back, we do not need to
            // check the url
             if( status == KErrNotFound && iVSSCacheEnabled )
                {
                 status = iphoneSpecificCache->RequestL( aTrans, aCacheMode, aCacheEntry );
                 if( (status  == KErrNotReady) || (status == KErrNone ) )
                 {
                 aCacheEntry.iCacheHandler = iphoneSpecificCache;
                 }
                 else
                 {
                 // No entry found in any cache. Default to normal cache
                 aCacheEntry.iCacheHandler = cache;
                 }
                }
            else
            {
            aCacheEntry.iCacheHandler = cache;
            }
           } //end if( cache )

        }//end  if( iCacheEnabled || iVSSCacheEnabled )
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RequestHeadersL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpCacheManager::RequestHeadersL(
    RHTTPTransaction& aTrans,
    THttpCacheEntry& aCacheEntry )
    {
    TInt status( KErrNotFound );
    if( ( iCacheEnabled || iVSSCacheEnabled ) && aCacheEntry.iCacheHandler )
        {
        status = aCacheEntry.iCacheHandler->RequestHeadersL( aTrans, aCacheEntry );
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RequestNextChunkL
//
// -----------------------------------------------------------------------------
//
EXPORT_C HBufC8* CHttpCacheManager::RequestNextChunkL(
    RHTTPTransaction& aTrans,
    TBool& aLastChunk,
    THttpCacheEntry& aCacheEntry )
    {
    HBufC8* bodyStr = NULL;
    if( ( iCacheEnabled || iVSSCacheEnabled ) && aCacheEntry.iCacheHandler )
        {
        bodyStr = aCacheEntry.iCacheHandler->RequestNextChunkL( aTrans, aLastChunk, aCacheEntry );
        }

    return bodyStr;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RequestClosed
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpCacheManager::RequestClosed(
    RHTTPTransaction* aTrans,
    THttpCacheEntry& aCacheEntry )
    {
    if( ( iCacheEnabled || iVSSCacheEnabled ) && aCacheEntry.iCacheHandler )
        {
        aCacheEntry.iCacheHandler->RequestClosed( aTrans, aCacheEntry );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::ReceivedResponseHeadersL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpCacheManager::ReceivedResponseHeadersL(
  RHTTPTransaction& aTrans,
    THttpCacheEntry& aCacheEntry )
    {
    if ( iCacheEnabled || iVSSCacheEnabled )
        {
        if ( iVSSCacheEnabled &&
             VSSRequestCheckL( aTrans, aTrans.Response().GetHeaderCollection(),
                               aTrans.Request().URI().UriDes() ) )
            {
            //Modify the cache handler if VSS specific
            aCacheEntry.iCacheHandler = iphoneSpecificCache;
            }
        if ( aCacheEntry.iCacheHandler )
            {
            aCacheEntry.iCacheHandler->ReceivedResponseHeadersL( aTrans, aCacheEntry );
            }
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::ReceivedResponseBodyDataL
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpCacheManager::ReceivedResponseBodyDataL(
  RHTTPTransaction& aTrans,
    MHTTPDataSupplier& aBodyDataSupplier,
    THttpCacheEntry& aCacheEntry )
    {
    if( ( iCacheEnabled || iVSSCacheEnabled ) && aCacheEntry.iCacheHandler )
        {
        aCacheEntry.iCacheHandler->ReceivedResponseBodyDataL( aTrans, aBodyDataSupplier, aCacheEntry );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::ResponseComplete
//
// -----------------------------------------------------------------------------
//
EXPORT_C void CHttpCacheManager::ResponseComplete(
    RHTTPTransaction& aTrans,
    THttpCacheEntry& aCacheEntry )
    {
    if( ( iCacheEnabled || iVSSCacheEnabled ) && aCacheEntry.iCacheHandler )
        {
        aCacheEntry.iCacheHandler->ResponseComplete( aTrans, aCacheEntry );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RemoveAllL
// Removes all files from HTTP cache. Also tries to remove orphaned files,
// i.e files found on disk, but not registered in the cache's lookup table
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpCacheManager::RemoveAllL()
    {
    TInt numOfBytes( 0 );
    // do not remove op cache
    if( iCacheEnabled )
        {
        numOfBytes = iCache->RemoveAllL();
        }

    //failure here is not mission critical
    TRAP_IGNORE( RemoveOrphanedFilesL() );

    return numOfBytes;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RemoveOrphanedFilesL
// Removes header/body files that exist on the file-system, but are not known to the in-memory Cache lookup table(s)
// We do this because cache performance degrades substantially if there are too many files in a Symbian FAT32 directory.
// Browser crash or out-of-battery situations may cause SaveLookuptable() to be not called, leading to such "orphaned files".
// Due to high file-server interaction, don't call this method from performance critical code.
// -----------------------------------------------------------------------------
void CHttpCacheManager::RemoveOrphanedFilesL()
    {
    // Only proceed if we are running the browser configuration, otherwise we will accidentally
    // remove content we shouldn't.
    // get our SID.
    if( !IsBrowser() )
        return;
    
    // Step 1. Get map of disk content
    RFs rfs;
    User::LeaveIfError( rfs.Connect() );
    CleanupClosePushL( rfs );
    
    // Map that contains pointers to fully-qualified file paths as Keys, and "to be deleted flag" as Value.
    // Initially we mark everything as a candidate for deletion.
    CHttpCacheFileHash *onDiskFilesMap = NULL;
    HttpCacheUtil::GenerateCacheContentHashMapL( onDiskFilesMap, rfs, iCacheFolder, KCacheFileNeedsDelete );
    CleanupStack::PushL( onDiskFilesMap );
    
    // mark everything in the cache as no delete needed.
    MarkAllCacheContentAsNoDelete( onDiskFilesMap );

    // tell any other clients that we are about to remove their cached content
    WipeAllOtherIndexFilesL( onDiskFilesMap, rfs );
    
    // delete any remaining marked files
    DeleteMarkedFilesL( onDiskFilesMap, rfs );

    CleanupStack::PopAndDestroy(1, onDiskFilesMap );
    CleanupStack::PopAndDestroy(1, &rfs );
    }

void CHttpCacheManager::MarkAllCacheContentAsNoDelete( CHttpCacheFileHash* aOnDiskFilesMap )
    {
    //Step 2. Get list of known (non-orphaned) files in each Cache's in-memory lookup table. Flag them as DO NOT DELETE
    //Ask CacheHandlers to add their KNOWN files to this array. No ownership transfer occurs.
    //Don't go ahead if any of the cache handlers choke to insure correct deletion of files.
    if( iCache )
        iCache->ValidateCacheEntriesL(aOnDiskFilesMap);
    if( iOperatorCache )
        iOperatorCache->ValidateCacheEntriesL(aOnDiskFilesMap);
    if( iphoneSpecificCache )
        iphoneSpecificCache->ValidateCacheEntriesL(aOnDiskFilesMap);
    }

void CHttpCacheManager::GenerateEmptyIndexFileL(const TDesC& aIndexFile, RFs& aRfs )
    {
    // Going to remove non-web client cache files in OrphanedFilesL call,
    // Signal to these clients by emptying (or creating) an 'empty' index file
    // do this before we start cleaning up files, to lessen the chances that any
    // of the files are in use when we're trying to delete them.
    
    // 'adopt' code from httpcachelookuptable for dealing with indexes.
    // save entries to index.dat
    RFileWriteStream writeStream;

    TInt ret = KErrNone;
    TInt tryCount = 0;
    for (tryCount = 0; tryCount < 5; tryCount++) 
        {
        ret = writeStream.Replace( aRfs, aIndexFile, EFileWrite );
        if (ret == KErrInUse)
            {
            // When the cache is full, it takes 65 - 85 miliseconds to write the index.
            // So wait 50 miliseconds and try again
            User::After(50000);
            }
        else
            {
            break;
            }
        }
    if( ret == KErrNone )
        {
        CleanupClosePushL( writeStream );
        writeStream.WriteInt32L( KCacheVersionNumber );
        writeStream.WriteInt32L( 0 ); // no entries in the index.
        writeStream.CommitL();
        CleanupStack::PopAndDestroy(); // writeStream
        }
    }

void CHttpCacheManager::WipeAllOtherIndexFilesL( CHttpCacheFileHash* aOnDiskFilesMap, RFs& aRfs )
    {
    // look through hashmap for any .dat files which don't belong to the instantiated caches and overwrite them with blank ones.
    THttpCacheFileHashIter iter( aOnDiskFilesMap->HashMap() );
    const TDesC* key;
    while ((key = iter.NextKey()) != 0)
        {
        const TFileInfo* value ( (iter.CurrentValue()) );
        if( value->iUserInt == KCacheFileNeedsDelete )
            {
            TPtrC ext( key->Right(KIndexFileExtension().Length()) );
            if (    ext.CompareF( KIndexFileExtension ) == 0 ) // find any .dat index files
                { 
                GenerateEmptyIndexFileL( *key , aRfs );
                }
            }
        }
    }

void CHttpCacheManager::DeleteMarkedFilesL( CHttpCacheFileHash* aOnDiskFilesMap, RFs& aRfs )
    {
    //Step 3. Delete all files on disk that don't belong to any of the Cache Handlers.
    CFileMan* fileMan = CFileMan::NewL( aRfs );
    THttpCacheFileHashIter iter( aOnDiskFilesMap->HashMap() );
    const TDesC* key;
    while ((key = iter.NextKey()) != 0)
        {
        const TFileInfo* value ( (iter.CurrentValue()) );
        if( value->iUserInt == KCacheFileNeedsDelete )
            {
            TPtrC ext( key->Right(KIndexFileExtension().Length()) );
            if (    ext.CompareF( KIndexFileExtension ) != 0 && // ignore any .dat index files
                    ext.CompareF( KValidationFileExtension ) != 0 ) // ignore any .val files
                { 
                fileMan->Delete( *key );
                }
            }
        }
    delete fileMan;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::RemoveL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpCacheManager::RemoveL(
    const TDesC8& aUrl )
    {
    TInt status( KErrNotFound );
    if( iCacheEnabled )
        {
        // do not remove op cache
        status = iCache->RemoveL( aUrl );
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::Find
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CHttpCacheManager::Find(
    const TDesC8& aUrl )
    {
    TBool found( EFalse );
    //
    if( iCacheEnabled || iVSSCacheEnabled )
        {
        CHttpCacheHandler* cache = CacheHandler( aUrl, NULL );
        //
        if( cache )
            {
            found = cache->Find( aUrl );
            }

        if( !found && iVSSCacheEnabled )
            {
            found = iphoneSpecificCache->Find( aUrl );
            }
        }
    return found;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::VSSHeaderCheck
//
// -----------------------------------------------------------------------------
//
TBool CHttpCacheManager::VSSHeaderCheck( TDes8*  aHttpHeaderString ) const
    {
    TBool found ( EFalse );
    if( aHttpHeaderString->Size() > 0 )
        {
        TPtrC8 nameStr8( KVSSHeaderFieldName() );

        TInt VSSnameLocation = aHttpHeaderString->FindC( nameStr8 ) ;

        if ( VSSnameLocation != KErrNotFound )
            {
            TPtrC8 valueStr8( KVSSHeaderFieldValue() );
            TInt VSSvalueLocation = aHttpHeaderString->FindC( KVSSHeaderFieldValue() );

            if ( (VSSvalueLocation != KErrNotFound ) && ( VSSnameLocation < VSSvalueLocation ) )
                {
                found = ETrue;
                }
            } //end if ( VSSnameLocation != KErrNotFound )
        } //end  if( aHttpHeaderString.Size() > 0 )

    return found;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::SaveL
//
// -----------------------------------------------------------------------------
//
EXPORT_C TBool CHttpCacheManager::SaveL(
    const TDesC8& aUrl,
    const TDesC8& aHeader,
    const TDesC8& aContent,
    const TDesC8&  aHttpHeaderString)
    {
    TBool saved( EFalse );
    //
    if( iCacheEnabled || iVSSCacheEnabled )
        {
        HBufC8* headerStr = HBufC8::NewL( aHttpHeaderString.Length() );
        headerStr->Des().Copy( aHttpHeaderString );
        TPtr8 headerStrPtr8 ( headerStr->Des() ); //Any Type of TPtrc8
        CHttpCacheHandler* cache = CacheHandler( aUrl, &headerStrPtr8 );
        delete headerStr;
        //
        if( cache )
            {
            saved = cache->SaveL( aUrl, aHeader, aContent );
            }
        }
    return saved;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::AddHeaderL
// Not supported for Vodafone specific storage.
// -----------------------------------------------------------------------------
//
EXPORT_C TInt CHttpCacheManager::AddHeaderL(
    const TDesC8& aUrl,
    const TDesC8& aName,
    const TDesC8& aValue )
    {
    TBool status( KErrNotFound );
    //
    if( iCacheEnabled || iVSSCacheEnabled )
        {
        CHttpCacheHandler* cache = CacheHandler( aUrl, NULL );
        //
        if( cache )
            {
            status = cache->AddHeaderL( aUrl, aName, aValue );
            }
        }
    return status;
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::CreateCacheHandlersL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheManager::HandleNotifyString(
    const TUint32 aKeyId,
    const TDesC& aValue )
    {
    // check offline mode
    if( aKeyId == KCoreAppUIsNetworkConnectionAllowed )
        {
        iOfflineMode = ( aValue == _L("1") ? EFalse : ETrue );
        }
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::CreateCacheHandlersL
//
// -----------------------------------------------------------------------------
//
void CHttpCacheManager::CreateCacheHandlersL()
    {
    // get our SID.
    RProcess process;
    TSecureId secId = process.SecureId();
    TUint32 secIdInt = secId;

    CRepository* repository = CRepository::NewLC(KCRUidCacheManager);
    CRepository* repositoryDiskLevel = CRepository::NewLC(KCRUidDiskLevel);

    // Get Cache Postpone Parameters.
    THttpCachePostponeParameters postpone;
    TBool newCentrepPresent;
    GetPostponeParamsL( newCentrepPresent, postpone, repository );

    // apply default cache configuration - no cache for anyone, sizes set to default.
    iCacheEnabled = EFalse;
    TInt cacheSize( KDefaultCacheSize );
    iCacheFolder.Copy( KDefaultCacheDir );
   
    TBool opCacheEnabled( EFalse );
    iOperatorCache = 0;
    TInt opCacheSize = KDefaultOperatorCacheSize;
    TFileName opCacheFolder( KDefaultOperatorCacheDir );
    
    iVSSCacheEnabled = EFalse;
    TInt vssCacheSize = KDefaultOperatorCacheSize;
    TFileName vssCacheFolder( KDefaultVssCacheDir );

    if(newCentrepPresent)
        {
        // if the new Centrep file is present, we trust it's configured properly.
        GetHttpCacheConfigL( *repository, iCacheEnabled, cacheSize, iCacheFolder );
        GetOperatorCacheConfigL( *repository, opCacheEnabled, opCacheSize, opCacheFolder );
        GetVSSCacheConfigL( *repository, iVSSCacheEnabled, vssCacheSize, vssCacheFolder );
        }
    
    // look for per-process overrides in central repository.  Do this before we get drive critical levels so we're on the right
    // drive.
    // This has a default string which applies the new per-client default configuration
    ApplyCacheOverridesL(*repository, secIdInt, iCacheEnabled, cacheSize, opCacheEnabled, iVSSCacheEnabled, iCacheFolder, KDefaultCacheDrive());

    TInt criticalLevel = 0;
    GetCriticalDriveLevelsL( *repositoryDiskLevel, iCacheFolder, criticalLevel );
    
    // Create any caches we should be using.
    if (iCacheEnabled)
        {
        CreateHttpCacheL( secIdInt, cacheSize, criticalLevel, postpone );

        if (opCacheEnabled)
            {
            CreateOperatorCacheL( *repository, opCacheFolder, opCacheSize, criticalLevel, postpone );
            }
        } //end if( iCacheEnabled )

    if ( iVSSCacheEnabled )
        {
        CreateVssCacheL( *repository, vssCacheFolder, vssCacheSize, criticalLevel, postpone );
        }

    CleanupStack::PopAndDestroy(2); // repository, repositoryDiskLevel
    CrashCheckL( secIdInt );
    }

void CHttpCacheManager::ApplyCacheOverridesL(CRepository& aRepository, const TUint32& aSecIdInt, TBool& aCacheEnabled, TInt& aCacheSize, TBool& aOpCacheEnabled, TBool& aVssCacheEnabled, TDes& aPath, const TDesC& aDefaultDrive)
    {
    TDriveUnit drive(aDefaultDrive);
    
    // set defaults
    if(aSecIdInt == KUIDBROWSERNG)       // for the browser, force use of Operator and VSS caches
        {
        aOpCacheEnabled = ETrue;
        aVssCacheEnabled = ETrue;
        }
    
    // read override string from centrep
    HBufC16 *overrideBuf = HBufC16::NewLC(64);
    TPtr overrideStr(overrideBuf->Des());
    TInt strLen;
    TInt err = aRepository.Get(KCacheManagerHttpCacheProcessOverride, overrideStr, strLen);
    if(strLen > overrideBuf->Length())
        {
        overrideBuf = overrideBuf->ReAllocL(strLen);
        // make sure cleanup stack contains correct pointer since ReAllocL always allocates a new des for larger space.
        CleanupStack::Pop();
        CleanupStack::PushL(overrideBuf);
        // reassign the TPtr
        overrideStr.Set(overrideBuf->Des());
        // pull in the whole string
        aRepository.Get(KCacheManagerHttpCacheProcessOverride, overrideStr, strLen);
        }
    // if we failed to load an override string, use the default.
    if( overrideStr.Length() == 0 )
        {
        CleanupStack::PopAndDestroy( overrideBuf );
        overrideBuf = KDefaultOverrideString().AllocLC();
        overrideStr.Set( overrideBuf->Des() );
        }
    // Built in Lex likes white space to separate strings, but easier to enter with ; separators.  Replace all ; with spaces.
    TInt pos=0;
    do{
        if(overrideStr[pos] == ';')
            {
            overrideStr[pos] = ' ';
            }
        pos++;
    }while(pos < overrideStr.Length());
    
    TLex overrideLex(overrideStr);
    do{
        TUint32 tempId;
        User::LeaveIfError(overrideLex.BoundedVal(tempId,EHex,KMaxTUint32));
        if(overrideLex.TokenLength() != 8)  // if we're not pointing at an SID in the string, we are incorrect and the override is broken.
            User::Leave(KErrCorrupt);
        overrideLex.SkipSpace();
        TInt32 tempCacheEnabled;
        User::LeaveIfError(overrideLex.BoundedVal(tempCacheEnabled,KMaxTInt32));
        overrideLex.SkipSpace();
        TInt32 tempCacheSize;
        User::LeaveIfError(overrideLex.BoundedVal(tempCacheSize,KMaxTInt32));
        overrideLex.SkipSpace();
        TDriveUnit tempDrive(overrideLex.NextToken());
        overrideLex.SkipSpaceAndMark();
        // found a hex SID matching ours, use the parameters.
        if(tempId == aSecIdInt)
            {
            aCacheEnabled = tempCacheEnabled;
            aCacheSize = tempCacheSize * 1024; // conf is in KB
            drive = tempDrive;
            break;
            }
    }while(!overrideLex.Eos());

    // Modify drive letter on aPath to match
    TParsePtr parsePath(aPath);
    TPtrC pathStr(parsePath.Path());
    TPath tempPath;
    tempPath.Format(_L("%c:%S"), TInt(drive)+'A', &pathStr);
    aPath.Copy(tempPath);
    HttpCacheUtil::EnsureTrailingSlash( aPath );
    
    CleanupStack::PopAndDestroy(overrideBuf);
    }

// -----------------------------------------------------------------------------
// CHttpCacheManager::CacheHandler
//
// -----------------------------------------------------------------------------
//
CHttpCacheHandler* CHttpCacheManager::CacheHandler(
    const TDesC8& aUrl,
    TDes8* aHttpHeaderString ) const
    {
    CHttpCacheHandler* cache;
    if( iVSSCacheEnabled && aHttpHeaderString && VSSHeaderCheck( aHttpHeaderString ) )
        {
        cache = iphoneSpecificCache;
        }
    else
        {
        cache = ( ( iOpDomain && HttpCacheUtil::OperatorCacheContent( iOpDomain->Des(), aUrl ) ) ? iOperatorCache : iCache );
        }
    return cache;
    }

void CHttpCacheManager::GetPostponeParamsL( TBool& aNewCentrepPresent, THttpCachePostponeParameters& aParams, CRepository* aRepo )
    {
    aNewCentrepPresent = EFalse;
    if (KErrNone == aRepo->Get(KCacheWritePostponeEnabled,
            aParams.iEnabled))
        {
        User::LeaveIfError(aRepo->Get(
                KCacheWritePostponeFreeRAMThreshold,
                aParams.iFreeRamThreshold));
        User::LeaveIfError(aRepo->Get(
                KCacheWritePostponeImmediateWriteThreshold,
                aParams.iImmediateWriteThreshold));
        User::LeaveIfError(aRepo->Get(KCacheWritePostponeWriteTimeout,
                aParams.iWriteTimeout));
        
        aNewCentrepPresent = ETrue;
        }
    }

void CHttpCacheManager::GetHttpCacheConfigL( CRepository& aRepository, TBool& aCacheEnabled, TInt& aCacheSize, TDes& aCacheFolder )
    {
    // General HTTP Cache
    TBool cacheEnabled( EFalse );
    aCacheFolder.Copy( KDefaultCacheDir );
    
    TInt err = aRepository.Get(KCacheManagerHttpCacheEnabled, cacheEnabled);
    aCacheEnabled = cacheEnabled;
    if( err == KErrNone )
        {
        // cache size
        TInt tempCacheSize;
        if( aRepository.Get(KCacheManagerHttpCacheSize, tempCacheSize) == KErrNone )
            aCacheSize = tempCacheSize;

        // cache location
        TFileName tempCacheLocation;
        if( aRepository.Get(KCacheManagerHttpCacheFolder, tempCacheLocation) == KErrNone )
            aCacheFolder.Copy( tempCacheLocation );
        }
    HttpCacheUtil::EnsureTrailingSlash( aCacheFolder );
    }

void CHttpCacheManager::GetOperatorCacheConfigL( CRepository& aRepository, TBool& aOpCacheEnabled, TInt& aOpCacheSize, TDes& aOpCacheFolder )
    {
    // Operator Cache
    aOpCacheEnabled = FeatureManager::FeatureSupported(KFeatureIdOperatorCache);
    if( aOpCacheEnabled )
        {
        TInt tempOpCacheSize;
        if( aRepository.Get(KOperatorCacheSize, tempOpCacheSize) == KErrNone )
            aOpCacheSize = tempOpCacheSize;

        // op cache folder
        TFileName tempOpCacheFolder;
        if( aRepository.Get(KOperatorCacheFolder, tempOpCacheFolder) == KErrNone )
            aOpCacheFolder.Copy( tempOpCacheFolder );
        }
    HttpCacheUtil::EnsureTrailingSlash( aOpCacheFolder );
    }

void CHttpCacheManager::GetVSSCacheConfigL( CRepository& aRepository, TBool& aVSSCacheEnabled, TInt& aVssCacheSize, TDes& aVssCacheFolder )
    {
    // VSS Cache
    TInt VSScacheEnabled(0);
    TInt err = aRepository.Get(KPhoneSpecificCacheEnabled, VSScacheEnabled);
    aVSSCacheEnabled = VSScacheEnabled;

    if ( err == KErrNone && aVSSCacheEnabled )
        {
        TInt tempVSSCacheSize;
        if( aRepository.Get(KPhoneSpecificCacheSize, tempVSSCacheSize) == KErrNone )
            aVssCacheSize = tempVSSCacheSize;

        TFileName tempVSScacheFolder;
        if( aRepository.Get(KPhoneSpecificCacheFolder, tempVSScacheFolder) == KErrNone )
            aVssCacheFolder.Copy( tempVSScacheFolder );
        }
    HttpCacheUtil::EnsureTrailingSlash( aVssCacheFolder );
    }

void CHttpCacheManager::GetCriticalDriveLevelsL( CRepository& aRepository, const TDesC& aCacheFolder, TInt& aCriticalLevel )
    {
    // get drive letter for sysutil
    TParsePtrC pathParser( aCacheFolder );
    TDriveUnit drive = pathParser.Drive();

    // get critical level
    // RAM drive can have different critical level
    TVolumeInfo vinfo;
    User::LeaveIfError(CCoeEnv::Static()->FsSession().Volume(vinfo, drive));
    //
    User::LeaveIfError(aRepository.Get((vinfo.iDrive.iType == EMediaRam ? KRamDiskCriticalLevel : KDiskCriticalThreshold), aCriticalLevel));
    }

void CHttpCacheManager::CreateHttpCacheL( const TInt& aSecIdInt, const TInt& aCacheSize, const TInt& aCriticalLevel, const THttpCachePostponeParameters& aPostpone )
    {
    // browser gets the normal index.dat name, other clients get <SID>.dat
    TFileName indexFile( KDefaultIndexFile() );
    if( aSecIdInt != KUIDBROWSERNG )
        {
        indexFile.Format(_L("%08x.dat"), aSecIdInt);
        }
    // create cache handler
    iCache = CHttpCacheHandler::NewL( aCacheSize, iCacheFolder, indexFile, aCriticalLevel, aPostpone);
    }

void CHttpCacheManager::CreateOperatorCacheL( CRepository& aRepository, const TDesC& aOpCacheFolder, const TInt& aOpCacheSize, const TInt& aCriticalLevel, const THttpCachePostponeParameters& aPostpone )
    {
    TBuf<512> url;
    // if domain is missing, then no need to read further
    if (aRepository.Get(KOperatorDomainUrl, url) == KErrNone)
        {
        HBufC8* opDomain8 = HBufC8::NewL(url.Length());
        CleanupStack::PushL(opDomain8);
        opDomain8->Des().Append(url);
    
        TInt slashPos = opDomain8->LocateReverse('/');
        if (slashPos == -1)
            {
            slashPos = 0;
            }
    
        TPtrC8 temp = opDomain8->Left(slashPos);
        iOpDomain = temp.AllocL();
        CleanupStack::PopAndDestroy(opDomain8);
    
        // create op cache
        iOperatorCache = CHttpCacheHandler::NewL( aOpCacheSize, aOpCacheFolder, KDefaultOpIndexFile(), aCriticalLevel, aPostpone);
        }
    }

void CHttpCacheManager::CreateVssCacheL( CRepository& aRepository, const TDesC& aVssCacheFolder, const TInt& aVssCacheSize, const TInt& aCriticalLevel, const THttpCachePostponeParameters& aPostpone )
    {
    //Get the white list
    TBuf<2048> whiteList;
    
    if (aRepository.Get(KPhoneSpecificCacheDomainUrl, whiteList) == KErrNone)
        {
        iVSSWhiteList = HBufC8::NewL(whiteList.Length());
        iVSSWhiteList->Des().Append(whiteList);
        }
    else
        {
        iVSSWhiteList = NULL;
        }
    
    // create cache handler
    iphoneSpecificCache = CHttpCacheHandler::NewL(aVssCacheSize, aVssCacheFolder, KDefaultVSSIndexFile(), aCriticalLevel, aPostpone);
    }

void CHttpCacheManager::CrashCheckL( const TInt& aSecIdInt )
    {
    if( aSecIdInt == KUIDBROWSERNG )
        {
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        // Remove orphan files if browser didn't close cache properly - should only happen very occasionally
        //
        // We leave a file on the drive when we are in use and remove it when we close properly
        // If the file is already there we expect files in the cache which have not been added to the index.
        //
        //////////////////////////////////////////////////////////////////////////////////////////////////////
        RFs rfs;
        User::LeaveIfError( rfs.Connect() );
        CleanupClosePushL( rfs );
        rfs.SetSessionPath( iCacheFolder );
        RFile crashTest;
        TInt err = crashTest.Create(rfs,KCrashCheck(),EFileRead);
        crashTest.Close();  
        if ( err == KErrAlreadyExists )
            {
    #ifdef __CACHELOG__
            HttpCacheUtil::WriteLog(0, _L("Crash detected - removing orphan files"));
    #endif
            RemoveOrphanedFilesL();
            }
        CleanupStack::PopAndDestroy( &rfs );
        }
    }

//  End of File
