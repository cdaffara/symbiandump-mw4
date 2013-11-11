/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  
 *
*/

#ifndef ALFPERFAPPSUITETESTCASESCROLL_H_
#define ALFPERFAPPSUITETESTCASESCROLL_H_

// INLUDES
#include "AlfPerfAppSuiteTestCaseControl.h"

// FORWARD DECLARATIONS
class CAlfImageLoaderUtil;
class CAlfImageVisual;
class CAlfViewportLayout;
class CAlfGridLayout;
class CAlfPerfAppSuiteTestCase;

// CLASS DECLARATION
class CAlfPerfAppSuiteTestCaseScroll : public CAlfPerfAppSuiteTestCaseControl
    {
private:

    enum TTextStyle
        {
        EPersonSuiteBig = 0,
        EPersonSuiteSmall,
        ENumberOfStyles
        };
public: // Construction and destrcution

    /**
     * C++ constructor.
     * @param aAppSuiteTestCase used as observer
     */
    CAlfPerfAppSuiteTestCaseScroll(CAlfPerfAppSuiteTestCase* aAppSuiteTestCase);

    /**
     * Destructor.
     */
    ~CAlfPerfAppSuiteTestCaseScroll();

    /**
     * Second phase constructor.
     * @param aEnv Alf environment.
     * @param aCaseId Testcase id.
     * @param aVisibleArea Screen area to use.
     */
    virtual void ConstructL( 
            CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea );      
public: // From CAlfPerfAppSuiteTestCaseControl

    /**
     * Starts execution.
     * If this method leaves, then request must not be completed.
     * By default, this method completes immediately.
     */		
    virtual void DoStartExecuteL();

    /**
     * Handles events.
     * @param aEvent event to be handled.
     * @return ETrue if consumed, EFalse otherwise.
     */	    
    virtual TBool OfferEventL( const TAlfEvent& aEvent );
    /**
     * Sets visible area.
     * @param aVisibleArea visible area.
     */	    
    virtual void SetVisibleArea( const TRect& aVisibleArea );
    
    /**
     * Informs that all textures have been loaded.
     * @param aErrorCode error code of texture loading
     */    
    void ImagesLoaded( TInt aErrorCode );

private: // New functions

    void CreateTextStylesL();

    /**
     * Starts the next animation cycle.
     */
    void NextCycleL();

    void ConstructPictureNumberSuiteL( 
            CAlfLayout* aParentLayout,
            const TAlfImage& aImage,
            const TInt aNumber );

    void ConstructPeopleSuiteL( 
            CAlfLayout* aParentLayout );

    CAlfGridLayout* ConstructPeopleSuiteListItemLayoutL(
            CAlfLayout* aParentLayout,
            TBool aSelected = EFalse );

    CAlfGridLayout* ConstructPeopleSuiteListItemTextLayoutL( 
            CAlfLayout* aParentLayout );

    void ConstructPeopleSuiteListHeaderL(
            CAlfLayout* aParentLayout,
            const TAlfImage& aImage,
            const TDesC& aText );

    void ConstructPeopleSuiteListItemL(
            CAlfLayout* aParentLayout,
            const TDesC& aName,
            const TDesC& aDescription,
            TBool aSelected = EFalse );    

private: // Data
    /**
     * Cycle counter.
     */        
    TInt iCycleCounter;

    /**
     * Image loader utility.
     * Owned.
     */
    CAlfImageLoaderUtil* iLoader;


    /**
     * Pointer to viewport layout.
     * Not owned.
     */
    CAlfViewportLayout* iViewPortLayout;

    /**
     * Current viewport position.
     */
    TAlfRealPoint       iViewPortPos;

    TInt iSuiteWidth;

    /**
     * Number of application suites in this testcase.
     */
    TInt iSuiteCount;


    /**
     * Lenghts of one trasition cycle.
     */
    TInt iCycleLenght;

    RArray<TInt> iTextStyles;
    
    /**
     * If it equals to iSuiteCount,
     * that means all application suites have loaded their textures.
     */
    TInt iLoadedImages;
    
    /**
     * Used for callback.
     * Not owned.
     */
    CAlfPerfAppSuiteTestCase* iAppSuiteTestCase;
    };

#endif /*ALFPERFAPPSUITETESTCASESCROLL_H_*/

// end of file

