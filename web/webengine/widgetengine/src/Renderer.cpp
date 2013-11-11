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
* Description:  This class represents the Widget Extension object
*
*/

// INCLUDE FILES
#include "config.h"
#include <eikmenub.h>
#include <eikenv.h>
#include <gdi.h>
#include <bitdev.h>
#include "Renderer.h"
#include "WebView.h"

//#include "WebSurface.h"
//#include "StaticObjectsContainer.h"

// EXTERNAL DATA STRUCTURES

// EXTERNAL FUNCTION PROTOTYPES

// CONSTANTS
const TUint8 KMaxTransitionCount = 30;
const TInt KMediumResolutionDpi = 200;
//NOTE: 5 transitions is plenty for Tube, but mileage may vary 
//      on future high-res devices. 
const TUint8 KMinTransitionCount = 5;
// LOCAL FUNCTION PROTOTYPES


// ----------------------------------------------------------------------------
// TransitionCb
//
//
//
// ----------------------------------------------------------------------------
TInt transitionCb( TAny* aPtr )
{
    static_cast<WidgetRenderer*>(aPtr)->transitionCb();
    return ETrue;
}


// ----------------------------------------------------------------------------
// WidgetRenderer::WidgetRenderer
//
//
//
// ----------------------------------------------------------------------------
WidgetRenderer::WidgetRenderer(MWidgetEngineCallbacks& aWidgetEngineCallback) : 
m_transitionbitmap(0),
m_fademask(0),
m_transitiontimer(0),
m_transitioncount(0), 
m_maxtransitions(KMaxTransitionCount),
m_widgetenginecallback( &aWidgetEngineCallback )
{
   // If a high-res device, use fewer transitions
   CEikonEnv* eikEnv = CEikonEnv::Static();
   if ( eikEnv )   {
       CWsScreenDevice& screenDev = *eikEnv->ScreenDevice();
       TUint dpi = screenDev.VerticalTwipsToPixels( KTwipsPerInch );
       if ( dpi > KMediumResolutionDpi ) {  //high resolution
           m_maxtransitions = KMinTransitionCount;
       }
   }
}

// ----------------------------------------------------------------------------
// WidgetRenderer::~WidgetRenderer
//
//
//
// ----------------------------------------------------------------------------
WidgetRenderer::~WidgetRenderer()
{
    if (m_transitiontimer) {
        m_transitiontimer->Cancel();        
        delete m_transitiontimer;
    }
    
    if (m_fademask) {
        delete m_fademask;
    }

    if (m_transitionbitmap) {
        delete m_transitionbitmap;
    }

}

// ----------------------------------------------------------------------------
// WidgetRenderer::prepareForTransition
//
//
//
// ----------------------------------------------------------------------------
void WidgetRenderer::prepareForTransitionL(const TDesC& /*aTransition*/)
{
    m_transitioncount = 0;   
    destroyTransitionBitmap();
    createTransitionBitmapL();
    destroyFadeMask();
    createFadeMaskL();
    drawFadeMaskL();
}

// ----------------------------------------------------------------------------
// WidgetRenderer::PerformTransitionL
//
//
//
// ----------------------------------------------------------------------------
void WidgetRenderer::performTransitionL()
{
    delete m_transitiontimer;
    m_transitiontimer = 0;
    TRAP_IGNORE(m_transitiontimer = CPeriodic::NewL( CActive::EPriorityLow ));
    m_transitioncount = 0;
    m_transitiontimer->Start( 20, 20, TCallBack( &::transitionCb, this ) );
}

// ----------------------------------------------------------------------------
// WidgetRenderer::transitionCb
//
//
//
// ----------------------------------------------------------------------------
void WidgetRenderer::transitionCb()
{
    m_transitioncount++;

    if (m_transitioncount==m_maxtransitions) {
        destroyTransitionBitmap();
        m_transitiontimer->Cancel();
        destroyFadeMask();
    }

    TRAP_IGNORE(drawFadeMaskL());
    
    m_widgetenginecallback->drawWidgetTransition();
    
}

// ----------------------------------------------------------------------------
// WidgetRenderer::createTransitionBitmapL
//
//
//
// ----------------------------------------------------------------------------
void WidgetRenderer::createTransitionBitmapL()
{

    if (m_widgetenginecallback->offscreenBitmap()) {
        m_transitionbitmap = new (ELeave) CFbsBitmap();
        User::LeaveIfError(m_transitionbitmap->Create( m_widgetenginecallback->offscreenBitmap()->SizeInPixels(), EColor16MU));
        CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(m_transitionbitmap);
        CleanupStack::PushL(dev);
        CFbsBitGc* gc;
        User::LeaveIfError(dev->CreateContext(gc));
        gc->BitBlt(TPoint(0,0),m_widgetenginecallback->offscreenBitmap());
        delete gc;
        CleanupStack::PopAndDestroy();        
    }
    
}

// ----------------------------------------------------------------------------
// WidgetRenderer::destroyTransitionBitmap
//
//
//
// ----------------------------------------------------------------------------
void WidgetRenderer::destroyTransitionBitmap()
{
    delete m_transitionbitmap;
    m_transitionbitmap = 0;
}

// ----------------------------------------------------------------------------
// WidgetRenderer::createFadeMaskL
//
//
//
// ----------------------------------------------------------------------------
void WidgetRenderer::createFadeMaskL()
{
    m_fademask = new (ELeave) CFbsBitmap();
    m_fademask->Create(m_transitionbitmap->SizeInPixels(),EGray256);
}

// ----------------------------------------------------------------------------
// WidgetRenderer::destroyFadeMask
//
//
//
// ----------------------------------------------------------------------------
void WidgetRenderer::destroyFadeMask()
{
    delete m_fademask;
    m_fademask = 0;
}


// ----------------------------------------------------------------------------
// WidgetRenderer::drawFadeMaskL
//
//
//
// ----------------------------------------------------------------------------
void WidgetRenderer::drawFadeMaskL()
{
    if (m_fademask) {
        
        CFbsBitmapDevice* dev = CFbsBitmapDevice::NewL(m_fademask);
        CleanupStack::PushL(dev);
        
        CFbsBitGc* gc;
        User::LeaveIfError(dev->CreateContext(gc));
        m_fademask->LockHeap();
        
        TSize s = m_fademask->SizeInPixels();
        TUint8* data = (TUint8*)m_fademask->DataAddress();
        TUint8* end = data + s.iWidth*s.iHeight;

        while ( data<end ) {
            *(data++) = (255*m_transitioncount/m_maxtransitions);
        }
        
        m_fademask->UnlockHeap();
        delete gc;
        CleanupStack::PopAndDestroy();
    }
    
}


// ----------------------------------------------------------------------------
// WidgetRenderer::drawTransition
//
//
//
// ----------------------------------------------------------------------------
void WidgetRenderer::drawTransition(CWindowGc& gc, CFbsBitmap* aCurrentBitmap)
{

    if (transitionInProgress()) {
        
        if (!m_transitionbitmap || !aCurrentBitmap || !m_fademask)
            return;

        TSize s = m_transitionbitmap->SizeInPixels();
        gc.BitBlt(TPoint(0,0),m_transitionbitmap);
        gc.BitBltMasked(TPoint(0,0),aCurrentBitmap, TRect(s), m_fademask, EFalse);
    }
}




