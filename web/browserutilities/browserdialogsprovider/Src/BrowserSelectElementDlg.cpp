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
#include "BrowserSelectElementDlg.h"
#include "BrowserSelectElementListBox.h"
#include "BrowserSelectElementModel.h"
#include "BrowserDialogsProviderAsyncExit.h"

#include <aknsfld.h>
#include <aknkeys.h>
#include <eikcapc.h>

// CONSTANTS
#include "BrowserDialogsProviderConstants.h"

// ================= MEMBER FUNCTIONS =========================================

// C++ default constructor can NOT contain any code, that
// might leave.
//
CBrowserSelectElementDlg::CBrowserSelectElementDlg ( 
                const TBrCtlSelectOptionType aBrCtlSelectOptionType,
                CArrayFix<TBrCtlSelectOptionData>& aOptionsOrg ) :
                    iContentValid( ETrue ), 
                    iBrCtlSelectOptionType ( aBrCtlSelectOptionType ),
                    iOptionsOrg( aOptionsOrg )
    {
    }

CBrowserSelectElementDlg::~CBrowserSelectElementDlg()
    {   
    delete iListBox;
    delete iHistoryList;
    delete iAsyncExit;
    }
//-----------------------------------------------------------------------------
// CBrowserSelectElementDlg::NewL
//-----------------------------------------------------------------------------
//
CBrowserSelectElementDlg* CBrowserSelectElementDlg::NewL( 
                        const TDesC& aTitle, 
                        const TBrCtlSelectOptionType aBrCtlSelectOptionType,
                        CArrayFix<TBrCtlSelectOptionData>& aOptionsOrg )
    {
    CBrowserSelectElementDlg* self = new ( ELeave ) CBrowserSelectElementDlg( 
                                                        aBrCtlSelectOptionType,
                                                        aOptionsOrg );
    CleanupStack::PushL( self );

    self->ConstructL( aTitle );

    CleanupStack::Pop();

    return self;
    }

//-----------------------------------------------------------------------------
// CBrowserSelectElementDlg::ConstructL
//-----------------------------------------------------------------------------
//
void CBrowserSelectElementDlg::ConstructL( const TDesC& aTitle )
    {
    // Create the active object for this object
    iAsyncExit = CBrowserDialogsProviderAsyncExit::NewL( *this );
    // Resource
    iResource = R_AVKON_SOFTKEYS_OK_CANCEL__MARK;
    // Enable find pane
    TBool bIsFindPaneEnabled = (iBrCtlSelectOptionType & 0x100) ? ETrue : EFalse ;
    
    if(bIsFindPaneEnabled)
        {
        iBrCtlSelectOptionType = (TBrCtlSelectOptionType )(iBrCtlSelectOptionType & 0xEFF) ;
        }
    
    if (iBrCtlSelectOptionType == ESelectTypeSingle )
        {
        // It was decided that Radio Buttons add little value and unnecessary 
        // complexity to the code, ESelectTypeSingle is no longer supported.
        iBrCtlSelectOptionType = ESelectTypeNone;
        }
    else if ( iBrCtlSelectOptionType == ESelectTypeOkOnly ) 
        {
        iResource = R_AVKON_SOFTKEYS_OK_EMPTY__OK;
        }

    // Construct listbox and popup
    if ( iBrCtlSelectOptionType != ESelectTypeNone )
        {
        iListBox =
            new ( ELeave ) CBrowserSelectElementListBox(
                                                    iBrCtlSelectOptionType, 
                                                    iOptionsOrg );
        CAknPopupList::ConstructL( iListBox, iResource, 
                                                AknPopupLayouts::EMenuWindow );
        iListBox->ConstructL( *this );
        iListBox->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
        iListBox->CreateScrollBarFrameL( ETrue );
        iListBox->ScrollBarFrame()->SetScrollBarVisibilityL
                    ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
        iListBox->SetObserver( this );

        SetTitleL( aTitle );
        EnableFind();
        STATIC_CAST ( CBrowserSelectElementModel*,
                            iListBox->Model() )->Filter()->SetObserver( this );
        }
    else
        {
        CDesCArrayFlat* items = 
                        new ( ELeave ) CDesCArrayFlat ( KGranularityMedium );

        CleanupStack::PushL(items);
        iResource = R_AVKON_SOFTKEYS_SELECT_CANCEL__SELECT;
        for ( TInt i = 0; i<iOptionsOrg.Count(); i++ )
            {
            items->AppendL( iOptionsOrg.At(i).Text() );
            }

        // create listbox
        iHistoryList = new ( ELeave ) CAknSinglePopupMenuStyleListBox;

        // create popup
        CAknPopupList::ConstructL( iHistoryList, iResource, AknPopupLayouts::EMenuWindow );
                        
        iHistoryList->ConstructL( this, EAknListBoxSelectionList );
        iHistoryList->ItemDrawer()->ColumnData()->EnableMarqueeL( ETrue );
        iHistoryList->CreateScrollBarFrameL( ETrue );
        iHistoryList->ScrollBarFrame()->SetScrollBarVisibilityL
                    ( CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto );
        
        CTextListBoxModel* model = iHistoryList->Model();
        TBool hasItems = iOptionsOrg.Count();
        if ( hasItems  ) 
            {
            model->SetItemTextArray( items );
            model->SetOwnershipType( ELbmOwnsItemArray );
            CleanupStack::Pop( items );//ownership is taken by model        
            }
        else
          {
          CleanupStack::PopAndDestroy( items ); 
          }
        
        iHistoryList->HandleItemAdditionL();
        if ( hasItems )
          {
          iHistoryList->SetCurrentItemIndex( 0 );
          }
        SetTitleL( aTitle );
        if (bIsFindPaneEnabled)
          {
          EnableFind();
          STATIC_CAST( CAknFilteredTextListBoxModel*,
                    ListBox()->Model())->Filter()->SetObserver( this );
          }        
        }  
    
    }

