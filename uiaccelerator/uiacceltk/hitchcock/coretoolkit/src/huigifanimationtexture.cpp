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
* Description:   ?Description
*
*/



#include <uiacceltk/huigifanimationtexture.h>
#include <uiacceltk/HuiStatic.h>
#include <driveinfo.h>

CHuiGifAnimationTexture::TAnimationState SetFlag(TInt aValue, TInt aFlag)
    {
    return CHuiGifAnimationTexture::TAnimationState(aValue|aFlag);
    }

CHuiGifAnimationTexture::TAnimationState ClearFlag(TInt aValue, TInt aFlag)
    {      
    return CHuiGifAnimationTexture::TAnimationState(aValue^aFlag);
    }


EXPORT_C CHuiGifAnimationTexture* CHuiGifAnimationTexture::NewL(const TDesC& aFilename, 
                                                                CHuiTextureManager& aManager,
                                                                TInt aID,
                                                                THuiTextureUploadFlags aFlags)
    {
    CHuiGifAnimationTexture* self = new(ELeave) CHuiGifAnimationTexture(aManager,aFlags, aID);
    CleanupStack::PushL(self);
    self->ConstructL(aFilename);
    CleanupStack::Pop(); // self
    return self;
    }

EXPORT_C void CHuiGifAnimationTexture::Start()
    {
    iAnimationState = SetFlag(TInt(iAnimationState), TInt(ERunning));        
    if (iAnimationState&EStopped)
        {
        iAnimationState=ClearFlag(TInt(iAnimationState), TInt(EStopped));
        }
    
    if (iTexture)
        {
        iTexture->SetTextureChanged(ETrue);        
        }
    CHuiStatic::ContinueRefresh();
    }

EXPORT_C void CHuiGifAnimationTexture::Stop()
    {
    if (iAnimationState&ERunning)
        {
        iAnimationState=ClearFlag(TInt(iAnimationState), TInt(ERunning));
        }
    iAnimationState = SetFlag(TInt(iAnimationState), TInt(EStopped));
    }
    

EXPORT_C void CHuiGifAnimationTexture::EnableAnimation(TBool aEnable)
    { 
    if (aEnable)
    	{
    	iAnimationState = SetFlag(TInt(iAnimationState), TInt(EEnabled));	
    	}
    else 
    	{
    	iAnimationState = ClearFlag(TInt(iAnimationState), TInt(EEnabled));
    	}
    CHuiStatic::ContinueRefresh();
    }

EXPORT_C TInt CHuiGifAnimationTexture::Id() 
    {
    return iId;
    }
    
void CHuiGifAnimationTexture::AdvanceTime(TReal32 aElapsedTime) __SOFTFP
    {
    if (((iAnimationState & EInitializing) || (iFrameInterval == -1)) || (iAnimationState & EStopped)
    	|| !(iAnimationState & EEnabled))
        {
        return;
        }

    iElapsedTime+=aElapsedTime;
                                             
    TReal32 time = (TReal(iFrameInterval))/1000000.0f;
    if (iElapsedTime >= time)
        {
        const TInt loadingTextureId = iCurrentTexture ? iTextureId1 : iTextureId2;
        if ( !iManager.IsLoaded( loadingTextureId ) )
            {
            return;
            }

        iCurrentTexture^=1;
    
        if (iCurrentTexture)
            // use texture 2
            {
            iTexture = iTexture2;
            TInt nextToLoad = iTexture2->iFrameNumber+1;
            if (nextToLoad >= iFrameCount)
                {
                nextToLoad = 0;
                }
            
            delete iTexture1;
            iTexture1 = 0;   
                       
            TRAP_IGNORE(iTexture1 = &iManager.LoadTextureL(*iFilename,EHuiTextureUploadFlagDefault, iTextureId1, nextToLoad))
            }
        else
            // use texture 1    
            {
            iTexture = iTexture1;
            TInt nextToLoad = iTexture1->iFrameNumber+1;
            if (nextToLoad >= iFrameCount)
                {
                nextToLoad = 0;
                }
            
            delete iTexture2;
            iTexture2 = 0;   
 
            TRAP_IGNORE(iTexture2 = &iManager.LoadTextureL(*iFilename,EHuiTextureUploadFlagDefault, iTextureId2, nextToLoad))

            }
        iElapsedTime = 0.0f;
        
        }
    // not enough time was elapsed,
    // if the animation is not stopped
    // request continuerefresh in order
    // not to go to sleep
    else 
        {
        iTexture->SetTextureChanged(ETrue);            
        CHuiStatic::ContinueRefresh();
        }
    }
    

