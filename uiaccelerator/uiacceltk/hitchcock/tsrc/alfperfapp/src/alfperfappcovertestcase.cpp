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
* Description:  CAlfPerfAppCoverTestCase implementation.
*
*/


#include "alfperfappcovertestcase.h"
#include "alfperfapp.hrh"

#include <aknutils.h>
#include <gdi.h>
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
#include <alf/alfgc.h>
#include <alf/alfdisplay.h>
#include <alf/alfcontrol.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfroster.h>
#include <alf/alfcommand.h>
#include <alf/alfevent.h>
#include <alf/alftexturemanager.h>
#include <alf/alfimagevisual.h>
#include <alf/alftextvisual.h>
#include <alf/alfimageloaderutil.h>
#include <alf/alfutil.h>
#include <alf/alflayout.h>
#include <alf/alfviewportlayout.h>
#include <alf/alfgridlayout.h>
#include <alf/alfflowlayout.h>
#include <alf/alfdecklayout.h>
#include <alf/alftransformation.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfborderbrush.h>
#include <alf/alfbrusharray.h>

#include <alfperfapp_imagetest.mbg>

#include <alfperfappappui.h>


/**
 * Control group for Cover test cases.
 */
const TInt KAlfPerfAppCoverControlGroup = 2;

/**
 * Complete now command.
 */
const TInt KAlfPerfAppCoverCmdCompleteNow = 0x6000;


/**
 * Cover flow test case control.
 */
class CAlfPerfAppCoverTestCaseControl : public CAlfControl
    {
public:
        
    // Common constants
    static const TInt KTransitionTime;
    static const TInt KTransitionWaitTime;
    static const TInt KRotationAngle;

    static CAlfPerfAppCoverTestCaseControl* NewL(
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea );

protected:
    /**
     * Constructor.
     */
    CAlfPerfAppCoverTestCaseControl();
        
    /**
     * Base class constructor. Derived classes may
     * override this, but base class needs to be called.
     */
    virtual void ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea );
        
public:
    /**
     * Destructor.
     */
    ~CAlfPerfAppCoverTestCaseControl();
        
    /**
     * Starts execution of the test case. By default,
     * request status is stored to local variable.
     */
    void StartExecuteL( TRequestStatus& aStatus );    
    
    /**
     * Cancels execution. This control and environment will be
     * deleted soon after calling this method.
     */
    void CancelExecution();
    
    /**
     * Starts execution.
     * If this method leaves, then request must not be completed.
     * By default, this method completes immediately.
     */
    virtual void DoStartExecuteL();
    
    /**
     * Sets visible area.
     * @param aVisibleArea visible area.
     */
    virtual void SetVisibleArea( const TRect& aVisibleArea );    
    
    // From base class CAlfControl:
    
    /**
     * Handles events.
     * Derived classes should forward to base class.
     * @param aEvent event to be handled.
     * @return ETrue if consumed, EFalse otherwise.
     */
    virtual TBool OfferEventL( const TAlfEvent& aEvent );
    
protected:
    /**
     * Next cycle.
     */
    void NextCycleL();

    /**
     * Changes the view port left or right according to the given amount. 
     * The amount can be negative or positive.
     */
    void MoveImages( 
        TReal32 aAmount, 
        TReal32 aTransitionTime = KTransitionTime );

    /**
     * Changes the scaling. The current image is scaled to normal size.
     * The previous image is scaled down. 
     * This should be called when images are moved.
     * @see MoveImages
     */
    void UpdateScaling( TReal32 aTransitionTime = KTransitionTime );
    
    /**
     * Rotates Image from 0to 360 and 0 to -360 in alternate cycles.
     */
    void RotateImage( );
    
    /**
     * Clips the display layout, zooms into the image by scaling it to
     * multiples of cycle count and also moves the image with X and Y offset of 5.
     * It gives the Clip, zoom and panning effect to image.
     */
    void ClipandZoom( );
    
    /**
     * A way to round floating point number to integer.
     */
    TInt RoundFloatToInt( TReal32 aVal );
    
    /**
     * Completes automatically after specified duration.
     * @param aDuration duration in ms
     */
    void CompleteAfterL( TInt aDuration );
        
    /**
     * Completes current request.
     */
    void CompleteNow( TInt aErrorCode );

    /**
     * Returns test case id.
     */
    inline TInt CaseId() const;
        
    /**
     * Returns ETrue if test case execution is still ongoing.
     */
    inline TBool IsExecutionOngoing() const;
    
    /**
     * Test case id.
     */
    TInt iCaseId;
    
