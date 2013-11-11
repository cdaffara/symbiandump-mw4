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
* Description:  Handles the viewing of a single frame. If the page is not frame
*               enabled, this class is used as the single view.  If frame
*               enabled, there is one instance of this class for each frame.
*
*/


#include "WebSurface.h"
#include <bitdev.h>
#include <fbs.h>

WebSurface* WebSurface::NewL(TDisplayMode mode)
	{
    WebSurface* self = new (ELeave) WebSurface(mode);
	CleanupStack::PushL( self );
	self->ConstructL();
	CleanupStack::Pop( self );
	return self;
	}

WebSurface::~WebSurface()
	{
	delete m_offscreenbitmap;
	}
	
WebSurface::WebSurface(TDisplayMode mode) : m_displaymode( mode )
	{
	// do nothing
	}
	
void WebSurface::ConstructL()
	{
	// Create a bitmap to be used offscreen
	m_offscreenbitmap = new (ELeave) CFbsBitmap();
	User::LeaveIfError( m_offscreenbitmap->Create( TSize(0,0),  m_displaymode) );
	}
	
void WebSurface::flip( const TPoint& pt, CBitmapContext& gc ) const
	{
    // put offscreen bitmap onto the screen
    gc.BitBlt( pt, m_offscreenbitmap );
	}
