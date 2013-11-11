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
#ifndef __RENDERTHEMESYMBIAN_H__
#define __RENDERTHEMESYMBIAN_H__

#include "config.h"

#include "RenderTheme.h"
#include "Event.h"
#include "Document.h"
#include "Image.h"
#include "GraphicsContext.h"
#include <../bidi.h>
#include <eikenv.h>
#include "RenderBox.h"
#include <bitmaptransforms.h>
#include "WebCoreGraphicsContext.h"
#include "MaskedBitmap.h"
#include "BitmapImage.h"
#include "WebCoreWidget.h"

// implement Symbian theme here

namespace WebCore {

enum {
    ECheckBoxOn,
    ECheckBoxOff,
    ERadioOn,
    ERadioOff,
    ESelectArrow
};

class RenderThemeSymbian : public RenderTheme
{
public:
    RenderThemeSymbian();
    virtual ~RenderThemeSymbian();
    // A method asking if the theme's controls actually care about redrawing when hovered.
    bool supportsHover(const RenderStyle*) const { return true; }
    
    void setCheckboxSize(RenderStyle*) const;    
    void setRadioSize(RenderStyle*) const;

    void adjustButtonStyle(CSSStyleSelector*, RenderStyle*, Element*) const;
    void adjustTextFieldStyle(CSSStyleSelector*, RenderStyle*, Element*) const;
    void adjustMenuListStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const;
    void adjustMenuListButtonStyle(CSSStyleSelector* selector, RenderStyle* style, Element* e) const;
    
    bool paintCheckbox(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r);
    bool paintButton(RenderObject*, const RenderObject::PaintInfo&, const IntRect&);
    bool paintMenuListButton(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r);
    bool paintMenuList(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r);
    bool paintRadio(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r);
    bool paintTextField(RenderObject*, const RenderObject::PaintInfo&, const IntRect&);
    bool paintTextArea(RenderObject*, const RenderObject::PaintInfo&, const IntRect&);
    

    bool isControlStyled(const RenderStyle*, const BorderData&,
                                 const BackgroundLayer&, const Color&) const;
    bool controlSupportsTints(const RenderObject*) const;
    void systemFont(int propId, FontDescription&) const;
    
    Color platformActiveSelectionBackgroundColor() const; 
    Color platformInactiveSelectionBackgroundColor() const;
    Color platformActiveSelectionForegroundColor() const;
    Color platformInactiveSelectionForegroundColor() const;
    
    void addIntrinsicMargins(RenderStyle*) const;
    void close();
    bool supportsFocus(EAppearance) const;
    void paintButtonDecorations(RenderObject* o, const RenderObject::PaintInfo& i, const IntRect& r);

    bool supportsFocusRing(const RenderStyle*) const;
    void scaleImage(int type, int scalingFactor);
    void scaleImageL(int type, int scalingFactor);

    void cancel();
    void run();

    Image* m_checkBoxOn;
    Image* m_checkBoxOff;
    Image* m_scaledCheckBoxOn;
    Image* m_scaledCheckBoxOff;
    int m_scalingForCheckBoxOn;
    int m_scalingForCheckBoxOff;

    Image* m_radioButtonOn;
    Image* m_radioButtonOff;
    Image* m_scaledRadioOn;
    Image* m_scaledRadioOff;
    int m_scalingForRadioOn;
    int m_scalingForRadioOff;

    Image* m_selectArrow;
    Image* m_scaledSelectArrow;
    int m_scalingForSelectArrow;

    CBitmapScaler* m_bitmapScaler;
    CActiveSchedulerWait* m_asw;
};


class SyncScaler : public CActive
{
public:
    SyncScaler();
    ~SyncScaler();
    void init();
    void RunL();
    void DoCancel();
    TInt RunError(TInt aError);
    int m_error;
};

}
#endif// !__RENDERTHEMESYMBIAN_H__