private:
    TInt iCycleCount;
    
    /**
     * Pointer to request status.
     * Not owned.
     */
    TRequestStatus* iStatus;
        
    CAlfLayout* iLayout;
    TReal32 iViewportPosition;
    TReal32 iPreviousViewportPosition;
    CAlfViewportLayout* iViewport;
    };

        
// ============================ MEMBER FUNCTIONS ===============================
//
// -----------------------------------------------------------------------------
// Checks if specified case is supported by this class.
// -----------------------------------------------------------------------------
//
TBool CAlfPerfAppCoverTestCase::IsSupported( TInt aCaseId )
    {
    return aCaseId == EAlfPerfAppCover 
        || aCaseId == EAlfPerfAppCoverLandscape
        || aCaseId == EAlfPerfAppCoverRotate
        || aCaseId == EAlfPerfAppCoverClipZoomLargeImage
        || aCaseId == EAlfPerfAppCoverClipZoomBigImage    
        || aCaseId == EAlfPerfAppCoverClipZoomVga
        || aCaseId == EAlfPerfAppCoverClipZoomAutoSize;
    }

CAlfPerfAppCoverTestCase* CAlfPerfAppCoverTestCase::NewL( TInt aCaseId, TInt aSequenceIndex )
    {
    if ( !IsSupported( aCaseId ) )
        {
        User::Leave( KErrNotSupported );
        }

    CAlfPerfAppCoverTestCase* self = 
        new (ELeave) CAlfPerfAppCoverTestCase( aCaseId, aSequenceIndex);
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfPerfAppCoverTestCase::~CAlfPerfAppCoverTestCase()
    {
    if ( iEnv && iControl )
        {
        iEnv->TextureManager().RemoveLoadObserver( this );
        iControl->CancelExecution();
        iEnv->DeleteControlGroup( KAlfPerfAppCoverControlGroup );
        }
    }

// -----------------------------------------------------------------------------
// Starts setup phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppCoverTestCase::SetupL( 
        CAlfEnv& aEnv, const TRect& /*aVisibleArea*/, TRequestStatus& aStatus )
    {
    iEnv = &aEnv;
    // the status pointer is taken for completing it 
    // when all the images are loaded.
    iStatus = &aStatus;
    
    CAlfPerfAppAppUi* appUi = (CAlfPerfAppAppUi*) EikonEnv()->AppUi();
    iOrientation = appUi->Orientation();
    
    switch ( iCaseId )
        {
        case EAlfPerfAppCover:
            {
            appUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationPortrait );    
            break;
            }

        case EAlfPerfAppCoverLandscape:
            {
            appUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationLandscape );    
            break;
            }
        case EAlfPerfAppCoverRotate:
        case EAlfPerfAppCoverClipZoomLargeImage:
        case EAlfPerfAppCoverClipZoomBigImage:
        case EAlfPerfAppCoverClipZoomVga:
        case EAlfPerfAppCoverClipZoomAutoSize:
            {         
            break;
            }    
        default:
            User::Leave( KErrNotSupported );
            break;
        }
    

    TRect rect;
    AknLayoutUtils::LayoutMetricsRect( 
        AknLayoutUtils::EApplicationWindow, 
        rect );
    
    iEnv->SetFullScreenDrawing( ETrue );
    CAlfDisplay& display = 
        iEnv->NewDisplayL( rect, CAlfEnv::ENewDisplayFullScreen );

#if 1
    // black seems to be the default color
    display.SetClearBackgroundL( CAlfDisplay::EClearWithColor );
