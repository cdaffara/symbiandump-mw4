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
* Description:   Definition of a soft key control class. CHuiSoftKeyControl 
*                is a control that displays the soft key labels and handles 
*                soft key input events.
*
*/



#ifndef __HUISOFTKEYCONTROL_H__
#define __HUISOFTKEYCONTROL_H__


#include <uiacceltk/HuiControl.h>
#include <uiacceltk/HuiSkin.h>
#include <uiacceltk/HuiImage.h>

/* Forward declarations. */
class CHuiAppLayout;
class CHuiTextVisual;
class CHuiAnchorLayout;
class CHuiImageBrush;

/** Softkey button positions. */
enum THuiSoftKeyPosition
    {
    EHuiSoftKeyUpper,
    EHuiSoftKeyLower,
    EHuiSoftKeyLeft,
    EHuiSoftKeyRight
    };


/**
 * @ deprecated: whole SoftKeyControl functionality is deprecated
 *
 * CHuiSoftKeyControl displays softkey labels and listens for key input.
 *
 * Soft key controls always recide either on the bottom of the UI
 * (in portrait mode) or at the right side of the UI (in landscape mode).
 * No other alignments are currently supported in the Toolkit.
 *
 * Soft keys automatically align and animate themselves when used. When
 * mode is changed from portrait to landscape or vice versa the soft
 * key controls first exit to the edge where they were aligned and enter
 * from the new location edge. The exit-enter animation is also used to
 * to change the softkey labels.
 *
 * The application user can animate the soft keys manually by calling
 * methods in CHuiSoftKeyControl: EnterView() and ExitView()
 *
 * Furthermore animation duration times can be controlled using
 * SetAnimationTimes() -method. If user want's to disable the
 * animations altogether (?) she/he can set the animation duration
 * times to zero.
 */
