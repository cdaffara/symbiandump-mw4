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


#include "config.h"
#include "Pasteboard.h"
#include "Event.h"
#include "DocumentFragment.h"
#include "Editor.h"
#include "markup.h"

namespace WebCore {

Pasteboard::Pasteboard()
{
}

Pasteboard* Pasteboard::generalPasteboard()
{
    static Pasteboard* pasteboard = new Pasteboard();
    return pasteboard;
}

void Pasteboard::writeSelection(Range* selectedRange, bool canSmartCopyOrDelete, Frame* frame)
{
}

bool Pasteboard::canSmartReplace()
{
    return false;
}

String Pasteboard::plainText(Frame* frame)
{
    return String();
}

PassRefPtr<DocumentFragment> Pasteboard::documentFragment(Frame* frame, PassRefPtr<Range> context,
                                                          bool allowPlainText, bool& chosePlainText)
{
    return 0;
}

void Pasteboard::writeURL(const KURL&, const String&, Frame*)
{
}

void Pasteboard::writeImage(Node*, const KURL&, const String& )
{
}

void Pasteboard::clear()
{
}

}
