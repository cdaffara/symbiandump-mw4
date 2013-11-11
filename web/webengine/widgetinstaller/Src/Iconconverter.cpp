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
* Description: 
*     Icon convert to convert icon for png to mbm format
*
*
*/

#include <fbs.h>
#include <imageconversion.h>
#include <bitmaptransforms.h>
#include "IconConverter.h"
#include "WidgetInstaller.h"


// CONSTANTS
_LIT( KTempPath,"c:\\system\\temp\\" );

const TInt KIconSizeLarge = 88;
const TInt KIconSizeMedium = 32;
const TInt KIconSizeSmall = 24;


//using namespace SwiUI;

// ============================================================================
// CIconConverter::NewL()
// two-phase constructor
//
// @since 3.1
// @param aController - controller for callback to notify the completion
// @param aFs - file session
// @return pointer to CIconConverter
// ============================================================================
//
CIconConverter* CIconConverter::NewL(
    MConverterController* aController,
    RFs& aFs )
    {
    CIconConverter* self =
        new(ELeave) CIconConverter( aController , aFs );
    CleanupStack::PushL( self );

    self->ConstructL();

    CleanupStack::Pop( self );
    return self;
    }

// ============================================================================
// CIconConverter::CIconConverter()
// C++ default constructor
//
// @since 3.1
// ============================================================================
CIconConverter::CIconConverter(
    MConverterController* aController,
    RFs& aFs ) :
        CActive( EPriorityStandard ),
        iController( aController ),
        iFs( aFs )
    {
    RFbsSession::Connect();
    CActiveScheduler::Add( this );
    }

// ============================================================================
// CIconConverter::ConstructL()
// Symbian default constructor
//
// @since 3.1
// ============================================================================
void CIconConverter::ConstructL()
    {
    // create the destination bitmap
    iOriginalBitmap = new ( ELeave ) CFbsBitmap;
    iOriginalBitmapMask = new ( ELeave ) CFbsBitmap;

    iTempBitmap = new ( ELeave ) CFbsBitmap;
    iTempBitmapMask = new ( ELeave ) CFbsBitmap;
    iTempPath = KTempPath().AllocL();
    iIconSizes = new CArrayFixFlat<TSize>( 3 );
    iIconSizes->InsertL( 0, TSize( KIconSizeLarge, KIconSizeLarge ) );
    iIconSizes->InsertL( 1, TSize( KIconSizeMedium, KIconSizeMedium ) );
    iIconSizes->InsertL( 2, TSize( KIconSizeSmall, KIconSizeSmall ) );
    }


// ============================================================================
// CIconConverter::~CIconConverter()
// destructor
//
// @since 3.1
// ============================================================================
CIconConverter::~CIconConverter()
    {
    Cancel();

    // CImageDecoder must be deleted first otherwise a related thread might panic
    if ( iImageDecoder )
        {
        delete iImageDecoder;
        }
    if ( iOriginalBitmap )
        {
        delete iOriginalBitmap;
        }
    if ( iOriginalBitmapMask )
        {
        delete iOriginalBitmapMask;
        }
    if ( iOutputFileName )
        {
        delete iOutputFileName;
        }
    if ( iTempBitmap )
        {
        delete iTempBitmap;
        }
    if ( iTempBitmapMask )
        {
        delete iTempBitmapMask;
        }
    if ( iScaler )
        {
        delete iScaler;
        }
    if ( iTempPath )
        {
        delete iTempPath;
        }
    iIconFile.Close();
    iIconPngFile.Close();
    RFbsSession::Disconnect();
    if ( iIconSizes )
        {
        iIconSizes->Reset();
        delete iIconSizes;
        }
    }


// ============================================================================
// CIconConverter::StartToDecodeL
// use image decoder to decode the image
//
// @since 3.1
// ============================================================================
void CIconConverter::StartToDecodeL(
    const TDesC& aInputFileName,
    const TDesC& aOutputFileName )
    {
    iState = EConvertingFile;
    delete iImageDecoder;
    iImageDecoder = NULL;

    delete iOutputFileName;
    iOutputFileName = 0;

    iOutputFileName = aOutputFileName.AllocL();

    // create the decoder
    iImageDecoder = CImageDecoder::FileNewL( iFs, aInputFileName );

    // Extract information about the image, now we've read the header
    TFrameInfo info = iImageDecoder->FrameInfo( 0 );

    iOriginalBitmap->Create( info.iOverallSizeInPixels, info.iFrameDisplayMode );

    // If the PNG has a built in transparency, use it to build the mask
    if ( info.iFlags & TFrameInfo::ETransparencyPossible )
        {
        // If we have a full alpha channel, use that
        if ( info.iFlags & TFrameInfo::EAlphaChannel )
            {
            User::LeaveIfError( iOriginalBitmapMask->Create(
                info.iOverallSizeInPixels,
                EGray256 ) );
            }
        else
            {
            User::LeaveIfError( iOriginalBitmapMask->Create(
                info.iOverallSizeInPixels,
                EGray2 ) );
            }

        iImageDecoder->Convert(
            &iStatus, *iOriginalBitmap, *iOriginalBitmapMask );
        }
    else
        {
        iImageDecoder->Convert( &iStatus, *iOriginalBitmap );
        }

    // start conversion to bitmap
    SetActive();
    }

