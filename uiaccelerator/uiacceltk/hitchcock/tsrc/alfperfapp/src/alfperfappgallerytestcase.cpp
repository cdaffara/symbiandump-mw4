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
* Description:  CAlfPerfAppGalleryTestCase implementation.
*
*/


#include "alfperfappgallerytestcase.h"
#include "alfperfappappui.h"
#include "alfperfapp.hrh"

#include <aknutils.h>
#include <alf/alfenv.h>
#include <alf/alfdisplay.h>
#include <alf/alfcontrol.h>
#include <alf/alfcontrolgroup.h>
#include <alf/alfroster.h>
#include <alf/alfcommand.h>
#include <alf/alfevent.h>
#include <alf/alftexturemanager.h>
#include <alf/alfimagevisual.h>
#include <alf/alfimageloaderutil.h>
#include <alf/alfutil.h>
#include <alf/alflayout.h>
#include <alf/alfviewportlayout.h>
#include <alf/alfgridlayout.h>
#include <alf/alftransformation.h>
#include <alf/alfgradientbrush.h>
#include <alf/alfimagebrush.h>
#include <alf/alftextvisual.h>
#include <alf/alfbrusharray.h>
#include <alf/alfshadowborderbrush.h>
#include <alf/alfdecklayout.h>
#include <alf/alflayout.h>
#include <alf/alfborderbrush.h>

#include <alfperfapp_imagetest.mbg>

/**
 * Control group for image test cases.
 */
const TInt KAlfPerfAppGalleryControlGroup = 3;

/**
 * Complete now command.
 */
const TInt KAlfPerfAppGalleryCmdCompleteNow = 0x7000;

/**
 * Abstract gallery test case control.
 */
class CAlfPerfAppGalleryTestCaseControl : public CAlfControl,
	public MAlfTextureLoadingCompletedObserver
    {
public:

    // Common constants
    static const TInt KTransitionTime;
    static const TInt KTransitionWaitTime;

    /**
     * Constructor.
     */
    CAlfPerfAppGalleryTestCaseControl();

    /**
     * Destructor.
     */
    ~CAlfPerfAppGalleryTestCaseControl();

    static CAlfPerfAppGalleryTestCaseControl* NewL(
    		CAlfEnv& aEnv, TInt aCaseId, const TRect& /*aVisibleArea*/);

    /**
     * Base class constructor. Derived classes may
     * override this, but base class needs to be called.
     */
    virtual void ConstructL(
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea );

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
     * Sets observer for texture loading.
     * @param aObserver Observer class.
     */
    void SetImagesLoadedObserver( MImagesLoadedObserver* aObserver );

    /**
     * Sets focused image.
     * @param aImageTag Tag for the image to be focused.
     */
    void SetImageFocusL( const TDesC8& aImageTag );

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

    // From base class MAlfTextureLoadingCompletedObserver
    void TextureLoadingCompleted( CAlfTexture& aTexture, TInt aTextureId,
			TInt aErrorCode );


protected:
    void MoveImages(
        TReal32 aAmount,
        TReal32 aTransitionTime = KTransitionTime );

	/**
	 * Next cycle for scrolling the Grid layout.
	 */
	void NextScrollCycleL();

	/**
	 * Next cycle for focusing image.
	 */
	void NextFocusCycleL();

	/**
	 * Sets image focus based on event cycle.
	 */
    void UpdateImageFocus( TInt aCycle );

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
    /**
     * Pointer to request status.
     * Not owned.
     */
    TRequestStatus* iStatus;

    CAlfLayout* iGridLayout;
    TReal32 iViewportPosition;
    TReal32 iPreviousViewportPosition;
    CAlfViewportLayout* iViewport;
    CAlfLayout* iImageInfoLayout;

    TInt iCycleCount;
    TInt iFocusChangedCount;
    TInt iGridScrolledCount;
    TInt iLoadedImages;

	CAlfImageVisual* iFocusedVisual;

    MImagesLoadedObserver* iObserver;

	RPointerArray<TDesC> iImageCaptions;

	RPointerArray<TDesC8> iImagesToBeFocused;
    };

