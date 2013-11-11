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


// INCLUDE FILES
#include <../bidi.h>
#include <AknUtils.h>
#include <gulicon.h>
#include "WebPopupDrawer.h"
#include "WebView.h"
#include "WebCannedImages.h"
#include "BrCtl.h"

#include "StaticObjectsContainer.h"

// constants
const int KPopupTimeout = 5000 * 1000;
const int KBorderSize = 8;
const int KRowMargin = 5;
const int KIconMargin = 5;
const int KSpaceUnderneath = 30;

using namespace WebCore;

// ============================= LOCAL FUNCTIONS ===============================
TInt removePopupCallback( TAny* aPtr )
{
    static_cast<WebPopupDrawer*>(aPtr)->removePopup();
    return KErrNone;
}

// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WebPopupDrawer::NewL
// The two-phase Symbian constructor
// -----------------------------------------------------------------------------
//
WebPopupDrawer* WebPopupDrawer::NewL(WebView& webView, TPtrC& textToDraw, TRect& elRect )
{
    WebPopupDrawer* self = new (ELeave) WebPopupDrawer( webView, elRect  );
    CleanupStack::PushL(self);
    self->constructL(textToDraw);
    CleanupStack::Pop(); //self
    return self;
}

// -----------------------------------------------------------------------------
// WebPopupDrawer::WebPopupDrawer
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
WebPopupDrawer::WebPopupDrawer(WebView& webView, TRect& elRect )
: m_webView( webView ) , m_elRect(elRect)
{
}

