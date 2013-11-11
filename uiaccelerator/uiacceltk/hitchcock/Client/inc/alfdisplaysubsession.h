/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:   Sub-session for display
*
*/



#ifndef R_ALFDISPLAYSUBSESSION_H
#define R_ALFDISPLAYSUBSESSION_H

#include <e32std.h>
#include "alfclient.h"
#include "alf/alfdisplay.h"

/**
 *  Sub-session for display
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS( RAlfDisplaySubSession ): public RSubSessionBase 
    {

public:

    /**
     * Open subsession for given session.
     *
     * @param aSession Used session.
     * @param aRect Display area.
     * @return Error code.
     */
    TInt Open( RAlfClient& aSession, const TRect& aRect, TInt aDisplayType, TUid aBufferUid );
    
    /**
     *  Closes the subsession.
     */
    void Close();
    
    /**
     * Set background clearing mode. 
     *
     * @param aClearBackground Mode. See CAlfDisplay::TClearMode.
     * @return Error code.
     */
    TInt SetClearBackground( TInt aClearBackground );
    
    /**
     * Set visible area. 
     *
     * @param aRect Visible area rect.
     * @return Error code.
     */
    TInt SetVisibleArea( const TRect& aRect, TBool aForce = EFalse );
    
    /**
     * Returns visible area. 
     *
     * @param aRect Visible area rect is set here (if returns KErrNone).
     * @return Error code.
     */
    TInt VisibleArea( TRect& aRect ) const;
    
    /**
     * Sets whole display as dirty. 
     *
     */
    void SetDirty();
    
    /**
     * Sets redering quality. 
     *
     */
    TInt SetQuality(TAlfQuality aRenderingQuality);
    
    /**
     * Gets redering quality. 
     *
     */
    TInt Quality(TAlfQuality& aRenderingQuality) const;

    /**
     * Sets depth test. 
     *
     */
    TInt SetUseDepth(TBool aUseDepth);
    
    /**
     * Sets intended usage of display. 
     *
     */
    TInt SetUsage(TUint aUsageHint);
    
    /**
     * Sets display background items. 
     *
     */
    TInt SetBackgroundItemsL(const RArray<TAlfDisplayBackgroundItem>& aItems);    

    /**
     * Forwards a pointer event to display.
     *
     */
    TInt HandlePointerEventL(const TPointerEvent& aPointerEvent);

    /**
     * Suppresses automatic fading.
     * @param aSuppress ETrue to suppress, EFalse to enable back.
     */
    TInt SuppressAutomaticFading( TBool aSuppress );
    
    /**
     * Set the client window where the visual or layout will be drawn to.
     * @param aWindowGroupId Window group id.
     * @param aClientWindowHandle Client side window handle.
     * @param aVisualHandle Visual handle.
     */
    void SetClientWindowForDrawingL(TInt aWindowGroupId, TInt aClientWindowHandle, TInt aVisualHandle);

    };


#endif // R_ALFDISPLAYSUBSESSION_H
