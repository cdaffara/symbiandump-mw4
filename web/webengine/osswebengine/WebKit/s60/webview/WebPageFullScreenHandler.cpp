/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies).
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


// INCLUDE FILES
#include <../bidi.h>
#include <aknappui.h>
#include <AknUtils.h>
#include "StaticObjectsContainer.h"
#include "WebCannedImages.h"
#include "WebPageFullScreenHandler.h"
#include "WebView.h"
#include "BrCtl.h"

using namespace WebCore;

const int KFullScreenButtonHeight = 60;
const int KFullScreenButtonWidth = 60;
const int KFullScreenButtonBuff  = 5;

// -----------------------------------------------------------------------------
// WebPageFullScreenHandler::NewL
// The two-phase Symbian constructor
// -----------------------------------------------------------------------------
//
WebPageFullScreenHandler* WebPageFullScreenHandler::NewL(WebView& webView)
{
    WebPageFullScreenHandler* self = new (ELeave) WebPageFullScreenHandler( webView );
    CleanupStack::PushL(self);
    self->ConstructL();
    CleanupStack::Pop(); //self
    return self;
}

// -----------------------------------------------------------------------------
// WebPageFullScreenHandler::WebPointerEventHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
WebPageFullScreenHandler::WebPageFullScreenHandler(WebView& webView) : m_webView(&webView)
{
}

// -----------------------------------------------------------------------------
// WebPageFullScreenHandler::constructL
// The constructor that can contain code that might leave.
// -----------------------------------------------------------------------------
//
void WebPageFullScreenHandler::ConstructL()
{
    m_buttonIcon = StaticObjectsContainer::instance()->webCannedImages()->getImage(WebCannedImages::EImageEscFullScreen);
    TPoint pos = CalculatePosition();
    BaseConstructL(m_webView, pos, m_buttonIcon.m_img, m_buttonIcon.m_msk, ETrue);
    Hide();   
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
//
WebPageFullScreenHandler::~WebPageFullScreenHandler()
{
}


TPoint WebPageFullScreenHandler::CalculatePosition()
{
    TPoint pos = m_webView->Rect().iBr;
    TSize iconSize = m_buttonIcon.m_img->SizeInPixels();
    
    pos -= iconSize;
    pos -= TPoint(KFullScreenButtonBuff, KFullScreenButtonBuff);
    //pos -= m_webView->PositionRelativeToScreen();
    return pos;
}

//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::showEscBtnL
// Draws the full screen button on the screen
//-------------------------------------------------------------------------------

void WebPageFullScreenHandler::showEscBtnL() 
{
    if (AknLayoutUtils::PenEnabled()) {
        Show();
    }
}

//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::hideEscBtnL
// Draws the button on the screen
//-------------------------------------------------------------------------------

void WebPageFullScreenHandler::hideEscBtnL()
{
    if (AknLayoutUtils::PenEnabled()) {
        Hide();    
    }
}

//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::SizeChanged
// Handle changes in size of the displayable area.  
//-------------------------------------------------------------------------------

void WebPageFullScreenHandler::SizeChanged(void)
{
    if (AknLayoutUtils::PenEnabled()) {
        TPoint pos = CalculatePosition();
        SetPos(pos);
    }
}

//-------------------------------------------------------------------------------
// WebPageFullScreenHandler::fullScreenMode
// Returns true if a page is in the full screen mode
//-------------------------------------------------------------------------------

bool WebPageFullScreenHandler::isFullScreenMode()
{
    return StaticObjectsContainer::instance()->fullScreenMode();
}


void WebPageFullScreenHandler::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
   switch (aPointerEvent.iType)
       {
       case TPointerEvent::EButton1Down:
           m_isTouchDown = ETrue;
           break;

       case TPointerEvent::EButton1Up:
           {
           if (m_isTouchDown) {
               Hide();
               m_webView->notifyFullscreenModeChangeL( false );
           }
           m_isTouchDown = EFalse;
           }
           break;          
        }       
}