NONSHARABLE_CLASS(CHuiSoftKeyControl) : public CHuiControl
	{
public:

    /** @beginAPI */

    /**
	 * @ deprecated
     *
	 * Mask flags to indicate the controlled softkey label.
     * These flags can be used as parameter to the EnterView()
     * and ExitView() methods.
     */
    enum THuiSoftkeyLabel
        {
        ESoftkeyLabelLeft = 0x1,
        ESoftkeyLabelRight = 0x2
        };

	/* Constructors and destructor. */

	/**
	 * @ deprecated
     *
	 * Construct a new softkey control.
	 */
	IMPORT_C static CHuiSoftKeyControl* NewL(CHuiEnv& aEnv);

	/**
	 * @ deprecated
     *
	 * Construct a new softkey control and leave it on the cleanup stack.
	 */
	IMPORT_C static CHuiSoftKeyControl* NewLC(CHuiEnv& aEnv);

	/**
	 * @ deprecated
     *
     * Second-phase constructor for constructing the control from a resource
	 * definition.
	 *
	 * @param aReader  Resource reader pointing to the beginning of the
	 *                 control's data.
	 */
	//void ConstructFromResourceL(TResourceReader& aReader);

	/**
	 * @ deprecated
     *
	 * Destructor.
	 */
	IMPORT_C virtual ~CHuiSoftKeyControl();


	/* Methods. */

	/**
	 * @ deprecated
     *
	 * Sets the text of the button. The method automatically animates the
     * soft key control so that the changed button label is first exit from
     * the view and later entered back with new label text.
	 *
	 * @param aKeyPos         Position of the key to change.
	 * @param aText           Softkey label text.
	 * @param aAnimateControl If this argument is true, the whole soft key
     *                        control animates (exits and enters view).
     *                        Otherwise only the altered softkey label
     *                        animates.
	 */
	IMPORT_C void SetLabelL(THuiSoftKeyPosition aKeyPos, const TDesC& aText,
                            TBool aAnimateControl = EFalse);

    /**
	 * @ deprecated
     *
     * Sets background image to the softkeys.
     *
     * @param aImage            The background image.
     * @param aLeftBorderWidth  Width of left border section of the background,
     *                          in pixels. This determines should background be
     *                          expanded to the left, over the edge of the
     *                          visual. Set to  zero to draw only inside
     *                          the softkey label.
     * @param aLeftBorderWidth  Width of right border section of the background,
     *                          in pixels. This determines should background be
     *                          expanded to the right, over the edge of the
     *                          visual. Set to zero to draw only inside
     *                          the softkey label.
     */
    IMPORT_C void SetBackgroundL(THuiImage aImage, TInt aLeftBorderWidth, TInt aRightBorderWidth);

	/**
	 * @ deprecated
     *
	 * Sets the softkey animation duration times. The duration time specifies
     * how fast softkeys enter / exit from the view. Both enter and exit
     * animation duration times default to 500 milliseconds.
	 *
	 * @param aEnterTransition Time taken for softkey control to enter the view.
     *                         Time duration is specified in milliseconds.
	 * @param aExitTransition  Time taken for softkey control to exit the view.
     *                         Time duration is specified in milliseconds.
	 */
	IMPORT_C void SetAnimationTimes(TUint aEnterTransition, TUint aExitTransition);

	/**
	 * @ deprecated
     *
	 * Softkey enters view using entering animation.
	 *
	 * @param aLabelMask Bitwise mask parameter constructed of enumerators in
     *                   THuiSoftkeyLabel. With this parameter user can define
     *                   whether to animate left softkey, right softkey or both
     *                   (default).
     * @see THuiSoftkeyLabel.
	 */
	IMPORT_C void EnterView(TUint aLabelMask = ESoftkeyLabelLeft | ESoftkeyLabelRight);

	/**
	 * @ deprecated
     *
	 * Soft key exists view using exit animation.
	 *
	 * @param aLabelMask Bitwise mask parameter constructed of enumerators in
     * THuiSoftkeyLabel. With this parameter user can define whether to animate
     * left softkey, right softkey* or both (default). @see THuiSoftkeyLabel
	 */
	IMPORT_C void ExitView(TUint aLabelMask = ESoftkeyLabelLeft | ESoftkeyLabelRight);

    /**
	 * @ deprecated
     *
     * Sets the background type of the softkey control. Affects text labels.
     * 
     * @param aBackgroundType  Background type.
     */
    IMPORT_C void SetBackgroundType(THuiBackgroundType aBackgroundType);

    /** @endAPI */

	/**
	 * @ deprecated
     *
	 * Event handler.
	 */
	virtual TBool OfferEventL(const THuiEvent& aEvent);

	/**
	 * @ deprecated
     *
	 * Notifies the soft key control of updated layout of its children.
	 * The positioning of the labels is different depending on display
	 * orientation.
	 */
	void VisualLayoutUpdated(CHuiVisual& aVisual);


protected:

    /* Constructors. */

	/**
	 * @ deprecated
     *
	 * Constructor.
	 */
	CHuiSoftKeyControl(CHuiEnv& aEnv);

	/**
	 * @ deprecated
     *
	 * Second-phase constructor.
	 */
	void ConstructL();


private:

    /* Private methods */

    /**
     * Resets the anchor points of the left and right softkey label
     * according to the location where softkeys should be rendered.
     *
     * @param aSkinLocation One of the four locations where softkeys are drawn
     * (top, bottom, left, right).
     * @param aShown Boolean value indicating whether the softkeys should be
     * visible or hidden.
     * @param aLabelMask Mask which defines the modified keys (left, right or both labels). @see THuiSoftkeyLabel.
     */
    void SetAnchorPoints(THuiSkinLocation aSkinLocation,
                         TBool aShown = ETrue,
                         TUint aLabelMask = ESoftkeyLabelLeft | ESoftkeyLabelRight);

    /**
     * Exits the soft key control. Exit is done to the location specified in the parameter.
     *
     * @param aTarget Location where the soft keys are exited.
     * @param aLabelMask Mask which defines the keys to exit (left, right or both labels). @see THuiSoftkeyLabel.
     * @return Time for exit transition. This can be zero or current exit transition time
     * depending on whether any animation is played or not.
     */
    TUint ExitView(THuiSkinLocation aTarget,
                   TUint aLabelMask = ESoftkeyLabelLeft | ESoftkeyLabelRight);

    /**
     * Custom events that drive the animations in soft key control.
     */
    enum TCustomEvent
        {
        ECustomEventEnterView = 200,
        ECustomEventUpdateLabel,
        ECustomEventUpdateLabels
        };

private:

    /** Label of the left key. */
    CHuiTextVisual* iLeftLabel;
    HBufC* iLeftText;
    /** Background brush for the left key. */
    CHuiImageBrush * iLeftLabelImageBrush;

    /** Label of the right key. */
    CHuiTextVisual* iRightLabel;
    HBufC* iRightText;
    /** Background brush for the right key. */
    CHuiImageBrush * iRightLabelImageBrush;

    /** Grid layout used to align the label visuals */
    CHuiAnchorLayout* iLayout;

    /** Current skin location. This is initialized to none. */
    THuiSkinLocation iCurrentLocation;

    /** Exit transition duration time in milliseconds */
    TUint iExitTransitionTime;

    /** Enter transition duration time in milliseconds */
    TUint iEnterTransitionTime;
	};

#endif  // __HUISOFTKEYCONTROL_H__
