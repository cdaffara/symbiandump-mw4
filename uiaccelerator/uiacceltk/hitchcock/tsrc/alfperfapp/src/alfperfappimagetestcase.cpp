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
* Description:  CAlfPerfAppImageTestCase implementation.
*
*/


#include "alfperfappbasetestcasecontrol.h"
#include "alfperfappimagetestcase.h"
#include "alfperfapp.hrh"
#include "alfperfappconfigconstants.h"
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
#ifdef __NVG
    #include <alf/alfbitmapprovider.h>
#endif
#include <e32math.h>

#include <alfperfapp_imagetest.mbg>

/*
 * Square root of sequence length for the image sequence cases
 */
const TInt SEQUENCE_LENGTH_SQRT = 7;

/**
 * Control group for image test cases.
 */
const TInt KAlfPerfAppImageControlGroup = 1;

/**
 * Start next 'cycle' command.
 */
const TInt KAlfPerfAppImageCmdNext = 0x5001;

/**
 * Amount of image visuals.
 */
const TInt KAlfPerfAppImageTestCaseBasicImageCount = 16;

/**
 * Image data for test cases.
 */
_LIT( KAlfPerfAppImageTestCaseData, "\\resource\\apps\\alfperfapp_imagetest.mif" );
_LIT( KAlfPerfAppImageTestCaseNvgCSData, "\\resource\\apps\\alfperfapp_imagetest_NVGCS.mif" );
// NVG TLV Files have to be loaded from a certain place defined by icontype.cfg (in c:\ root)
_LIT( KAlfPerfAppImageTestCaseNvgTLVData, "\\data\\nvg_mif\\3musk.mif" );

#ifdef __NVG
// Enum that defines what icon (normal or NVG) is being used
// with the basic scale & movement test
enum TMoveScaleIcon
    {
    EMoveScaleIconMif = 0,
    EMoveScaleIconCS,
    EMoveScaleIconTLV
    };
#endif

/**
 * Basic image scale & movement test case.
 */
class CAlfPerfAppImageTestCaseBasic : public CAlfPerfAppBaseTestCaseControl
#ifdef __NVG
                                    , public MAlfBitmapProvider
#endif
    {
public:
    CAlfPerfAppImageTestCaseBasic();
    ~CAlfPerfAppImageTestCaseBasic();
    
    virtual void ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea );      
    virtual void DoStartExecuteL();
    virtual TBool OfferEventL( const TAlfEvent& aEvent );
    virtual void SetVisibleArea( const TRect& aVisibleArea );    

#ifdef __NVG
    void SetIconType( TMoveScaleIcon aIconType );
#endif
    
private:
    
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
 
#ifdef __NVG
    void ProvideBitmapL(TInt aId, CFbsBitmap*& aBitmap, CFbsBitmap*& aMaskBitmap);
    CFbsBitmap* iBitmapToBeProvided;
    TMoveScaleIcon iIconType;
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
    };
        
/**
 * Many images at the same time
 */
#ifdef __NVG
class CAlfPerfAppImageTestCaseManyImages : public CAlfPerfAppBaseTestCaseControl, public MAlfBitmapProvider
#else
class CAlfPerfAppImageTestCaseManyImages : public CAlfPerfAppBaseTestCaseControl
#endif
    {
public:
    CAlfPerfAppImageTestCaseManyImages();
    ~CAlfPerfAppImageTestCaseManyImages();
    
    virtual void ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea, TInt aSequenceIndex, TInt aVisualCount );      
    virtual void DoStartExecuteL();
    virtual TBool OfferEventL( const TAlfEvent& aEvent );
    virtual void SetVisibleArea( const TRect& aVisibleArea ); 

#ifdef __NVG
    void ProvideBitmapL(TInt /*aId*/, CFbsBitmap*& aBitmap, CFbsBitmap*& /*aMaskBitmap*/);
#endif
    
private:
    
    /**
     * Next cycle.
     */
    void NextCycleL();
    
    /**
     * Returns opacity for index.
     */
	TBool OpacityForIndex( TInt aIndex, TReal32& aOpacity );
    
    /**
     * Returns size for index.
     */
    TBool SizeForIndex( TInt aIndex, TAlfRealSize& aSize );
    
    /**
     * Returns position for index.
     */
    TBool PositionForIndex( TInt aIndex, TAlfRealPoint& aPos );
 

     
private:

#ifdef __NVG
    CFbsBitmap* iBitmapToBeProvided;