// ============================ MEMBER FUNCTIONS ===============================
const TInt CAlfPerfAppGalleryTestCaseControl::KTransitionTime = 1000;
const TInt CAlfPerfAppGalleryTestCaseControl::KTransitionWaitTime = 1000;
// -----------------------------------------------------------------------------
// Checks if specified case is supported by this class.
// -----------------------------------------------------------------------------
//
TBool CAlfPerfAppGalleryTestCase::IsSupported( TInt aCaseId )
    {
    return aCaseId > EAlfPerfAppGalleryMin &&
           aCaseId < EAlfPerfAppGalleryMax;
    }

CAlfPerfAppGalleryTestCase* CAlfPerfAppGalleryTestCase::NewL( TInt aCaseId, TInt aSequenceIndex )
    {
    if ( !IsSupported( aCaseId ) )
        {
        User::Leave( KErrNotSupported );
        }

    CAlfPerfAppGalleryTestCase* self =
        new (ELeave) CAlfPerfAppGalleryTestCase( aCaseId, aSequenceIndex );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfPerfAppGalleryTestCase::~CAlfPerfAppGalleryTestCase()
    {
    if ( iEnv && iControl )
        {
        iEnv->TextureManager().RemoveLoadObserver( iControl );
        iControl->CancelExecution();
        iEnv->DeleteControlGroup( KAlfPerfAppGalleryControlGroup );
        }
    iImages.Close();
    delete iLoader;
    }

void CAlfPerfAppGalleryTestCase::ImagesLoaded( TInt aErrorCode )
	{
	if( iStatus )
		{
		CAlfPerfAppTestCase::CompleteNow( *iStatus, aErrorCode );
		iStatus = 0;
		}
	}

// -----------------------------------------------------------------------------
// Starts setup phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppGalleryTestCase::SetupL(
        CAlfEnv& aEnv, const TRect& /*aVisibleArea*/, TRequestStatus& aStatus )
    {
    iEnv = &aEnv;

    iStatus = &aStatus;

    CAlfPerfAppAppUi* appUi = (CAlfPerfAppAppUi*) EikonEnv()->AppUi();

    iOrientation = appUi->Orientation();

    switch ( iCaseId )
        {
        case EAlfPerfAppGallery:
        	{
        	appUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationPortrait );
            break;
        	}
        case EAlfPerfAppGalleryLandscape:
        	{
        	appUi->SetOrientationL( CAknAppUiBase::EAppUiOrientationLandscape );
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
    display.SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

    CAlfControlGroup& group =
        iEnv->NewControlGroupL( KAlfPerfAppGalleryControlGroup );

    iControl = CAlfPerfAppGalleryTestCaseControl::NewL(*iEnv, iCaseId, rect );

    group.AppendL( iControl ); // ownership passed to control group.
    iControl->SetImagesLoadedObserver( this );

    display.Roster().ShowL( group );
    }

// -----------------------------------------------------------------------------
// Starts execution phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppGalleryTestCase::ExecuteL( TRequestStatus& aStatus )
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
void CAlfPerfAppGalleryTestCase::TearDown()
    {
    CAlfPerfAppAppUi* appUi = (CAlfPerfAppAppUi*) EikonEnv()->AppUi();
    TRAP_IGNORE( appUi->SetOrientationL( iOrientation ) );
    // Execution side will delete CAlfEnv, so it will delete everything
    // related to this.
    iEnv = NULL;
    iControl = NULL;
    }

void CAlfPerfAppGalleryTestCase::HandleVisibleAreaChange( const TRect& aRect )
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

TInt CAlfPerfAppGalleryTestCase::CaseID()
    {
        return iCaseId;
    }

CAlfPerfAppGalleryTestCase::CAlfPerfAppGalleryTestCase( TInt aCaseId, TInt aSequenceIndex )
    : CAlfPerfAppTestCase(aSequenceIndex), iCaseId ( aCaseId )
    {
    }

void CAlfPerfAppGalleryTestCase::ConstructL()
    {
    }

// Implementation of CAlfPerfAppGalleryTestCaseControl:

static void GetImageTestCasePrivatePath( TFileName& aPath )
    {
    CEikonEnv::Static()->FsSession().PrivatePath( aPath );
    ::CompleteWithAppPath( aPath );
    }

CAlfPerfAppGalleryTestCaseControl::CAlfPerfAppGalleryTestCaseControl()
    {
    }

CAlfPerfAppGalleryTestCaseControl* CAlfPerfAppGalleryTestCaseControl::NewL(
	CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea )
	{
	CAlfPerfAppGalleryTestCaseControl* self = new (ELeave) CAlfPerfAppGalleryTestCaseControl();
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aCaseId, aVisibleArea );
    CleanupStack::Pop( self );
    return self;
	}

