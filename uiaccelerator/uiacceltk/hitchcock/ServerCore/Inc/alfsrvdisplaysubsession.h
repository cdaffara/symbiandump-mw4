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
* Description:   Display sub session.
*
*/



#ifndef C_ALFSRVDISPLAYSUBSESSION_H
#define C_ALFSRVDISPLAYSUBSESSION_H

#include "alfsrvsubsessionbase.h"
#include <uiacceltk/huidisplaybackgrounditem.h>

class CAlfAppSrvSessionBase;
class CHuiDisplay;
class CHuiDisplayCoeControl;

/**
 *  Display sub session.
 *
 *  @lib alfappservercore.lib
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(CAlfSrvDisplaySubSession): public CAlfSrvSubSessionBase
    {

public:

    /**
     * Two-phased constructor
     *
     * @param aSession Session in which this sub session belongs into.
     * @param aRect Display size.
     * @param aWindowGroupId Client window group ID.
     * @return New object. Ownership transferred.
     */
    static CAlfSrvDisplaySubSession* NewL( CAlfAppSrvSessionBase& aSession, 
                                           const TRect& aRect,
                                           TInt aWindowGroupId,
                                           TInt aDisplayType,
                                           const TUid& aBufferUid );

    /**
     * Two-phased constructor. Leaves the objenct onto the cleanup stack.
     *
     * @param aSession Session in which this sub session belongs into.
     * @param aRect Display size.
     * @param aWindowGroupId Client window group ID.
     * @return New object on stack. Ownership transferred.
     */
    static CAlfSrvDisplaySubSession* NewLC( CAlfAppSrvSessionBase& aSession,
                                            const TRect& aRect,
                                           TInt aWindowGroupId,
                                           TInt aDisplayType,
                                           const TUid& aBufferUid );

    /**
     * Destructor.
     */
    virtual ~CAlfSrvDisplaySubSession();

    /**
     * Returns the used display.
     *
     * @return Used display.
     */
    CHuiDisplay& Display();
    
    /**
     * Called when session is about to get focus.
     */
    void SetSessionFocused(TBool aFocused=ETrue);

// From CAlfSrvSubSessionBase

    /**
     * From CAlfSrvSubSessionBase
     * Can process the given message?
     *
     * @param aMessage The message.
     * @return ETrue if can process the message.
     */
    TBool CanProcessMessage(const RMessage2& aMessage) const;
    
    /**
     * From CAlfSrvSubSessionBase
     * Processes the message.
     *
     * @param aMessage The message.
     */
    void DoProcessMessageL(const RMessage2& aMessage);

    TBool IsTvOut() const;

    TInt DisplayType() const;

private:

    CAlfSrvDisplaySubSession( CAlfAppSrvSessionBase& aSession,
                              const TRect& aRect,
                              TInt aWindowGroupId,
                              TInt aDisplayType,
                              const TUid& aBufferUid );

    void ConstructL();

    void SetClearBackgroundL( const RMessage2& aMessage );
    void SetVisibleAreaL( const RMessage2& aMessage, TBool aForce = EFalse );
    void VisibleAreaL( const RMessage2& aMessage );
    void SetDirtyL( const RMessage2& aMessage );
    void SetQualityL( const RMessage2& aMessage );
    void QualityL( const RMessage2& aMessage );
    void SetUseDepthL( const RMessage2& aMessage );        
    void SetUsageL( const RMessage2& aMessage );
    void SetBackgroundItemsL( const RMessage2& aMessage );
    void HandlePointerEventL( const RMessage2& aMessage );
    void SuppressAutomaticFadingL( const RMessage2& aMessage );
	void SetClientWindowForDrawingL( const RMessage2& aMessage );

    /**
     * Updates non fading to container.
     */
    void UpdateAutomaticFading();

    // from subsessionbase
    CHuiDisplay* AsHuiDisplay();
    
    TBool UseVisualAreaClipping();

private: // data
    
    CHuiDisplay* iDisplay;
    
    TInt iDisplayType;
    TUid iScreenBufferUid;
    TRect iRect;
    TInt iDisplayClearBackground;
    TInt iDisplayRenderingQuality;
    TBool iDisplayUseDepthTest;
    TUint iDisplayUsage;
    RArray<THuiDisplayBackgroundItem> iBackgroundItems;
    TBool iAutomaticFadingSuppressed;
    };

#endif // C_ALFSRVDISPLAYSUBSESSION_H