#else
    // for development/debugging purposes it is easier to see different
    // components when there is a background.
    display.SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );
#endif

    CAlfControlGroup& group = 
        iEnv->NewControlGroupL( KAlfPerfAppCoverControlGroup );

    iLoadedImages = 0;
    // the setup phase is completed only after the all images are 
    // loaded by the texture manager. see the construction of the
    // control class.
    aEnv.TextureManager().AddLoadObserverL( this );

    iControl = CAlfPerfAppCoverTestCaseControl::NewL(
            *iEnv, 
            iCaseId, 
            rect );

    TRAPD(error,group.AppendL( iControl );) // ownership passed to control group.
    if(error != KErrNone)
        {
        delete iControl;
        iControl = 0;
        }
    
    
    display.Roster().ShowL( group );
    }

// -----------------------------------------------------------------------------
// Starts execution phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppCoverTestCase::ExecuteL( TRequestStatus& aStatus )
    {
    // SetupL must have been called first.
    __ASSERT_ALWAYS( iEnv, User::Invariant() );
    __ASSERT_ALWAYS( iControl, User::Invariant() );
    
    iControl->StartExecuteL( aStatus );
    }

// -----------------------------------------------------------------------------
// Tears down.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppCoverTestCase::TearDown()
    {
    CAlfPerfAppAppUi* appUi = (CAlfPerfAppAppUi*) EikonEnv()->AppUi();
    TRAP_IGNORE( appUi->SetOrientationL( iOrientation ) );    

    // Execution side will delete CAlfEnv, so it will delete everything
    // related to this.
    iEnv = NULL;
    iControl = NULL;
    }

void CAlfPerfAppCoverTestCase::HandleVisibleAreaChange( const TRect& aRect )
    {
    if ( iEnv && iControl )
        {
        if ( iEnv->DisplayCount() > 0 )
            {
            iEnv->PrimaryDisplay().SetVisibleArea( aRect );
            }

        iControl->SetVisibleArea( aRect );
        }
    }
  
TInt CAlfPerfAppCoverTestCase::CaseID()
    {
    return iCaseId;
    }

CAlfPerfAppCoverTestCase::CAlfPerfAppCoverTestCase( TInt aCaseId, TInt aSequenceIndex )
    : CAlfPerfAppTestCase(aSequenceIndex), iCaseId ( aCaseId )
    {
    }

void CAlfPerfAppCoverTestCase::ConstructL()
    {
    }

const TInt KNumberOfImages = 4;

void CAlfPerfAppCoverTestCase::TextureLoadingCompleted(
    CAlfTexture& /*aTexture*/, TInt /*aTextureId*/, TInt aErrorCode)
    {
    if( aErrorCode != KErrNone && iStatus )
        {
        User::RequestComplete( iStatus, aErrorCode );
        iStatus = NULL;
        }
    iLoadedImages++;
    TInt noOfImagesToLoad = 1;
    if( CaseID() == EAlfPerfAppCover || 
        CaseID() == EAlfPerfAppCoverLandscape )
        {
        noOfImagesToLoad = KNumberOfImages;
        }
    if ( iStatus && ( iLoadedImages == noOfImagesToLoad ) )
        {
        User::RequestComplete( iStatus, KErrNone );
        iStatus = NULL;
        }
    }
        
// Implementation of CAlfPerfAppCoverTestCaseControl:
const TInt CAlfPerfAppCoverTestCaseControl::KTransitionTime = 1000;
const TInt CAlfPerfAppCoverTestCaseControl::KTransitionWaitTime = 1000;
const TInt CAlfPerfAppCoverTestCaseControl::KRotationAngle = 360;

CAlfPerfAppCoverTestCaseControl* CAlfPerfAppCoverTestCaseControl::NewL(  
    CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea )
    {
    CAlfPerfAppCoverTestCaseControl* 
        result = new (ELeave) CAlfPerfAppCoverTestCaseControl;
    CleanupStack::PushL( result );
    result->ConstructL( aEnv, aCaseId, aVisibleArea );
    CleanupStack::Pop();
    return result;
    }
        