#endif
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
    
    
    /**
     * Test case parameters
     */
    TInt iVisualCount; 				// Number of visual to be created
    TInt iVisualColums;
    TInt iVisualRows;
    TAlfRealPoint iVisualSpeed;    	// Pixels to move between cycles
    TFileName iImageFileName;		// File name of the texture image file	
	TInt iImageCount;				// Number of different images
	TSize iImageSize;				// Image size
    TAlfTextureFlags iTextureFlags;	// Texture flags	
    TAlfRealSize iVisualSize;		// Initial size of the visual
    TAlfRealPoint iVisualSizeVel; 	// Velocity of visual size change per cycle	
    TInt iSimulatedPointerEventCount; // Number of pointer events sent to visula in one cycle
    //TSize iAnimatedAreaSize;
    TInt iLastIndexInAnimatedArea;	// the index of the last visual that is animated, all before this are also animated
    TInt iSequenceIndex;
    };
        
// ============================ MEMBER FUNCTIONS ===============================

// -----------------------------------------------------------------------------
// Checks if specified case is supported by this class.
// -----------------------------------------------------------------------------
//
TBool CAlfPerfAppImageTestCase::IsSupported( TInt aCaseId )
    {
    return aCaseId > EAlfPerfAppImageMin && 
           aCaseId < EAlfPerfAppImageMax;
    }

CAlfPerfAppImageTestCase* CAlfPerfAppImageTestCase::NewL( TInt aCaseId, TInt aSequenceIndex )
    {
    if ( !IsSupported( aCaseId ) )
        {
        User::Leave( KErrNotSupported );
        }

    CAlfPerfAppImageTestCase* self = 
        new (ELeave) CAlfPerfAppImageTestCase( aCaseId, aSequenceIndex );
    CleanupStack::PushL( self );
    self->ConstructL();
    CleanupStack::Pop( self );
    return self;
    }

CAlfPerfAppImageTestCase::~CAlfPerfAppImageTestCase()
    {
    if ( iEnv && iControl )
        {
        iControl->CancelExecution();
        iEnv->DeleteControlGroup( KAlfPerfAppImageControlGroup );
        }
    }

// -----------------------------------------------------------------------------
// Starts setup phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppImageTestCase::SetupL( 
        CAlfEnv& aEnv, const TRect& aVisibleArea, TRequestStatus& aStatus )
    {
    
    iEnv = &aEnv;

    CAlfDisplay* display = 0;
    CAlfControlGroup& group = iEnv->NewControlGroupL( KAlfPerfAppImageControlGroup );
    CAlfPerfAppBaseTestCaseControl* control = 0;
    iTotalVisualCount = -1;
    iAnimationDivider = 0;
    
    if(SequenceIndex() != 0)
        {
        // Calculate 
        TInt caseNum = SEQUENCE_LENGTH_SQRT - (SequenceIndex()-1) / SEQUENCE_LENGTH_SQRT - 1;
        TReal trg = 0.0;
        Math::Pow(trg, 2.0, caseNum);
        iAnimationDivider = trg+0.5;
        
        TRect rect;
        AknLayoutUtils::LayoutMetricsRect( 
                AknLayoutUtils::EApplicationWindow, 
                rect );
        iEnv->SetFullScreenDrawing( ETrue );
        
        
        TInt counter = 10 - ((SequenceIndex()-1)%SEQUENCE_LENGTH_SQRT) - 1; // 2^3...2^9 always choose the 
        trg = 0.0;
        Math::Pow(trg, 2.0, counter);
        iTotalVisualCount = trg+0.5;
        
        display = &iEnv->NewDisplayL( rect, CAlfEnv::ENewDisplayFullScreen );

        display->SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

        control = new (ELeave) CAlfPerfAppImageTestCaseManyImages();
        CleanupStack::PushL( control );
        ((CAlfPerfAppImageTestCaseManyImages*)control)->ConstructL( *iEnv, iCaseId, aVisibleArea,SequenceIndex(),iTotalVisualCount );

        }
    else
        {
        display = &iEnv->NewDisplayL( aVisibleArea, CAlfEnv::ENewDisplayAsCoeControl);
        display->SetClearBackgroundL( CAlfDisplay::EClearWithSkinBackground );

        switch ( iCaseId )
            {
            case EAlfPerfAppImageBasicMoveScale:
                control = new (ELeave) CAlfPerfAppImageTestCaseBasic;
                CleanupStack::PushL( control );
                control->ConstructL( *iEnv, iCaseId, aVisibleArea );
                break;

#ifdef __NVG
            case EAlfPerfAppImageBasicMoveScaleNvgCS:
                CAlfPerfAppImageTestCaseBasic* temp = new (ELeave) CAlfPerfAppImageTestCaseBasic;
                temp->SetIconType(EMoveScaleIconCS);
                control = temp;
                CleanupStack::PushL( control );
                control->ConstructL( *iEnv, iCaseId, aVisibleArea);
                break;
            
            case EAlfPerfAppImageBasicMoveScaleNvgTLV:
                CAlfPerfAppImageTestCaseBasic* temp2 = new (ELeave) CAlfPerfAppImageTestCaseBasic;
                temp2->SetIconType(EMoveScaleIconTLV); 
                control = temp2;
                CleanupStack::PushL( control );
                control->ConstructL( *iEnv, iCaseId, aVisibleArea);
                break;
                
            case EAlfPerfAppImageManyNvgCSImages:
            case EAlfPerfAppImageManyNvgTLVImages:
#endif
            case EAlfPerfAppImageManyImages:
            case EAlfPerfAppImageManyGifImages:
            case EAlfPerfAppImageManyPngImages:
            case EAlfPerfAppImageManySvgImages:
            case EAlfPerfAppImageManyImagesWithPointerEvent:
            case EAlfPerfAppImageManyImagesWithAutoSize:
            case EAlfPerfAppImageAnimateFull:
            case EAlfPerfAppImageAnimate1of2:
            case EAlfPerfAppImageAnimate1of4:
            case EAlfPerfAppImageAnimate1of8:
                control = new (ELeave) CAlfPerfAppImageTestCaseManyImages;
    			CleanupStack::PushL( control );
    			((CAlfPerfAppImageTestCaseManyImages*)control)->ConstructL( *iEnv, iCaseId, aVisibleArea,SequenceIndex(),iTotalVisualCount );
                break;

            default:
                User::Leave( KErrNotSupported );
                break;
            }

        }
    group.AppendL( control ); // ownership passed to control group.
    iControl = control;
    CleanupStack::Pop( control );    
    
    display->Roster().ShowL( group );
    
    // This must be last statement.
    CAlfPerfAppTestCase::CompleteNow( aStatus, KErrNone );
    }

