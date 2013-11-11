/*
 * This file is part of the popup menu implementation for <select> elements in WebCore.
 *
 * Copyright (C) 2006 Apple Computer, Inc.
 * Copyright (C) 2007 Nokia Inc. 
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Library General Public
 * License as published by the Free Software Foundation; either
 * version 2 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Library General Public License for more details.
 *
 * You should have received a copy of the GNU Library General Public License
 * along with this library; see the file COPYING.LIB.  If not, write to
 * the Free Software Foundation, Inc., 59 Temple Place - Suite 330,
 * Boston, MA 02111-1307, USA.
 */

#include "config.h"
#include "../../bidi.h"
#include "PopupMenu.h"

#include "Frame.h"
#include "Page.h"
#include "FrameLoader.h"
#include "FrameView.h"
#include "RenderMenuList.h"
#include "WebFrame.h"
#include "WebView.h"
#include "BrCtl.h"

#include <e32base.h>
#include <BrCtlDialogsProvider.h>

const TInt KDefaultSize(1);
namespace WebCore {

static void ResetAndDestroy(TAny *aPtr);
PopupMenu::PopupMenu(PopupMenuClient* client) : m_popupClient(client)
{
}
void ResetAndDestroy(TAny *aPtr)
{
    RPointerArray<HBufC>* items = (RPointerArray<HBufC>*) aPtr;
    items->ResetAndDestroy();
}
void PopupMenu::show(const IntRect&, FrameView* v, int index)
{
    TRAP_IGNORE(showL(v, index));    
}

void PopupMenu::showL(FrameView* v, int index)
{
    if (v->frame()->page()->mainFrame()->loader()->provisionalDocumentLoader()) {
        return;
    }
    WebView* wv = kit(v->frame()->page());
    MBrCtlDialogsProvider* dialogs = wv->brCtl()->brCtlDialogsProvider();
    int size = client()->listSize();
    CArrayFix<TBrCtlSelectOptionData>* options = 
            new CArrayFixFlat<TBrCtlSelectOptionData>((size>0) ? size : KDefaultSize);
    RPointerArray<HBufC> items((size>0) ? size : KDefaultSize);
    CleanupStack::PushL(TCleanupItem(&ResetAndDestroy,&items));
    
    for (int i = 0; i < size; i++) {
        // better separator
        if (client()->itemIsSeparator(i)) {
            TBrCtlSelectOptionData data(_L("----------"), false, false, false);
            options->AppendL(data);
        }
        else {
            HBufC16* itemStr = client()->itemText(i).des().AllocL();
            CleanupStack::PushL(itemStr);
            if (itemStr != NULL) {
                TBrCtlSelectOptionData data(*itemStr, i==client()->selectedIndex(), false, client()->itemIsEnabled(i));
                options->AppendL(data);
                items.AppendL(itemStr);
            }
            CleanupStack::Pop();
        }
    }
    
    
    dialogs->DialogSelectOptionL(KNullDesC(), (TBrCtlSelectOptionType)(ESelectTypeSingle | ESelectTypeWithFindPane), *options);

    int newIndex = index;
    CleanupStack::PopAndDestroy(&items);

        int count = options->Count();
        for (newIndex = 0 ; newIndex < count && !options->At(newIndex).IsSelected(); newIndex++) {}
    if (newIndex == count) {
            newIndex = index;
    }
    m_popupClient->hidePopup();

    if (index != newIndex && newIndex >= 0) {
        m_popupClient->valueChanged(newIndex);
    }
    delete options;
}

void PopupMenu::hide()
{
    // impossible
}

bool PopupMenu::itemWritingDirectionIsNatural()
{
    return true;
}

// PopupMenu
void PopupMenu::updateFromElement()
{
}

PopupMenu::~PopupMenu()
{
    
}


}