CAlfPerfAppCoverTestCaseControl::CAlfPerfAppCoverTestCaseControl()  
    {
    }
        
CAlfPerfAppCoverTestCaseControl::~CAlfPerfAppCoverTestCaseControl()
    {
    CompleteNow( KErrCancel );
    }

const TInt KShadowWidth = 20; // in pixels


static void AddBorderBrushL( 
    CAlfEnv& aEnv,
    CAlfVisual& aVisual, 
    const TRgb& aColor,
    TInt aThickness, 
    TInt aOffset )
    {
    aVisual.EnableBrushesL();
    CAlfBorderBrush* brush = CAlfBorderBrush::NewLC(
        aEnv,
        aThickness, 
        aThickness,
        aOffset,
        aOffset );
    brush->SetColor( aColor );
    aVisual.Brushes()->AppendL( brush, EAlfHasOwnership );
    CleanupStack::Pop( brush );
    }

static void AddFrameL( CAlfEnv& aEnv, CAlfVisual& aVisual )
    {
    const TInt KNarrowBorderThickness = 2;
    const TInt KBorderThickness = 4;
    AddBorderBrushL( 
        aEnv, 
        aVisual, 
        TRgb( 75, 75, 75 ), 
        KNarrowBorderThickness, 
        KNarrowBorderThickness );
    AddBorderBrushL( 
        aEnv, 
        aVisual, 
        TRgb( 55, 55, 55 ), 
        KBorderThickness, 
        KNarrowBorderThickness + KBorderThickness );
    AddBorderBrushL( 
        aEnv, 
        aVisual, 
        TRgb( 25, 25, 25 ), 
        KNarrowBorderThickness, 2*KNarrowBorderThickness + KBorderThickness );
    }

const TReal32 KFocusedImageScale = 1.0f;
const TReal32 KSmallImageScale = .65f;

static void DownScaleImageL( CAlfVisual& aVisual, TInt aTransitionTime )
    {
    TAlfTimedValue presser;
    presser.SetValueNow( KFocusedImageScale );
    presser.SetTarget( KSmallImageScale, aTransitionTime );
    CAlfTransformation& lessening = aVisual.Transformation();
    lessening.LoadIdentity();
    lessening.Scale( presser, presser );
    }

static CAlfGridLayout* AddItemL( 
    CAlfEnv& aEnv,
    CAlfControl& aControl, 
    CAlfLayout& aLayout, 
    CAlfTexture& aTexture )
    {
    // grid size: two lines, one item on the line
    CAlfGridLayout* item = CAlfGridLayout::AddNewL( aControl, 1, 1, &aLayout );

    // solid background for the item
    item->EnableBrushesL();
    CAlfGradientBrush* brush = CAlfGradientBrush::NewLC( aEnv );
    brush->SetColor( TRgb( 60, 60, 60 ) ); // some gray
    item->Brushes()->AppendL( brush, EAlfHasOwnership );
    CleanupStack::Pop( brush );
    
    RArray<TInt> weights;
    CleanupClosePushL( weights );
    // this makes the image space much larger than the text box below it.
    User::LeaveIfError( weights.Append( 100 ) );
    User::LeaveIfError( weights.Append( 15 ) );
    item->SetRowsL( weights );
    CleanupStack::PopAndDestroy();
    
    CAlfImageVisual* visual = CAlfImageVisual::AddNewL( aControl, item );
    visual->SetImage( TAlfImage( aTexture ) );
    visual->EnableTransformationL();

    return item;
    }

static CAlfGridLayout* AddItemTextL( 
    CAlfControl& aControl, 
    CAlfLayout& aParent, 
    const TDesC& aArtist,
    const TDesC& aTitle )
    {
    // creates a grid for holding text lines.
    // grid size: two lines, one item on the line
    CAlfGridLayout* texts = CAlfGridLayout::AddNewL( aControl, 1, 2, &aParent );
    texts->EnableTransformationL();

    CAlfTextVisual* upper = CAlfTextVisual::AddNewL( aControl, texts );
    upper->SetStyle( EAlfTextStyleNormal, EAlfBackgroundTypeDark );
    upper->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
    upper->SetTextL( aArtist );
    CAlfTextVisual* lower = CAlfTextVisual::AddNewL( aControl, texts );
    lower->SetStyle( EAlfTextStyleSmall );
    lower->SetWrapping( CAlfTextVisual::ELineWrapTruncate );
    lower->SetTextL( aTitle );

    return texts;
    }