// -----------------------------------------------------------------------------
// Starts execution phase.
// -----------------------------------------------------------------------------
//
void CAlfPerfAppImageTestCase::ExecuteL( TRequestStatus& aStatus )
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
void CAlfPerfAppImageTestCase::TearDown()
    {
    // Execution side will delete CAlfEnv, so it will delete everything
    // related to this.
    iEnv = NULL;
    iControl = NULL;
    }

void CAlfPerfAppImageTestCase::HandleVisibleAreaChange( const TRect& aRect )
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

TInt CAlfPerfAppImageTestCase::CaseID()
    {
    return iCaseId;
    }

CAlfPerfAppImageTestCase::CAlfPerfAppImageTestCase( TInt aCaseId, TInt aSequenceIndex )
    : CAlfPerfAppTestCase(aSequenceIndex), iCaseId ( aCaseId )
    {
    }

void CAlfPerfAppImageTestCase::ConstructL()
    {
    }

// Implementation of CAlfPerfAppImageTestCaseBasic:
#ifdef __NVG
CAlfPerfAppImageTestCaseBasic::CAlfPerfAppImageTestCaseBasic(): iIconType(EMoveScaleIconMif)
#else
CAlfPerfAppImageTestCaseBasic::CAlfPerfAppImageTestCaseBasic()
#endif
    {
    }

CAlfPerfAppImageTestCaseBasic::~CAlfPerfAppImageTestCaseBasic()
    {
    iImages.Close();
    
    delete iLoader;
    }

