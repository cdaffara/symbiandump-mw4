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
* Description:   History View
*
*/



// INCLUDE FILES
#include <../bidi.h>
#include "PageView.h"
#include  <coemain.h>
#include "pagescaler.h"
#include "WebView.h"
#include "BrCtl.h"

// EXTERNAL DATA STRUCTURES
//extern  ?external_data;

// EXTERNAL FUNCTION PROTOTYPES
//extern ?external_function( ?arg_type,?arg_type );

// CONSTANTS



// ============================= LOCAL FUNCTIONS ===============================


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// CPageView::CPageView
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
CPageView::CPageView( WebView& aView )
    : m_webView(&aView)
    {

    }

// -----------------------------------------------------------------------------
// ?CPageView::ConstructL
// Symbian 2nd phase constructor can leave.
// -----------------------------------------------------------------------------
//
void CPageView::ConstructL()
    {
    CBrCtl* brCtl = m_webView->brCtl();
    SetContainerWindowL(*brCtl);
    SetRect(brCtl->Rect());
    m_webView->pageScaler()->SetFullScreenMode(ETrue);
    m_webView->pageScaler()->SetContainerRect(TRect(Rect().Size()));
    m_webView->pageScaler()->SetVisible(ETrue);
    }


// -----------------------------------------------------------------------------
// CPageView::NewL
// Two-phased constructor.
// -----------------------------------------------------------------------------
//
CPageView* CPageView::NewL( WebView& aView )
    {
    CPageView* self = new( ELeave ) CPageView(aView);

    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();

    return self;
    }


// Destructor
CPageView::~CPageView()
    {
    m_webView->pageScaler()->SetVisible(EFalse);
    m_webView->pageScaler()->SetFullScreenMode(EFalse);
    }


// ----------------------------------------------------------------------------
// CPageView::Draw
//
// Draws the bitmap
// for the viewable content.
// ----------------------------------------------------------------------------
void CPageView::Draw(
    const TRect& aRect  ) const
    {
    CWindowGc& gc = SystemGc();
    m_webView->pageScaler()->SetVisible(ETrue);
    m_webView->pageScaler()->Draw(gc, aRect);
    }

// ----------------------------------------------------------------------------
// CPageView::SizeChanged
// public Class Method
// Updates the size of the offscreen bitmap  to improve performance.
// ----------------------------------------------------------------------------
void CPageView::SizeChanged()
    {
    m_webView->pageScaler()->SetContainerRect(TRect(Rect().Size()));
    }

// ----------------------------------------------------------------------------
// CPageView::HandlePointerEventL
//
// Handle pointer event
// ----------------------------------------------------------------------------
void CPageView::HandlePointerEventL(const TPointerEvent& aPointerEvent)
    {
    m_webView->HandlePointerEventL(aPointerEvent);
    }


//  End of File
