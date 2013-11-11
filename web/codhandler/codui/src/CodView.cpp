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
* Description:  Implementation of class CCodView.
*
*/


// INCLUDE FILES

#include <eikrted.h>
#include <txtrich.h>
#include <barsread.h>
#include <AknUtils.h>
#include <AknDef.h>
#include <StringLoader.h>
#include <CodData.h>
#include <CodDefs.h>
#include <avkon.rsg>
#include <CodUi.rsg>
#include "CodView.h"
#include "CodLogger.h"
#include <Oma2Agent.h>
#include <AknUtils.h>
#include <aknlayout.cdl.h>

#include "eikon.hrh"


// ================= TYPES =======================

/**
* Mime type to resource id mapping entry.
*/
struct TMimeAndResId
    {
    const TText8* iName;    ///< MIME type.
    TInt iResId;            ///< Resource id.
    };

// ================= CONSTANTS =======================

/// Number of bytes in a kilobyte.
LOCAL_D const TInt KCodKbyte = 1024;

/**
* Null-terminated MIME-to-resId mapping table.
* Order of entries is important (from most specific to most generic), as
* entries are evaluated linearly.
*/
LOCAL_D const TMimeAndResId KCodMimeAndResIds[] =
    {
//      { _S8( "game: no such MIME" ),                  R_QTN_CD_TYPE_GAME },
        { _S8( "application/vnd.nokia.ringing-tone" ),  R_QTN_CD_TYPE_RTONE },
        { _S8( "application/x-nokiaGameData" ),         R_QTN_CD_TYPE_GAME_LEVEL },
        { _S8( "application/vnd.oma.drm" ),             R_QTN_CD_TYPE_NOT_CLASSIFIED },
        { _S8( "application/" ),                        R_QTN_CD_TYPE_APPLICATION },
        { _S8( "image/vnd.nokia.ota-bitmap" ),          R_QTN_CD_TYPE_OPERATOR_LOGO },
        { _S8( "image/vnd.nok-oplogo" ),                R_QTN_CD_TYPE_OPERATOR_LOGO },
        { _S8( "image/vnd.nok-picturemessage" ),        R_QTN_CD_TYPE_PICTURE_MESSAGE },
        { _S8( "image/" ),                              R_QTN_CD_TYPE_IMAGE },
        { _S8( "audio/" ),                              R_QTN_CD_TYPE_SOUND },
        { _S8( "text/X-vCard" ),                        R_QTN_CD_TYPE_VCARD },
        { _S8( "text/" ),                               R_QTN_CD_TYPE_DOCUMENT },
        { _S8( "" ),                                    R_QTN_CD_TYPE_NOT_CLASSIFIED },
        { NULL,                                         0 }
    };

// ================= MEMBER FUNCTIONS =======================

// ---------------------------------------------------------
// CCodView:CCodView()
// ---------------------------------------------------------
//
CCodView::CCodView()
    {
    }

// ---------------------------------------------------------
// CCodView:ConstructL()
// ---------------------------------------------------------
//
void CCodView::ConstructL()
    {
    CreateWindowL();
    TResourceReader rr;
    iCoeEnv->CreateResourceReaderLC( rr, R_COD_UI_RTXED );
    ConstructFromResourceL( rr );
    CleanupStack::PopAndDestroy();  // rr
    }

// ---------------------------------------------------------
// CCodView:ConstructFromResourceL()
// ---------------------------------------------------------
//
void CCodView::ConstructFromResourceL( TResourceReader& aReader )
    {
    iEditor = new (ELeave) CEikRichTextEditor();
    iEditor->ConstructFromResourceL( aReader );
    iEditor->SetContainerWindowL( *this );

    CRichText* richText = iEditor->RichText();

    iNameStyle = CParagraphStyle::NewL
        (
        *richText->GlobalParaFormatLayer(),
        *richText->GlobalCharFormatLayer()
        );

    iValueStyle = CParagraphStyle::NewL
        (
        *richText->GlobalParaFormatLayer(),
        *richText->GlobalCharFormatLayer()
        );

    SetStylesL();

    iStyles = CStyleList::NewL();
    RParagraphStyleInfo nameStyleInfo( iNameStyle );
    User::LeaveIfError( iStyles->AppendL( &nameStyleInfo ) );
    RParagraphStyleInfo valueStyleInfo( iValueStyle );
    User::LeaveIfError( iStyles->AppendL( &valueStyleInfo ) );

    richText->SetStyleListExternallyOwned( *iStyles );

    iSbFrame = new (ELeave) CEikScrollBarFrame( this, NULL, ETrue );
    iSbFrame->SetScrollBarVisibilityL
        ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
    iSbFrame->SetTypeOfVScrollBar( CEikScrollBarFrame::EArrowHead );
    }