void CAlfPerfAppImageTestCaseBasic::ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea )
    {
    CAlfPerfAppBaseTestCaseControl::ConstructL( aEnv, aCaseId, aVisibleArea );
    CAlfTextureManager& textureManager = aEnv.TextureManager();
    
    iLoader = new (ELeave) CAlfImageLoaderUtil;
    iLoader->SetSize( TSize( 64, 64 ) );
    
    const TInt max = 4;
    TInt count = 0;
    TInt baseId = 16384;
    
    TFileName imageFile;

#ifdef __NVG
    TBool isNvg = EFalse;
    if (iIconType == EMoveScaleIconCS)
        {
        imageFile.Append( KAlfPerfAppImageTestCaseNvgCSData );
        isNvg = ETrue;
        }
    else if (iIconType == EMoveScaleIconTLV)
        {
        imageFile.Append( KAlfPerfAppImageTestCaseNvgTLVData );
        isNvg = ETrue;
        }
    else if (iIconType == EMoveScaleIconMif)
        {
        imageFile.Append( KAlfPerfAppImageTestCaseData );
        }
    else
        {
        // Error! Leave
        User::Leave(KErrArgument);
        }
    // Add the drive letter to the imageFile path
    ::CompleteWithAppPath( imageFile );
#else
    imageFile.Append( KAlfPerfAppImageTestCaseData );
    ::CompleteWithAppPath( imageFile );
#endif
    
    while ( count < max && baseId < EMbmAlfperfapp_imagetestLastElement )
        {
#ifdef __NVG
        if (isNvg)
            {
            TAlfTextureFlags flags = EAlfTextureFlagDefault;
            // Use the modified AknIconUtils to create the extended bitmap
            CFbsBitmap* extendedBitmap;
            CFbsBitmap* extendedMask;
            if(iIconType == EMoveScaleIconTLV)
                {
                // The TLV icon file has only one icon
                AknIconUtils::CreateIconL(extendedBitmap, extendedMask, imageFile, 16384, 16385);
                }
            else
                {
                AknIconUtils::CreateIconL(extendedBitmap, extendedMask, imageFile, baseId, baseId + 1);
                }
            AknIconUtils::ExcludeFromCache(extendedBitmap);
            AknIconUtils::SetSize( extendedBitmap, TSize(64,64) );
            
            // This will be returned, when ProvideBitmapL is called for creating the texture
            //delete iBitmapToBeProvided;
            iBitmapToBeProvided = extendedBitmap;
            delete extendedMask;
            
            count++;
            textureManager.CreateTextureL(count, this, flags);
            }
        else
            {
            // Else load the icons normally from the mif file
            count++;
            textureManager.CreateTextureL(
                count,
                iLoader->CreateImageLoaderL(
                    KAknsIIDNone,
                    imageFile,
                    baseId,
                    baseId + 1 ),
                EAlfTextureFlagDefault );
            }
#else
        count++;
        textureManager.CreateTextureL(
            count,
            iLoader->CreateImageLoaderL(
                KAknsIIDNone,
                imageFile,
                baseId,
                baseId + 1 ),
            EAlfTextureFlagDefault );
#endif
        baseId += 2; // odd values are masks
        }
    
    for ( TInt ii = 0; ii < KAlfPerfAppImageTestCaseBasicImageCount; ii++ )
        {
        CAlfImageVisual* visual = CAlfImageVisual::AddNewL( *this );
        visual->SetFlag( EAlfVisualFlagManualLayout );
        visual->SetOpacity( 1.0f );
        
        visual->SetOpacity( OpacityForIndex( ii ) );
        TAlfRealSize sz = SizeForIndex( ii );
        visual->SetSize( sz );
        visual->SetPos( PositionForIndex( ii, sz ) );
        
        const CAlfTexture* texture = textureManager.Texture( 1 + ( ii & 3 ) );
        TAlfImage image( *texture );
        visual->SetImage( image );
        
        iImages.AppendL( visual );        
        }
    }
    
#ifdef __NVG
void CAlfPerfAppImageTestCaseBasic::SetIconType(TMoveScaleIcon aIconType)
    {
    iIconType = aIconType;
    }
#endif

void CAlfPerfAppImageTestCaseBasic::DoStartExecuteL()
    {
    NextCycleL();
    
    CompleteAfterL( 5000 );
    }

void CAlfPerfAppImageTestCaseBasic::SetVisibleArea( 
        const TRect& /*aVisibleArea*/ )
    {
    if ( IsExecutionOngoing() )
        {
        Env().CancelCustomCommands( this, KAlfPerfAppImageCmdNext );
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        }
    }

TBool CAlfPerfAppImageTestCaseBasic::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() && 
         aEvent.CustomParameter() == KAlfPerfAppImageCmdNext )
        {
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        return ETrue;
        }
    
    return CAlfPerfAppBaseTestCaseControl::OfferEventL( aEvent );
    }

void CAlfPerfAppImageTestCaseBasic::NextCycleL()
    {
    TAlfCustomEventCommand command( KAlfPerfAppImageCmdNext, this );
    User::LeaveIfError( Env().Send( command, 500 ) );
    
    iCycleCounter++;
    
    for ( TInt ii = 0; ii < iImages.Count(); ii++ )
        {
        CAlfImageVisual* visual = iImages[ ii ];
                            
        TInt speed = KVisualAnimSpeed;
        if ( ii & 1 )
            {
            speed /= 2;
            }

        visual->SetOpacity( TAlfTimedValue( OpacityForIndex( ii ), speed ) );
        TAlfRealSize sz = SizeForIndex( ii );
        visual->SetSize( sz, speed );
        visual->SetPos( PositionForIndex( ii, sz ), speed );                                    
        }
    }

TReal32 CAlfPerfAppImageTestCaseBasic::OpacityForIndex( TInt aIndex )
    {
    switch ( ( iCycleCounter + aIndex ) % 3 ) 
        {
        case 0:
            return 1.0f;
        case 1:
            return 0.5f;
        case 2:
        default:
            return 0.25f;
        }
    }

TAlfRealSize CAlfPerfAppImageTestCaseBasic::SizeForIndex( TInt aIndex )
    {
    switch ( ( iCycleCounter + aIndex ) % 3 ) 
        {
        case 0:
            return TAlfRealSize( 25.f, 25.f );
        case 1:
            return TAlfRealSize( 50.f, 50.f );
        case 2:
        default:
            return TAlfRealSize( 100.f, 100.f );
        }
    }
    
