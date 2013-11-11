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
* Description:  alfperfapp gallery test case class definition.
*
*/



#ifndef ALFPERFAPPGALLERYTESTCASE_H_
#define ALFPERFAPPGALLERYTESTCASE_H_

#include <e32base.h>
#include <alf/alfimagevisual.h>
#include "alfperfapptestcase.h"
#include <alf/alfdisplaybackgrounditem.h>
#include <aknappui.h>

class CAlfImageLoaderUtil;
class CAlfLayout;
class CAlfViewportLayout;
class CAlfPerfAppGalleryTestCaseControl;

class MImagesLoadedObserver
    {
public:
    virtual void ImagesLoaded( TInt aErrorCode ) = 0;
    };

/**
 *  CAlfPerfAppGalleryTestCase class.
 */
class CAlfPerfAppGalleryTestCase : public CAlfPerfAppTestCase, public MImagesLoadedObserver
    {
public:
    /**
     * Returns ETrue if test case is supported.
     * @param aCaseId test case id.
     * @return ETrue if supported.
     */
    static TBool IsSupported( TInt aCaseId );

    static CAlfPerfAppGalleryTestCase* NewL( TInt aCaseId, TInt aSequenceIndex );
    ~CAlfPerfAppGalleryTestCase();

    // From base class CAlfPerfAppTestCase:
    virtual void SetupL( 
        CAlfEnv& aEnv, 
        const TRect& aVisibleArea, 
        TRequestStatus& aStatus );
    virtual void ExecuteL( TRequestStatus& aStatus );
    virtual void TearDown();   
    virtual void HandleVisibleAreaChange( const TRect& aRect );
    virtual TInt CaseID();

    // From MImagesLoadedObserver
    void ImagesLoaded( TInt aErrorCode );
    
private:
    
    CAlfPerfAppGalleryTestCase( TInt aCaseId, TInt aSequenceIndex );
    void ConstructL();

private:

    /**
     * Test case of which execution this instance was created.
     */ 
    TInt iCaseId;

    /**
     * Alf environment.
     * Not owned.
     */
    CAlfEnv* iEnv;

    /**
     * Test case control.
     * Not owned.
     */
    CAlfPerfAppGalleryTestCaseControl* iControl;

    /**
     * Image visuals.
     */
    RPointerArray< CAlfImageVisual > iImages;
        
    /**
     * Cycle counter.
     */        
    TInt iCycleCounter;
    /**
     * Image loader utility.
     * Owned.
     */
    CAlfImageLoaderUtil* iLoader;
    
    TRequestStatus* iStatus;
    
    /**
     * A temporary store for holding the status of orientation
     * information of the application before the test case
     * is started. The previous value is restored in teardown.
     */
    CAknAppUiBase::TAppUiOrientation iOrientation;

    };

#endif /*ALFPERFAPPGALLERYTESTCASE_H_*/
