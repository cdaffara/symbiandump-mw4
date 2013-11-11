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


#ifndef __WEBFORMFILLPOPUP_H__
#define __WEBFORMFILLPOPUP_H__

//  INCLUDES
#include "FormFillPopup.h"
#include <fepbase.h>
#include <eikbctrl.h>
#include <eiklbo.h>

class WebView;
class TBrCtlSelectOptionData;
class PopupSelectListBox;
class MFormFillCallback;

namespace WebCore
{
    class Frame;
}

class WebFormFillPopup : public MFormFillPopup,
                     public CEikBorderedControl,
                     public MEikListBoxObserver
{
public:
    static WebFormFillPopup* NewL(WebView* parent, CFont* font, MFormFillCallback* callback);
    virtual ~WebFormFillPopup();

    // from MFormFillPopup
    void addRow(const TPtrC&);
    void invalidate();
    void clear();
    void setLocationHintInDoc(const TRect& r, WebCore::Frame* frame);

    // from CEikBorderedControl
    TCoeInputCapabilities InputCapabilities() const;
    TKeyResponse OfferKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );
    TInt CountComponentControls() const;
    CCoeControl* ComponentControl( TInt aIndex )  const;
    void SizeChanged();
    void MakeVisible(TBool aVisible);
    void HandlePointerEventL(const TPointerEvent& aPointerEvent);
    TKeyResponse HandleKeyEventL( const TKeyEvent& aKeyEvent, TEventCode aType );

    // from MEikListBoxObserver
    void HandleListBoxEventL(CEikListBox* aListBox, TListBoxEvent aEventType);

    // other methods
    void positionListBox();

    // called by webkitview when main view is updated
    void reDraw();
    void handleCommandL(int command);

private:
    WebFormFillPopup(WebView* parent, CFont* font, MFormFillCallback* callback);
    void ConstructL(CFont* aFont);
    void ConstructListBoxL(CFont* aFont);
    void ClearData();

    PopupSelectListBox*         m_listBox;
    WebView*                    m_parent;
    RPointerArray<TBrCtlSelectOptionData>  m_data;
    TPoint                      m_pos;
    bool                        m_posChanged;
    int                         m_inputHeight;
    CFont*                      m_font;
    MFormFillCallback*          m_callback;
};

#endif // __WEBFORMFILLPOPUP_H__