TAlfRealPoint CAlfPerfAppImageTestCaseBasic::PositionForIndex( 
        TInt aIndex, const TAlfRealSize& aSize )
    {
    const TSize KSize( Env().PrimaryDisplay().VisibleArea().Size() );
    
    switch ( ( iCycleCounter + aIndex ) & 3 )
        {
        case 0:
            return TAlfRealPoint( 0.0f, 0.0f ); 
            
        case 1:
            return TAlfRealPoint( KSize.iWidth - aSize.iWidth, KSize.iHeight - aSize.iHeight ); 
            
        case 2:
            return TAlfRealPoint( KSize.iWidth - aSize.iWidth, 0.0f ); 
            
        case 3:
        default:
            return TAlfRealPoint( 0.0f, KSize.iHeight - aSize.iHeight ); 
        }
    }

#ifdef __NVG
void CAlfPerfAppImageTestCaseBasic::ProvideBitmapL(TInt /*aId*/, CFbsBitmap*& aBitmap, CFbsBitmap*& /*aMaskBitmap*/)
    {
    aBitmap = iBitmapToBeProvided;
    }
#endif

// Implementation of CAlfPerfAppImageTestCaseManyImages:

CAlfPerfAppImageTestCaseManyImages::CAlfPerfAppImageTestCaseManyImages()
    {
    }

CAlfPerfAppImageTestCaseManyImages::~CAlfPerfAppImageTestCaseManyImages()
    {
    iImages.Close();
    
    delete iLoader;
    }
    
