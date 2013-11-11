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
* Description:  WebFormFillPopup.cpp
*
*/

// INCLUDE FILES
#include <../bidi.h>
#include "WebFormFillPopup.h"
#include "WebView.h"
#include "WebFrame.h"
#include "WebFrameView.h"
#include "BrCtlDefs.h"
#include "PopupSelectListBox.h"
#include "BrCtlDialogsProvider.h"
#include "WebFepTextEditor.h"
#include "FormFillCallback.h"

#include <aknenv.h>
#include <coemain.h>
#include <eikpanic.h>
#include <aknedsts.h>
#include <AknUtils.h>

#include "eikon.hrh"


static const TInt KMaxNumToShow = 6; // max number of list items to show
static const TInt KInitArraySize = 10; // initial array size
static const TInt KBorderSize = 1; // List Box Border size
static const TInt KListBoxPadding = 12;
static const TInt KListBoxMinWidth = 100;
static const TInt KListBoxMargin = 6;

WebFormFillPopup* WebFormFillPopup::NewL(WebView* parent, CFont* font, MFormFillCallback* callback)
{
    WebFormFillPopup* self = new (ELeave) WebFormFillPopup(parent, font, callback);
    CleanupStack::PushL(self);
    self->ConstructL(font);
    CleanupStack::Pop();
    return self;
}

WebFormFillPopup::WebFormFillPopup(WebView* parent, CFont* font, MFormFillCallback* callback) :
m_parent(parent), m_font(font), m_callback(callback)
{
}

WebFormFillPopup::~WebFormFillPopup()
{
    MakeVisible(EFalse);
    m_parent->setFormFillPopup(NULL);
    delete m_listBox;
}

void WebFormFillPopup::ConstructL(CFont* aFont)
{
    m_parent->setFormFillPopup(this);
    ConstructListBoxL(aFont);
    ClearData();
}

void WebFormFillPopup::ConstructListBoxL(CFont* aFont)
{
    m_listBox = new (ELeave) PopupSelectListBox();
    m_listBox->ConstructL(m_parent, EFalse, aFont);

    m_listBox->CreateScrollBarFrameL(ETrue);
    m_listBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);

    m_listBox->SetListBoxObserver(this);
    m_listBox->SetBorder(TGulBorder::ESingleGray);
    m_listBox->SetBackground( TRgb(212, 212, 212) );

    m_listBox->SetFocus(ETrue);

    //iEikonEnv->EikAppUi()->AddToStackL(this);
    m_listBox->ActivateL();
    m_listBox->SetRect(TRect(TPoint(0,0), TSize(10,10)));
    MakeVisible(EFalse);
}

void WebFormFillPopup::addRow(const TPtrC& row)
{
    TBrCtlSelectOptionData* data = new TBrCtlSelectOptionData(row, EFalse, EFalse, EFalse);
    m_data.Append(data);
}

void WebFormFillPopup::invalidate()
{
    MakeVisible(ETrue);
    positionListBox();

    // now update the listbox
    m_listBox->UpdateContentL(m_data);
}

void WebFormFillPopup::ClearData()
{
    m_data.ResetAndDestroy();
    MakeVisible(EFalse);
}

void WebFormFillPopup::positionListBox()
{
    TRect r = m_parent->Rect();
    r.Shrink(KListBoxMargin, KListBoxMargin);

    // size
    TInt width = 0;
    for (TInt i=0; i<m_data.Count(); ++i)
    {
        TInt w = m_font->MeasureText(m_data[i]->Text());
        width = width > w ? width : w;
    }
    width += KListBoxPadding;

    // clamp width to [100, screen_width]
    width = width > KListBoxMinWidth ? width : KListBoxMinWidth;
    width = width < r.Width() ? width : r.Width();

    // only change the rect when it gets wider or position changed, to avoid
    // unnecessary flicking.
    TInt x = m_pos.iX;
    TInt y = m_pos.iY;
    if (width > m_listBox->Rect().Width() || m_posChanged)
    {
        // x position
        if (x + width > r.Width() )
            x = r.Width() - width;
        x = x > 0 ? x : 0;

        // y position
        TInt height = m_font->HeightInPixels()*KMaxNumToShow;
            y += m_inputHeight;

        SetRect(TRect(TPoint(x, y), TSize(width, height)));
    }
}

TKeyResponse WebFormFillPopup::OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TKeyResponse response = EKeyWasNotConsumed;

    // We only pass ArrowUp, ArrowDown and Activate to listbox, to prevent Listbox
    // eating up all the keyevents.
    switch (aKeyEvent.iCode)
        {

        case EKeyUpArrow:             // North
        case EKeyDownArrow:           // South
            response = m_listBox->OfferKeyEventL(aKeyEvent, aType);
            break;

        case EKeyDevice3:
        case EKeyEnter:
            response = m_listBox->OfferKeyEventL(aKeyEvent, aType);
            m_callback->cancelPopup();
            break;

        case EKeyLeftArrow:           // West
        case EKeyRightArrow:          // East
            MakeVisible(EFalse);
            response = m_parent->OfferKeyEventL(aKeyEvent, aType);
            m_callback->cancelPopup();
            break;

// All of the diagonal KeyEvents are allowed to flow through the "default" case...
//
//      case EKeyRightUpArrow:        // Northeast
//      case EStdKeyDevice11:         //   : Extra KeyEvent supports diagonal event simulator wedge
//      case EKeyRightDownArrow:      // Southeast
//      case EStdKeyDevice12:         //   : Extra KeyEvent supports diagonal event simulator wedge
//      case EKeyLeftDownArrow:       // Southwest
//      case EStdKeyDevice13:         //   : Extra KeyEvent supports diagonal event simulator wedge
//      case EKeyLeftUpArrow:         // Northwest
//      case EStdKeyDevice10:         //   : Extra KeyEvent supports diagonal event simulator wedge
        default:
            response = m_parent->OfferKeyEventL(aKeyEvent, aType);
            break;

    }

    return response;
}

