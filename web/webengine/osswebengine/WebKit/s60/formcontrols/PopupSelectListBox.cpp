/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  PopupSelectListBox.cpp
*
*/

// INCLUDE FILES

#include <../bidi.h>
#include <eiktxlbm.h>
#include <eiklbi.h>
#include <gulbordr.h>
#include <eikfrlbd.h> //CFormattedCellListBoxData

#include "PopupSelectListBox.h"
#include "WebView.h"
#include "WebFepTextEditor.h"
#include "BrCtlDialogsProvider.h"

// CONSTANTS

const static TInt KItemSpacerInPixels = 2;

void PopupSelectListBox::ConstructL(WebView* aParent, TBool aMultipleSelection, const CFont* aFont)
    {
    m_parent = aParent;
    m_focused = EFalse;
    m_multipleSelection = aMultipleSelection;
    
    CTextListBoxModel* model=new(ELeave) CTextListBoxModel();
    iModel = model;
    model->ConstructL();
    
//    CFormattedCellListBoxData* listBoxData =  CFormattedCellListBoxData::NewL();
//    CFormattedCellListBoxItemDrawer* itemDrawer = new(ELeave) CFormattedCellListBoxItemDrawer((CTextListBoxModel*)iModel,aFont,listBoxData);
//    listBoxData->EnableMarqueeL(ETrue);
    CTextListItemDrawer* itemDrawer = new(ELeave) CTextListItemDrawer((CTextListBoxModel*)iModel,aFont);
    iItemDrawer = itemDrawer;
 
    iItemDrawer->SetDrawMark(m_multipleSelection);
    if (m_multipleSelection)
        {
        CEikListBox::ConstructL(aParent, EPaintedSelection);
        }
    else
        {
        CEikListBox::ConstructL(aParent,0);
        }
    SetBorder(TGulBorder::EDeepSunken);
    SetDimmed(EFalse);
    SetItemHeightL(aFont->HeightInPixels() + KItemSpacerInPixels);
    }
    
//
// PopupSelectListBox::UpdateContentL
// Updates the list
//
void PopupSelectListBox::UpdateContentL(const RPointerArray<TBrCtlSelectOptionData>& aOptions)
    {    
    if (m_itemArray)
        {
        m_itemArray->Reset();
        }
    else
        {
        m_itemArray = new (ELeave) CDesC16ArrayFlat(aOptions.Count()+1);
        }
    TInt selectedIdx(-1);
    for (TInt i=0; i<aOptions.Count(); i++)
        {
        TBrCtlSelectOptionData* option = aOptions[i];
        m_itemArray->AppendL(option->Text());
                
        if (option->IsSelected())
            {
            selectedIdx = i;
            View()->SelectItemL(selectedIdx);
            }
        }
    
    this->Model()->SetItemTextArray(m_itemArray);
    this->Model()->SetOwnershipType(ELbmDoesNotOwnItemArray);

    if (m_multipleSelection)
        {
        HandleItemAdditionL();
        }
    else
        {
        Reset();
        }
    
    HandleViewRectSizeChangeL();
    
    // Set the selected item
    if (selectedIdx > -1)
        {
        this->SetCurrentItemIndexAndDraw(selectedIdx);
        }

    DrawNow();
    }
//
//  Override ListBox SetFocus to prevent top item from being selected by default if list is multi-select
//
void PopupSelectListBox::SetFocus(TBool aFocus)
    {
    m_focused = aFocus;
    CEikListBox::SetFocus(aFocus);
    }

// Destructor
PopupSelectListBox::~PopupSelectListBox()
    {
    ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff,CEikScrollBarFrame::EOff);
    DrawNow();
    delete m_itemArray;
    }

void PopupSelectListBox::SetBackground(TRgb aColor)
    {
    View()->SetBackColor(aColor);
    iItemDrawer->SetBackColor(aColor);
    }

TCoeInputCapabilities PopupSelectListBox::InputCapabilities() const
    {
    return TCoeInputCapabilities( TCoeInputCapabilities::ENavigation);//, m_parent->fepTextEditor(), NULL, TUid::Uid(0x100056de), m_parent->fepTextEditor() );
    }

//  End of File
