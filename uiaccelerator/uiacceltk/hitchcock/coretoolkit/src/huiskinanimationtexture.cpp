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
* Description:   Texture class to support Legacy theme animation
*
*/


#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/huiskinanimationtexture.h>
#include <uiacceltk/HuiStatic.h> 
#include <uiacceltk/HuiTextureIf.h>
#include <uiacceltk/HuiThemeManager.h>

#include <AknsItemData.h>
#include <AknsUtils.h>
#include <AknsItemDef.h>


NONSHARABLE_CLASS(CHuiSkinBitmapProvider) : public CBase, public MHuiBitmapProvider
	{
public:
	CHuiSkinBitmapProvider()
	    {}
	// from MHuiBitmapProvider
	void ProvideBitmapL(TInt, CFbsBitmap*&, CFbsBitmap*&)
	    {
	    User::Leave(KErrNotSupported);
	    }
							 
    void SetSize(TSize)
        {
        }
    
    TInt NumberOfImages()	// Number of images in anim.
        {
        return 0;    
        }
	
	// Load bitmap data based on Item ID
	TBool LoadBitmapDataL(const TAknsItemID& )
	    {
	    return EFalse;
	    }
		
	// Frame interval of current animation frame
	TInt FrameInterval()
	    {
	    return -1;
	    }
 
    /*	The animation can be played in any of the ways described below. */
	enum EHuiPlayMode
		{
		/** Plays the animation once, from the first frame to the last one. */
		EPlay		= 0,
		/** Plays the animation from the first frame to the last one 
					continuously. */
		ECycle		= 1,
		/** Plays the animation from the first frame to the last one then 
					from the last frame to the first continuously. */
		EBounce		= 2
		};

	TSize iSize;
	CAknsBmpAnimItemData* iAnimData;	// Animation data retrieved from Skin
	TAknsBmpAnimFrameInfo* iFramesInfo;	// Frame based info from iAnimData
	TInt iFrameInterval;				// Interval at which frame should be refreshed
	TInt iBmpCount;					// Number of images in the animation
	TInt iCounter;					// Index of animation being shown now
    EHuiPlayMode iPlayMode;			// Play mode- cycle, play or bounce
    TInt iBounceIncreasing;
	};


/*------------------------------Beging of CHuiAnimationTexture---------------*/
    
// ---------------------------------------------------------------------------
// DEPRECATED: NewL:: Two phase constructor
// ---------------------------------------------------------------------------
//
EXPORT_C CHuiSkinAnimationTexture* CHuiSkinAnimationTexture::NewL(
														const TDesC& aFilename, 
                                                        CHuiTextureManager& aManager,
                                                        TInt aID,
                                                        THuiTextureUploadFlags aFlags)
    {
    CHuiSkinAnimationTexture* self = 
    				new(ELeave) CHuiSkinAnimationTexture(aManager,aFlags, aID);
    CleanupStack::PushL(self);
    self->ConstructL(aFilename);
    CleanupStack::Pop(); // self
    return self;
    }

// ---------------------------------------------------------------------------
// Start():: Start animation by setting status flag
// ---------------------------------------------------------------------------
//
EXPORT_C void CHuiSkinAnimationTexture::Start()
    {
    iAnimationState |= ERunning;        
    iAnimationState &= ~EStopped;
    }

// ---------------------------------------------------------------------------
// Stop():: Stop animation by setting status flag
// ---------------------------------------------------------------------------
//
EXPORT_C void CHuiSkinAnimationTexture::Stop()
    {
    iAnimationState &= ~ERunning;
    iAnimationState |= EStopped;
    }
    
// ---------------------------------------------------------------------------
// Id():: Retrieve Texture Id
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CHuiSkinAnimationTexture::Id() 
    {
    return iId;
    }
    
// ---------------------------------------------------------------------------
// AdvanceTime():: Animation callback from Framework
// ---------------------------------------------------------------------------
//
void CHuiSkinAnimationTexture::AdvanceTime(TReal32 aElapsedTime) __SOFTFP
    {
    iFrameInterval = iSkinBitmapProvider->FrameInterval();

    if (((iAnimationState & EInitializing) || (iFrameInterval == -1)) || (iAnimationState&EStopped))
        {
        return;
        }

    iElapsedTime+=aElapsedTime;
    
                                             
    TReal32 time = (TReal(iFrameInterval))/1000.0f;
    if (iElapsedTime >= time)
        {

      	if(iManager.IsLoaded(iId))
      	    {
      	    iManager.UnloadTexture(iId);
      	    }

    	iId = CHuiStatic::GenerateId();

        TRAP_IGNORE(iTexture = &iManager.CreateTextureL(iId,
        					(MHuiBitmapProvider*)iSkinBitmapProvider,iFlags));

        iElapsedTime = 0.0f;
        } 
    }
    

// ---------------------------------------------------------------------------
// TextureLoadingCompleted():: Callback from Framework..finished loading aTexture
// ---------------------------------------------------------------------------
//
void CHuiSkinAnimationTexture::TextureLoadingCompleted(CHuiTexture& ,//aTexture,
                             TInt aTextureId,
                             TInt aErrorCode)
    {
    if (aErrorCode == KErrNone && iAnimationState & EInitializing && aTextureId == iId)
        {
        iAnimationState |= ERunning;
        iAnimationState &= ~EInitializing;
        }
    }


// ---------------------------------------------------------------------------
// Constructor CHuiSkinAnimationTexture
// ---------------------------------------------------------------------------
//
CHuiSkinAnimationTexture::CHuiSkinAnimationTexture(CHuiTextureManager& aManager,
										THuiTextureUploadFlags aFlags,TInt aId) : 
									    CHuiAnimatedTexture(aManager),
									    iFrameInterval(-1),
									    iFrameCount(0),
									    iAnimationState(EInitializing),
									    iFlags(aFlags),
									    iId(aId)
    {
    }

// ---------------------------------------------------------------------------
// DEPRECATED: ConstructL : 2 phase constructor
// ---------------------------------------------------------------------------
//
void CHuiSkinAnimationTexture::ConstructL(const TDesC&)
    {
    User::Leave(KErrNotSupported);
    }

// ---------------------------------------------------------------------------
// Destructor 
// ---------------------------------------------------------------------------
//
EXPORT_C CHuiSkinAnimationTexture::~CHuiSkinAnimationTexture()
    {
    iManager.iLoadObservers.RemoveIfFound(*this);
    delete iSkinBitmapProvider;
    }