// ============================================================================
// CIconConverter::RunL()
// Handle various stages of icon conversion
//
// @since 3.1
// ============================================================================
void CIconConverter::RunL()
    {
    // If there is an error in the previous stage, then leave. Otherwise,
    // call the handle function
    User::LeaveIfError( iStatus.Int() );

    switch ( iState )
        {
    case EConvertingFile:
        DoProcessMaskL();
        break;

    case EScalingIcon:
        DoMaskScalingL();
        break;

    case EScalingMask:
    case EFinalize:
        TRAPD( err, DoIconStoreL() );
        User::LeaveIfError( err );
        break;

    default:
    User::Leave( KErrNotSupported );
        break;
        };

    }

// ============================================================================
// CIconConverter::RunError()
// Notify client with error
//
// @since 3.1
// ============================================================================
TInt CIconConverter::RunError( TInt aError )
    {
    // If any error occurred, then complete the client with the error.
    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, aError );
        }

    iController->NotifyCompletionL( aError );

    return KErrNone;
    }

// ============================================================================
// CIconConverter::DoCancel()
// cancel icon conversion
//
// @since 3.1
// ============================================================================
void CIconConverter::DoCancel()
    {
    switch (iState)
        {
    case EConvertingFile:
        if ( iImageDecoder )
            {
            iImageDecoder->Cancel();
            }

        break;

    case EScalingIcon:
    case EScalingMask:
        if ( iScaler )
            {
            iScaler->Cancel();
            }
        break;

        };

    if ( iClientStatus )
        {
        User::RequestComplete( iClientStatus, KErrCancel );
        }

    // no need to call NotifyCompletionL() because cancel can only be
    // caused by the client
    }

// ============================================================================
// CIconConverter::DoProcessMaskL()
// process the bitmap mask
//
// @since 3.1
// ============================================================================
void CIconConverter::DoProcessMaskL()
    {
    // we use white to mean transparent at this stage, simply for efficiency
    // since all the canvases we will copy in to begin as white

    if ( iOriginalBitmapMask->Handle() == 0 )
        {
        // Create a mask that shows the whole bitmap as an icon
        // (all black)
        User::LeaveIfError( iOriginalBitmapMask->Create(
            iOriginalBitmap->SizeInPixels(), EGray2 ) );
        CFbsBitmapDevice* device =
            CFbsBitmapDevice::NewL( iOriginalBitmapMask );
        CleanupStack::PushL( device );

        CFbsBitGc* gc;
        User::LeaveIfError( device->CreateContext( gc ) );
        gc->SetBrushStyle( CGraphicsContext::ESolidBrush );
        gc->SetDrawMode( CGraphicsContext::EDrawModePEN );
        gc->SetBrushColor( KRgbBlack );
        // Create a big black image
        gc->Clear();
        delete gc;
        CleanupStack::PopAndDestroy( device );
        }
    else
        {
        // Invert the mask obtained from the PNG
        CFbsBitmapDevice* device =
            CFbsBitmapDevice::NewL( iOriginalBitmapMask );
        CleanupStack::PushL(device);
        CFbsBitGc* gc;
        User::LeaveIfError( device->CreateContext( gc ) );
        gc->SetDrawMode( CGraphicsContext::EDrawModeNOTSCREEN );
        gc->Clear();
        delete gc;
        CleanupStack::PopAndDestroy( device );
        }

    // Scale the icon to the sizes required
    iCurrentSizeIndex = 0;
    DoIconScalingL();
    }

// ============================================================================
// CIconConverter::DoIconScalingL()
// Scale the bitmap
//
// @since 3.1
// ============================================================================
void CIconConverter::DoIconScalingL()
    {
    // free any current icons to prevent memory leaks
    iTempBitmap->Reset();
    // current target size
    TSize size = iIconSizes->At( iCurrentSizeIndex );

    iState = EScalingIcon;
    // Create a canvas to hold the scaled icon, of the same depth
    User::LeaveIfError(
        iTempBitmap->Create( size, iOriginalBitmap->DisplayMode() ) );
    DoScalingL( *iOriginalBitmap, *iTempBitmap );
    }