// ---------------------------------------------------------
// CCodView::~CCodView()
// ---------------------------------------------------------
//
CCodView::~CCodView()
    {
    delete iUndefinedValue;
    delete iSbFrame;
    delete iEditor;
    delete iStyles;
    }

// ---------------------------------------------------------
// CCodView:SetDataL()
// ---------------------------------------------------------
//
void CCodView::SetDataL( const CCodData& aData, TBool aCod )
    {
    iEditor->RichText()->Reset();
    AddLineL( R_QTN_CD_DETAIL_NAME, aData.Count() > 1 && aData.Name().Length()? aData.Name() : (*aData[aData.ActiveDownload()]).Name() );
    HBufC* buf = AllocTypeNameL( DisplayType( aData ) );
    CleanupStack::PushL( buf );
    AddLineL( R_QTN_CD_DETAIL_TYPE, *buf );
    CleanupStack::PopAndDestroy( buf );
    if ( aCod )
        {
        AddLineL( R_QTN_CD_DETAIL_PRICE, aData.Price() );
        }
    AddLineL( R_QTN_CD_DETAIL_DESCRIPT, aData.Description() );

    // Displaying size requires some extra fiddling with B/kB notation.
    TInt size = aData.Size();
    if ( size >= KCodKbyte )
        {
        // Round up.
        buf = StringLoader::LoadLC
            ( R_QTN_SIZE_KB, (size - 1) / KCodKbyte + 1, iCoeEnv );
        }
    else
        {
        buf = StringLoader::LoadLC( R_QTN_SIZE_B, size, iCoeEnv );
        }
    // R_QTN_SIZE_KB is " %N kB", and R_QTN_SIZE_B is " %N B" (with leading
    // spaces!!!). Obviously this is a system resource which was adjusted to
    // someone's particular "tabulating" needs...
    // So we have to remove leading/trailing spaces.
    buf->Des().Trim();
    AddLineL( R_QTN_CD_DETAIL_SIZE, *buf );
    CleanupStack::PopAndDestroy( buf );

    AddLineL( R_QTN_CD_DETAIL_VENDOR, aData.Vendor() );
    delete iUndefinedValue; // Not needed anymore.
    iUndefinedValue = NULL;
    SizeChanged();
    }

// ---------------------------------------------------------
// CCodView:Clear()
// ---------------------------------------------------------
//
void CCodView::Clear()
    {
    iEditor->RichText()->Reset();
    SizeChanged();
    }

// ---------------------------------------------------------
// CCodView::MinimumSize()
// ---------------------------------------------------------
//
TSize CCodView::MinimumSize()
    {
    return iEikonEnv->EikAppUi()->ClientRect().Size();
    }

// ---------------------------------------------------------
// CCodView::SizeChanged()
// ---------------------------------------------------------
//
void CCodView::SizeChanged()
    {
    if ( OwnsWindow() )
        {
        // View as main control of App UI.
        iEditor->SetRect( Rect() );
        }
    else
        {
        // View in dialog.
        // Dialog sets Rect() so it includes the softkey area.
        TAknLayoutRect editorRect;
        editorRect.LayoutRect
            ( Rect(), AknLayout::main_pane( Rect(), 0, 0, 1 ) );
        iEditor->SetRect( editorRect.Rect() );
        }
    iEditor->DrawNow();
    TRAP_IGNORE( UpdateScrollBarL( /*aTile=*/ETrue ) );
    }

// ---------------------------------------------------------
// CCodView::CountComponentControls()
// ---------------------------------------------------------
//
TInt CCodView::CountComponentControls() const
    {
    return iSbFrame->CountComponentControls() + 1;
    }

// ---------------------------------------------------------
// CCodView::ComponentControl()
// ---------------------------------------------------------
//
CCoeControl* CCodView::ComponentControl( TInt aIndex ) const
    {
    if ( aIndex == 0 )
        {
        return iEditor;
        }
    else
        {
        return iSbFrame->ComponentControl( aIndex - 1 );
        }
    }