const TInt KNumberOfImages = 10;
const TInt KImagePadding = 5;
const TReal32 KNumberOfLinesInView = 3.5;
//const TReal32 KNumberOfColumnsInView = 4.0;
const TReal32 KNumberOfLinesInGrid = 5.0;
const TReal32 KNumberOfColumnsInGrid = 4.0;

void CAlfPerfAppGalleryTestCaseControl::ConstructL(
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea )
    {
    iCaseId = aCaseId;
    CAlfControl::ConstructL( aEnv );

    TFileName privatePath;
    GetImageTestCasePrivatePath( privatePath );
    aEnv.TextureManager().SetImagePathL( privatePath );

    iLoadedImages = 0;

    // Viewport for grid
    iViewport = CAlfViewportLayout::AddNewL( *this, 0 );
    iViewport->SetClipping( EFalse );
    iViewport->SetVirtualSize( TAlfRealSize( 1.0, KNumberOfLinesInGrid ), 0 );
    iViewport->SetViewportSize( TAlfRealSize( 1.0, KNumberOfLinesInView ), 0 );
    iViewport->SetViewportPos( TAlfRealPoint( iViewportPosition, 0 ), 0 );

    // Background for viewport
    CAlfGradientBrush* brush = CAlfGradientBrush::NewLC( aEnv );
    brush->SetDirection( CAlfGradientBrush::EDirectionDown );
    brush->SetLayer( EAlfBrushLayerBackground );
    brush->SetColor( KRgbBlack );
    brush->AppendColorL( 1.0f, KRgbGray );
    iViewport->EnableBrushesL( ETrue );
    iViewport->Brushes()->AppendL( brush, EAlfHasOwnership );
    CleanupStack::Pop( brush );

    // Grid for images
    iGridLayout = CAlfGridLayout::AddNewL(
        *this,
        KNumberOfColumnsInGrid,
        KNumberOfLinesInGrid,  // lines of images
        iViewport );

    // Images
    // NOTE: Now using small images for OpenVG, because of memory issues.
    // These files may be replaced for BitGDI if needed. 
    _LIT( KImage1, "alfphotos_small1.jpg" );
    _LIT( KImage2, "alfphotos_small2.jpg" );
    _LIT( KImage3, "alfphotos_small3.jpg" );
    _LIT( KImage4, "alfphotos_small4.jpg" );
    _LIT( KImage5, "alfphotos_small5.jpg" );
    _LIT( KImage6, "alfphotos_small6.jpg" );
    _LIT( KImage7, "alfphotos_small7.jpg" );
    _LIT( KImage8, "alfphotos_small8.jpg" );
    _LIT( KImage9, "alfphotos_small9.jpg" );
    _LIT( KImage10, "jpg_sun_small.jpg" );
    
    // Visual tags
    _LIT8( KVisual1, "visual1" );
    _LIT8( KVisual2, "visual2" );
    _LIT8( KVisual3, "visual3" );
    _LIT8( KVisual4, "visual4" );
    _LIT8( KVisual5, "visual5" );
    _LIT8( KVisual6, "visual6" );
    _LIT8( KVisual7, "visual7" );
    _LIT8( KVisual8, "visual8" );
    _LIT8( KVisual9, "visual9" );
    _LIT8( KVisual10, "visual10" );
    _LIT8( KVisual11, "visual11" );
    _LIT8( KVisual12, "visual12" );
    _LIT8( KVisual13, "visual13" );
    _LIT8( KVisual14, "visual14" );
    _LIT8( KVisual15, "visual15" );
    _LIT8( KVisual16, "visual16" );
    _LIT8( KVisual17, "visual17" );
    _LIT8( KVisual18, "visual18" );
    _LIT8( KVisual19, "visual19" );
    _LIT8( KVisual20, "visual20" );

    // Captions for image metadata text field
    _LIT( KImageCaption1, "alfphotos1" );
    _LIT( KImageCaption5, "alfphotos5" );
    _LIT( KImageCaption8, "alfphotos8" );

    // The order of the visuals in image focusing
    iImagesToBeFocused.Append( &KVisual17 );
    iImagesToBeFocused.Append( &KVisual5 );
    iImagesToBeFocused.Append( &KVisual1 );

    // The corresponding image captions in image focusing
    iImageCaptions.Append( &KImageCaption8 );
    iImageCaptions.Append( &KImageCaption5 );
    iImageCaptions.Append( &KImageCaption1 );

    aEnv.TextureManager().AddLoadObserverL( this );

    // Load textures for images
    CAlfTexture& texture1 = aEnv.TextureManager().LoadTextureL(
        KImage1,
        EAlfTextureFlagRetainResolution,
        KAlfAutoGeneratedTextureId );
    CAlfTexture& texture2 = aEnv.TextureManager().LoadTextureL(
        KImage2,
        EAlfTextureFlagRetainResolution,
        KAlfAutoGeneratedTextureId );
    CAlfTexture& texture3 = aEnv.TextureManager().LoadTextureL(
        KImage3,
        EAlfTextureFlagRetainResolution,
        KAlfAutoGeneratedTextureId );
    CAlfTexture& texture4 = aEnv.TextureManager().LoadTextureL(
        KImage4,
        EAlfTextureFlagRetainResolution,
        KAlfAutoGeneratedTextureId );
    CAlfTexture& texture5 = aEnv.TextureManager().LoadTextureL(
        KImage5,
        EAlfTextureFlagRetainResolution,
        KAlfAutoGeneratedTextureId );
    CAlfTexture& texture6 = aEnv.TextureManager().LoadTextureL(
        KImage6,
        EAlfTextureFlagRetainResolution,
        KAlfAutoGeneratedTextureId );
    CAlfTexture& texture7 = aEnv.TextureManager().LoadTextureL(
        KImage7,
        EAlfTextureFlagRetainResolution,
        KAlfAutoGeneratedTextureId );
    CAlfTexture& texture8 = aEnv.TextureManager().LoadTextureL(
        KImage8,
        EAlfTextureFlagRetainResolution,
        KAlfAutoGeneratedTextureId );
    CAlfTexture& texture9 = aEnv.TextureManager().LoadTextureL(
        KImage9,
        EAlfTextureFlagRetainResolution,
        KAlfAutoGeneratedTextureId );
    CAlfTexture& texture10 = aEnv.TextureManager().LoadTextureL(
        KImage10,
        EAlfTextureFlagRetainResolution,
        KAlfAutoGeneratedTextureId );

    // Add visuals to image grid
    CAlfImageVisual* visual1 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual1->SetImage( TAlfImage( texture1 ) );
    visual1->SetPadding( KImagePadding );
    visual1->SetTagL( KVisual1 );
    CAlfImageVisual* visual2 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual2->SetImage( TAlfImage( texture2 ) );
    visual2->SetPadding( KImagePadding );
    visual2->SetTagL( KVisual2 );
    CAlfImageVisual* visual3 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual3->SetImage( TAlfImage( texture3 ) );
    visual3->SetPadding( KImagePadding );
    visual3->SetTagL( KVisual3 );
    CAlfImageVisual* visual4 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual4->SetImage( TAlfImage( texture4 ) );
    visual4->SetPadding( KImagePadding );
    visual4->SetTagL( KVisual4 );
    CAlfImageVisual* visual5 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual5->SetImage( TAlfImage( texture5 ) );
    visual5->SetPadding( KImagePadding );
    visual5->SetTagL( KVisual5 );
    CAlfImageVisual* visual6 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual6->SetImage( TAlfImage( texture6 ) );
    visual6->SetPadding( KImagePadding );
    visual6->SetTagL( KVisual6 );
    CAlfImageVisual* visual7 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual7->SetImage( TAlfImage( texture7 ) );
    visual7->SetPadding( KImagePadding );
    visual7->SetTagL( KVisual7 );
    CAlfImageVisual* visual8 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual8->SetImage( TAlfImage( texture8 ) );
    visual8->SetPadding( KImagePadding );
    visual8->SetTagL( KVisual8 );
    CAlfImageVisual* visual9 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual9->SetImage( TAlfImage( texture9 ) );
    visual9->SetPadding( KImagePadding );
    visual9->SetTagL( KVisual9 );
    CAlfImageVisual* visual10 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual10->SetImage( TAlfImage( texture10 ) );
    visual10->SetPadding( KImagePadding );
    visual10->SetTagL( KVisual10 );
    CAlfImageVisual* visual11 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual11->SetImage( TAlfImage( texture2 ) );
    visual11->SetPadding( KImagePadding );
    visual11->SetTagL( KVisual11 );
    CAlfImageVisual* visual12 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual12->SetImage( TAlfImage( texture3 ) );
    visual12->SetPadding( KImagePadding );
    visual12->SetTagL( KVisual12 );
    CAlfImageVisual* visual13 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual13->SetImage( TAlfImage( texture4 ) );
    visual13->SetPadding( KImagePadding );
    visual13->SetTagL( KVisual13 );
    CAlfImageVisual* visual14 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual14->SetImage( TAlfImage( texture5 ) );
    visual14->SetPadding( KImagePadding );
    visual14->SetTagL( KVisual14 );
    CAlfImageVisual* visual15 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual15->SetImage( TAlfImage( texture6 ) );
    visual15->SetPadding( KImagePadding );
    visual15->SetTagL( KVisual15 );
    CAlfImageVisual* visual16 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual16->SetImage( TAlfImage( texture7 ) );
    visual16->SetPadding( KImagePadding );
    visual16->SetTagL( KVisual16 );
    CAlfImageVisual* visual17 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual17->SetImage( TAlfImage( texture8 ) );
    visual17->SetPadding( KImagePadding );
    visual17->SetTagL( KVisual17 );
    CAlfImageVisual* visual18 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual18->SetImage( TAlfImage( texture9 ) );
    visual18->SetPadding( KImagePadding );
    visual18->SetTagL( KVisual18 );
    CAlfImageVisual* visual19 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual19->SetImage( TAlfImage( texture1 ) );
    visual19->SetPadding( KImagePadding );
    visual19->SetTagL( KVisual19 );
    CAlfImageVisual* visual20 = CAlfImageVisual::AddNewL( *this, iGridLayout );
    visual20->SetImage( TAlfImage( texture2 ) );
    visual20->SetPadding( KImagePadding );
    visual20->SetTagL( KVisual20 );

    // Focus on visual 1
	CAlfBorderBrush* actualBorder = CAlfBorderBrush::NewLC( Env(), 4, 4, 0, 0 );
	actualBorder->SetColor( KRgbBlue );
	visual1->EnableBrushesL( ETrue );
	visual1->Brushes()->AppendL( actualBorder, EAlfHasOwnership );
	CleanupStack::Pop( actualBorder );

    // Area for image metadata
    TRect textFieldArea;
    textFieldArea.SetWidth( aVisibleArea.Width() );
    textFieldArea.SetHeight( aVisibleArea.Height() / 7 );

    // Layout for image metadata
    CAlfGridLayout* iImageInfoLayout = CAlfGridLayout::AddNewL( *this, 1, 1, 0 );
    iImageInfoLayout->EnableBrushesL( ETrue );
    iImageInfoLayout->SetFlag( EAlfVisualFlagManualSize );
    iImageInfoLayout->SetFlag( EAlfVisualFlagManualPosition );
    iImageInfoLayout->SetRect( textFieldArea, 0 );
    iImageInfoLayout->SetPos( TAlfRealPoint( 0 , aVisibleArea.Height() - textFieldArea.Height() ) , 0 );

    // Text for image metadata
    CAlfTextVisual* text = CAlfTextVisual::AddNewL( *this, iImageInfoLayout );
    text->SetTextL( KImageCaption1 );
    text->EnableBrushesL( ETrue );
    text->SetColor( KRgbWhite );
    text->SetTagL( _L8( "text1" ) );

    // Background for image metadata
    CAlfGradientBrush* textBackgroundBrush = CAlfGradientBrush::NewLC( aEnv );
    textBackgroundBrush->SetColor( KRgbGray );
    textBackgroundBrush->SetOpacity( 0.5f );
    textBackgroundBrush->SetLayer( EAlfBrushLayerBackground );
    iImageInfoLayout->Brushes()->AppendL( textBackgroundBrush, EAlfHasOwnership );
    CleanupStack::Pop( textBackgroundBrush );

    // Border for image metadata
    CAlfBorderBrush* border = CAlfBorderBrush::NewLC( aEnv, 2, 2, 5, 5 );
    border->SetOpacity( 0.5f );
    iImageInfoLayout->Brushes()->AppendL( border, EAlfHasOwnership );
    CleanupStack::Pop( border );
    }

