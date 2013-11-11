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
*                enabled, this class is used as the single view.  If frame
*                enabled, there is one instance of this class for each frame.
*
*/


#ifndef __WEBKITSURFACE_H__
#define __WEBKITSURFACE_H__

#include <e32base.h>
#include <gdi.h>

//forward declarations
class CFbsBitmapDevice;
class CFbsBitmap;
class CBitmapContext;
class WebView;

class WebSurface : public CBase
{
public:
    static WebSurface* NewL( TDisplayMode mode = EColor16MU );
	
	~WebSurface();
	
	void flip( const TPoint& pt, CBitmapContext& gc ) const;
	
	CFbsBitmap* offscreenBitmap() const				{ return m_offscreenbitmap;	}
	WebView* topView() const				        { return m_topview; }
	void setView(WebView* view)			{ m_topview = view; }
	TDisplayMode displayMode() const				{ return m_displaymode; }
	
private:
    WebSurface(TDisplayMode aMode);
	void ConstructL();	
			
private:
    CFbsBitmap*             m_offscreenbitmap;  	// owned
    TDisplayMode 			m_displaymode; 		// indicates the display mode that is used by the browser
    WebView*			    m_topview;
};

#endif //__WEBKITSURFACE_H__