// ---------------------------------------------------------
// CCodView::OfferKeyEventL()
// ---------------------------------------------------------
//
TKeyResponse CCodView::OfferKeyEventL
( const TKeyEvent& aKeyEvent, TEventCode aType )
    {
    TKeyResponse response = EKeyWasNotConsumed;
    switch ( aKeyEvent.iCode )
        {
        case EKeyLeftUpArrow:     // Northwest
        case EStdKeyDevice10:     //   : Extra KeyEvent supports diagonal event simulator wedge
        case EKeyUpArrow:         // North
        case EKeyRightUpArrow:    // Northeast
        case EStdKeyDevice11:     //   : Extra KeyEvent supports diagonal event simulator wedge
            {
            ScrollUpL();
            response = EKeyWasConsumed;
            break;
            }

        case EKeyLeftDownArrow:   // Southwest
        case EStdKeyDevice13:     //   : Extra KeyEvent supports diagonal event simulator wedge
        case EKeyDownArrow:       // South
        case EKeyRightDownArrow:  // Southeast
        case EStdKeyDevice12:     //   : Extra KeyEvent supports diagonal event simulator wedge
            {
            ScrollDownL();
            response = EKeyWasConsumed;
            break;
            }

        default:
            {
            response = iEditor->OfferKeyEventL( aKeyEvent, aType );
            break;
            }
        }
    return response;
    }

// ---------------------------------------------------------
// CCodView::Draw()
// ---------------------------------------------------------
//
void CCodView::Draw( const TRect& aRect ) const
    {
    SystemGc().Clear( aRect );
    }

// ----------------------------------------------------------
// CCodView::HandleResourceChange()
// ----------------------------------------------------------
//
void CCodView::HandleResourceChange( TInt aType )
    {
    CCoeControl::HandleResourceChange( aType );
    if ( aType == KEikDynamicLayoutVariantSwitch )
        {
        TRAP_IGNORE( SetStylesL() );    // Fontspec might have changed.
        SizeChanged();                  // Relayout.
        }
    }

// ----------------------------------------------------------
// CCodView::SetStylesL()
// ----------------------------------------------------------
//
void CCodView::SetStylesL()
    {
    TCharFormat charFormat;
    TCharFormatMask charFormatMask;
    charFormat.iFontPresentation.iTextColor = KRgbBlack;
    charFormatMask.SetAttrib( EAttFontTypeface );
    charFormatMask.SetAttrib( EAttFontHeight );
    charFormatMask.SetAttrib( EAttColor );
    charFormat.iFontSpec = AknLayoutUtils::FontFromId
        ( EAknLogicalFontPrimaryFont )->FontSpecInTwips();
    iNameStyle->CharFormatLayer()->SetL( charFormat, charFormatMask );
    charFormat.iFontSpec = AknLayoutUtils::FontFromId
        ( EAknLogicalFontSecondaryFont )->FontSpecInTwips();
    iValueStyle->CharFormatLayer()->SetL( charFormat, charFormatMask );
    }

// ---------------------------------------------------------
// CCodView:AddLineL()
// ---------------------------------------------------------
//
void CCodView::AddLineL( TInt aNameResId, const TDesC& aValue )
    {
    TPtrC value( aValue );
    if ( !value.Length() )
        {
        // Empty value. Show "undefined".
        if ( !iUndefinedValue )
            {
            // Not yet loaded, do it now. Subsequent calls will reuse
            // this buffer, so it is not loaded more than once.
            // After all lines are added, the buffer is deleted
            // (in SetDataL()), to spare memory.
            iUndefinedValue = iCoeEnv->AllocReadResourceL
                ( R_QTN_CD_VALUE_UNDEFINED );
            }
        value.Set( *iUndefinedValue );
        }

    CRichText* richText = iEditor->RichText();
    TInt start = richText->DocumentLength();
    if ( start )
        {
        // This is not the first line. End previous paragraph.
        richText->InsertL( start, CEditableText::EParagraphDelimiter );
        }
    HBufC* name = iCoeEnv->AllocReadResourceLC( aNameResId );
    start = richText->DocumentLength();
    richText->InsertL( start, *name );
    TInt end = richText->DocumentLength();
    richText->ApplyParagraphStyleL
        (
        *iStyles->At( 0 ).iStyle,
        start,
        end - start,
        CParagraphStyle::ERetainNoSpecificFormats
        );
    richText->InsertL( end, CEditableText::EParagraphDelimiter );
    CleanupStack::PopAndDestroy( name );
    start = richText->DocumentLength();
    richText->InsertL( start, value );
    end = richText->DocumentLength();
    richText->ApplyParagraphStyleL
        (
        *iStyles->At( 1 ).iStyle,
        start,
        end - start,
        CParagraphStyle::ERetainNoSpecificFormats
        );
    // Paragraph delimiter is not added - next call will do it.
    // This avoids an extra empty line (after the last).
    }