CAlfPerfAppGalleryTestCaseControl::~CAlfPerfAppGalleryTestCaseControl()
    {
    CompleteNow( KErrCancel );
    iImageCaptions.Reset();
    iImagesToBeFocused.Reset();
    }

void CAlfPerfAppGalleryTestCaseControl::SetImagesLoadedObserver( MImagesLoadedObserver* aObserver )
	{
	iObserver = aObserver;
	}

// -----------------------------------------------------------------------------
// Notify observers that all images have been loaded.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppGalleryTestCaseControl::TextureLoadingCompleted(
		CAlfTexture& /*aTexture*/, TInt /*aTextureId*/, TInt aErrorCode)
	{
	// inform if there is any error
	if( aErrorCode != KErrNone )
	    {
	    iObserver->ImagesLoaded( aErrorCode );
	    }

	iLoadedImages++;
	if (iLoadedImages == KNumberOfImages)
		{
		iObserver->ImagesLoaded( KErrNone );
		}
	}

void CAlfPerfAppGalleryTestCaseControl::StartExecuteL( TRequestStatus& aStatus )
    {
    iCycleCount = 0;
    iFocusChangedCount = 0;
    iGridScrolledCount = 0;
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

void CAlfPerfAppGalleryTestCaseControl::CancelExecution()
    {
    CompleteNow( KErrCancel );
    }

void CAlfPerfAppGalleryTestCaseControl::DoStartExecuteL()
    {
    NextScrollCycleL();
    CompleteAfterL( KTransitionWaitTime );
    }

void CAlfPerfAppGalleryTestCaseControl::SetVisibleArea(
        const TRect& /*aVisibleArea*/ )
    {
    }

TBool CAlfPerfAppGalleryTestCaseControl::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() &&
         aEvent.CustomParameter() == KAlfPerfAppGalleryCmdCompleteNow )
        {
        const TInt KNumberOfExecutedMovements = 15;
        if ( iCycleCount < KNumberOfExecutedMovements )
        	{
// Disabling the image focusing between scroll cycles
#if 0
        	TInt odd = iCycleCount % 2;
        	if( odd == 0 )
        		{
#endif
        		NextScrollCycleL();
        		CompleteAfterL( KTransitionWaitTime );
#if 0
        		}
        	else
        		{
        		NextFocusCycleL();
        		CompleteAfterL( 0 );
        		}
#endif
        	}
        else
        	{
        	CompleteNow( KErrNone );
        	}

        return ETrue;
        }

    return CAlfControl::OfferEventL( aEvent );
    }