// Handle the key event from touch device
TKeyResponse WebFormFillPopup::HandleKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType)
{
    TKeyResponse response = EKeyWasNotConsumed;

    // We only pass ArrowUp, ArrowDown and Activate to listbox, to prevent Listbox
    // eating up all the keyevents.
    switch (aKeyEvent.iCode)
        {

        case EKeyUpArrow:             // North
        case EKeyDownArrow:           // South
            response = m_listBox->OfferKeyEventL(aKeyEvent, aType);
            break;

        case EKeyDevice3:
        case EKeyEnter:
            response = m_listBox->OfferKeyEventL(aKeyEvent, aType);
            m_callback->cancelPopup();
            break;

        case EKeyLeftArrow:           // West
        case EKeyRightArrow:          // East
            MakeVisible(EFalse);
            m_callback->cancelPopup();
            response = EKeyWasConsumed;
            break;

        // All of the diagonal KeyEvents are to be ignored and consumed
        case EKeyRightUpArrow:        // Northeast
        case EStdKeyDevice11:         //   : Extra KeyEvent supports diagonal event simulator wedge
        case EKeyRightDownArrow:      // Southeast
        case EStdKeyDevice12:         //   : Extra KeyEvent supports diagonal event simulator wedge
        case EKeyLeftDownArrow:       // Southwest
        case EStdKeyDevice13:         //   : Extra KeyEvent supports diagonal event simulator wedge
        case EKeyLeftUpArrow:         // Northwest
        case EStdKeyDevice10:         //   : Extra KeyEvent supports diagonal event simulator wedge
            response = EKeyWasConsumed;
            break;

        // Allow vkb keyevents to be handled later.
        default:
            response = EKeyWasNotConsumed;
            break;

    }

    return response;
}

CCoeControl* WebFormFillPopup::ComponentControl( TInt /*aIndex*/ ) const
{
    return m_listBox;
}

TInt WebFormFillPopup::CountComponentControls() const
{
    return 1;
}


void WebFormFillPopup::SizeChanged()
{
    m_listBox->SetRect( Rect() );
}

void WebFormFillPopup::HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType)
{
    if (aListBox != m_listBox)
        return;

    if (aEventType == EEventItemDoubleClicked || aEventType == EEventEnterKeyPressed)
    {
        // get the selected item from listbox
        m_listBox->View()->UpdateSelectionL(CListBoxView::ESingleSelection);
        const CListBoxView::CSelectionIndexArray* selected = m_listBox->SelectionIndexes();
        if (m_listBox->IsFocused())
        {
            MakeVisible(EFalse);
            m_callback->autoComplete(m_data[selected->At(0)]->Text());
        }
    }
    else if (aEventType == EEventPenDownOnItem) {
        if (!m_listBox->IsFocused()) {
            m_listBox->SetFocus(ETrue); // highlight first item
            m_listBox->DrawNow();
        }
    }
}

TCoeInputCapabilities WebFormFillPopup::InputCapabilities() const
{
    // tricky!!! we need to retain T9 capabilities of WebKitView.
    return m_parent->fepTextEditor()->InputCapabilities();
}

void WebFormFillPopup::clear()
{
    ClearData();
}

void WebFormFillPopup::setLocationHintInDoc(const TRect& r, WebCore::Frame* frame)
{    
    if (!frame)
        return;
    WebFrame* webFrame = kit(frame);

    // now lets reverse back to the root frame to figure out the screen position
    TPoint pt = webFrame->frameView()->frameCoordsInViewCoords(r.iTl);
    m_posChanged = !(pt == m_pos);

    m_pos = pt;
    m_inputHeight = r.Height();
}

void WebFormFillPopup::MakeVisible(TBool aVisible)
{
    if (aVisible == IsVisible())
        return;

    CCoeControl::MakeVisible(aVisible);

    if ( aVisible )
    {
        m_listBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EAuto);
        m_listBox->MakeVisible(ETrue);
        // for touch we want to prevent list from focusing and closing the VKB
        if (!AknLayoutUtils::PenEnabled()){
            iEikonEnv->EikAppUi()->AddToStackL(this);
        }
    }
    else
    {
        m_listBox->ScrollBarFrame()->SetScrollBarVisibilityL(CEikScrollBarFrame::EOff, CEikScrollBarFrame::EOff);
        m_listBox->MakeVisible(EFalse);
        iEikonEnv->EikAppUi()->RemoveFromStack(this);
    }
}

void WebFormFillPopup::reDraw()
{
    m_listBox->DrawNow();
}

void WebFormFillPopup::handleCommandL(int command)
{
    switch (command)
    {
    case TBrCtlDefs::ECommandCancel:
        MakeVisible(EFalse);
        m_callback->cancelPopup();
        break;
    }
}

void WebFormFillPopup::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    if (Rect().Contains(aPointerEvent.iPosition)) {
        m_listBox->HandlePointerEventL( aPointerEvent );
    }
    else {
        m_callback->cancelPopup();
    }
}

//  End of File
