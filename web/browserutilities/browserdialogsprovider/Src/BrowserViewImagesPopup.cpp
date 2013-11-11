/*
* Copyright (c) 2004 Nokia Corporation and/or its subsidiary(-ies).
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
*      View images popup box
*      
*
*/


// USER INCLUDES
#include "BrowserDialogsProvider.h"
#include "BrowserDialogsProvider.hrh"
#include "BrowserViewImagesPopup.h"
#include "PhotoOperationWait.h"
#include "BrowserViewImagesListBox.h"

// SYSTEM INCLUDES
#include <DocumentHandler.h>
#include <apmstd.h>
#include <uri16.h>
#include <aknenv.h>
#include <avkon.mbg>
#include <aknEditStateIndicator.h>
#include <AknsListBoxBackgroundControlContext.h>
#include <AiwGenericParam.h>

// Resources
#include <BrowserDialogsProvider.rsg>

//-----------------------------------------------------------------------------
//  CBrowserViewImagesPopup::CBrowserViewImagesPopup()
//
//  Default Constructor
//-----------------------------------------------------------------------------
//
CBrowserViewImagesPopup::CBrowserViewImagesPopup()
    {
    }

//-----------------------------------------------------------------------------
//  CBrowserViewImagesPopup::~CBrowserViewImagesPopup()
//
//  Destructor
//-----------------------------------------------------------------------------
//
CBrowserViewImagesPopup::~CBrowserViewImagesPopup()
    {
    delete iListBox;
    delete iPhotoOperationWait;
    delete iSkinContext;
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesPopup::ConstructL( 
//                            CArrayFixFlat<TBrCtlImageCarrier>& aPageImages )
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesPopup::ConstructL( 
                            CArrayFixFlat<TBrCtlImageCarrier>& aPageImages )
    {
    delete iListBox;
    iListBox = NULL;
    iListBox = new (ELeave) CBrowserViewImagesListBox();
    
    ConstructL( iListBox, 
                        R_DIALOGSPROVIDER_VIEWIMAGES_BUTTONS, 
                        AknPopupLayouts::EMenuDoubleLargeGraphicWindow );

    iListBox->ConstructL( *this, aPageImages );
    iListBox->CreateScrollBarFrameL( ETrue );
    iListBox->ScrollBarFrame()->SetScrollBarVisibilityL( 
                        CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iListBox->SetObserver(this);

    iListBox->StartConversionL();        // start thumbnailing

    HBufC* title = CEikonEnv::Static()->AllocReadResourceL(
                                                R_BROWSER_QUERY_SELECT_IMAGE);
    CleanupStack::PushL(title);
    SetTitleL(*title);
    CleanupStack::PopAndDestroy();  // title
    }


//-----------------------------------------------------------------------------
//  CBrowserViewImagesPopup::ConstructL( CEikListBox *aListBox, 
//                                      TInt aCbaResource,
//                                      AknPopupLayouts::TAknPopupLayouts aType )
//  Private constructor
//-----------------------------------------------------------------------------
//
void CBrowserViewImagesPopup::ConstructL( CEikListBox *aListBox, 
                                        TInt aCbaResource,
                                        AknPopupLayouts::TAknPopupLayouts aType )
    {
    CAknPopupList::ConstructL(aListBox, aCbaResource, aType);
    iPhotoOperationWait = CPhotoOperationWait::NewL();

    TAknsItemID tileIID = KAknsIIDSkinBmpListPaneNarrowA;

    AknLayoutUtils::SAknLayoutRect tile = { 0, 0,0, ELayoutEmpty, ELayoutEmpty, 20, 144 };
    AknLayoutUtils::SAknLayoutRect bg = { 0, 0, 0, ELayoutEmpty, ELayoutEmpty, 176, 144 };
    if (AknLayoutUtils::LayoutMirrored())
    {
        tile.iL = ELayoutEmpty;
        tile.iR = 0;
    }
    
    TRect clientRect( 0, 0, 176, 144 );
    TAknLayoutRect tileRect;
    tileRect.LayoutRect(clientRect, tile);  
    TAknLayoutRect bgRect;
    bgRect.LayoutRect(clientRect, bg);

    iSkinContext = CAknsListBoxBackgroundControlContext::NewL(
        KAknsIIDSkinBmpMainPaneUsual, 
        bgRect.Rect(), //TODOVRa: Should come from LAF!
        EFalse, tileIID,
        tileRect.Rect() ); //TODOVRa: Should come from LAF!    
    }


// ---------------------------------------------------------------------------
// CBrowserViewImagesPopup::HandleListBoxEventL( CEikListBox* aListBox,
//                                                  TListBoxEvent aEventType )
// ---------------------------------------------------------------------------
//
void CBrowserViewImagesPopup::HandleListBoxEventL( CEikListBox* aListBox, 
                                                    TListBoxEvent aEventType )
    {
    if((aEventType==EEventEnterKeyPressed || aEventType==EEventItemDoubleClicked) && aListBox==ListBox())
        {
        ProcessCommandL( EDialogsProviderCmdViewImagesView );
        }
    }


// ---------------------------------------------------------------------------
// CBrowserViewImagesPopup::HandleControlEventL( CCoeControl* /*aControl*/, 
//                                                  TCoeEvent /*aEventType*/ )
// ---------------------------------------------------------------------------
//
void CBrowserViewImagesPopup::HandleControlEventL( CCoeControl* /*aControl*/, 
                                                    TCoeEvent /*aEventType*/ )
    {
    // CAknPopupList::HandleControlEventL(aControl, aEventType);
    }


// ---------------------------------------------------------------------------
// CBrowserViewImagesPopup::PhotoOperationL()
// ---------------------------------------------------------------------------
//
void CBrowserViewImagesPopup::PhotoOperationL()
    {
    const TBrCtlImageCarrier& img = STATIC_CAST( CBrowserViewImagesListBox*, ListBox() )->CurrentlySelectedImage();
    iPopupFader.FadeBehindPopup( this, this, EFalse );
    PhotoOperationL(img, iPhotoOperationWait);
    iPopupFader.FadeBehindPopup( this, this, ETrue );
    }


// ---------------------------------------------------------------------------
// CBrowserViewImagesPopup::LaunchPhotoAlbumL()
// ---------------------------------------------------------------------------
//
void CBrowserViewImagesPopup::LaunchPhotoAlbumL()
    {
    if(!iPhotoOperationWait->IsStarted())
        {
        iPhotoOperationWait->SetViewActivate(ETrue);

        TRAPD( err, PhotoOperationL() )

        if ( err == KErrNone )
            {
            iPhotoOperationWait->SetViewActivate(EFalse);
            }
        else
            {
            iPhotoOperationWait->SetViewActivate(EFalse);
            User::Leave( err );
            }               
        }
    }


// ---------------------------------------------------------------------------
// CBrowserViewImagesPopup::ProcessCommandL( TInt aCommandId )
// ---------------------------------------------------------------------------
//
void CBrowserViewImagesPopup::ProcessCommandL( TInt aCommandId )
    {
    switch(aCommandId)
        {
        case EDialogsProviderCmdViewImagesView:
            LaunchPhotoAlbumL();
            break;
        case EAknSoftkeyCancel:
        default:
            CancelPopup();
            break;
        }
    }

// ---------------------------------------------------------------------------
// CBrowserViewImagesPopup::ProcessCommandL( TInt aCommandId )
// ---------------------------------------------------------------------------
//
void CBrowserViewImagesPopup::PhotoOperationL(const TBrCtlImageCarrier& aImg, CPhotoOperationWait* aPhotoOperationWait)
    {
    TBool ownPhotoOperationWait = EFalse;
    if (!aPhotoOperationWait)
        {
        aPhotoOperationWait = CPhotoOperationWait::NewL();
        ownPhotoOperationWait = ETrue;
        }
    HBufC* apu;
    TInt locate;
    
    CEikProcess* process = CEikonEnv::Static()->Process();
    CDocumentHandler* docHandler = CDocumentHandler::NewLC(process);
    
    // Getting name for image from url.
    TUriParser16 urlParser;
    if ( aImg.AltText().Length() )
        {
        urlParser.Parse( aImg.AltText() );
        }
    else
        {
        urlParser.Parse( aImg.Url() );
        }
    apu = urlParser.Extract(EUriPath).AllocLC();
    locate = apu->LocateReverseF('/');
    locate++;
    apu->Des().Delete(0, locate);
    
    // Inform DocumentHandler of image type, if not recognized but handled
    TBuf8<128> mimeType;
    _LIT(KWbmpDataType, "image/vnd.wap.wbmp");
    _LIT(KOtaDataType, "image/vnd.nokia.ota-bitmap");
    _LIT( KEmptyDataType, "" );
    
    if ( aImg.ImageType() == EImageTypeWbmp )
        {
        // WBMP image
        mimeType.Copy(KWbmpDataType);
        }
    else if ( aImg.ImageType() == EImageTypeOta )
        {
        // OTA image
        mimeType.Copy( KOtaDataType );
        }
    else
        {
        mimeType.Copy( KEmptyDataType  );
        }
    
    TDataType dataType( mimeType );
    
    // remove fading behind popup during displaying the image
//    iPopupFader.FadeBehindPopup( this, this, EFalse );
    
    // launch photo album
    CAiwGenericParamList& paramList = docHandler->InParamListL();
    
     // Allow save
    TBool allowSave( ETrue );
    TAiwVariant allowSaveVariant(allowSave);
    TAiwGenericParam genericParamAllowSave( EGenericParamAllowSave, 
                                            allowSaveVariant );
    paramList.AppendL( genericParamAllowSave );
    
    RFile tempFile; 
    TRAPD(err, docHandler->SaveTempFileL( aImg.RawData(), dataType, apu->Des(), tempFile ));
    if (err != KErrNone)
        {
        if (aImg.AltText().Length())
            {
            urlParser.Parse( aImg.Url() );
            CleanupStack::PopAndDestroy(); // apu
            apu = NULL;
            apu = urlParser.Extract(EUriPath).AllocLC();
            locate = apu->LocateReverseF('/');
            locate++;
            apu->Des().Delete(0, locate);
            docHandler->SaveTempFileL( aImg.RawData(), dataType, apu->Des(), tempFile );
            }
        else
            {
            User::Leave(err);
            }
        }
    CleanupClosePushL( tempFile );
    docHandler->OpenFileEmbeddedL( tempFile, dataType, paramList );
    CleanupStack::PopAndDestroy( &tempFile ); // tempFile

    // wait until user returns from the photo album
    docHandler->SetExitObserver(aPhotoOperationWait);
    aPhotoOperationWait->Wait();
    
    // restore fading behind popup
//    iPopupFader.FadeBehindPopup( this, this, ETrue );
    
    CleanupStack::PopAndDestroy(2);  // docHandler,apu

    if (ownPhotoOperationWait)
        {
        delete aPhotoOperationWait;
        aPhotoOperationWait = NULL;
        }
    }


// ---------------------------------------------------------------------------
// CBrowserViewImagesPopup::MopSupplyObject( TTypeUid aId )
// ---------------------------------------------------------------------------
//
TTypeUid::Ptr CBrowserViewImagesPopup::MopSupplyObject( TTypeUid aId )
    {
    return CAknPopupList::MopSupplyObject(aId);
    }

// ---------------------------------------------------------------------------
// CBrowserViewImagesPopup::AttemptExitL(TBool aAccept)
//
// Overrides CAknPopupList's AttemptExitL
// Popup should not be deleted if still in use
// ---------------------------------------------------------------------------
//
void CBrowserViewImagesPopup::AttemptExitL(TBool aAccept)
    {

    if ( IsAppShutterActive() && iPhotoOperationWait->IsStarted() )
        {
        iPhotoOperationWait->DoCancel();
        }

    if (!iPhotoOperationWait->IsStarted())
        {
        if (iReturn) //Always not null unless ExecuteLD leaves
            *iReturn = aAccept;
        //delete this;
        }
        //Need to call the base class api also
        CAknPopupList::AttemptExitL(aAccept);
    }

// ---------------------------------------------------------------------------
// CBrowserViewImagesPopup::IsAppShutterActive
// ---------------------------------------------------------------------------
//
TBool CBrowserViewImagesPopup::IsAppShutterActive()
    {
    CAknAppUi* shuttingApp = CAknEnv::AppWithShutterRunning();

    if ( shuttingApp )
        {
        return ETrue;
        }
    else
        {
        return EFalse;
        }
    }
// End of File