// ============================================================================
// CIconConverter::DoMaskScalingL()
// Scale the bitmap mask
//
// @since 3.1
// ============================================================================
void CIconConverter::DoMaskScalingL()
    {
    // Reset the mask to prevent memory leaks
    iTempBitmapMask->Reset();
    // current target size
    TSize size = iIconSizes->At( iCurrentSizeIndex );

    iState = EScalingMask;
    // Create a canvas to hold the scaled icon, of 8 bit colour depth
    User::LeaveIfError( iTempBitmapMask->Create( size, EGray256 ) );
    DoScalingL( *iOriginalBitmapMask, *iTempBitmapMask );
    }

// ============================================================================
// CIconConverter::DoScalingL()
// Scale
//
// @since 3.1
// ============================================================================
void CIconConverter::DoScalingL(
    CFbsBitmap& aBitmapSource, CFbsBitmap& aBitmapTarget )
    {
    ScalerL().Scale( &iStatus, aBitmapSource, aBitmapTarget, ETrue );
    SetActive();
    }

// ============================================================================
// CIconConverter::ScalerL()
// Create bitmap scalar
//
// @since 3.1
// ============================================================================
CBitmapScaler& CIconConverter::ScalerL()
    {
    if ( iScaler == NULL )
        {
        iScaler = CBitmapScaler::NewL();
        // always use highest quality scaling
        User::LeaveIfError( iScaler->SetQualityAlgorithm( CBitmapScaler::EMaximumQuality ) );
        }
    return *iScaler;
    }

// ============================================================================
// CIconConverter::DoIconStoreL()
// Store icon and mask files
//
// @since 3.1
// ============================================================================
void CIconConverter::DoIconStoreL()
    {
    // Store the icon and its mask in temporary files until we are ready
    // to create the final icon

    // Icon is stored at index n, mask at index n+1
    TInt iconIndex = iCurrentSizeIndex * 2;
    TFileName iconFile = *iTempPath;
    GetTempIconName( iconIndex++, iconFile );

    TFileName maskFile = *iTempPath;
    GetTempIconName( iconIndex, maskFile );

    // invert the masks before saving

    CFbsBitmapDevice* device = CFbsBitmapDevice::NewL( iTempBitmapMask );
    CleanupStack::PushL( device );

    CFbsBitGc* gc;
    User::LeaveIfError( device->CreateContext( gc ) );
    gc->SetDrawMode( CGraphicsContext::EDrawModeNOTSCREEN );
    gc->Clear();

    delete gc;
    CleanupStack::PopAndDestroy( device );

    // save the bitmaps
    User::LeaveIfError( iTempBitmap->Save( iconFile ) );
    User::LeaveIfError( iTempBitmapMask->Save( maskFile ) );

    if ( ++iCurrentSizeIndex < iIconSizes->Count() )
        {
        // do the next icon size
        DoIconScalingL();
        }
    else
        {
        DoCreateFinalIconL();
        }

    }

// ============================================================================
// CIconConverter::DoCreateFinalIconL()
// Create the final icon
//
// @since 3.1
// ============================================================================
void CIconConverter::DoCreateFinalIconL()
    {
    TInt i, elements = 0;
    // one icon, one mask per size
    TInt bitmapCount = iIconSizes->Count() * 2;

    TFileName** filenames = new ( ELeave ) TFileName*[bitmapCount];
    CleanupStack::PushL( filenames );
    TInt32* uniqueIds = new ( ELeave ) TInt32[bitmapCount];
    CleanupStack::PushL( uniqueIds );

    TInt err = KErrNone;

    for ( i = 0; i < bitmapCount; ++i )
        {
        filenames[i] = NULL;
        filenames[i] = new TFileName( *iTempPath );
        elements = i;
        if ( filenames[i] == NULL )
            {
            // we need to cleanup this structure
            err = KErrNoMemory;
            goto cleanup;
            }
        GetTempIconName( i, *filenames[i] );
        uniqueIds[i] = 0;
        }

    TRAP( err, CFbsBitmap::StoreL(
        *iOutputFileName, bitmapCount, ( const TDesC** )filenames, uniqueIds ) );

cleanup:
    for ( i = 0; i <= elements; ++i )
        {
        if ( filenames[i] == NULL )
            {
            // if we failed to allocate a filename, then we would not have continued
            break;
            }
        else
            {
            delete filenames[i];
            }
        }

    CleanupStack::PopAndDestroy( 2, filenames );
    iController->NotifyCompletionL( err );
    }

// ============================================================================
// CIconConverter::GetTempIconName()
// Get temporary icon name
//
// @since 3.1
// ============================================================================
void CIconConverter::GetTempIconName( TInt aIndex, TFileName& aIconName )
    {
    _LIT( KIcon, "ICON" );
    _LIT( KBmp, ".MBM" );
    aIconName.Append( KIcon );
    aIconName.AppendNum( static_cast<TInt64>( aIndex ) );
    aIconName.Append( KBmp );
    }