static void GetImageTestCasePrivatePath( TFileName& aPath )
    {
    CEikonEnv::Static()->FsSession().PrivatePath( aPath );
    ::CompleteWithAppPath( aPath );
    }

const TReal32 KViewportOffset = -.25f;
const TReal32 KViewportOffsetY = -.25f;

void CAlfPerfAppCoverTestCaseControl::ConstructL( 
    CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea )
    {
    iCaseId = aCaseId;
    CAlfControl::ConstructL( aEnv );

    TFileName privatePath;
    GetImageTestCasePrivatePath( privatePath );
    aEnv.TextureManager().SetImagePathL( privatePath );

    // this is the top most visual
    CAlfDeckLayout* top = CAlfDeckLayout::AddNewL( *this );

    // view port for scrolling
    // this viewport needs to be added first for the deck
    // to be under the shadows.
    iViewport = CAlfViewportLayout::AddNewL( *this, top );
    iViewport->SetClipping( EFalse );
    
    iViewport->SetViewportSize( TAlfRealSize( 1.5, 1.5 ), 0 );
    iViewport->SetViewportPos( 
        TAlfRealPoint( 
            iViewportPosition + KViewportOffset, 
            KViewportOffsetY ), 
        0 );
    // building the grid for holding images and stuff scrolled
     iLayout = CAlfGridLayout::AddNewL( 
          *this, 
          1, 
          1,  // just one line of images
          iViewport );   
     TBuf<50> KImage = _L("alfphotos_small1.jpg");
     TInt textureFlag = EAlfTextureFlagRetainResolution;
     switch(CaseId())
             {
             case EAlfPerfAppCover:
             case EAlfPerfAppCoverLandscape:         
                  {
                  // one liner grid with left shadow, empty space, and right shadow
                  CAlfGridLayout* shadows = CAlfGridLayout::AddNewL( *this, 3, 1, top );
            
                  RArray<TInt> weights;
                  CleanupClosePushL( weights );
                  // the shadows are only small portition of 
                  // the size of the empty space between them
                  User::LeaveIfError( weights.Append( 5 ) );
                  User::LeaveIfError( weights.Append( 200 ) );
                  User::LeaveIfError( weights.Append( 5 ) );
                  shadows->SetColumnsL( weights );
                  CleanupStack::PopAndDestroy();
                
                  // building of the left side shadow
                  CAlfImageVisual* leftShadow = CAlfImageVisual::AddNewL( *this, shadows );
                  leftShadow->SetFlag( EAlfVisualFlagManualSize );
                  leftShadow->SetSize( 
                     TAlfRealPoint( KShadowWidth, aVisibleArea.Height() ) );
                  leftShadow->EnableBrushesL();
                  CAlfGradientBrush* leftBrush = CAlfGradientBrush::NewLC( aEnv );
                  leftBrush->SetLayer( EAlfBrushLayerForeground );
                  leftBrush->SetColor( KRgbBlack, 1.f );
                  leftBrush->AppendColorL( 0.f, KRgbBlack, 1.f );
                  leftBrush->AppendColorL( 1.0f, KRgbBlack, 0.f );
                  leftShadow->Brushes()->AppendL( leftBrush, EAlfHasOwnership );
                  CleanupStack::Pop( leftBrush );
            
                  // this is just an empty visual to get the layout correct,
                  // shadows take small portitions on left and right.
                  // middle part of the screen is left empty for the viewport.
                  CAlfImageVisual* emptySpace = CAlfImageVisual::AddNewL( *this, shadows );
            
                  // building of the right side shadow
                  CAlfImageVisual* rightShadow = CAlfImageVisual::AddNewL( *this, shadows );
                  rightShadow->SetFlag( EAlfVisualFlagManualSize );
                  rightShadow->SetSize( 
                    TAlfRealPoint( KShadowWidth, aVisibleArea.Height() ) );
                  rightShadow->EnableBrushesL();
                  CAlfGradientBrush* rightBrush = CAlfGradientBrush::NewLC( aEnv );
                  rightBrush->SetLayer( EAlfBrushLayerForeground );
                  rightBrush->SetColor( KRgbBlack, 0.f );
                  rightBrush->AppendColorL( .0f, KRgbBlack, 0.f );
                  rightBrush->AppendColorL( 1.0f, KRgbBlack, 1.f );
                  rightShadow->Brushes()->AppendL( rightBrush, EAlfHasOwnership );
                  CleanupStack::Pop( rightBrush );
                  
                  // NOTE: Now using small images for OpenVG, because of memory issues.
                  // These files may be replaced for BitGDI if needed.
                  _LIT( KImage2, "alfphotos_small2.jpg");
                  _LIT( KImage3, "alfphotos_small3.jpg");
                  _LIT( KImage4, "alfphotos_small4.jpg");
                  KImage = _L("alfphotos_small1.jpg");
                  
                  static_cast<CAlfGridLayout* >( iLayout )->SetColumns( KNumberOfImages );
                  iViewport->SetVirtualSize( TAlfRealSize( KNumberOfImages, 1.0 ), 0 );               
            
                  CAlfTexture& texture2 = aEnv.TextureManager().LoadTextureL(
                    KImage2, 
                    TAlfTextureFlags( textureFlag ), 
                    KAlfAutoGeneratedTextureId );
                
                  CAlfTexture& texture3 = aEnv.TextureManager().LoadTextureL(
                     KImage3, 
                     TAlfTextureFlags( textureFlag ), 
                     KAlfAutoGeneratedTextureId );
                
                  CAlfTexture& texture4 = aEnv.TextureManager().LoadTextureL(
                     KImage4, 
                     TAlfTextureFlags( textureFlag ),
                     KAlfAutoGeneratedTextureId );
                
                  CAlfGridLayout* visual2 = AddItemL( aEnv, *this, *iLayout, texture2 );
                  visual2->EnableTransformationL();
                  AddFrameL( aEnv, *visual2 );
               
                  CAlfGridLayout* visual3 = AddItemL( aEnv, *this, *iLayout, texture3 );
                  visual3->EnableTransformationL();
                  AddFrameL( aEnv, *visual3 );
                
                  CAlfGridLayout* visual4 = AddItemL( aEnv, *this, *iLayout, texture4 );
                  visual4->EnableTransformationL();
                  AddFrameL( aEnv, *visual4 );                  
                
                  CAlfGridLayout* text2 = AddItemTextL( 
                     *this, *visual2, _L( "Gwen Stefani" ), _L("Hollaback Girl") );
            
                  CAlfGridLayout* text3 = AddItemTextL( 
                     *this, *visual3, _L( "No Doubt" ), _L("Don't Speak") );
            
                  CAlfGridLayout* text4 = AddItemTextL( 
                     *this, *visual4, _L( "Jenni Vartiainen" ), _L("Ihmisten edessä") );     
                
                  DownScaleImageL( *visual2, 0 );
                  DownScaleImageL( *visual3, 0 );
                  DownScaleImageL( *visual4, 0 );
                  break;
                  }   
              case EAlfPerfAppCoverRotate:
                  {                      
                  KImage = _L("alfphotos_small1.jpg");
                  break;
                  }    
              case EAlfPerfAppCoverClipZoomLargeImage:
                  {
                  KImage = _L("alfphotos_5mpx.jpg");
                  iLayout->SetClipping( ETrue );
                  iViewport->SetClipping( ETrue );
                  top->SetClipping( ETrue );
                  break;
                  }
              case EAlfPerfAppCoverClipZoomBigImage:
                  {
                  KImage = _L("alfphotos_2mpx.jpg");
                  iLayout->SetClipping( ETrue );
                  iViewport->SetClipping( ETrue );
                  top->SetClipping( ETrue );
                  break;
                  }
              case EAlfPerfAppCoverClipZoomVga:
                  {
                  KImage = _L("alfphotos_vga.jpg");
                  iLayout->SetClipping( ETrue );
                  iViewport->SetClipping( ETrue );
                  top->SetClipping( ETrue );
                  break;
                  }
              case EAlfPerfAppCoverClipZoomAutoSize:
                  {
                  KImage = _L("alfphotos_small9.jpg");
                  iLayout->SetClipping( ETrue );
                  iViewport->SetClipping( ETrue );
                  top->SetClipping( ETrue );
                  textureFlag |= EAlfTextureFlagAutoSize;
                  break;
                  }
             }
             CAlfTexture& texture1 = aEnv.TextureManager().LoadTextureL(
                    KImage, 
                    TAlfTextureFlags( textureFlag ), 
                    KAlfAutoGeneratedTextureId );
             CAlfGridLayout* visual1 = AddItemL( aEnv, *this, *iLayout, texture1 );
             CAlfGridLayout* text1 = AddItemTextL( 
                    *this, *visual1, _L( "TikTak" ), _L("Sinkut 99 - 07 Disc 1") );
             visual1->EnableTransformationL();                    
             visual1->Visual( 0 ).SetClipping( EFalse );
             if ( CaseId() == EAlfPerfAppCover ||
                  CaseId() == EAlfPerfAppCoverLandscape )
                 {
                 AddFrameL( aEnv, *visual1 );
                 DownScaleImageL( *visual1, 0 );
                 }     
    }
        