// ---------------------------------------------------------------------------
// CCodView::UpdateScrollBarL
// ---------------------------------------------------------------------------
//
void CCodView::UpdateScrollBarL( TBool aTile )
    {
    CTextLayout* textLayout = iEditor->TextLayout();

    if ( textLayout->IsBackgroundFormatting() )
        {
        return;
        }

    TRect viewRect( iEditor->TextView()->ViewRect() );
    TEikScrollBarModel vSbModel;
    vSbModel.iThumbPosition = textLayout->PixelsAboveBand();
    vSbModel.iScrollSpan =
        textLayout->FormattedHeightInPixels() - viewRect.Height();
    vSbModel.iThumbSpan = 1;

    CLOG(( \
        5, \
        _L("rectHeight(%d) viewRectHeigh(%d) pixelsAboveBand(%d) formattedHeight(%d)"), \
        Rect().Height(), \
        viewRect.Height(), \
        textLayout->PixelsAboveBand(), \
        textLayout->FormattedHeightInPixels() \
    ));

    if ( vSbModel.iScrollSpan <= 0 )
        {
        vSbModel.iThumbPosition = 0;
        vSbModel.iScrollSpan = 0;
        }

    if ( aTile )
        {
        TEikScrollBarFrameLayout layout;
        TEikScrollBarModel hSbModel;
        iSbFrame->TileL
            ( &hSbModel, &vSbModel, viewRect, viewRect, layout );
        }

    iSbFrame->MoveVertThumbTo( vSbModel.iThumbPosition );
    }

// ---------------------------------------------------------------------------
// CCodView::AllocTypeNameL
// ---------------------------------------------------------------------------
//
HBufC* CCodView::AllocTypeNameL( const TDesC8& aMimeType )
    {
    const TMimeAndResId* mapping;
    TInt resId = R_QTN_CD_VALUE_UNDEFINED;
    for ( mapping = &KCodMimeAndResIds[0]; mapping->iName; mapping++ )
        {
        TPtrC8 name( mapping->iName );
        // Looking for partial match (leftmost part of aMimeType is compared
        // to mapping->iName).
        if ( name.Length() <= aMimeType.Length() )
            {
            if ( aMimeType.Left( name.Length() ).CompareF( name ) == 0 )
                {
                resId = mapping->iResId;
                break;
                }
            }
        }
    return StringLoader::LoadL( resId, iCoeEnv );
    }

// ---------------------------------------------------------------------------
// CCodView::ScrollUpL
// ---------------------------------------------------------------------------
//
void CCodView::ScrollUpL()
    {
    CTextLayout* textLayout = iEditor->TextLayout();
    if ( !textLayout->IsBackgroundFormatting() )
        {
        if ( textLayout->PixelsAboveBand() > 0 )
            {
            iEditor->MoveDisplayL( TCursorPosition::EFLineUp );
            UpdateScrollBarL( /*aTile=*/EFalse );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCodView::ScrollDownL
// ---------------------------------------------------------------------------
//
void CCodView::ScrollDownL()
    {
    CTextLayout* textLayout = iEditor->TextLayout();
    if ( !textLayout->IsBackgroundFormatting() )
        {
        TRect viewRect( iEditor->TextView()->ViewRect() );
        if ( textLayout->FormattedHeightInPixels() -
             textLayout->PixelsAboveBand() - viewRect.Height() > 0 )
            {
            iEditor->MoveDisplayL( TCursorPosition::EFLineDown );
            UpdateScrollBarL( /*aTile=*/EFalse );
            }
        }
    }

// ---------------------------------------------------------------------------
// CCodView::DisplayType
// ---------------------------------------------------------------------------
//
TPtrC8 CCodView::DisplayType( const CCodData& aData )
    {
    for ( TInt i = 0; i < (*aData[aData.ActiveDownload()]).Types().MdcaCount(); i++ )
        {
        TPtrC8 type( (*aData[aData.ActiveDownload()]).Types().MdcaPoint( i ) );
        if (
            type.Compare( KOma1DrmMessageContentType ) &&
            type.Compare( KOma1XmlRoContentType ) &&
            type.Compare( KOma1WbxmlRoContentType ) &&
            type.Compare( KOma2RoContentType ) &&
            type.Compare( KOma2TriggerContentType ) &&
            type.Compare( KOma1DcfContentType ) &&
            type.Compare( KOma2DcfContentType ) &&
            type.Compare( KOmaImportContentType )
           )
            {
            return type;    // Non-DRM, return it.
            }
        }
    return (*aData[aData.ActiveDownload()]).Type();    // By default returns the first, whatever it is.
    }