void CAlfPerfAppGalleryTestCaseControl::NextScrollCycleL()
	{
	static const TReal32 movements[] = { -2, 1, 1 };

	const TInt size = 3; //sizeof movements;
	const TInt index = iGridScrolledCount % size;

	MoveImages(movements[ index ]);

	++iCycleCount;
	++iGridScrolledCount;
	}


void CAlfPerfAppGalleryTestCaseControl::NextFocusCycleL()
	{
	const TInt size = 3;
	const TInt index = iFocusChangedCount % size;

	UpdateImageFocus( index );

	++iCycleCount;
	++iFocusChangedCount;
	}



void CAlfPerfAppGalleryTestCaseControl::UpdateImageFocus( TInt aIndex )
	{
	// TODO: Checking if textVisual can be found
	CAlfTextVisual* textVisual = static_cast<CAlfTextVisual*>( FindTag( _L8( "text1" ) ) );
    TRAP_IGNORE(textVisual->SetTextL( *iImageCaptions.operator[]( aIndex ) ));
    TRAP_IGNORE(SetImageFocusL( *iImagesToBeFocused.operator[]( aIndex ) ));
	}


void CAlfPerfAppGalleryTestCaseControl::MoveImages(
    TReal32 aAmount,
    TReal32 aTransitionTime )
    {
    iPreviousViewportPosition = iViewportPosition;
    iViewportPosition -= aAmount;

    if ( iViewportPosition < 0 )
        {
        iViewportPosition = 0;
        }
    else if ( iViewportPosition > ( KNumberOfLinesInView - 1.0 ) )
        {
        iViewportPosition = KNumberOfLinesInView - 1.0;
        }

    iViewport->SetViewportPos(
            TAlfRealPoint( 0, iViewportPosition ),
        aTransitionTime );
    }