void CAlfPerfAppImageTestCaseManyImages::ConstructL( 
        CAlfEnv& aEnv, TInt aCaseId, const TRect& aVisibleArea, TInt aSequenceIndex, TInt aVisualCount )
    {
    CAlfPerfAppBaseTestCaseControl::ConstructL( aEnv, aCaseId, aVisibleArea );
    CAlfTextureManager& textureManager = aEnv.TextureManager();
    
    TFileName privatePath;
    CEikonEnv::Static()->FsSession().PrivatePath( privatePath );
    ::CompleteWithAppPath( privatePath );
    textureManager.SetImagePathL( privatePath );
    
	TSize displaySize(Env().PrimaryDisplay().VisibleArea().Size());
    TInt KItemStep = 20;
    
    // Parameters for test cases
    
    iImageFileName 			= _L("\\resource\\apps\\alfperfapp_imagetest.mif");	
    iVisualSpeed 			= TAlfRealPoint(1,0); 	
	iImageCount 			= 4;
    iVisualCount 			= (aVisualCount != -1) ? aVisualCount : KVisualCout;	
    iVisualColums			= displaySize.iWidth/KItemStep;
    iVisualRows				= displaySize.iHeight/KItemStep;;
    iTextureFlags 			= EAlfTextureFlagDefault;	
	iVisualSize				= TAlfRealSize( 25.f, 25.f );
    iVisualSizeVel			= TAlfRealPoint();	
	iSimulatedPointerEventCount = 0;
	iLastIndexInAnimatedArea 	= iVisualCount;
	iImageSize					= TSize( 64, 64 );
	iSequenceIndex 				= aSequenceIndex;
	
	if (iSequenceIndex != 0)
		{
      	TInt caseNum = SEQUENCE_LENGTH_SQRT - (aSequenceIndex-1) / SEQUENCE_LENGTH_SQRT - 1;
       	TReal trg = 0.0;
       	Math::Pow(trg, 2.0, caseNum);
       	TInt pow = trg+0.5;
       	TReal animatedAreaRelativeSize = 1.f/pow;
       	
       	iImageFileName = _L("alfperfapp_imagetest.png");
    	iLastIndexInAnimatedArea = iVisualCount*animatedAreaRelativeSize+0.5;
    	iLastIndexInAnimatedArea = (iLastIndexInAnimatedArea < 1) ? 1 : iLastIndexInAnimatedArea;
       	RDebug::Print(_L("CAlfPerfAppImageTestCaseManyImages::ConstructL(). Animated visual count = %d, percentage=%3.2f"), iLastIndexInAnimatedArea, animatedAreaRelativeSize);
    	
		trg = 0.0;
		Math::Sqrt(trg,TReal(iVisualCount));
    	iVisualColums = (trg + 0.5);
		iVisualRows = ((TReal)iVisualCount/iVisualColums) + 0.5;

		TSize displaySize(Env().PrimaryDisplay().VisibleArea().Size());
		iVisualSize = TAlfRealSize((TReal32)displaySize.iWidth/iVisualColums, (TReal32)displaySize.iHeight/iVisualRows);
		iImageCount = 1;	
		iImageSize	= TSize(iVisualSize.iWidth+0.5, iVisualSize.iHeight+0.5);

       	iTextureFlags = EAlfTextureFlagDoNotRetainResolution;
		}
    else
    	{
    	   
	    switch(CaseId())
	    	{
	        case EAlfPerfAppImageManyImages:
	        	break;
	       
	        case EAlfPerfAppImageManyGifImages:
	            iImageFileName = _L("alfperfapp_imagetest.gif");
	        	break;
	       
	        case EAlfPerfAppImageManyPngImages:
	            iImageFileName = _L("alfperfapp_imagetest.png");
	        	break;
	       
	        case EAlfPerfAppImageManySvgImages:
	            iImageFileName = _L("alfperfapp_imagetest.svg");
	        	break;
#ifdef __NVG
	        case EAlfPerfAppImageManyNvgCSImages:
	            iImageFileName = KAlfPerfAppImageTestCaseNvgCSData;
                //iImageFileName = _L("icons_mif_new2.mif");
                break;

	        case EAlfPerfAppImageManyNvgTLVImages:
				iImageFileName = KAlfPerfAppImageTestCaseNvgTLVData;
	            //iImageFileName = _L("TLV\\3musk.mif");
	            break;
#endif
	        case EAlfPerfAppImageManyImagesWithPointerEvent:
	        	iSimulatedPointerEventCount = 10;
	        	break;
	       
	        case EAlfPerfAppImageManyImagesWithAutoSize:
        		iTextureFlags = EAlfTextureFlagAutoSize;
        		iVisualSizeVel = TAlfRealPoint(10,10);
	        	break;
	        	
	        case EAlfPerfAppImageAnimateFull:
	        	{
    			iVisualCount = KVisualCout;	
				iLastIndexInAnimatedArea 	= iVisualCount * KRelativeAnimatedArea ;
				iImageFileName = _L("alfperfapp_imagetest.svg");
	        	
       			TReal trg = 0.0;
       			Math::Sqrt(trg,TReal(iVisualCount));
	        	iVisualColums = (trg + 0.5);
       			iVisualRows = ((TReal)iVisualCount/iVisualColums) + 0.5;

       			iVisualSize = TAlfRealSize((TReal32)displaySize.iWidth/iVisualColums, (TReal32)displaySize.iHeight/iVisualRows);
 	    		iImageCount = 1;	
 				iImageSize	= TSize(iVisualSize.iWidth+0.5, iVisualSize.iHeight+0.5);

	        	break;
	        	}
	        case EAlfPerfAppImageAnimate1of2:
	        	{
	        	TReal animatedAreaRelativeSize = 1.f/2.f;
    			iVisualCount = KVisualCout;	
    			iImageFileName = _L("alfperfapp_imagetest.svg");
	        	iLastIndexInAnimatedArea = iVisualCount*animatedAreaRelativeSize;
	        	
       			TReal trg = 0.0;
       			Math::Sqrt(trg,TReal(iVisualCount));
	        	iVisualColums = (trg + 0.5);
       			iVisualRows = ((TReal)iVisualCount/iVisualColums) + 0.5;

       			iVisualSize = TAlfRealSize((TReal32)displaySize.iWidth/iVisualColums, (TReal32)displaySize.iHeight/iVisualRows);
 	    		iImageCount = 1;	
 				iImageSize	= TSize(iVisualSize.iWidth+0.5, iVisualSize.iHeight+0.5);

	        	break;
	        	}
	       
	        case EAlfPerfAppImageAnimate1of4:
	        	{	
	        	TReal animatedAreaRelativeSize = 1.f/4.f;
    			iVisualCount = KVisualCout;	
    			iImageFileName = _L("alfperfapp_imagetest.svg");
	        	iLastIndexInAnimatedArea = iVisualCount*animatedAreaRelativeSize;
	        	
       			TReal trg = 0.0;
       			Math::Sqrt(trg,TReal(iVisualCount));
	        	iVisualColums = (trg + 0.5);
       			iVisualRows = ((TReal)iVisualCount/iVisualColums) + 0.5;

       			iVisualSize = TAlfRealSize((TReal32)displaySize.iWidth/iVisualColums, (TReal32)displaySize.iHeight/iVisualRows);
 	    		iImageCount = 1;	
 				iImageSize	= TSize(iVisualSize.iWidth+0.5, iVisualSize.iHeight+0.5);

	        	break;
	        	}
	       
	        case EAlfPerfAppImageAnimate1of8:
	        	{
	        	TReal animatedAreaRelativeSize = 1.f/8.f;
    			iVisualCount = KVisualCout;	
    			iImageFileName = _L("alfperfapp_imagetest.svg");
	        	iLastIndexInAnimatedArea = iVisualCount*animatedAreaRelativeSize;
	        	
       			TReal trg = 0.0;
       			Math::Sqrt(trg,TReal(iVisualCount));
	        	iVisualColums = (trg + 0.5);
       			iVisualRows = ((TReal)iVisualCount/iVisualColums) + 0.5;

       			iVisualSize = TAlfRealSize((TReal32)displaySize.iWidth/iVisualColums, (TReal32)displaySize.iHeight/iVisualRows);
 	    		iImageCount = 1;	
 				iImageSize	= TSize(iVisualSize.iWidth+0.5, iVisualSize.iHeight+0.5);

	        	break;
	        	}
	       
	    	default:
	            User::Leave( KErrNotSupported );
	            break;
	 		}
    	}
    
    TParsePtr parse(iImageFileName);
    	
    
#ifdef __NVG
    if ( (CaseId() == EAlfPerfAppImageManyNvgCSImages || 
         CaseId() == EAlfPerfAppImageManyNvgTLVImages) && 
         parse.Ext().CompareF(_L(".mif")) == KErrNone)
        {
        TAlfTextureFlags flags = EAlfTextureFlagDefault;
        // Use the modified AknIconUtils to create the extended bitmap
        CFbsBitmap* extendedBitmap;
        CFbsBitmap* extendedMask;
        
        AknIconUtils::CreateIconL( extendedBitmap, extendedMask, iImageFileName, 16384, 16385);
        AknIconUtils::ExcludeFromCache(extendedBitmap);
        AknIconUtils::SetSize( extendedBitmap, TSize(64,64) );
        
        // This will be returned, when ProvideBitmapL is called for creating the texture
        delete iBitmapToBeProvided;
        iBitmapToBeProvided = extendedBitmap;
        // We don't need the mask
        delete extendedMask;
        
        for ( TInt count = 0;  count < iImageCount; count++ )
            {
            CAlfTexture& texture = textureManager.CreateTextureL(count+1, this, flags);
            }
        }
    else if (parse.Ext().CompareF(_L(".mif")) == KErrNone)
#else
    if (parse.Ext().CompareF(_L(".mif")) == KErrNone)
#endif
        {    	
    	iLoader = new (ELeave) CAlfImageLoaderUtil;
    	iLoader->SetSize( iImageSize );
    	
    	// mif file is from absolute path \\resource\\apps\\
    	textureManager.SetImagePathL(_L(""));
    	
    	TInt baseId = 16384;
    	TInt count = 0;    	
	    while ( count < iImageCount )
	        {        
	        count++;
	        textureManager.CreateTextureL(
	            count,
	            iLoader->CreateImageLoaderL(
	                KAknsIIDNone,
	                iImageFileName,
	                baseId,
	                baseId + 1 ),
	            	iTextureFlags );
	        baseId += 2; // odd values are masks 
	        }   
       	}
    else if (parse.Ext().CompareF(_L(".svg")) == KErrNone)

    	{    	
    	iLoader = new (ELeave) CAlfImageLoaderUtil;
    	iLoader->SetSize( iImageSize );
    
    	TInt count = 0;    	
	    while ( count < iImageCount )
	        {        
	        count++;
	        textureManager.CreateTextureL(
	            count,
	            iLoader->CreateSVGImageLoaderL(
	                iImageFileName),
	            iTextureFlags );
	        }   
       	}
    else
    	{
	    for ( TInt count = 0;  count < iImageCount; count++ )
	        {        
	        CAlfTexture& texture = textureManager.LoadTextureL(iImageFileName, iTextureFlags, count+1);
	        }   
    	}
       	
 	for ( TInt ii = 0; ii < iVisualCount; ii++ )
        {
        CAlfImageVisual* visual = CAlfImageVisual::AddNewL( *this );
        iImages.AppendL( visual );        

        visual->SetFlag( EAlfVisualFlagManualLayout );
        visual->SetOpacity( 1.0f );
        TReal32 opacity = 1.0;
        if (OpacityForIndex( ii, opacity ))
        	{
   	        visual->SetOpacity( opacity );                                 
        	}
        visual->SetSize( iVisualSize );
        TAlfRealSize size(0,0);
        if (SizeForIndex( ii, size ))
        	{
   	        visual->SetSize( size );                                  
        	}
        
        visual->SetPos( TAlfRealPoint(0,0) );
        TAlfRealPoint pos(0,0);
        if (PositionForIndex( ii, pos ))
        	{
   	        visual->SetPos( pos );                                    
        	}
        
        const CAlfTexture* texture = textureManager.Texture( 1 + ( ii % iImageCount ) );
        TAlfImage image( *texture );
        visual->SetImage( image );
        
        }
    }
    
