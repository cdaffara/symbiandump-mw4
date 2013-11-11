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
#include <AknUtils.h>
#include "WebPageZoomHandler.h"
#include "SettingsContainer.h"
#include <AknVolumePopup.h>
#include "WebView.h"
#include "BrCtl.h"
#include "WebFrameView.h"
#include "WebFrame.h"


const int KZoomSliderDefaultValue = 50;
const int KZoomSliderMinRange  = 0;
const int KZoomSliderStepSize  = 10;
const int KZoomSliderTimeout   = 5;
const int KBitmapUpdateTimeout = 100*1000;

int bitmapUpdateTimerCb( void* ptr );


// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// WebPageZoomHandler::NewL
// The two-phase Symbian constructor
// -----------------------------------------------------------------------------
//
WebPageZoomHandler* WebPageZoomHandler::NewL(WebView& webView)
{
    WebPageZoomHandler* self = new (ELeave) WebPageZoomHandler( webView );
    CleanupStack::PushL(self);
    self->constructL();
    CleanupStack::Pop(); //self
    return self;
}

// -----------------------------------------------------------------------------
// WebPageZoomHandler::WebPointerEventHandler
// C++ default constructor can NOT contain any code, that
// might leave.
// -----------------------------------------------------------------------------
//
WebPageZoomHandler::WebPageZoomHandler(WebView& webView)
: m_webView(&webView), m_zoomSlider(0)
{
}

// -----------------------------------------------------------------------------
// WebPageZoomHandler::constructL
// The constructor that can contain code that might leave.
// -----------------------------------------------------------------------------
//
void WebPageZoomHandler::constructL()
{
    m_bitmapUpdateTimer = CPeriodic::NewL(CActive::EPriorityHigh);
    m_stepSize = KZoomSliderStepSize;
}

// -----------------------------------------------------------------------------
// Destructor
// -----------------------------------------------------------------------------
WebPageZoomHandler::~WebPageZoomHandler()
{
    if ( m_bitmapUpdateTimer && m_bitmapUpdateTimer->IsActive() )
        {
        m_bitmapUpdateTimer->Cancel();
        }

    delete m_bitmapUpdateTimer;

    if ( m_zoomSlider )
        {
        if( m_zoomSlider->IsVisible()) 
            {
            m_zoomSlider->CloseVolumePopup();
            }
        delete m_zoomSlider;
        }
}
// -----------------------------------------------------------------------------

void WebPageZoomHandler::showZoomSliderL()
{
    if ( AknLayoutUtils::PenEnabled() )
        {
        if (m_zoomSlider) {
            delete m_zoomSlider;
            m_zoomSlider = NULL;
            }
        m_zoomSlider = CAknVolumePopup::NewL( NULL, ETrue);
        m_zoomSlider->SetValueType( EAknSliderValuePercentage );
        m_zoomSlider->SetStepSize( m_stepSize );
        m_zoomSlider->SetAdjustmentIndicatorDisplay( ETrue );
        m_zoomSlider->SetTimeOutAsSecond( KZoomSliderTimeout );
        m_zoomSlider->ClosePopupWhenTappedOutside( EFalse );
        m_zoomSlider->SetRange( m_webView->minZoomLevel(), m_webView->maxZoomLevel());
        m_zoomSlider->SetValue( m_webView->scalingFactor());
        m_zoomSlider->SetObserver(this);
    
        TRect currRect = m_webView->brCtl()->Rect();
        TPoint newPos ( currRect.iTl.iX + ( currRect.iBr.iX - m_zoomSlider->Size().iWidth ) , currRect.iTl.iY + 30 );
    
        m_zoomSlider->SetPosition( newPos );
        m_zoomSlider->ShowVolumePopupL();
        }
}

void WebPageZoomHandler::hideZoomSliderL()
{
    if ( AknLayoutUtils::PenEnabled() )
        {
        if (m_zoomSlider) 
            {
            if( m_zoomSlider->IsVisible())
                m_zoomSlider->CloseVolumePopup();
    
            delete m_zoomSlider;
            m_zoomSlider = NULL;
            m_webView->notifyZoomSliderModeChangeL(false);
            }
        }
}

void WebPageZoomHandler::setZoomLevel(int zoomLevel)
{
    m_webView->setBitmapZoomLevel(zoomLevel);
}


void WebPageZoomHandler::HandleControlEventL(CCoeControl* aControl,TCoeEvent aEventType)
{
    if ( AknLayoutUtils::PenEnabled() )
        {
        TBool sliderVisibility = false;
        TInt zoomValue = 0;
    
        if(m_zoomSlider == aControl) {
    
            if (m_bitmapUpdateTimer && m_bitmapUpdateTimer->IsActive())
                m_bitmapUpdateTimer->Cancel();
    
            sliderVisibility = m_zoomSlider->IsVisible();
    
            switch(aEventType) {
                case EEventStateChanged:
            	case EEventRequestFocus:
                    if(sliderVisibility) {
                        zoomValue = m_zoomSlider->Value();
                        if(zoomValue >= m_webView->minZoomLevel() && zoomValue <= m_webView->maxZoomLevel()){
                            setZoomLevel(zoomValue);
                        }
                        else if( zoomValue < m_webView->minZoomLevel()) {
    
                              m_zoomSlider->SetValue( m_webView->scalingFactor() );
                        }
                    }
                    break;
    
                default:
                    break;
            }
            if(sliderVisibility && m_bitmapUpdateTimer)
                m_bitmapUpdateTimer->Start( KBitmapUpdateTimeout,KBitmapUpdateTimeout,TCallBack(&bitmapUpdateTimerCb,this));
    
    
            if( !sliderVisibility && EEventStateChanged == aEventType ) {
                m_webView->notifyZoomSliderModeChangeL(false);
            }
        }
    } 
}


int WebPageZoomHandler::stepSize()
{
    return m_stepSize;
}


WebView* WebPageZoomHandler::view( void )
{
    return m_webView;
}

bool WebPageZoomHandler::isActive( void )
{
    return ( m_zoomSlider && m_zoomSlider->IsVisible())?true:false;
}
void WebPageZoomHandler::updateBitmap(void)
{
    m_bitmapUpdateTimer->Cancel();
    m_webView->restoreZoomLevel(m_webView->scalingFactor());
}

int bitmapUpdateTimerCb(void* ptr)
{
    ((WebPageZoomHandler*)ptr)->updateBitmap();
    return 0;
}

//  End of File
