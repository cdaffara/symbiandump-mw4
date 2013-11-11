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
*
*/



// INCLUDE FILES
#include "WebCannedImages.h"
#include <data_caging_path_literals.hrh>
#include "../../bidi.h"
#include <AknsUtils.h>
#include <AknsItemID.h>
#include <eikenv.h>

#ifdef __BROWSER_SDK
#include <WebKitIcons_sdk.mbg>
#else
#include <WebKitIcons.mbg>
#endif
// tot:move ibeam from bitmap to svg
#include <webkit.mbg>

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS

#ifdef __BROWSER_SDK
_LIT( KBrowserSvgFile, "Webkiticons_sdk.mif" );
_LIT( KBrowserBitmapsFile, "webkit_sdk.mbm" );
#else
_LIT( KBrowserSvgFile, "webkiticons.mif" );
_LIT( KBrowserBitmapsFile, "webkit.mbm" );
#endif

// MACROS

// LOCAL CONSTANTS AND MACROS

// MODULE DATA STRUCTURES

// LOCAL FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// ============================= LOCAL FUNCTIONS ===============================

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WebCannedImages::WebCannedImages
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
WebCannedImages::WebCannedImages( )
    {
    }

// -----------------------------------------------------------------------------
// WebCannedImages::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void WebCannedImages::ConstructL()
    {
    m_mbmfile = WebCannedImages::getImageDirL();
    m_mbmfile->Des().Append( KBrowserBitmapsFile );
    m_svgfile = WebCannedImages::getImageDirL();
    m_svgfile->Des().Append( KBrowserSvgFile );
    }

// -----------------------------------------------------------------------------
// WebCannedImages::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
WebCannedImages* WebCannedImages::NewL()
    {
    WebCannedImages* self = new( ELeave ) WebCannedImages( );

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }

// Destructor
WebCannedImages::~WebCannedImages()
    {
    // remove all loaded images in the dictionary
    for (TInt i = 0; i < KNumberOfCannedImages; i++)
      {
      TCannedImageData imageData = m_imgarray[i];

      // don't delete the error image now, it's life time is maintained by
      // ImageRenderer's ref-counting.
      if( i != EImageMissing )
          {
          delete imageData.m_img;
          delete imageData.m_msk;
          }
      }

    delete m_mbmfile;
    delete m_svgfile;
    }