#ifdef __NVG
void CAlfPerfAppImageTestCaseManyImages::ProvideBitmapL(TInt /*aId*/, CFbsBitmap*& aBitmap, CFbsBitmap*& /*aMaskBitmap*/)
    {
    CFbsBitmap* bmp = new(ELeave) CFbsBitmap();
    bmp->Duplicate(iBitmapToBeProvided->Handle());
    aBitmap = bmp;
    };
#endif

void CAlfPerfAppImageTestCaseManyImages::DoStartExecuteL()
    {
    NextCycleL();
    
    CompleteAfterL( 5000 );
    }

void CAlfPerfAppImageTestCaseManyImages::SetVisibleArea( 
        const TRect& /*aVisibleArea*/ )
    {
    if ( IsExecutionOngoing() )
        {
        Env().CancelCustomCommands( this, KAlfPerfAppImageCmdNext );
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        }
    }

TBool CAlfPerfAppImageTestCaseManyImages::OfferEventL( const TAlfEvent& aEvent )
    {
    if ( aEvent.IsCustomEvent() && 
         aEvent.CustomParameter() == KAlfPerfAppImageCmdNext )
        {
        TRAPD( err, NextCycleL() );
        if ( err != KErrNone )
            {
            CompleteNow( err );
            }
        return ETrue;
        }
    
    return CAlfPerfAppBaseTestCaseControl::OfferEventL( aEvent );
    }