void CAlfPerfAppCoverTestCaseControl::StartExecuteL( TRequestStatus& aStatus )
    {
    iCycleCount = 0;
    iViewportPosition = 0; 
    iPreviousViewportPosition = -1; // no previous position
    
    iStatus = &aStatus;
    *iStatus = KRequestPending;
    
    TRAPD( err, DoStartExecuteL() );
    if ( err != KErrNone )
        {
        iStatus = NULL;
        User::Leave( err );
        }
    }

void CAlfPerfAppCoverTestCaseControl::CancelExecution()
    {
    CompleteNow( KErrCancel );
    }

void CAlfPerfAppCoverTestCaseControl::DoStartExecuteL()
    {
    NextCycleL();
    CompleteAfterL( KTransitionWaitTime );
    }

void CAlfPerfAppCoverTestCaseControl::SetVisibleArea( 
        const TRect& /*aVisibleArea*/ )
    {
    }

TBool CAlfPerfAppCoverTestCaseControl::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() && 
         aEvent.CustomParameter() == KAlfPerfAppCoverCmdCompleteNow )
        {
        const TInt KNumberOfExecutedMovements = 15;
        if ( iCycleCount < KNumberOfExecutedMovements )
            {
            NextCycleL();
            CompleteAfterL( KTransitionWaitTime );
            }
        else
            {
            CompleteNow( KErrNone );
            }
        
        return ETrue;
        }

    return CAlfControl::OfferEventL( aEvent );
    }