//-----------------------------------------------------------------------------
// CBrowserSelectElementDlg::HandleListBoxEventL
//-----------------------------------------------------------------------------
//
void CBrowserSelectElementDlg::HandleListBoxEventL
                            ( CEikListBox* aListBox, TListBoxEvent aEventType )
    {
    TBool penSelect = EFalse;
    
    if(AknLayoutUtils::PenEnabled())
        {
        if(aEventType == EEventItemDoubleClicked)
            {
            penSelect = ETrue;
            }
        }
    
    if ( (aEventType == EEventEnterKeyPressed || penSelect ) &&
         ( ( iBrCtlSelectOptionType != ESelectTypeMultiple ) || 
                iOptionsOrg.At( aListBox->CurrentItemIndex() ).HasOnPick() ) 
                                                )
        {
        if (!iAsyncExit->IsActive())
            iAsyncExit->Start();
        }
    }

//-----------------------------------------------------------------------------
// CBrowserSelectElementDlg::HandleControlEventL
//-----------------------------------------------------------------------------
//
void CBrowserSelectElementDlg::HandleControlEventL
    ( CCoeControl* aControl, TCoeEvent aEventType )
    {
    if ( ( aControl == ListBox() &&
                    aEventType == MCoeControlObserver::EEventRequestCancel ) )
        {
        AttemptExitL( EFalse );
        }   
    else if ( aControl == ((CCoeControl*)FindBox()) && 
        aEventType == MCoeControlObserver::EEventStateChanged )
        {        
        CEikButtonGroupContainer * cbaGroup = CEikButtonGroupContainer::Current();
        /*Some text has been written to findbox, in filtered selectioncase 
          highlight selectable item. */ 
        if ( iBrCtlSelectOptionType != ESelectTypeNone)
            {
            STATIC_CAST ( CBrowserSelectElementListBox*,
                                        ListBox() )->HighlightSelectableItem();
            }
        if ( iBrCtlSelectOptionType == ESelectTypeOkOnly ) return;            
        if ( STATIC_CAST( CAknFilteredTextListBoxModel*, ListBox()->Model())->Filter()->FilteredNumberOfItems() == 0 ) 
            {
            cbaGroup->SetCommandSetL(R_AVKON_SOFTKEYS_CANCEL); 
            cbaGroup->DrawNow();             
            } 
        else  
            {
            cbaGroup->SetCommandSetL(iResource); 
            cbaGroup->DrawNow(); 
            }        
        }
    }

//-----------------------------------------------------------------------------
// CBrowserSelectElementDlg::AttemptExitL
//-----------------------------------------------------------------------------
//
void CBrowserSelectElementDlg::AttemptExitL( TBool aAccept )
    {
    // This block comes from aknPopup.cpp.
    // See CAknPopupList::RemoveFindFiltering()
    // It had to be duplicated here because the method was private.
    if ( FindBox() )
        { 
        // this removes filtering from popup list so that listboxes 
        // state is valid for application to read.
        TInt currentItemIndex = ListBox()->CurrentItemIndex();
        TInt realCurrentItemIndex = 
            currentItemIndex >= 0 ? 
            STATIC_CAST( CAknFilteredTextListBoxModel*,
            ListBox()->Model())->Filter()->FilteredItemIndex(currentItemIndex) 
            : KErrNotFound;

        FindBox()->SetSearchTextL( KNullDesC );
        STATIC_CAST( CAknFilteredTextListBoxModel*,
                        ListBox()->Model() )->Filter()->HandleOfferkeyEventL();

        if ( realCurrentItemIndex >= 0 )
            {
            ListBox()->SetCurrentItemIndex(realCurrentItemIndex);
            }
        }

    if( aAccept && iContentValid )
        {
        StoreSelections();
        }
    
    CAknPopupList::AttemptExitL( aAccept );
    }

//-----------------------------------------------------------------------------
// CBrowserSelectElementDlg::DestroyPopupL()
//-----------------------------------------------------------------------------
//
void CBrowserSelectElementDlg::DestroyPopupL()
    {
    iContentValid = ETrue;
    AttemptExitL( ETrue );
    }


//-----------------------------------------------------------------------------
// CBrowserSelectElementDlg::StoreSelections
//-----------------------------------------------------------------------------
//
void CBrowserSelectElementDlg::StoreSelections()
    {
    if( iContentValid )
        {
        // unset selection
        for ( TInt ii = 0; ii < iOptionsOrg.Count(); ++ii )
            {
            iOptionsOrg.At( ii ).SetIsSelected( EFalse );
            }

        // set user selection
        if ( iBrCtlSelectOptionType == ESelectTypeMultiple )
            {            
            const CArrayFix<TInt>* selection;
            if ( iBrCtlSelectOptionType == ESelectTypeNone )
                {
                selection = iHistoryList->SelectionIndexes();
                }
            else
                {
                selection = iListBox->SelectionIndexes();
                }
            
            for ( TInt ii=0; ii<selection->Count(); ++ii )
                {
                iOptionsOrg.At( selection->At( ii ) ).SetIsSelected(ETrue);
                }
            }
        else
            {
            TInt currIndex;
            if ( iBrCtlSelectOptionType == ESelectTypeNone )
                {
                currIndex = iHistoryList->CurrentItemIndex();
                }
            else
                {
                currIndex = iListBox->CurrentItemIndex();
                }
            if ( currIndex >= 0 )
                {
                iOptionsOrg.At( currIndex ).SetIsSelected( ETrue );
                }
            }
        }
    }
//  End of File
