/*
* Copyright (c) 2002 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include "BrowserSelectElementListBox.h"
#include "BrowserSelectElementModel.h"
#include "BrowserSelectElementItemDrawer.h"

#include <aknsfld.h>

// CONSTANTS
#include "BrowserDialogsProviderConstants.h"

#include "eikon.hrh"


// ================= MEMBER FUNCTIONS =======================
// ====== TBrCtlFavouritesListboxFilterTextArray =======

TBrCtlSelectElementFilterTextArray::TBrCtlSelectElementFilterTextArray(
                            const CArrayFix<TBrCtlSelectOptionData>& aOptions )
                            : iOptions ( aOptions )
    {
    }

// ---------------------------------------------------------
// TBrCtlFavouritesListboxFilterTextArray::MdcaCount
// ---------------------------------------------------------
//
TInt TBrCtlSelectElementFilterTextArray::MdcaCount() const
    {
    return iOptions.Count();
    }


// ---------------------------------------------------------
// TBrCtlFavouritesListboxFilterTextArray::MdcaPoint
// ---------------------------------------------------------
//
TPtrC TBrCtlSelectElementFilterTextArray::MdcaPoint ( TInt aIndex ) const
    {
    return iOptions.At( aIndex ).Text();
    }




// ================= MEMBER FUNCTIONS =======================
// ====== TBrCtlSelectElementItemTextArray =======

TBrCtlSelectElementItemTextArray::TBrCtlSelectElementItemTextArray(
                            const CArrayFix<TBrCtlSelectOptionData>& aOptions )
                            : iOptions( aOptions )
    {
    iText = NULL;
    }

TBrCtlSelectElementItemTextArray::~TBrCtlSelectElementItemTextArray()
    {
    delete iText;
    }

// ----------------------------------------------------------------------------
// TBrCtlSelectElementItemTextArray::AddPrefixLC
// ----------------------------------------------------------------------------
//
HBufC* TBrCtlSelectElementItemTextArray::AddPrefixLC(
                                                const TDesC& aText,
                                                TBool aIsOptionGroup ) const
    {
    // title = prefix + text + zero terminator
    TPtrC prefix( aIsOptionGroup ? KPrefixOptionGroup : KPrefixOption );
    HBufC* title = HBufC::NewL( prefix.Length() + aText.Length() + 1 );

    CleanupStack::PushL( title );

    // Copy the text
    HBufC* buf = HBufC::NewL( aText.Length() );
    TPtr ptr = buf->Des();
    ptr.Copy( aText );
    title->Des().Append( prefix );
    title->Des().Append( ptr );
    title->Des().ZeroTerminate();

    delete buf;

    return title;
    }

// ----------------------------------------------------------------------------
// TBrCtlSelectElementItemTextArray::FormatTextL
// ----------------------------------------------------------------------------
//
void TBrCtlSelectElementItemTextArray::FormatTextL(
                                    const TBrCtlSelectOptionData& optionData )
    {
    // free the previous text
    if ( iText )
        {
        delete iText;
        iText = NULL;
        }

    // save the text in the right format
    HBufC* title = AddPrefixLC( optionData.Text(), optionData.IsOptGroup() );
    CleanupStack::Pop();            // title
    iText = title;
    }

// ----------------------------------------------------------------------------
// TBrCtlSelectElementItemTextArray::MdcaCount
// ----------------------------------------------------------------------------
//
TPtrC TBrCtlSelectElementItemTextArray::MdcaPoint( TInt aIndex ) const
    {
    TBrCtlSelectElementItemTextArray* self =
                        CONST_CAST( TBrCtlSelectElementItemTextArray*, this );

    // prepare the new text
    TRAPD( err, self->FormatTextL( iOptions.At( aIndex ) ) );

    if ( !err )
        {
        return iText->Des();
        }
    else
        {
        return TPtrC( KNullDesC );
        }
    }



// ================= MEMBER FUNCTIONS =========================================

// ----------------------------------------------------------------------------
// CBrowserSelectElementListBox::CBrowserSelectElementListBox
// ----------------------------------------------------------------------------
//
CBrowserSelectElementListBox::CBrowserSelectElementListBox(
                        const TBrCtlSelectOptionType aBrCtlSelectOptionType,
                        const CArrayFix<TBrCtlSelectOptionData>& aOptions ) :
                            iOptions( aOptions ),
                            iBrCtlSelectOptionType( aBrCtlSelectOptionType )
    {
    }


// ----------------------------------------------------------------------------
// CBrowserSelectElementListBox::ConstructL
// ----------------------------------------------------------------------------
//
void CBrowserSelectElementListBox::ConstructL( const CCoeControl& aParent )
    {
    iHasNotMoved = ETrue;
    iModel = CBrowserSelectElementModel::NewL( iOptions );
    STATIC_CAST (CBrowserSelectElementModel*,iModel)->SetItemArray();
    CreateItemDrawerL();

    CEikListBox::ConstructL( &aParent, EAknListBoxMultiselectionList );

    // this method call is required for correct support of
    // pictographs in S60 Japanese
    EnableExtendedDrawingL();

    SetSelectionIndexesL();
    ItemDrawer()->FormattedCellData()->SetControl(this);
    ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
    }


// ----------------------------------------------------------------------------
// CBrowserSelectElementListBox::CreateItemDrawerL
// ----------------------------------------------------------------------------
//
void CBrowserSelectElementListBox::CreateItemDrawerL()
    {
    CFormattedCellListBoxData* data = CPopupFormattedListBoxData::NewL();
    CleanupStack::PushL( data );
    iItemDrawer = new (ELeave) CBrowserSelectElementItemDrawer
        ( *Model(), *LatinBold13(), *data, iBrCtlSelectOptionType, iOptions );
    CleanupStack::Pop();    // data
    STATIC_CAST( CBrowserSelectElementItemDrawer*,
                 iItemDrawer )->LoadIconsL();
    }

// ----------------------------------------------------------------------------
// CBrowserSelectElementListBox::OfferKeyEventL
// ----------------------------------------------------------------------------
//
TKeyResponse CBrowserSelectElementListBox::OfferKeyEventL(
    const TKeyEvent& aKeyEvent,
    TEventCode aType )
    {
    TKeyResponse resp;
    TInt index = CurrentItemIndex();
    iHasNotMoved = EFalse;
    switch ( aKeyEvent.iCode )
        {
        case EKeyEscape:
            ReportEventL( MCoeControlObserver::EEventRequestCancel );
            resp = EKeyWasConsumed;
            break;
        case EKeyLeftUpArrow:         // Northwest
        case EStdKeyDevice10:         //   : Extra KeyEvent supports diagonal event simulator wedge
        case EKeyUpArrow:             // North
        case EKeyRightUpArrow:        // Northeast
        case EStdKeyDevice11:         //   : Extra KeyEvent supports diagonal event simulator wedge

            for ( ;( --index >= 0 );)
            {

            if (!(iOptions.At(((CBrowserSelectElementModel*)Model())->Filter()
                ->FilteredItemIndex(index)).IsOptGroup())) // find the previous option
                {
                break;
                }
            }

            if ( index >= 0 )
                {
                SetCurrentItemIndexAndDraw( index );
                }
            else    // no previous option
                {
                if ( TopItemIndex() != 0 )
                    {
                    // show the invisible part
                    SetTopItemIndex( 0 );
                    DrawNow();
                    }
                }
            resp = EKeyWasConsumed;
            break;

        case EKeyLeftDownArrow:       // Southwest
        case EStdKeyDevice13:         //   : Extra KeyEvent supports diagonal event simulator wedge
        case EKeyDownArrow:           // South
        case EKeyRightDownArrow:      // Southeast
        case EStdKeyDevice12:         //   : Extra KeyEvent supports diagonal event simulator wedge
            {
            TInt count = 0;

            //Findbox is visible, accept only filtered items.
            //
            if ( ((CBrowserSelectElementModel*)Model())->Filter() )
                {
                count = ((CBrowserSelectElementModel*)Model())->Filter()
                    ->FilteredNumberOfItems();
                }

            for ( ;( ++index < count ); )
            {

            if ( !(iOptions.At(((CBrowserSelectElementModel*)Model())->Filter()
                ->FilteredItemIndex(index)).IsOptGroup()) ) // find the next option
                {
                break;
                }
            }

            if ( index < count )
                {
                SetCurrentItemIndexAndDraw( index );
                }
            resp = EKeyWasConsumed;
            }
            break;

        case EKeyOK:
        case EKeyEnter:
            {

            if ( iBrCtlSelectOptionType != ESelectTypeMultiple )
                {
                // select the current item
                View()->ClearSelection();
                View()->SelectItemL( index );
                resp = EKeyWasConsumed;
                // need to report the event because might be onpick
                ReportListBoxEventL
                    ( MEikListBoxObserver::EEventEnterKeyPressed );

                }
            else
                {
                if (index >= 0 && !(iOptions.At(((CBrowserSelectElementModel*)Model())->
                    Filter()->FilteredItemIndex(index)).IsOptGroup()))
                    {
                    resp = CEikListBox::OfferKeyEventL
                        ( aKeyEvent, aType );

                    ReportListBoxEventL
                        ( MEikListBoxObserver::EEventEnterKeyPressed );
                    }
                else
                    {
                    // Avoid to select optiongroup text
                    resp = EKeyWasConsumed;
                    }
                }
            }
            break;

        default:
            // workaround for autoselection problem
            // (Ok softkey selects focused item)
            if ( aKeyEvent.iCode != EKeyApplicationF )
                {
                resp = CEikListBox::OfferKeyEventL( aKeyEvent, aType );
                }
            else
                {
                resp = EKeyWasConsumed;
                }
        }

    return resp;
    }

// ----------------------------------------------------------------------------
// CBrowserSelectElementListBox::HandlePointerEventL
// ----------------------------------------------------------------------------
//
void CBrowserSelectElementListBox::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    CAknSingleGraphicPopupMenuStyleListBox::HandlePointerEventL(aPointerEvent);
    TInt index = CurrentItemIndex();
    iHasNotMoved = EFalse;
    switch (aPointerEvent.iType)
        {
        case TPointerEvent::EButton1Up:
            if ( iBrCtlSelectOptionType != ESelectTypeMultiple )
                {
                // select the current item
                View()->ClearSelection();
                View()->SelectItemL( index );
                ReportListBoxEventL( MEikListBoxObserver::EEventEnterKeyPressed );
                }
            else
                {
                if (index >= 0 && !(iOptions.At(((CBrowserSelectElementModel*)Model())->
                    Filter()->FilteredItemIndex(index)).IsOptGroup()))
                    {
                    View()->ToggleItemL(index);
                    ReportListBoxEventL( MEikListBoxObserver::EEventEnterKeyPressed );
                    }
                }
            break;
        default:
            break;
        }
    }

// ----------------------------------------------------------------------------
// CBrowserSelectElementListBox::FocusChanged
// ----------------------------------------------------------------------------
//
void CBrowserSelectElementListBox::FocusChanged( TDrawNow aDrawNow )
    {
    // Do nothing until the listbox is fully constructed
    // The dialogpage sets the focus before calling ConstructL
    if ( iView )
        {
        CEikListBox::FocusChanged( aDrawNow );
        }
    }

// ----------------------------------------------------------------------------
// CBrowserSelectElementListBox::SizeChanged
// ----------------------------------------------------------------------------
//
void CBrowserSelectElementListBox::SizeChanged()
    {
    CAknSingleGraphicPopupMenuStyleListBox::SizeChanged();
    AdjustHighlight();
    }

// ----------------------------------------------------------------------------
// CBrCtlSelectElementListBox::SetSelectionIndexesL
// ----------------------------------------------------------------------------
//
void CBrowserSelectElementListBox::SetSelectionIndexesL()
    {
    CListBoxView::CSelectionIndexArray* arrayOfSelectionIndexes;
    arrayOfSelectionIndexes = new (ELeave) CArrayFixFlat<TInt>(10);
    CleanupStack::PushL( arrayOfSelectionIndexes );
    for ( TInt ii = 0; ii < iOptions.Count(); ++ii )
        {
        if ( iOptions.At( ii ).IsSelected() || iOptions.At( ii ).IsOptGroup() )
            {
            arrayOfSelectionIndexes->AppendL( ii );
            }
        }
    CEikListBox::SetSelectionIndexesL( arrayOfSelectionIndexes );
    CleanupStack::PopAndDestroy(); // arrayOfSelectionIndexes
    AdjustHighlight();
    }


//-----------------------------------------------------------------------------
// CBrowserSelectElementListBox::HighlightSelectableItem
//-----------------------------------------------------------------------------
//
void CBrowserSelectElementListBox::HighlightSelectableItem()
    {
    AdjustHighlight();
    }


//-----------------------------------------------------------------------------
// CBrowserSelectElementListBox::AdjustHighlight
//-----------------------------------------------------------------------------
//
void CBrowserSelectElementListBox::AdjustHighlight()
    {
    const CListBoxView::CSelectionIndexArray* selection = SelectionIndexes();
    TInt count = Min( iOptions.Count(), Model()->NumberOfItems() );
    TInt i;
    TBool filterExist = EFalse;
    TBool notFound = ETrue;

    //at first time the highlight should be on the item
    //which was selected last time
    if ( iHasNotMoved )
        {
        for ( i = 0 ; i < count ; i++ )
            {
            if ( iOptions.At( i ).IsSelected() )
                {
                ScrollToMakeItemVisible( i );
                SetCurrentItemIndexAndDraw( i );
                break;
                }
            }
        return;
        }

    if ( ( (CBrowserSelectElementModel*)Model() )->Filter() )
        {
        filterExist = ETrue;
        count = ( (CBrowserSelectElementModel*) Model() )->
            Filter()->FilteredNumberOfItems();
        }

    for ( i = 0 ; i < count ; i++ )
        {
        TInt index = i;

        if ( filterExist )
            {
            index = ( (CBrowserSelectElementModel*) Model() )->
                                                    Filter()->
                                                    FilteredItemIndex( i );
            }

        TBrCtlSelectOptionData optdat = iOptions.At( index );

        if ( !optdat.IsOptGroup() && filterExist ) // find the next option
            {
            TPtrC data(optdat.Text());
            TBuf<KSearchFieldBuferLength> criteria;
            ( (CAknSearchField*)( ((CAknFilteredTextListBoxModel*)Model())
                        ->Filter()->FindBox() ) )->GetSearchText( criteria );

            if ( AknFind::IsFindMatch( data,criteria ) )
                {
                notFound = EFalse;
                break;
                }
            }
        }

    if ( notFound )
        {
        for ( i = 0; i < count; i++ )
            {
            if (iOptions.At( i ).IsSelected() )
                {
                ScrollToMakeItemVisible( i );
                SetCurrentItemIndexAndDraw( i );
                break;
                }
            }
        return;
        }

    if ( i < count)
        {
        ScrollToMakeItemVisible(i);
        SetCurrentItemIndexAndDraw( i );
        }
    else if ( selection->Count() ) // selected array except the optgrup becaue optg always selected
        {
        for ( i = 0 ; i < selection->Count() ; i++ )
            {
            if ( !iOptions.At( selection->At( i ) ).IsOptGroup() )
                {    // make visible (scroll ) and highlight it
                ScrollToMakeItemVisible( selection->At( i ) );
                SetCurrentItemIndexAndDraw( selection->At( i ) );
                break;
                }
            }
        }
    }
//-----------------------------------------------------------------------------
// CBrowserSelectElementListBox::HandleResourceChangeL
//-----------------------------------------------------------------------------
//
void CBrowserSelectElementListBox::HandleResourceChangeL( TInt aType )
    {
    if ( aType == KAknsMessageSkinChange )
        {
        CArrayPtr<CGulIcon>* array =
                            ItemDrawer()->ColumnData()->IconArray();

        array->ResetAndDestroy();
        delete array;

        STATIC_CAST( CBrowserSelectElementItemDrawer*,
                                            iItemDrawer )->LoadIconsL();
        }
    }

//  End of File