void CHuiGifAnimationTexture::TextureLoadingCompleted(CHuiTexture& aTexture,
                             TInt aTextureId,
                             TInt /*aErrorCode*/)
    {
    if ((iAnimationState & EInitializing) && aTexture.iFrameCount > 1 && aTextureId == iTextureId1)
        {
        delete iTexture;
        iTexture = iTexture1;
        delete iTexture2;
        iTexture2 = NULL;
        TRAP_IGNORE(iTexture2 = &iManager.LoadTextureL(*iFilename,EHuiTextureUploadFlagDefault, iTextureId2, 1))
        iFrameInterval = aTexture.iFrameInterval;
        iFrameCount = aTexture.iFrameCount;
        return;
        }
    if ((iAnimationState & EInitializing) && aTextureId == iTextureId2)
        {
        iAnimationState = SetFlag(TInt(iAnimationState),TInt(ERunning));
        iAnimationState = ClearFlag(TInt(iAnimationState), TInt(EInitializing));
        }
    }


CHuiGifAnimationTexture::CHuiGifAnimationTexture(CHuiTextureManager& aManager,THuiTextureUploadFlags aFlags,TInt aId) : 
    CHuiAnimatedTexture(aManager),
    iTexture2(NULL),
    iCurrentTexture(0),
    iAnimationState(EInitializing),
    iFrameInterval(-1),
    iFrameCount(0),
    iFlags(aFlags),
    iId(aId),
    iOriginalFileInUse(EFalse)
    {
    }

void CHuiGifAnimationTexture::ConstructL(const TDesC& aFilename)
    {
    CHuiAnimatedTexture::BaseConstructL();
    iTextureId1 = CHuiStatic::GenerateId();
    iTextureId2 = CHuiStatic::GenerateId();
    
    // To load GIF animation, you should load textures in sequence - 
    // previous state is needed. Thus, this group indicates that
    // previous state can be shared between these textures.
    RArray<TInt> groupIds;
    CleanupClosePushL( groupIds );
    groupIds.AppendL( iTextureId1 );
    groupIds.AppendL( iTextureId2 );
    iAnimatedTextureGroupId = iManager.AddAnimatedTextureGroupL( groupIds );
    CleanupStack::PopAndDestroy( &groupIds );
    
	// Alf locks the file while drawing textures hence preventing any other operations on this file
	// so copy file to RAM and use that instead.
    TParse parse;
    parse.Set(aFilename, NULL, NULL);
    parse.NameAndExt();
    
    TChar drive;
    User::LeaveIfError( DriveInfo::GetDefaultDrive(DriveInfo::EDefaultRam, drive ) );
        
    // 32 extra for textureid and 4 for "temp"
    iFilename = HBufC::NewL(parse.NameAndExt().Size() + 4 + 32+ 4);
    TPtr ptr = iFilename->Des();
    
    ptr.Append(drive);
    ptr.Append(_L(":\\temp"));
    // with the texture id:s the
    // name should be "unique"
    ptr.AppendNum(iTextureId1);
    ptr.AppendNum(iTextureId2);
    ptr.Append(parse.NameAndExt());
    
    RFs fs;
    User::LeaveIfError(fs.Connect());
    
    CFileMan* fileman = CFileMan::NewL(fs);
    CleanupStack::PushL(fileman);
    TInt err = fileman->Copy(aFilename, iFilename->Des());
    
    CleanupStack::PopAndDestroy();
    
    if(err != KErrNone)
    	{
	    iOriginalFileInUse = ETrue;
	    delete iFilename;    
        iFilename = aFilename.AllocL();
	    }
    
    
    iTexture1 = &iManager.LoadTextureL(
        *iFilename,EHuiTextureUploadFlagDefault, iTextureId1, 0);     
    iManager.iLoadObservers.AppendL(*this);
    
   	iAnimationState = SetFlag(TInt(iAnimationState), TInt(EEnabled));	
    }

EXPORT_C CHuiGifAnimationTexture::~CHuiGifAnimationTexture()
    {
    if ( iAnimatedTextureGroupId )
        {
        iManager.RemoveAnimatedTextureGroup( iAnimatedTextureGroupId );
        }
    iManager.iLoadObservers.RemoveIfFound(*this);

    delete iTexture1;
    delete iTexture2;

    iTexture = 0;    
    
    // Don't delete the file if the original
    // file was in use instead of the RAM copy
    if (!iOriginalFileInUse)
        {
        RFs fs;
        TInt err = fs.Connect();
        if(err == KErrNone)    
            {
            CFileMan* fileman = 0;
            TRAP(err, fileman = CFileMan::NewL(fs));
            if(err == KErrNone)
                {
                fileman->Delete(iFilename->Des());
                }
            }
        }

    delete iFilename;
    }