void CAlfPerfAppCoverTestCaseControl::NextCycleL()
    {
    static const TReal32 movements[] = { -1, -1, -1, 1, 1, 1 }; 
    const TInt size = 6; //sizeof movements;
    const TInt index = iCycleCount % size;   
        
   
    switch(CaseId())
      {
      case EAlfPerfAppCoverRotate:    
          RotateImage( );
          break;     
      case EAlfPerfAppCover:
      case EAlfPerfAppCoverLandscape:
          MoveImages( movements[ index ] );   
          UpdateScaling();  
          break;
      case EAlfPerfAppCoverClipZoomLargeImage:
      case EAlfPerfAppCoverClipZoomBigImage:
      case EAlfPerfAppCoverClipZoomVga:
      case EAlfPerfAppCoverClipZoomAutoSize:
          ClipandZoom();
          break;
      }
    ++iCycleCount;
    }

void CAlfPerfAppCoverTestCaseControl::MoveImages( 
    TReal32 aAmount, 
    TReal32 aTransitionTime )
    {
    iPreviousViewportPosition = iViewportPosition;
    iViewportPosition -= aAmount;

    if ( iViewportPosition < 0 )
        {
        iViewportPosition = 0;                
        }
    else if ( iViewportPosition > ( KNumberOfImages - 1.0 ) )
        {
        iViewportPosition = KNumberOfImages - 1.0;    
        }

    iViewport->SetViewportPos(
        TAlfRealPoint( iViewportPosition + KViewportOffset, KViewportOffsetY ), 
        aTransitionTime );
    }

