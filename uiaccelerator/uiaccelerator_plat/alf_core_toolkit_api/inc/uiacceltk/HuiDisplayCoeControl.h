/*
* Copyright (c) 2006-2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Definition of the CHuiDisplayCoeControl class. 
*                CHuiDisplayCoeControl is a CCoeControl that has its own 
*                CHuiDisplay instance attached to the control's RWindow.
*
*/



#ifndef __HUIDISPLAYCOECONTROL_H__
#define __HUIDISPLAYCOECONTROL_H__


#include <e32base.h>
#include <uiacceltk/HuiPlatform.h>


/* Forward declarations. */
class CHuiEnv;
class CHuiDisplay;


/**
 * CHuiCoeControlDisplay is CCoeControl-derived, window-owning control that 
 * is attached to a CHuiDisplay instance. The entire area of the control is 
 * used for rendering. 
 */
class CHuiDisplayCoeControl : public CCoeControl, 
                              public MCoeControlObserver
    {
public:

    /* Constructors and destructor. */

    /**
     * Construct a new CoeControl that has a rendering surface.
     *
     * @param aEnv  Environment.
     */
    IMPORT_C static CHuiDisplayCoeControl* NewL(CHuiEnv& aEnv, const TRect& aRect, TInt aFlags = 0);
    
    /**
     * Construct a new CoeControl that has a rendering surface, and leaves it 
     * on the cleanup stack.
     *
     * @param aEnv  Environment.
     */
    IMPORT_C static CHuiDisplayCoeControl* NewLC(CHuiEnv& aEnv, const TRect& aRect, TInt aFlags = 0);

    /**
     * Destructor.
     */
    IMPORT_C ~CHuiDisplayCoeControl();


    /* Methods. */

    /**
     * Returns the environment of this CHuiDisplayCoeControl.
     */
    IMPORT_C CHuiEnv& Env();

    /**
     * Returns the display of this CHuiDisplayCoeControl.
     */
    IMPORT_C CHuiDisplay& Display();
    

private: /* from CoeControl */

    /**
     * Called when the size of the control changes.
     */
    IMPORT_C void SizeChanged();

    /**
     * Returns the number of CoeControls inside this control.
     *
     * @return  Zero.
     */
    IMPORT_C TInt CountComponentControls() const;

    /**
     * Returns a pointer to a CoeControl inside this control.
     *
     * @return  <code>NULL</code>.
     */
    IMPORT_C CCoeControl* ComponentControl(TInt aIndex) const;

    /**
     * From CCoeControl. Invalidate drawing area. No synchronous drawing takes place here
     */
    IMPORT_C void Draw(const TRect& aRect) const;

    /**
     * Handles resource change.
     *
     * @see  S60 documentation.
     */ 
    IMPORT_C void HandleResourceChange(TInt aType);

    /**
     * Acts upon changes in the hosted control's state.
     *
     * @param   aControl    The control changing its state
     * @param   aEventType  The type of control event
     */
    IMPORT_C void HandleControlEventL(CCoeControl* aControl, TCoeEvent aEventType);

    /**
     * Called when a key input event is received by the display. The input
     * event is passed on to one of the controls in the display.
     *
     * @param aKeyEvent  Key event.
     * @param aType      Type of the event.
     */
    IMPORT_C TKeyResponse OfferKeyEventL(const TKeyEvent& aKeyEvent, TEventCode aType);

    /** ! future proofing, just basecall */
	IMPORT_C void MakeVisible(TBool aVisible);
	IMPORT_C void SetDimmed(TBool aDimmed);
	IMPORT_C void ConstructFromResourceL(TResourceReader& aReader);
	IMPORT_C void ActivateL();
	IMPORT_C void PrepareForFocusLossL();
	IMPORT_C void PrepareForFocusGainL();
	IMPORT_C TCoeInputCapabilities InputCapabilities() const;
	IMPORT_C void FocusChanged(TDrawNow aDrawNow);
	IMPORT_C void PositionChanged();
	IMPORT_C void SetContainerWindowL(const CCoeControl& aContainer);

protected:

    /* Constructors. */

    /**
     * Constructor.
     *
     * @param aEnv  The environment that manages this display.
     */
    IMPORT_C CHuiDisplayCoeControl(CHuiEnv& aEnv);

    /**
     * Second-phase constructor. Creates a window and an OpenGL context.
     *
     * @param aRect  Frame rectangle for container.
     * @param aFlags  New display flags from CHuiEnv.     
     */
    IMPORT_C virtual void ConstructL(const TRect& aRect, TInt aFlags);


    /* Utility methods. */

    /**
     * Handle pointer events.
     */
    IMPORT_C void HandlePointerEventL(const TPointerEvent& aPointerEvent);

    IMPORT_C virtual void DisplayCoeCntrlExtension(const TUid& aEstensionUid, TAny** aExtensionParams);

private:

    /** The environment that manages this display. */
    CHuiEnv& iEnv;

    /** Display attached to this CoeControl. Not owned. */
    CHuiDisplay* iDisplay;
    
    /** Previous working rectangle of this container control. */
    TRect iPrevRect;

    TAny* iSpare;
    };

#endif  // __HUIDISPLAY_H__