// -----------------------------------------------------------------------------
// WebCannedImages::GetImage
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
TCannedImageData& WebCannedImages::getImage( TInt imgid )
    {

  // load canned image on demond
  TCannedImageData& img = m_imgarray[imgid];
  if( img.m_img == NULL )
    {
#ifndef __SCALABLE_ICONS
    TRAP_IGNORE( WebCannedImages::loadImageDirL(imgid, *m_mbmfile, img));
#else
        switch (imgid)
            {
#ifdef __BROWSER_SDK
            case EImageMissing:
                loadSvg(EMbmWebkiticons_sdkQgn_indi_wml_image_not_shown,
                    KAknsIIDQgnIndiWmlImageNotShown, *m_svgfile, img);
                break;
            case EImageObject:
                loadSvg(EMbmWebkiticons_sdkQgn_prop_wml_object,
                    KAknsIIDQgnPropWmlObject, *m_svgfile, img);
                break;
            case EImageSelectArrow:
                loadSvg(EMbmWebkiticons_sdkQgn_prop_wml_selectarrow,
                    KAknsIIDQgnPropWmlSelectarrow, *m_svgfile, img);
                break;
            case EImageSelectFile:
                loadSvg(EMbmWebkiticons_sdkQgn_prop_wml_selectfile,
                    KAknsIIDQgnPropWmlSelectfile, *m_svgfile, img);
                break;
            case EImageRadioButtonOff:
                loadSvg(EMbmWebkiticons_sdkQgn_prop_wml_radiobutt_off,
                    KAknsIIDQgnPropWmlRadiobuttOff, *m_svgfile, img);
                break;
            case EImageRadioButtonOn:
                loadSvg(EMbmWebkiticons_sdkQgn_prop_wml_radiobutt_on,
                    KAknsIIDQgnPropWmlRadiobuttOn, *m_svgfile, img);
                break;
            case EImageCheckBoxOff:
                loadSvg(EMbmWebkiticons_sdkQgn_prop_wml_checkbox_off,
                    KAknsIIDQgnPropWmlCheckboxOff, *m_svgfile, img);
                break;
            case EImageCheckBoxOn:
                loadSvg(EMbmWebkiticons_sdkQgn_prop_wml_checkbox_on,
                    KAknsIIDQgnPropWmlCheckboxOn, *m_svgfile, img);
                break;
            case EImageFingerBitmap:
                loadSvg(EMbmWebkiticons_sdkQgn_indi_viewer_pointer_hand,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageArrowBitmap:
                loadSvg(EMbmWebkiticons_sdkQgn_indi_viewer_pointer,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageWaitArrowBitmap:
                loadSvg(EMbmWebkiticons_sdkQgn_indi_viewer_pointer_wait,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlCall:
                loadSvg(EMbmWebkiticons_sdkQgn_indi_browser_url_call,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlEmail:
                loadSvg(EMbmWebkiticons_sdkQgn_indi_browser_url_email,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlMm:
                loadSvg(EMbmWebkiticons_sdkQgn_indi_browser_url_mm,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlMms:
                loadSvg(EMbmWebkiticons_sdkQgn_indi_browser_url_mms,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlPage:
                loadSvg(EMbmWebkiticons_sdkQgn_indi_browser_url_page,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlVideo:
                loadSvg(EMbmWebkiticons_sdkQgn_indi_browser_url_video,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
#else
            case EImageMissing:
                loadSvg(EMbmWebkiticonsQgn_indi_wml_image_not_shown,
                    KAknsIIDQgnIndiWmlImageNotShown, *m_svgfile, img);
                break;
            case EImageObject:
                loadSvg(EMbmWebkiticonsQgn_prop_wml_object,
                    KAknsIIDQgnPropWmlObject, *m_svgfile, img);
                break;
            case EImageSelectArrow:
                loadSvg(EMbmWebkiticonsQgn_prop_wml_selectarrow,
                    KAknsIIDQgnPropWmlSelectarrow, *m_svgfile, img);
                break;
            case EImageSelectFile:
                loadSvg(EMbmWebkiticonsQgn_prop_wml_selectfile,
                    KAknsIIDQgnPropWmlSelectfile, *m_svgfile, img);
                break;
            case EImageRadioButtonOff:
                loadSvg(EMbmWebkiticonsQgn_prop_wml_radiobutt_off,
                    KAknsIIDQgnPropWmlRadiobuttOff, *m_svgfile, img);
                break;
            case EImageRadioButtonOn:
                loadSvg(EMbmWebkiticonsQgn_prop_wml_radiobutt_on,
                    KAknsIIDQgnPropWmlRadiobuttOn, *m_svgfile, img);
                break;
            case EImageCheckBoxOff:
                loadSvg(EMbmWebkiticonsQgn_prop_wml_checkbox_off,
                    KAknsIIDQgnPropWmlCheckboxOff, *m_svgfile, img);
                break;
            case EImageCheckBoxOn:
                loadSvg(EMbmWebkiticonsQgn_prop_wml_checkbox_on,
                    KAknsIIDQgnPropWmlCheckboxOn, *m_svgfile, img);
                break;
            case EImageFingerBitmap:
                loadSvg(EMbmWebkiticonsQgn_indi_viewer_pointer_hand,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageArrowBitmap:
                loadSvg(EMbmWebkiticonsQgn_indi_viewer_pointer,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageWaitArrowBitmap:
                loadSvg(EMbmWebkiticonsQgn_indi_viewer_pointer_wait,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlCall:
                loadSvg(EMbmWebkiticonsQgn_indi_browser_url_call,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlEmail:
                loadSvg(EMbmWebkiticonsQgn_indi_browser_url_email,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlMm:
                loadSvg(EMbmWebkiticonsQgn_indi_browser_url_mm,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlMms:
                loadSvg(EMbmWebkiticonsQgn_indi_browser_url_mms,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlPage:
                loadSvg(EMbmWebkiticonsQgn_indi_browser_url_page,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageUrlVideo:
                loadSvg(EMbmWebkiticonsQgn_indi_browser_url_video,
                    KAknsIIDDefault, *m_svgfile, img);
                break;

#endif
            case EImageSmartLinkPhone:
                loadSvg(EMbmWebkiticonsQgn_prop_nrtyp_phone,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageSmartLinkEmail:
                loadSvg(EMbmWebkiticonsQgn_indi_ai_nt_message,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageIBeam:
                loadSvg(EMbmWebkiticonsQgn_indi_viewer_pointer_text,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
           case EImageEscFullScreen:
                loadSvg(EMbmWebkiticonsQgn_indi_button_exit_fs,
                    KAknsIIDDefault, *m_svgfile, img);
                break;
            case EImageSelectMulti:
                TRAP_IGNORE(loadImageDirL(EMbmWebkitSelectmulti, *m_mbmfile, img ));
                break;
            default:
                TRAP_IGNORE(loadImageDirL(imgid, *m_mbmfile, img));
            }
#endif // __SCALABLE_ICONS
    }

  return img;

    }

// -----------------------------------------------------------------------------
// WebCannedImages::GetCannedImageDirL
// Gets the folder where the canned image is located
// -----------------------------------------------------------------------------
//
HBufC* WebCannedImages::getImageDirL( )
    {
    TFileName mbmDrive;
    HBufC* mbmFile = NULL;
    TParse parse;

    // get the drive letter - MBM is on the same drive as this DLL
    Dll::FileName( mbmDrive );
    parse.Set( mbmDrive, NULL, NULL );
    mbmDrive = parse.Drive();

    // allocate memory for MBM file name
    mbmFile = HBufC::NewL( KMaxFileName );

    // assemble the MBM file name
    TPtr ptr( mbmFile->Des() );
    ptr.SetLength( 0 );
    ptr.Append( mbmDrive );
    ptr.Append( KDC_APP_BITMAP_DIR );
    return mbmFile;
    }

// -----------------------------------------------------------------------------
// WebCannedImages::loadImageDirL
// Loads the canned image
// -----------------------------------------------------------------------------
//
void WebCannedImages::loadImageDirL( TInt imgid, const TDesC& filename, TCannedImageData& image )
    {
    // allocate a bitmap object
    image.m_img = new (ELeave) CFbsBitmap();

    // load an image from the MBM file
    TInt error = image.m_img->Load( filename, imgid );
    if (error!=KErrNone)
        {
        // FIXME: to avoid crash in Mars
        //User::Leave(KBrsrOutOfMemory);
        }

    // load the mask
    image.m_msk = new (ELeave) CFbsBitmap();

    error = image.m_msk->Load( filename, imgid);
    if (error!=KErrNone)
        {
        //User::Leave(KBrsrOutOfMemory);
        }
    }

// -----------------------------------------------------------------------------
// WebCannedImages::loadSvg
// Loads the canned image
// -----------------------------------------------------------------------------
//
void WebCannedImages::loadSvg( TInt imgid, TAknsItemID aknsid, const TDesC& filename, TCannedImageData& image )
    {
    // This stuff is copied from Kimono

    // Check the device resolution
    CEikonEnv* eikEnv = CEikonEnv::Static();
    CWsScreenDevice& screenDev = *eikEnv->ScreenDevice();

    TUint dpi = screenDev.VerticalTwipsToPixels(KTwipsPerInch);

    // Initialize size to be 12 pixels
    TInt defaultPx = 10;
    TInt px = 10;

    // Since web designers assume a 96 pixel per inch (ppi) display use that as the reference point.
    // Thus the multiplier should be:
    // ppi/96
    // Below is a list of the density of commercially available LCD displays.
    // Density Multiplier:
    // 400 ppi  4.2
    // 210 ppi  2.2
    // 200 ppi  2.1
    // 120 ppi  1.3
    // 100 ppi  1.0
    // 96 ppi   1.0
    if ((200 > dpi) && (dpi >= 120))
        {
        defaultPx = (TInt)((float)defaultPx * 1.7);
        }
    else if ((210 > dpi) && (dpi >= 200))
        {
        defaultPx = (TInt)((float)defaultPx * 2.1);
        }
    else if ((400 > dpi) && (dpi >= 210))
        {
        defaultPx = (TInt)((float)defaultPx * 2.2);
        }
    else if (dpi >= 400)
        {
        defaultPx = (TInt)((float)defaultPx * 4.2);
        }

    MAknsSkinInstance* skin = AknsUtils::SkinInstance();

    TRAPD(error, AknsUtils::CreateIconL( skin,
                                   aknsid,
                                   image.m_img,
                                   image.m_msk,
                                   filename,
                                   imgid,
                                   imgid+1));


    if (error!=KErrNone)
        {
        return;
        }


    // adjust the size for the disc, circle, square, radio and checkbox icons to be
    // slightly smaller than the other canned icons in order to improve look and feel
    // radio and checkbox
#ifdef __BROWSER_SDK
    if (imgid == EMbmWebkiticons_sdkQgn_prop_wml_selectfile ||
        imgid == EMbmWebkiticons_sdkQgn_prop_wml_selectarrow ||
        imgid == EMbmWebkiticons_sdkQgn_prop_wml_radiobutt_off ||
        imgid == EMbmWebkiticons_sdkQgn_prop_wml_radiobutt_on ||
        imgid == EMbmWebkiticons_sdkQgn_prop_wml_checkbox_off ||
        imgid == EMbmWebkiticons_sdkQgn_prop_wml_checkbox_on
        )
#else
    if (imgid == EMbmWebkiticonsQgn_prop_wml_selectfile ||
        imgid == EMbmWebkiticonsQgn_prop_wml_selectarrow ||
        imgid == EMbmWebkiticonsQgn_prop_wml_radiobutt_off ||
        imgid == EMbmWebkiticonsQgn_prop_wml_radiobutt_on ||
        imgid == EMbmWebkiticonsQgn_prop_wml_checkbox_off ||
        imgid == EMbmWebkiticonsQgn_prop_wml_checkbox_on
        )
#endif
        {
        px = (TInt)((float)defaultPx * .90);
        }
#ifdef __BROWSER_SDK
    else if (imgid == EMbmWebkiticons_sdkQgn_indi_viewer_pointer ||
        imgid == EMbmWebkiticons_sdkQgn_indi_viewer_pointer_hand ||
        imgid == EMbmWebkiticons_sdkQgn_indi_viewer_pointer_wait)
#else
    else if (imgid == EMbmWebkiticonsQgn_indi_viewer_pointer ||
        imgid == EMbmWebkiticonsQgn_indi_viewer_pointer_hand ||
        imgid == EMbmWebkiticonsQgn_indi_viewer_pointer_wait)
#endif
        {
        // cursor
        if (dpi < 200)
            {
            px = (TInt)((float)defaultPx * 1.1);
            }
        else
            {
            px = (TInt)((float)defaultPx * 1.3);
            }
        }
    else if (imgid == EMbmWebkiticonsQgn_indi_button_exit_fs)
        {
        px = (TInt)((float)defaultPx * 2);
        }
    else
        {
        px = (TInt)((float)defaultPx * 1.2);
        }

    AknIconUtils::SetSize(image.m_img,TSize(px,px)); //rect.Rect().Size());
    AknIconUtils::SetSize(image.m_msk,TSize(px,px)); //rect.Rect().Size());
    }


//  End of File
