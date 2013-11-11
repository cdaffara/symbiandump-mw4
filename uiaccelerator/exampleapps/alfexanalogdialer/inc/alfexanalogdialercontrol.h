/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   ALF control class for Analog Dialer example application, which
*                implements rotating dial pad.
*
*/



#ifndef ANALOGDIALER_CONTROL_H
#define ANALOGDIALER_CONTROL_H

#include <alf/alfcontrol.h>         // derivation from this class
#include <alf/alftexturemanager.h>  // derivation from this class

// Forward reference
class CAlfGridLayout;
class CAlfDeckLayout;

// Feedback launcher
class CAlfExAnalogDialerFeedback;

// Id of this CAlfExAnalogDialerControl
const TInt KAlfExAnalogDialerControlId = 1;

// Length of number display buffer
const TInt KNumberDisplayBufferLength = 64;

/**
 *  Control class for Analog Dialer
 *
 *  @since S60 5.0
 */
class CAlfExAnalogDialerControl :   public CAlfControl
                                    

    {
public:

    
    /* Constructors and destructor. */

    /**
     * First phase constructor of CAlfExAnalogDialerControl.
     *
     * @param aEnv ALF environment
     * @since S60 5.0
     */
    static CAlfExAnalogDialerControl* NewL(CAlfEnv& aEnv);

    /**
     * First phase constructor of CAlfExAnalogDialerControl.
     *
     * @param aEnv ALF environment
     * @since S60 5.0
     */
    static CAlfExAnalogDialerControl* NewLC(CAlfEnv& aEnv);

    /**
     * Destructor of CAlfExAnalogDialerAppUi
     *
     * @since S60 5.0
     */
    virtual ~CAlfExAnalogDialerControl();

    /**
     * Prepares for layout switch
     */
    void PrepareForLayoutSwitchL();


    /**
     * Makes layout switch. The layout switch with same directionality as previous one
     * but with different resolution is supported, too.
     *
     * @param aRect new layout for the screen area
     */
    void SwitchLayoutL();
  
private:


    /* Constructors */

    /**
     * Constructor of CAlfExAnalogDialerAppUi.
     * Private as this object must be constructed via NewL() or NewLC()
     *
     * @since S60 5.0
     */
    CAlfExAnalogDialerControl();

    /**
     * Second phase constructor of CAlfExAnalogDialerAppUi.
     *
     * @since S60 5.0
     */
    void ConstructL(CAlfEnv& aEnv);
    
 
    /* Methods. */
   
    /**
     * Creates portrait layout for application
     *
     */
    void SetupPortraitModeL();

    /**
     * Creates landspace layout for application
     *
     */
    void SetupLandscapeModeL();

// from base class CAlfControl
    /**
     * From CAlfControl.
     * Called when an input event is being offered to the control.
     *
     * @param aEvent  Event to be handled.
     *
     * @return  ETrue, if the event was handled.
     *          EFalse, otherwise
     */
    TBool OfferEventL(const TAlfEvent& aEvent);

// own methods

    /**
     * Creates root layout for application
     */
    void AddGridLayoutL();

    /**
     * Create display pane to display dialed number(s)
     */
    void AddDisplayPaneL();

    /**
     * Create dialer pane for the application
     */
    void AddDialerL();
                              
    // 
    enum TLayoutMode
        {
        ELayoutAnalogDialerPortrait=0,
        ELayoutAnalogDialerLandscape,
        };
                                
    /**
     * Resolves current layout data. Layout rectangular is retrieved from Alf environment
     *
     * @param aShorterSide smaller extent of application rectangular
     * @param aLongerSide bigger extent of application rectangular
     * 
     * @return  TLayoutMode
     * @since S60 5.0
     */
    TLayoutMode ResolveLayout(  TInt& aShorterSide,
                                TInt& aLongerSide) const;

    /**
     * Adds dialed number into number display 
     * @param aNumber number start from 0. Function adds internal +1 to the number
     */
    void AddNumberToDisplayPaneL(TInt aNumber);

    /**
     * Handles pointer down event
     * @param   aEvent event from ALF framework
     *
     * @since S60 5.0
     */
    TBool HandlePointerDownEventL(const TAlfEvent& aEvent);
    
    /**
     * Handles dragging events
     * @param   aEvent event from ALF framework
     *
     * @since S60 5.0
     */
    TBool HandleDraggingEventL(const TAlfEvent& aEvent);

    /**
     * Handles long tap event
     * @param   aEvent event from ALF framework
     *
     * @since S60 5.0
     */
    TBool HandlePointerLongTapEventL(const TAlfEvent& aEvent);

    /**
     * Handles pointer up event
     * @param   aEvent event from ALF framework
     *
     * @since S60 5.0
     */
    TBool HandlePointerUpEventL(const TAlfEvent& aEvent);

    /**
     * Toggles value of iDraggingData.iIsClockWiseOverZeroReached, if zero angle of X-axis 
     * crossed has been crossed.
     * @param aAngle of pointer event
     */
    void ResolveXAxisCrossing(TInt aAngle);
    
    /**
     * Calculate angle of triangle, when adjacent and opposite sides are known
     * @param aAdjacent length of adjacent side (x-coordinate)
     * @param aOpposite length of opposite side (y-coordinate)
     * @return angle of position in degrees
     */
    static TInt Angle(TReal32 aAdjacent, TReal32 aOpposite);
    
    /**
     * Calculate hypotenuse of triangle, when adjacent and opposite sides are known
     * @param aPosition position of pointer event
     * @return hypotenuse i.e. distance of pointer event from centre of the plate
     */
    static TInt Hypotenuse(const TPoint& aPosition);

    /**
     * Sets weights of grid for a new grid layout, or grid layout after orientation or resolution
     * change. ResolveLayout() can be used to get basic input parameters
     * 
     * aLayout      layout of application rectangular
     * aSquareSide  length of side of dialer pane, which must be square in form.
     *              Value of this must be same as shorter side of application rectangular.
     * aLongerSide  length of longer side of application rectangular
     * aPlateTop    offset of Y-coordinate of the plate
     */
    static void SetWeightsOfGridL(  CAlfGridLayout& aGridLayout,
                                    TLayoutMode     aLayout,
                                    TInt            aSquareSide,
                                    TInt            aLongerSide,
                                    TInt&           aPlateTop);
    
    /**
     * Converts position of TAlfEvent to plate centric position, in which centre of the 
     * plate represents position (0, 0). 
     * X coordinates to the left of the centre position are negative.
     * Y coordinates above the centre position are negative.
     *
     * @param aEvent pointer event from Alf
     * @param aPlateTopOffset difference between Y coordinate of top left corner of plate pane and
     *        top left corner of screen. Value is in pixels.
     * @param aSquareSide length of a side of plate pane square
     * @param aPlateCentricPoint returned position
     */
    static void ConvertEventToPlateCentricPosition(
                                        const TAlfEvent&    aEvent,
	                                    TInt                aPlateTopOffset,
	                                    TInt                aSquareSide,
	                                    TPoint&             aPlateCentricPoint);

    /**
     * Checks whether pointer event occurs between outer and inner radius
     * 
     * @param aPlateCentric position of event returned by ConvertEventToPlateCentricPosition
     * @param aOuterRadius radius of outer arc. In pixels
     * @param aInnerRadius radius of inner arc. In pixels.
     * @return ETrue if radius of event is <= aOuterRadius or >= aInnerRadius  
     *         EFalse otherwise
     */
    static TBool IsPointerEventOnArcPath(   TPoint& aPlateCentricPoint,
	                                        TInt    aOuterRadius,
	                                        TInt    aInnerRadius);

    /**
     * Checks whether pointer event follows path which is between Clear area and edges of plate
     * @param aEvent pointer event from Alf
     * @return TBool
     */
    TBool IsPointerEventFollowingNumbersArcPath(const TAlfEvent& aEvent) const;

    /**
     * Checks whether pointer event occurs on Clear area
     * @param aEvent pointer event from Alf
     * @return TBool
     */
    TBool IsPointerEventOnClearKey(const TAlfEvent& aEvent) const;

    /** 
     *  Perfoms actions needed when dragging (rotating plate) has finished.
     *  Actions are e.g. putting selected number into display, and rotating plate into idle position
     */
    void FinishDragging();
	                         
	/** 
	 * After rotating plate ends, resets data involved in calculations of turn angle of plate image.
	 */
    void ResetDraggingData();

	/** 
	 * After clear key event ends, resets data involved in Clear key event.
	 * The event started from pointer down event on Clear key area
	 */
    void ResetClearKeyData();

    /**
     * Rotates plate back to idle position 
     */
    static void RotatePlateToIdlePosition(CAlfImageVisual& aImage);

    /**
     * Rotates image to a given angle. As the angle may jump from 0->359,
     * calculation of rotation angle becomes more complicated than subtracting initial angle from 
     * current angle.
     *
     * @param aImage imagevisual to rotate
     * @param aInitialAngle initial angle of image visual
     * @param aTargetAngle current angle of image visual 
     * @param aIsClockWiseOverZeroReached whether angle has jumped from 0->359
     */
    static void RotatePlate(CAlfImageVisual&    aImage,
                            TInt                aInitialAngle,
                            TInt                aTargetAngle,
                            TBool               aIsClockWiseOverZeroReached);

    /**
     * Returns dialed number when initial angle and current angle is given.
     * @param aInitialAngle angle value between 0-360
     * @param aCurrentAngle angle value between 0-360
     * @return KErrNotFound, if plate has not rotated enough to select any value
     *         otherwise 0-9
     */
    TInt DialedNumber(  TInt aInitialAngle,
                        TInt aCurrentAngle) const;

    /**
     * Puts dialed number into number display.
     * @param aInitialAngle angle value between 0-360
     * @param aCurrentAngle angle value between 0-360
     */
    void DialedNumberToDisplay( TInt aInitialAngle,
                                TInt aCurrentAngle );

    /**
     * Clears number display; either wholly or last number.
     * @param aClearWholeDisplay clears number display wholly
     */
    void ClearNumberDisplay(TBool aClearWholeDisplay);

    /**
     * Debugs dragging data + given angle into RDebug
     */
    void DebugDraggingData(TInt aDegrees);
	                                    
private:

    // Image Ids for CAlfTextureManager
    enum TAnalogDialerImageIds
        {
        EImageIdAnalogDialerBgPt = 1,
        EImageIdAnalogDialerPlatePt,
        EImageIdAnalogDialerStopperPt,
        EImageIdAnalogDialerLast
        };

    // Tag enumeration definition. It is used in the .cpp file to index tags in tables 
    // defined .cpp file:
    // KTagArrayTheRest, which is tags for visuals, which are layout independent
    // Indexing of both arrays start from 0.
    enum TTagAnalogDialerVisual
        {
        ETagAnalogDialerRootLayout = 0,
        ETagAnalogDialerDisplayDeck,
        ETagAnalogDialerDisplayVisual,
        ETagAnalogDialerPlateDeck,
        ETagAnalogDialerNumbersVisual,
        ETagAnalogDialerPlateVisual,
        ETagAnalogDialerStopperVisual
        };
        

     // indices for KRowsAndColumnsOfGrid and KWeightsGrid arrays defined in .cpp file
    enum TRowsAndColumnsOfGrid
        {
        EColumn = 0, 
        ERow
        };

    // indices for grid elements 
    enum TIndicesGrid
        {
        EFirstGridElement = 0, 
        ESecondGridElement
        };

    /**
     * Data needed in calculating rotating angle of plate is grouped together.
     */
    struct TDraggingData
        {
        /** Initial pointer angle. It is used to calculate selected number.
         *  It is especially important, if stopper was not reached. 
         */
        TInt    iInitialAngle;

        /** Previous pointer angle. This is just used to optimise plate redrawing. 
         *  Plate is not redrawn after every dragging event
         */
        TInt    iPreviousAngle;

        /** 
         *  When plate is rotated clockwise, angle of pointer position changes from 0 to 359. 
         *  When this happens, this flag is set. When change from 359 to 0 happens,
         *  this flag is reset.
         */
        TBool   iIsClockWiseOverZeroReached;
        
        /** ETrue, if user has turned all the way to the stopper */
        TBool   iStopperReached;
        };

    /** Miscellaneous flags to memorise state of the control */
    enum TAnalogDialerControlFlags
        {
        /** Observer for long tap pointer event has been registered */
        EAnalogDialerControlFlagLongTapRegistered = 0x1,
        
        /** Long tap has been pressed */
        EAnalogDialerControlFlagLongTapPressed = 0x2,
        
        /** Set, if dragging is ongoing */
        EAnalogDialerControlDragging = 0x4,
        
        /** Set, if clear key area pressed */
        EAnalogDialerControlClearKeyEvent = 0x8,
        };

    /** Flags of this control */
    TInt                        iFlags;

    /**
     * Rotating plate image
     * Not own.
     */
    CAlfGridLayout*             iRootLayout;

    /**
     * Number display
     * Not own.
     */
    CAlfTextVisual*             iDisplayPane;
    
    /** Number current visible in the number display */
    TBuf<KNumberDisplayBufferLength> iDisplayNumber;
    
    /**
     * ELayoutAnalogDialerPt: application is running in portrait mode
     * ELayoutAnalogDialerLd: application is running in landscape mode
     */
    TLayoutMode                 iLayoutMode;
    
    /**
     * Feedback launcher
     * Own.
     */
    CAlfExAnalogDialerFeedback* iFeedback;
    
    /**
     * Rotating plate image
     * Not own.
     */
    CAlfImageVisual*            iPlate;

    /**
     * Stopper image
     * Not own.
     */
    CAlfImageVisual*            iStopper;
    
    /** ETrue, if dragging is ongoing */
    TBool                       iDragging;
    
    /** Length of the shorter edge of the plate */
    TInt                        iSquareSide; 

    /** Length of the longer edge of the application */
    TInt                        iLongerSide; 

    /** Y -coordinate of the plate */
    TInt                        iPlateTopOffset;                

    /** Data needed in calculating rotating angle of plate is grouped together.*/
    TDraggingData               iDraggingData;    
    };


#endif // ANALOGDIALER_CONTROL_H