// -----------------------------------------------------------------------------
// WebPopupDrawer::constructL
// The constructor that can contain code that might leave.
// -----------------------------------------------------------------------------
//
void WebPopupDrawer::constructL(TPtrC& textToDraw)
{
    if (textToDraw.Length() == 0)
    {
        User::Leave(KErrArgument);
    }
    m_textToDraw = textToDraw.AllocL();
    CleanupStack::PushL(m_textToDraw);
    m_removePopupPeriodic = CPeriodic::NewL( CActive::EPriorityLow );
    CleanupStack::PushL(m_removePopupPeriodic);
    m_removePopupPeriodic->Start( KPopupTimeout, 0, TCallBack( &removePopupCallback, this ) );
    sizeChangedL();
    constructSprite();
    CleanupStack::Pop(2, m_textToDraw); //m_removePopupPeriodic, m_textToDraw
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
WebPopupDrawer::~WebPopupDrawer()
{
    delete m_textToDraw;
    if (m_removePopupPeriodic)
    {
        m_removePopupPeriodic->Cancel();
        delete m_removePopupPeriodic;
    }
    delete m_lineWidthArray;
    delete m_wrappedArray;
    m_sprite.Close();
    delete m_spriteMaskBitmapDevice;
    delete m_spriteMaskBitmapContext;
    delete m_spriteMaskBitmap;
    delete m_icon;
    delete m_spriteBitmapDevice;
    delete m_spriteBitmapContext;
    delete m_spriteBitmap; 
}

// -----------------------------------------------------------------------------
// WebPopupDrawer::drawPopup
// Draw the popup.
// -----------------------------------------------------------------------------
//
void WebPopupDrawer::drawPopup()
{
    m_sprite.SetPosition(m_position);
}

// -----------------------------------------------------------------------------
// WebPopupDrawer::removePopup
// Stop displaying the popup.
// -----------------------------------------------------------------------------
//
void WebPopupDrawer::removePopup()
{
    m_webView.removePopup();
}

// -----------------------------------------------------------------------------
// WebPopupDrawer::sizeChangedL
// Recreate the bitmap to fit in the new size
// -----------------------------------------------------------------------------
//
void WebPopupDrawer::sizeChangedL()
{
    TRect rect(m_webView.Rect());
    if (rect != m_webViewRect)
    {
        if (rect.Width() > 0 && rect.Height() > 0)
        {
            m_webViewRect = rect;
            prepareTextL();
            createBitmapAndContextL();
        }
    }
}

// -----------------------------------------------------------------------------
// WebPopupDrawer::handleOfferKeyEventL
// Handles key events
// -----------------------------------------------------------------------------
//
TKeyResponse WebPopupDrawer::handleOfferKeyEventL( const TKeyEvent& /*aKeyEvent*/, TEventCode eventCode )
{
    if (eventCode == EEventKeyUp)
    {
        removePopup();
    }
    return EKeyWasConsumed;
}

// -----------------------------------------------------------------------------
// WebPopupDrawer::HandlePointerEventL
// Handles pointer events
// -----------------------------------------------------------------------------
//
void WebPopupDrawer::handlePointerEventL( const TPointerEvent& /*aPointerEvent*/)
{
    m_webView.IgnoreEventsUntilNextPointerUp();
    removePopup();
}

// -----------------------------------------------------------------------------
// WebPopupDrawer::prepareTextL
// Position and wrap the text called during construction and when the size of WebKit changes
// -----------------------------------------------------------------------------
//
void WebPopupDrawer::prepareTextL()
{
    delete m_lineWidthArray;
    m_lineWidthArray = NULL;
    delete m_wrappedArray;
    m_wrappedArray = NULL;
    TInt width = (8 * m_webViewRect.Width()) / 10 - 2 * KBorderSize;
    createIconIfNeededL();
    m_font = LatinBold16();
    m_lineWidthArray = new( ELeave ) CArrayFixFlat<TInt>(2);
    m_lineWidthArray->AppendL(width - m_icon->Bitmap()->SizeInPixels().iWidth - KIconMargin);
    TInt maxNumberOfRows = m_webViewRect.Height() / (m_font->HeightInPixels() + KRowMargin);
    TInt i;
    for (i = 1; i < maxNumberOfRows; i++) {
        m_lineWidthArray->AppendL(width);
    }
    m_wrappedArray = new( ELeave ) CArrayFixFlat<TPtrC>( 10 );
    AknTextUtils::WrapToArrayL( *m_textToDraw, *m_lineWidthArray, *m_font, *m_wrappedArray );
    TInt rowHeight( m_font->HeightInPixels() +  KRowMargin );
    m_size.iWidth = (8 * m_webViewRect.Width()) / 10;
    m_size.iHeight = (m_wrappedArray->Count() + 1 ) * rowHeight;
    if (m_size.iHeight + 2 * KSpaceUnderneath > m_webViewRect.Height()) {
        TInt toRemove((m_webViewRect.Height() - m_size.iHeight + 2 * KSpaceUnderneath + (rowHeight - 1)) / rowHeight);
        m_wrappedArray->Delete((m_wrappedArray->Count() - toRemove) / 2, toRemove);
        m_wrappedArray->Compress();
        m_size.iHeight = (m_wrappedArray->Count() + 1 ) * rowHeight;
    }
    m_position.iX = m_webViewRect.Width() / 10;
    
    if ( m_elRect.iBr.iY > 0 && m_elRect.iBr.iY < m_webViewRect.Height() &&
        m_webViewRect.Height() - m_elRect.iBr.iY - m_size.iHeight - KSpaceUnderneath >= 0) {
        m_position.iY = m_elRect.iBr.iY;
    }
    else if (m_size.iHeight + KSpaceUnderneath <= m_elRect.iTl.iY) {
        m_position.iY = m_elRect.iTl.iY - m_size.iHeight;
    }
    else {
        m_position.iY = (m_webViewRect.Height() - m_size.iHeight) / 2;// - KSpaceUnderneath;
    }
}

// -----------------------------------------------------------------------------
// WebPopupDrawer::CreateBitmapAndContextL
// Create bitmap and bitmap context
// -----------------------------------------------------------------------------
//
void WebPopupDrawer::createBitmapAndContextL()
{
    delete m_spriteBitmapDevice;
    m_spriteBitmapDevice = NULL;
    delete m_spriteBitmapContext;
    m_spriteBitmapContext = NULL;
    delete m_spriteBitmap;
    m_spriteBitmap = NULL;
    m_spriteBitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(m_spriteBitmap);
    User::LeaveIfError( m_spriteBitmap->Create( m_size, EColor16MA ) );
    m_spriteBitmapDevice = CFbsBitmapDevice::NewL( m_spriteBitmap );
    CleanupStack::PushL(m_spriteBitmapDevice);
    User::LeaveIfError( m_spriteBitmapDevice->CreateContext( m_spriteBitmapContext ) );
    //mask
    delete m_spriteMaskBitmapDevice;
    m_spriteMaskBitmapDevice = NULL;
    delete m_spriteMaskBitmapContext;
    m_spriteMaskBitmapContext = NULL;
    delete m_spriteMaskBitmap;
    m_spriteMaskBitmap = NULL;
    m_spriteMaskBitmap = new (ELeave) CFbsBitmap;
    CleanupStack::PushL(m_spriteMaskBitmap);
    User::LeaveIfError( m_spriteMaskBitmap->Create( m_size, EGray2 ) );
    m_spriteMaskBitmapDevice = CFbsBitmapDevice::NewL( m_spriteMaskBitmap );
    CleanupStack::PushL(m_spriteMaskBitmapDevice);
    User::LeaveIfError( m_spriteMaskBitmapDevice->CreateContext( m_spriteMaskBitmapContext ) );
    TRect r( TPoint( 0, 0 ), m_spriteMaskBitmap->SizeInPixels());
    m_spriteMaskBitmapContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
    m_spriteMaskBitmapContext->SetPenStyle( CGraphicsContext::ESolidPen );
    m_spriteMaskBitmapContext->SetBrushColor( TRgb( 0, 0, 0) );
    m_spriteMaskBitmapContext->SetPenColor( TRgb( 0, 0, 0) );
    m_spriteMaskBitmapContext->DrawRoundRect( r, TSize(KBorderSize * 2, KBorderSize * 2) );
    if( m_wrappedArray->Count() ) {
        TRect r( TPoint( 0, 0 ), m_spriteBitmap->SizeInPixels());
        m_spriteBitmapContext->SetBrushStyle( CGraphicsContext::ESolidBrush );
        m_spriteBitmapContext->SetPenStyle( CGraphicsContext::ESolidPen );
        m_spriteBitmapContext->SetBrushColor( TRgb( 0xFF, 0xFF, 0xdd) );
        m_spriteBitmapContext->SetPenColor( TRgb( 0, 0, 0) );
        m_spriteBitmapContext->DrawRoundRect( r, TSize(KBorderSize * 2, KBorderSize * 2) );
        m_spriteBitmapContext->UseFont( m_font );
        TInt rowHeight( m_font->HeightInPixels() +  KRowMargin );
        TInt x = KBorderSize;
        TInt y = KBorderSize + KRowMargin / 2;
        for( TInt i = 0; i < m_wrappedArray->Count(); i++, y += rowHeight ) {
            TPtrC text = (m_wrappedArray->At(i));           
            if (i == 0) {
                // draw icon
                m_spriteBitmapContext->BitBltMasked(TPoint(x, y - KRowMargin / 2), m_icon->Bitmap(),
                    TRect(TPoint(0, 0), m_icon->Bitmap()->SizeInPixels()), m_icon->Mask(), EFalse);
                m_spriteBitmapContext->DrawText( text, TPoint( x + m_icon->Bitmap()->SizeInPixels().iWidth + KIconMargin, y + rowHeight - m_font->DescentInPixels()) );
            }
            else {
                m_spriteBitmapContext->DrawText( text, TPoint( x, y + rowHeight - m_font->DescentInPixels()) );
            }
        }
        m_spriteBitmapContext->DiscardFont();
    }
    
    CleanupStack::Pop(4, m_spriteBitmap);
}

void WebPopupDrawer::constructSprite()
{        
    m_sprite = RWsSprite(m_webView.brCtl()->CCoeControlParent()->ControlEnv()->WsSession());
    m_sprite.Construct(m_webView.brCtl()->CCoeControlParent()->ControlEnv()->RootWin(), m_position,ESpriteNoChildClip);
    TSpriteMember spriteMem;
    spriteMem.iBitmap = m_spriteBitmap;
    spriteMem.iMaskBitmap = m_spriteMaskBitmap;
    spriteMem.iInvertMask = ETrue;
    m_sprite.AppendMember(spriteMem);
    m_sprite.Activate();
}

// -----------------------------------------------------------------------------
// WebPopupDrawer::createIconIfNeededL
// Determine which icon to load and load it
// -----------------------------------------------------------------------------
//
void WebPopupDrawer::createIconIfNeededL()
{
    if (m_icon == NULL) {
        _LIT(KHttpScheme, "http");
        _LIT(KHttpsScheme, "https");
        _LIT(KFileScheme, "file");
        _LIT(KMailToScheme, "mailto");
        _LIT(KTelScheme, "tel");
        _LIT(KRtspScheme, "rtsp");
        _LIT(KMmstoScheme, "mmsto");
        _LIT(KSmsScheme, "sms");
        WebCannedImages::TCannedImageNames imageType = WebCannedImages::EImageUrlPage;
        TInt i = m_textToDraw->Locate(':');
        if (i != KErrNotFound) {
            TPtrC schemePtr(m_textToDraw->Ptr(), i);
            if (KHttpScheme().CompareF(schemePtr) == 0) {
                imageType = WebCannedImages::EImageUrlPage;
            }
            else if (KHttpsScheme().CompareF(schemePtr) == 0) {
                imageType = WebCannedImages::EImageUrlPage;
            }
            else if (KFileScheme().CompareF(schemePtr) == 0) {
                imageType = WebCannedImages::EImageUrlPage;
            }
            else if (KMailToScheme().CompareF(schemePtr) == 0) {
                imageType = WebCannedImages::EImageUrlEmail;
            }
            else if (KTelScheme().CompareF(schemePtr) == 0) {
                imageType = WebCannedImages::EImageUrlCall;
            }
            else if (KRtspScheme().CompareF(schemePtr) == 0) {
                imageType = WebCannedImages::EImageUrlMm;
            }
            else if (KMmstoScheme().CompareF(schemePtr) == 0) {
                imageType = WebCannedImages::EImageUrlMms;
            }
            else if (KSmsScheme().CompareF(schemePtr) == 0) {
                imageType = WebCannedImages::EImageUrlEmail;
            }
        }
        TCannedImageData cannedImage = StaticObjectsContainer::instance()->webCannedImages()->getImage(imageType);
        if (cannedImage.m_msk) {
            m_icon = CGulIcon::NewL(cannedImage.m_img, cannedImage.m_msk);
        }
        else {
            m_icon = CGulIcon::NewL(cannedImage.m_img);
        }
    }
}

//  End of File
