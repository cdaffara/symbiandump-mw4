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
* Description:  alfperfapp cover test case class definition.
*
*/


#ifndef C_ALFPERFAPPCOVERTESTCASE_H
#define C_ALFPERFAPPCOVERTESTCASE_H

#include <e32base.h>
#include <aknappui.h>
#include <alf/alfimagevisual.h>
#include <alf/alftexturemanager.h>

#include "alfperfapptestcase.h"


class CAlfImageLoaderUtil;
class CAlfLayout;
class CAlfViewportLayout;

class CAlfPerfAppCoverTestCaseControl;


/**
 *  CAlfPerfAppCoverTestCase class for measuring the frame rate
 *  when images are scrolled left and right and scaled.
 */
class CAlfPerfAppCoverTestCase 
    : public CAlfPerfAppTestCase,
      public MAlfTextureLoadingCompletedObserver
    {
public:
    /**
     * Returns ETrue if test case is supported.
     * @param aCaseId test case id.
     * @return ETrue if supported.
     */
    static TBool IsSupported( TInt aCaseId );

    static CAlfPerfAppCoverTestCase* NewL( TInt aCaseId, TInt aSequenceIndex);
    virtual ~CAlfPerfAppCoverTestCase();

    // From base class CAlfPerfAppTestCase:
    virtual void SetupL( 
        CAlfEnv& aEnv, 
        const TRect& aVisibleArea, 
        TRequestStatus& aStatus );
    virtual void ExecuteL( TRequestStatus& aStatus );
    virtual void TearDown();   
    virtual void HandleVisibleAreaChange( const TRect& aRect );
    
    // From base class MAlfTextureLoadingCompletedObserver
    void TextureLoadingCompleted( 
        CAlfTexture& aTexture, TInt aTextureId, TInt aErrorCode );
    virtual TInt CaseID();
    
protected:
    
    CAlfPerfAppCoverTestCase( TInt aCaseId, TInt aSequenceIndex );
    void ConstructL();

private:
private:
    /**
     * Test case of which execution this instance was created.
     */ 
    TInt iCaseId;

    /**
     * A temporary store for holding the status of orientation
     * information of the application before the test case
     * is started. The previous value is restored in teardown.
     */
    CAknAppUiBase::TAppUiOrientation iOrientation;
    
    /**
     * Alf environment.
     * Not owned.
     */
    CAlfEnv* iEnv;

    /**
     * Test case control.
     * Not owned.
     */
    CAlfPerfAppCoverTestCaseControl* iControl;
    
private:
    
#if 0
    /**
     * Next cycle.
     */
    void NextCycleL();
    
    /**
     * Returns opacity for index.
     */
    TReal32 OpacityForIndex( TInt aIndex );
    
    /**
     * Returns size for index.
     */
    TAlfRealSize SizeForIndex( TInt aIndex );
    
    /**
     * Returns position for index.
     */
    TAlfRealPoint PositionForIndex( TInt aIndex, const TAlfRealSize& aSize );
#endif

private:

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

    TInt iLoadedImages;
    TRequestStatus* iStatus;

    };

#endif // C_ALFPERFAPPCOVERTESTCASE_H

