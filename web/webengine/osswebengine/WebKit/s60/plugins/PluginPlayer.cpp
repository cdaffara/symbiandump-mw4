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
* Description:  Page overview control
*
*/
#include "../../bidi.h"

#include "PluginPlayer.h"
#include "BrCtl.h"
#include "PluginWin.h"
#include "PluginSkin.h"
#include "WebView.h"
#include "WebFrame.h"
#include <AknUtils.h>

PluginPlayer* PluginPlayer::NewL( CBrCtl& parent, PluginWin* plugin )
    {
    PluginPlayer* self = new (ELeave) PluginPlayer(parent, plugin );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop();
    return self;
    }

PluginPlayer::PluginPlayer( CBrCtl& parent, PluginWin* plugin )
    : m_brctl( parent ), m_plugin( plugin )
    {
    }

void PluginPlayer::ConstructL()
    {
    SetContainerWindowL( m_brctl );
    SetRect( m_brctl.Rect() );
    if (AknLayoutUtils::PenEnabled()) {
        DrawableWindow()->SetPointerGrab(ETrue);
        EnableDragEvents();
    }

    ActivateL();
    }

PluginPlayer::~PluginPlayer()
    {
    }

void PluginPlayer::Draw( const TRect& rect ) const
    {
    CWindowGc& gc = SystemGc();
    
    // draw the background
    gc.SetPenStyle( CGraphicsContext::ESolidPen );
    gc.SetPenColor( TRgb( 128, 128, 128 ) );
    gc.SetBrushColor( TRgb( 192, 192, 192 ) );
    gc.SetBrushStyle( CGraphicsContext::EDiamondCrossHatchBrush );
    gc.DrawRect( rect );
    }

TInt PluginPlayer::CountComponentControls() const
    {
    if( m_plugin )
        return 1;
    else
        return 0;
    }

CCoeControl *PluginPlayer::ComponentControl(TInt /*aIndex*/) const
    {
    return m_plugin;
    }


TKeyResponse PluginPlayer::OfferKeyEventL(const TKeyEvent keyEvent, TEventCode type)
    {
    return m_plugin->OfferKeyEventL( keyEvent, type );
    }

void PluginPlayer::start()
    {
    m_orgparent = m_plugin->Parent();    
    m_orgrect = TRect( m_plugin->Position(), m_plugin->Size() );
    
    m_plugin->SetParent( this );

    // try to guess what part the user wants to see, this way
    // we can avoid unnecessary scrolling;
    // - if the content can be made fully visible, we just center the content.
    // - if any of the edge is clipped away, we try to show as much as possible while still
    // keep the opposite edge visible.
    // - if both edge are clipped away, don't do anything.
    const TRect& rect = Rect();
    TInt x = ( rect.Width() - m_orgrect.Width() )/2;
    TInt y = ( rect.Height() - m_orgrect.Height() )/2;

    // horizontal
    if( x < 0 )
        {
        if( m_orgrect.iBr.iX < rect.iBr.iX )
            x = m_orgrect.iTl.iX + rect.iBr.iX - m_orgrect.iBr.iX;    // right edge visible
        else if( m_orgrect.iTl.iX > rect.iTl.iX )
            x = rect.iTl.iX;                                        // left edge visible
        else
            x = m_orgrect.iTl.iX;                                    // both edges are clipped away
        }

    // vertical
    if( y < 0 )
        {
        if( m_orgrect.iBr.iY < rect.iBr.iY )
            y = m_orgrect.iTl.iY + rect.iBr.iY - m_orgrect.iBr.iY;    // bottom edge visible
        else if( m_orgrect.iTl.iY > rect.iTl.iY )
            y = rect.iTl.iY;                                        // top edge visible
        else
            y = m_orgrect.iTl.iY;                                    // both edges are clipped away
        }
        
    m_plugin->SetRect( TRect( TPoint(x,y), m_orgrect.Size() ) );
    m_plugin->makeVisible( ETrue );

    DrawNow();
    // not sure why we need this for bavp
    m_plugin->SetRect( TRect( TPoint(x,y), m_orgrect.Size() ) );
    TRAP_IGNORE( m_plugin->setPluginFocusL(ETrue) );
    }

void PluginPlayer::stop()
    {
    TRAP_IGNORE( m_plugin->setPluginFocusL(EFalse) );
    m_plugin->SetParent( m_orgparent );
    m_plugin->SetRect( TRect( m_orgrect.iTl, m_orgrect.Size() ) );
    m_plugin->makeVisible( EFalse );
    }

void PluginPlayer::HandlePointerEventL(const TPointerEvent& aPointerEvent)
{
    m_plugin->HandlePointerEventL(aPointerEvent);
}

// END OF FILE
