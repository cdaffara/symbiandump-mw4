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
#include "FrameView.h"

#include "Document.h"
#include "FrameSymbian.h"
#include "WebCoreFrameBridge.h"
#include "RenderObject.h"

namespace WebCore {


void FrameView::updateBorder()
{
    Symbian(m_frame.get())->bridge()->Callback().SetHasBorder( hasBorder() );
}

bool FrameView::passMousePressEventToSubframe(MouseEventWithHitTestResults& mev, Frame* subframe)
{
    return Symbian(m_frame.get())->passSubframeEventToSubframe(mev, subframe);
}

bool FrameView::passMouseMoveEventToSubframe(MouseEventWithHitTestResults& mev, Frame* subframe)
{
    return Symbian(m_frame.get())->passSubframeEventToSubframe(mev, subframe);
}

bool FrameView::passMouseReleaseEventToSubframe(MouseEventWithHitTestResults& mev, Frame* subframe)
{
    return Symbian(m_frame.get())->passSubframeEventToSubframe(mev, subframe);
}

bool FrameView::passWheelEventToSubframe(PlatformWheelEvent&, Frame* subframe)
{
    return Symbian(m_frame.get())->passWheelEventToWidget(subframe->view());
}

bool FrameView::passMousePressEventToScrollbar(MouseEventWithHitTestResults& mev, PlatformScrollbar* scrollbar)
{
    return true;//Symbian(m_frame.get())->passMouseDownEventToWidget(scrollbar);
}

}