void CAlfPerfAppGalleryTestCaseControl::SetImageFocusL( const TDesC8& aImageTag )
	{

	//TODO: No checking if FindTag finds anything
	CAlfImageVisual* visual =
			static_cast<CAlfImageVisual*>( FindTag( aImageTag ) );

	if( iFocusedVisual && iFocusedVisual->Brushes() )
		{
		// TODO: At the moment, no checking of the brush type
		CAlfBorderBrush* brush =
				static_cast<CAlfBorderBrush*>( &iFocusedVisual->Brushes()->At( 0 ) );
		iFocusedVisual->Brushes()->Remove( 0 );
		}

	if( visual )
		{
		CAlfBorderBrush* actualBorder = CAlfBorderBrush::NewLC( Env(), 4, 4, 0, 0 );
		if(actualBorder)
		    {
		    actualBorder->SetColor( KRgbBlue );
		    }
		visual->EnableBrushesL( ETrue );
		visual->Brushes()->AppendL( actualBorder, EAlfHasOwnership );
		CleanupStack::Pop( actualBorder );
		}

	iFocusedVisual = visual;
	}

void CAlfPerfAppGalleryTestCaseControl::CompleteAfterL( TInt aDuration )
    {
    // In order to use this service, base class StartExecuteL must
    // have been called.
    __ASSERT_ALWAYS( iStatus, User::Invariant() );

    TAlfCustomEventCommand command( KAlfPerfAppGalleryCmdCompleteNow, this );
    User::LeaveIfError( Env().Send( command, aDuration ) );
    }

void CAlfPerfAppGalleryTestCaseControl::CompleteNow( TInt aErrorCode )
    {
    if ( iStatus )
        {
        User::RequestComplete( iStatus, aErrorCode );
        iStatus = NULL;
        }
    }

inline TInt CAlfPerfAppGalleryTestCaseControl::CaseId() const
    {
    return iCaseId;
    }

inline TBool CAlfPerfAppGalleryTestCaseControl::IsExecutionOngoing() const
    {
    return ( iStatus != NULL );
    }