void CAlfPerfAppImageTestCaseManyImages::NextCycleL()
    {
    TAlfCustomEventCommand command( KAlfPerfAppImageCmdNext, this );
    User::LeaveIfError( Env().Send( command, 500 ) );
    
    iCycleCounter++;
    
    // Update cycle state
    for ( TInt ii = 0; ii < iImages.Count(); ii++ )
        {
        CAlfImageVisual* visual = iImages[ ii ];
                            
        TInt speed = KVisualAnimSpeed;
        TReal32 opacity = 1.0;
        if (OpacityForIndex( ii, opacity ))
        	{
   	        visual->SetOpacity( TAlfTimedValue( opacity, speed ) );                                 
        	}
        TAlfRealSize size(0,0);
        if (SizeForIndex( ii, size ))
        	{
   	        visual->SetSize( size, speed );                                  
        	}      
        
        TAlfRealPoint pos(0,0);
        if (PositionForIndex( ii, pos ))
        	{
   	        visual->SetPos( pos, speed );                                    
        	}
        }
    
    // Send pointer events, if wanted    
    for (TInt i=0; i <iSimulatedPointerEventCount; i++)
    	{
    	const TSize KSize( Env().PrimaryDisplay().VisibleArea().Size() );
 
   		TPoint pos(AlfUtil::RandomInt(0, KSize.iWidth-1), AlfUtil::RandomInt(0, KSize.iWidth-1));
   		
   		// down
    	TPointerEvent eventDown(TPointerEvent::EButton1Down, 0, pos, TPoint(0,0));
   	    Env().PrimaryDisplay().HandlePointerEventL( eventDown );
   	    
   	    //up
    	TPointerEvent eventUp(TPointerEvent::EButton1Up, 0, pos, TPoint(0,0));
   	    Env().PrimaryDisplay().HandlePointerEventL( eventUp );
    	}        
    }

TBool CAlfPerfAppImageTestCaseManyImages::OpacityForIndex( TInt aIndex, TReal32 &aOpacity )
    {
    if (aIndex <= iLastIndexInAnimatedArea)
    	{
    	if (iCycleCounter&0x01)
    		{
    		aOpacity = 0.4;
    		}
    	else
    		{
    		aOpacity = 0.8;
    		}
    	return ETrue;	
    	}
        
    
   	return EFalse;	
	
    }


TBool CAlfPerfAppImageTestCaseManyImages::SizeForIndex( TInt aIndex, TAlfRealSize& aSize )
    {
    // Call only if needed as Size().Target() will flush the command buffer!
    if ((iVisualSizeVel.iX != 0) && (iVisualSizeVel.iY != 0))
    	{
   		TAlfRealSize size = iImages[ aIndex ]->Size().Target();
    	aSize = TAlfRealSize( size.iWidth + iVisualSizeVel.iX, size.iHeight + iVisualSizeVel.iY );
    	return ETrue;
    	}
    return EFalse;	
    }
    
    
TBool CAlfPerfAppImageTestCaseManyImages::PositionForIndex( 
        TInt aIndex, TAlfRealPoint& aPos )
    {
    if (iCycleCounter == 0 || aIndex <= iLastIndexInAnimatedArea)
    	{
    	TInt index = aIndex + iCycleCounter;
    	TInt gridIndex = index;
    	TInt columnPos = gridIndex % iVisualColums;
    	TInt rowPos = aIndex / iVisualColums;
     	columnPos = columnPos % iVisualColums;
    	aPos = TAlfRealPoint( columnPos*iVisualSize.iWidth, rowPos*iVisualSize.iHeight );   		    	
        return ETrue;
   		}
   	else
   		{
        return EFalse;
   		}
    }


TTestCaseSpecificResultText CAlfPerfAppImageTestCase::getCaseSpecificResultL()
    {
    TTestCaseSpecificResultText result = HBufC::NewL(KAlfPerfAppMaxCharsInSpecificResultText);
    if(SequenceIndex() != 0 && CaseID() == EAlfPerfAppImageManyImages)
        {
        result->Des().Append(_L("Total Visual Count: "));
        result->Des().AppendNum(iTotalVisualCount);
        result->Des().Append(_L(" Animated: 1/"));
        result->Des().AppendNum(iAnimationDivider);
        }
    return result;
        
    }