void CAlfPerfAppCoverTestCaseControl::UpdateScaling( 
    TReal32 aTransitionTime )
    {
    const TInt current = RoundFloatToInt( iViewportPosition );
    const TInt previous = RoundFloatToInt( iPreviousViewportPosition );
        
    if ( previous >= 0 )
        {
        CAlfVisual& currentVisual = iLayout->Visual( current );
        CAlfVisual& previousVisual = iLayout->Visual( previous );

        TAlfTimedValue puller;
        puller.SetValueNow( KSmallImageScale );
        puller.SetTarget( KFocusedImageScale, aTransitionTime );
        CAlfTransformation& enlarging = currentVisual.Transformation();
        enlarging.LoadIdentity();
        enlarging.Scale( puller, puller );

        TRAP_IGNORE(DownScaleImageL( previousVisual, aTransitionTime ));
        }
    }

void CAlfPerfAppCoverTestCaseControl::RotateImage( )
    {    
    TInt angle = KRotationAngle;
    if ( iCycleCount%2 )
        {
        angle = -KRotationAngle;
        }
    
    CAlfVisual& currentVisual = iLayout->Visual( 0 );   
    TAlfTimedValue rotator;
    rotator.SetValueNow( 0 );        
    rotator.SetTarget( angle, KTransitionTime );
    CAlfTransformation& rotation = currentVisual.Transformation();
    rotation.LoadIdentity();
    rotation.Rotate( rotator );                
    }

void CAlfPerfAppCoverTestCaseControl::ClipandZoom( )
    {    
    CAlfLayout& layout = static_cast< CAlfLayout& >( iLayout->Visual( 0 ) );
    CAlfVisual& imgVisual = layout.Visual( 0 );
    imgVisual.Move( TAlfRealPoint( -5, -5 ),  KTransitionTime);
    
    TAlfTimedValue scaler;
    scaler.SetValueNow( KFocusedImageScale *  ( iCycleCount + 1 ) );
    scaler.SetTarget( KFocusedImageScale *  ( iCycleCount + 2 ) , KTransitionTime );
    CAlfTransformation& enlarging = layout.Transformation();
    enlarging.LoadIdentity();
    enlarging.Scale( scaler, scaler );  
    }

TInt CAlfPerfAppCoverTestCaseControl::RoundFloatToInt( TReal32 aVal )
    {
    return aVal < 0 ? (TInt)( aVal - 0.5f ) : (TInt)( aVal + 0.5f );
    }    

void CAlfPerfAppCoverTestCaseControl::CompleteAfterL( TInt aDuration )
    {
    // In order to use this service, base class StartExecuteL must
    // have been called.
    __ASSERT_ALWAYS( iStatus, User::Invariant() );
    
    TAlfCustomEventCommand command( KAlfPerfAppCoverCmdCompleteNow, this );
    User::LeaveIfError( Env().Send( command, aDuration ) );
    }
        
void CAlfPerfAppCoverTestCaseControl::CompleteNow( TInt aErrorCode )
    {
    if ( iStatus )
        {
        User::RequestComplete( iStatus, aErrorCode );
        iStatus = NULL;
        }
    }

inline TInt CAlfPerfAppCoverTestCaseControl::CaseId() const
    {
    return iCaseId;
    }

inline TBool CAlfPerfAppCoverTestCaseControl::IsExecutionOngoing() const
    {
    return ( iStatus != NULL );
    }



