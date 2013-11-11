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

#include <apmrec.h>
#include <apgcli.h>
#include <eikappui.h>
#include <eikenv.h>
#include <eikbtgpc.h>
#include <StringLoader.h>
#include <WidgetUi.rsg>
#include <avkon.hrh>
#include <aknmessagequerydialog.h>
#include <ActiveApDb.h>
#include <escapeutils.h>
#include <uri16.h>
#include <BrCtlDefs.h>
#include "WidgetUiObserver.h"
#include "WidgetUiWindow.h"
#include "WidgetUiWindowView.h"
#include "WidgetUiWindowManager.h"
#include "WidgetUiWindowContainer.h"

#ifndef RD_PF_SEC_APPARC
#include <genericparam.h>
#else
#ifndef __BROWSER_SDK
#include "AiwGenericParam.h"
#endif
#endif

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
_LIT( KFileScheme, "file://" );
_LIT( KDefaultDrivePath, "C:\\" );
_LIT( KPathChar, "\\" );
_LIT( KLprojExt, ".lproj" );

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES
TInt doTakeSnapshot( TAny* ptr );

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CWidgetUiObserver::UpdateBrowserVScrollBarL
// -----------------------------------------------------------------------------
//
void CWidgetUiObserver::UpdateBrowserVScrollBarL(
    TInt aDocumentHeight,
    TInt aDisplayHeight,
    TInt aDisplayPosY )
    {
    (void)aDocumentHeight;
    (void)aDisplayHeight;
    (void)aDisplayPosY;
#ifdef _DEBUG
    _LIT(KUpdateScrollbar, "Document height = %d, Display height = %d, Display pos = %d");

    if ( iCanLog )
        {
        iFileLogger.WriteFormat( KUpdateScrollbar, aDocumentHeight, aDisplayHeight, aDisplayPosY );
        }
#endif
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::UpdateBrowserHScrollBarL
// -----------------------------------------------------------------------------
//
void CWidgetUiObserver::UpdateBrowserHScrollBarL(
    TInt aDocumentWidth,
    TInt aDisplayWidth,
    TInt aDisplayPosX )
    {
    (void)aDocumentWidth;
    (void)aDisplayWidth;
    (void)aDisplayPosX;
#ifdef _DEBUG
    _LIT(KUpdateScrollbar, "Document width = %d, Display width = %d, Display pos = %d");

    if ( iCanLog )
        {
        iFileLogger.WriteFormat( KUpdateScrollbar, aDocumentWidth, aDisplayWidth, aDisplayPosX );
        }
#endif
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::NotifyLayoutChange
// -----------------------------------------------------------------------------
//
void CWidgetUiObserver::NotifyLayoutChange( TBrCtlLayout aNewLayout )
    {
    (void)aNewLayout;
#ifdef _DEBUG
    _LIT(KUpdateLayout, "New layout is  %d");
    RDebug::Printf("CWidgetUiObserver::NotifyLayoutChange - beginning");
    if ( iCanLog )
        {
        iFileLogger.WriteFormat( KUpdateLayout, aNewLayout) ;
        }
#endif
    if( iWindow->WidgetMiniViewState() == EPublishStart )
        {
        //iWindow->PublishSnapShot();
        iCollectBitmapTimer->Cancel();
        iCollectBitmapTimer->Start(TCallBack(&doTakeSnapshot,iWindow));
        } 
    }
TInt doTakeSnapshot( TAny* ptr )
   {
   RDebug::Printf("CWidgetUiObserver::doTakeSnapshot");
    static_cast<CWidgetUiWindow*>(ptr)->PublishSnapShot(); 
    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::UpdateTitle
// -----------------------------------------------------------------------------
//
void CWidgetUiObserver::UpdateTitleL( const TDesC& /*aTitle*/ )
    {
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::UpdateSoftkeyL
// -----------------------------------------------------------------------------
//
void CWidgetUiObserver::UpdateSoftkeyL( TBrCtlKeySoftkey aKeySoftkey,
                                      const TDesC& aLabel,
                                      TUint32 /*aCommandId*/,
                                      TBrCtlSoftkeyChangeReason aBrCtlSoftkeyChangeReason )
    {
    // ignore idle reason.  this comes when brctl calls for default softkey update
    if(aBrCtlSoftkeyChangeReason != EChangeReasonIdle)
        {
        if (aKeySoftkey == EKeyRight && aLabel.Length() == 0)
            {
            //reset right softkey to "Exit"
            if (!iSoftkeyExitStr)
                {
                    iSoftkeyExitStr = StringLoader::LoadL( R_WIDGETUI_SOFTKEY_EXIT );
                }
            iWindow->SetSoftKeyLabelL( aKeySoftkey, *iSoftkeyExitStr );
            }
        else if (aKeySoftkey == EKeyLeft && aLabel.Length() == 0)
            {
                if (!iLeftSoftkeyStr)
                    {
                        iLeftSoftkeyStr = StringLoader::LoadL(R_WIDGETUI_SOFTKEY_OPTION);
                    }
                iWindow->SetSoftKeyLabelL( aKeySoftkey, *iLeftSoftkeyStr );
            }
        else
            {
            iWindow->SetSoftKeyLabelL( aKeySoftkey, aLabel );
            }
        }
    
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::HandleBrowserLoadEventL
// -----------------------------------------------------------------------------
//
void CWidgetUiObserver::HandleBrowserLoadEventL(
    TBrCtlDefs::TBrCtlLoadEvent aLoadEvent,
    TUint /*aSize*/,
    TUint16 /*aTransactionId*/ )
    {
    if (aLoadEvent == TBrCtlDefs::EEventLoadFinished)
        {
        iWindow->SetWidgetLoaded(ETrue);
        }
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::ResolveEmbeddedLinkL
// -----------------------------------------------------------------------------
//
TBool CWidgetUiObserver::ResolveEmbeddedLinkL(const TDesC& aEmbeddedUrl,
                                              const TDesC& aCurrentUrl,
                                              TBrCtlLoadContentType aLoadContentType,
                                              MBrCtlLinkContent& aEmbeddedLinkContent)
    {
    (void)aCurrentUrl;
    (void)aLoadContentType;
#ifdef _DEBUG
    _LIT(KResolveEmbeddedLink, "Resolve embedded link  aEmbeddedUrl = :%s:, aCurrentUrl = :%s:, aLoadContentType = %d");

    if ( iCanLog )
        {
        iFileLogger.WriteFormat( KResolveEmbeddedLink, aEmbeddedUrl, aCurrentUrl, aLoadContentType );
        }
#endif
    if ( IsFileScheme( aEmbeddedUrl ) )
        {

        // DENY if accessing anything inside WidgetUI's private dir AND that file is not inside 
        // the widget's own "sandbox". We define an installed widget's "sandbox" to be anything
        // under e:/private/10282822/<widget ID>/

        // Convert URL form to a more friendly path form, with locale-specific subdirectory
        TFileName lprojName;
        iWindow->WindowManager().WidgetUIClientSession().GetLprojName( lprojName );
        TranslateURLToFilenameL( aEmbeddedUrl, lprojName ); 
        
		iFs.PrivatePath(iAppPrivatePath);        
        // TRUE if e:/private/10282822/*/* was requested. It may or may not be inside a widget's sandbox.
        TBool isInsidePrivateDir = ( iFileName && ( (*iFileName).FindF(iAppPrivatePath) == KMaxDriveName ) ) ? ETrue : EFalse; 

        HBufC* widgetPath = iWindow->WidgetPath();          // The widget's sandbox. Object not ours.
        // TRUE if e:/private/10282822/<widgetID>/foo.js was requested. This is INSIDE widget's sandbox
        TBool isSandboxed = ( iFileName && widgetPath && (*iFileName).FindF( *widgetPath ) == 0 ) ? ETrue : EFalse;
        
        if ( isInsidePrivateDir && !isSandboxed )
            {   //Acess denied!
            User::Leave(KErrAccessDenied);
            }
        
        if ( !isSandboxed )
            { // For files in Public areas, don't use localized subdirectory
            TranslateURLToFilenameL( aEmbeddedUrl, KNullDesC); 
            } 
        
        HBufC8* buf = ReadFileL( *iFileName);
        
        if ( !buf && isSandboxed )
            { // In case of failure, fall-back to generic/non-localized content
            TranslateURLToFilenameL( aEmbeddedUrl, KNullDesC );
            buf = ReadFileL( *iFileName);
            }
        
        if ( !buf )
            {
            User::Leave(KErrGeneral);  
            }
        
        CleanupStack::PushL( buf );
        HBufC* contentType = NULL;
        TPtrC p( NULL, 0 );
        contentType = RecognizeLC( *iFileName, *buf );
        aEmbeddedLinkContent.HandleResolveComplete( *contentType, p, buf );
        CleanupStack::PopAndDestroy( 2, buf ); // contentType, buf
        return ETrue;
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::ResolveLinkL
// -----------------------------------------------------------------------------
//
TBool CWidgetUiObserver::ResolveLinkL( const TDesC& aUrl, const TDesC& aCurrentUrl,
                                     MBrCtlLinkContent& aBrCtlLinkContent )
    {
    (void)aCurrentUrl;
#ifdef _DEBUG
    _LIT(KResolveLink, "Resolve embedded link  aUrl = :%s:, aCurrentUrl = :%s:");

    if ( iCanLog )
        {
        iFileLogger.WriteFormat( KResolveLink, aUrl, aCurrentUrl );
        }
#endif
    //Re-use existing logic upstairs to enforce inter-widget (and intra WidgetUI) security policies.
    return ResolveEmbeddedLinkL(aUrl, aCurrentUrl, ELoadContentTypeAny, aBrCtlLinkContent);
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::CancelAll
// -----------------------------------------------------------------------------
//
void CWidgetUiObserver::CancelAll()
    {
#ifdef _DEBUG
    _LIT(KCancelAll, "All requests canceled");

    if ( iCanLog )
        {
        iFileLogger.WriteFormat( KCancelAll );
        }
#endif
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::CWidgetUiObserver()
// -----------------------------------------------------------------------------
//
CWidgetUiObserver::CWidgetUiObserver( CWidgetUiWindow& aWindow )
    : iWindow( &aWindow ),
      iFileName( NULL )
    {
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::ConstructL
// -----------------------------------------------------------------------------
//
void CWidgetUiObserver::ConstructL()
    {
#ifdef _DEBUG
    _LIT(KDir, "WidgetUi");
    _LIT(KFile, "WidgetUi.log");
    TInt err( 0 );

    err = iFileLogger.Connect();
    if ( err == KErrNone )
        {
        iFileLogger.CreateLog( KDir(), KFile(), EFileLoggingModeOverwrite );
        iCanLog = ETrue;
        }
#endif

    iHandler = CDocumentHandler::NewL( CEikonEnv::Static()->Process() ) ;
        // Get WidgetUI's private directory (e.g. c:/private/10282822/)
    iCollectBitmapTimer = CIdle::NewL(CActive::EPriorityLow);
    iFs.Connect();
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::~CWidgetUiObserver
// -----------------------------------------------------------------------------
//
CWidgetUiObserver::~CWidgetUiObserver()
    {

#ifdef _DEBUG
    if ( iCanLog )
        {
        iFileLogger.CloseLog();
        iFileLogger.Close();
        }
#endif
    delete iFileName;
    delete iHandler;
    delete iSoftkeyExitStr;
    delete iLeftSoftkeyStr;
    delete iCollectBitmapTimer; 
    iFs.Close(); 
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::NewL
// -----------------------------------------------------------------------------
//
CWidgetUiObserver* CWidgetUiObserver::NewL( CWidgetUiWindow& aWindow )
    {
    CWidgetUiObserver* self = new(ELeave)CWidgetUiObserver( aWindow );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(1, self);

    return self;
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::IsFileScheme
// -----------------------------------------------------------------------------
//
TBool CWidgetUiObserver::IsFileScheme( const TDesC& aFileName )
    {
    if ( aFileName.Length()> 0 && aFileName.Ptr() )
        {
        if ( aFileName.FindF(KFileScheme) == 0 )
            {
            return ETrue;
            }
        else
            {//Check for paths are that not URI, but of the form..
             // c:/dir/foo.txt. This is to accomodate browser engine inconsistencies. 
            TUriParser16 parser;
            if( parser.Parse( aFileName ) == KErrNone )
                {
                TPtrC16 scheme = parser.Extract( EUriScheme );
                // As per the URI RFC, the part before : is the Scheme
                if ( scheme.Length() == (KMaxDriveName-1) ) 
                    {
                    return ETrue;
                    }
                }
            }
        }

    return EFalse;
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::ReadFile
// -----------------------------------------------------------------------------
//
HBufC8* CWidgetUiObserver::ReadFileL( const TDesC& aFileName )
    {
    RFile file;

    if (KErrNone != file.Open( CCoeEnv::Static()->FsSession(), aFileName, EFileRead ) )
        {
        return NULL;
        }

    CleanupClosePushL( file );

    TInt size;
    User::LeaveIfError( file.Size( size ) );
    HBufC8* buf = HBufC8::NewLC( size );
    TPtr8 bufPtr( buf->Des() );
    User::LeaveIfError( file.Read( bufPtr ) );
    CleanupStack::Pop( buf );
    CleanupStack::PopAndDestroy( &file );
    return buf;
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::RecognizeL
// -----------------------------------------------------------------------------
//
HBufC* CWidgetUiObserver::RecognizeLC( const TDesC& aFileName, const TDesC8& aData )
    {
    TDataRecognitionResult dataType;
    RApaLsSession apaSession;
    TInt ret;

    CleanupClosePushL(apaSession);
    User::LeaveIfError( apaSession.Connect() );

    // Ask the application architecture to find the file type
    ret = apaSession.RecognizeData( aFileName, aData, dataType );
    apaSession.Close();
    CleanupStack::PopAndDestroy(1, &apaSession);
    
    TPtrC8 mimeTypePtr = dataType.iDataType.Des8();
    TInt len = mimeTypePtr.Length() + 1;
    HBufC* contentTypeString = HBufC::NewLC( len );;

    if ( ret == KErrNone &&
        ( dataType.iConfidence == CApaDataRecognizerType::ECertain ) ||
        ( dataType.iConfidence == CApaDataRecognizerType::EProbable ) )
        {
        // If the file type was found, try to match it to a known file type
        contentTypeString->Des().Copy( mimeTypePtr );
        contentTypeString->Des().ZeroTerminate();
        }

    return contentTypeString;
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::TranslateURLToFilenameL
// Translate the file name from a URL to a valid file name in the system.
// -----------------------------------------------------------------------------
//
TBool CWidgetUiObserver::TranslateURLToFilenameL( const TDesC& aFileName, const TDesC& aLanguageDir )
    {
    // This function accepts URLs in the following format:
    // file://filename.xxx
    // file:///filename.xxx
    // file://c:/filename.xxx
    // file:///c:/filename.xxx
    // c:/dir/file.txt (the browser engine can send these too!)
    TInt count( 0 );
    TInt index( 0 );
    TBool drvLetter = EFalse;
    TUint16 c;
    
    HBufC* decodedUrl = EscapeUtils::EscapeDecodeL(aFileName);
    CleanupStack::PushL( decodedUrl );
    
    // Throw away the intial file:// part, if present
    TPtrC urlPtr( aFileName );
    if (urlPtr.FindF( KFileScheme) == 0 )
        {
        urlPtr.Set( decodedUrl->Mid( KFileScheme().Length() ) );    
        }
    
    // Make sure there are enough characters in the filename before
    // trying to check them
    count = urlPtr.Length() + aLanguageDir.Length() + KLprojExt().Length() + KPathChar().Length();
    if ( urlPtr.Length() > 0 ) //do nothing for trivial input
        {  
    // Skip the first '/' if there is one
    if ( urlPtr[0] == '/' )
        {
        urlPtr.Set( urlPtr.Mid( 1 ) );
        }
    // Is there a drive letter?
    if ( urlPtr.Length() > 1 )
        {
        // Can check for drive letter
        if ( urlPtr[1 + index] == ':' )
            {
            drvLetter = ETrue;
            }
        }
    if ( drvLetter == EFalse )
        {
        // Plan for a c:\ type drive prefix to what we already tallied above
        count += KDefaultDrivePath().Length();
        }
    delete iFileName;
    iFileName = NULL;
    iFileName = HBufC::NewL( count );
    if ( !drvLetter )
        {
        iFileName->Des().Append( KDefaultDrivePath );
        }

    TBool fragment( EFalse );
    // Convert relative path containing /./ and /../ to absolute path
    for ( ; index < urlPtr.Length() && !fragment; index ++ )
        {
        switch ( urlPtr[index] )
            {
            case '#':    // Check if there is a fragment '#'
                {
                fragment = ETrue;
                continue; // Just stop there
                }

            case '/':
                {
                iFileName->Des().Append( KPathChar );
                break;
                }
            case '.':
                {
                if ( index > 1 && urlPtr[index - 1] == '/' )
                    {
                    if ( index < count - 1 && urlPtr[ index + 1 ] == '/' )
                        {
                        index ++; // Skip  ./
                        break;
                        }
                    if ( index > 2 && index < count - 3 &&
                         urlPtr[ index + 1 ] == '.' && urlPtr[ index + 2 ] == '/' )
                        {
                        TInt i = index - 2;

                        for ( ; i > 0 && urlPtr[i] != '/'; i-- ) {} // Skip  /../

                        iFileName->Des().SetLength( iFileName->Des().Length() - (index - i) );
                        index += 2;
                        break;
                        }
                    }
                }
                // No break
                //lint -fallthrough

            default:
                {
                c = urlPtr[ index ];
                iFileName->Des().Append( &c, 1 );
                break;
                }
            }   // end of switch
        }

    if ( aLanguageDir.Length() )
        {
        TChar backSlash = '\\';
        TInt pos = iFileName->Des().LocateReverse( backSlash );
        if ( pos != KErrNotFound )
            {
            TPtr p = iFileName->Des();
            p.Insert(pos, KLprojExt );
            p.Insert(pos, aLanguageDir );
            p.Insert(pos, KPathChar );
            }
        }
        } //Do nothing for any trivial input
    CleanupStack::PopAndDestroy(1, decodedUrl); // decodedUrl
    return ETrue;
}

// -----------------------------------------------------------------------------
// CWidgetUiObserver::NewDownloadL
// -----------------------------------------------------------------------------
//
TBool CWidgetUiObserver::NewDownloadL( TUint aTransactionID,
                                     const TDesC& aFileName,
                                     const TDesC& aContentType,
                                     const TDesC& aUrl )
    {
    (void)aTransactionID;
    (void)aFileName;
    (void)aContentType;
    (void)aUrl;
#ifdef _DEBUG
    _LIT(KNewDownload1L, "New Download with Trans ID %d, Local filename :%s:");
    _LIT(KNewDownload2L, "New Download with Trans ID %d, Content Type :%s:");
    _LIT(KNewDownload3L, "New Download with Trans ID %d, URL :%s:");

    if ( iCanLog )
        {
        iFileLogger.WriteFormat( KNewDownload1L, aTransactionID, aFileName.Ptr() );
        iFileLogger.WriteFormat( KNewDownload2L, aTransactionID, aContentType.Ptr() );
        iFileLogger.WriteFormat( KNewDownload3L, aTransactionID, aUrl.Ptr() );
        }
#endif

    return ETrue;
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::HandleDownloadEventL
// -----------------------------------------------------------------------------
//
void CWidgetUiObserver::HandleDownloadEventL( TUint aTransactionID,
                                            TBrCtlDownloadEvent aDownloadEvent,
                                            TUint aValue )
    {
    (void)aTransactionID;
    (void)aDownloadEvent;
    (void)aValue;
#ifdef _DEBUG
    _LIT(KDownloadEvent, "Download Event with Trans ID %d, Event %d, Value %d");
    if ( iCanLog )
        {
        iFileLogger.WriteFormat( KDownloadEvent, aTransactionID, aDownloadEvent, aValue );
        }
#endif
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::OpenWindowL
// -----------------------------------------------------------------------------
//
CBrCtlInterface* CWidgetUiObserver::OpenWindowL(TDesC& /*aUrl*/, TDesC* /*aTargetName*/, TBool /*aUserInitiated*/, TAny* /*aReserved*/)
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::FindWindowL
// -----------------------------------------------------------------------------
//
CBrCtlInterface* CWidgetUiObserver::FindWindowL( const TDesC& /*aTargetName*/ ) const
    {
    return NULL;
    }

// -----------------------------------------------------------------------------
// CWidgetUiObserver::HandleWindowCommandL
// -----------------------------------------------------------------------------
//
void CWidgetUiObserver::HandleWindowCommandL( const TDesC& aTargetName, TBrCtlWindowCommand aCommand )
    {
    if( aTargetName == KNullDesC() && aCommand == ECloseWindow)
        {
        // prevent background widget from closing active window
        if (iWindow == iWindow->WindowManager().ActiveWindow())
            {
            iWindow->WindowManager().CloseWindowsAsync(EFalse); // close active window
            }
        }
    }

// End of File
