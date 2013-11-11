/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Texture
*
*/



#include "alf/alftexture.h"
#include "alf/alfenv.h"
#include "alf/alftexturemanager.h"
#include "alfclient.h"

struct CAlfTexture::TPrivateData
    {
    TInt iServerSideHandle;

    CAlfEnv* iEnv;

    /** Specify upload behavior - how to convert the bitmap
      to texture. */
    TAlfTextureFlags iFlags;

    /** Resource location for the texture. */
    HBufC* iFileName;

    /** Bitmap content provider. Alternative content (re)loading
     * mechanism to filenames (loading from a file). */
    MAlfBitmapProvider* iBitmapProvider; 

    /** A flag to tell if texture is ready to be used */
    TBool iHasContent;

    TSize iTextureMaxSize;

    TSize iSize;

    TInt iId;
    
    TUid iManagerId;
    
    TBool iAnimated;
    
    TInt iPriority;
    
    /** Flags to tell that texture has been released by unloading or release */
    TInt iReleaseFlags;
    
    TAlfTextureAutoSizeParams iAutoSizeParams;
    
    TBool iRefCounted;
    
    TInt iRefCountingAction;
    };


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfTexture::CAlfTexture()
	{
		
	}


// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTexture::~CAlfTexture()
	{
	if ( iData )
	    {
	    iData->iEnv->Client().TextureDelete(iData->iId, iData->iManagerId.iUid);    
	    delete iData->iFileName;
	    iData->iFileName = NULL;

    	CAlfTextureManager* manager = NULL;
    	if (iData->iManagerId == TUid::Uid(0))
    	    {
    	    manager = &iData->iEnv->TextureManager();	        
    	    }
    	else
    	    {
    	    manager = iData->iEnv->SharedTextureManager(iData->iManagerId);	            
    	    } 	       

    	if (iData->iId != 0)
    	    {
        	manager->RemoveTexture(*this);
    	    }
	    }
	delete iData;
	iData = NULL;
	}

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
CAlfTexture* CAlfTexture::NewL(CAlfEnv& aEnv, TUid aManagerUid, TInt aId, 
    TInt aBitmapHandle, TInt aMaskHandle, TAlfTextureFlags aFlags,TBool aAnimated)
	{
	CAlfTexture* self = 
		CAlfTexture::NewLC(aEnv, aManagerUid, aId, aBitmapHandle, aMaskHandle, aFlags,aAnimated);        
    CleanupStack::Pop( self );
    return self;		
	}
	

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
CAlfTexture* CAlfTexture::NewLC(CAlfEnv& aEnv, TUid aManagerUid, TInt aId, 
    TInt aBitmapHandle, TInt aMaskHandle, TAlfTextureFlags aFlags,TBool aAnimated)
	{
    CAlfTexture* self = new( ELeave ) CAlfTexture();
    CleanupStack::PushL( self );
    self->ConstructL(aEnv, aManagerUid, aId, aBitmapHandle, aMaskHandle, aFlags,aAnimated);
    return self;	
	}

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfTexture::ConstructL(CAlfEnv& aEnv, TUid aManagerUid, TInt aId, 
    TInt aBitmapHandle, TInt aMaskHandle, TAlfTextureFlags aFlags, TBool aAnimated)
	{
    iData = new (ELeave) TPrivateData;
    iData->iEnv = &aEnv;
    iData->iId = aId;
    iData->iFileName = NULL;
    iData->iBitmapProvider = NULL;
    iData->iFlags = aFlags;
    iData->iTextureMaxSize = TSize(0,0);
    iData->iManagerId = aManagerUid; 
    iData->iAnimated = aAnimated;
    iData->iPriority = EAlfTexturePriorityNormal;
    iData->iReleaseFlags = 0;
    iData->iAutoSizeParams = TAlfTextureAutoSizeParams();
    iData->iRefCounted = EFalse;
    iData->iRefCountingAction = CAlfTexture::ERefCountingActionUnload;
    if (aAnimated)
        {
        iData->iServerSideHandle = NULL;
        }
    else
        {
        
        iData->iServerSideHandle = 
        	iData->iEnv->Client().TextureCreateL( aId, aBitmapHandle, 
        	aMaskHandle, aFlags, iData->iManagerId.iUid );    	            
	    }
    if (aBitmapHandle != 0 || aMaskHandle != 0)
        {
        iData->iHasContent = ETrue;	
        }
    else
        {
        iData->iHasContent = EFalse;	        
        }    
	
	CAlfTextureManager* manager = NULL;
	if (iData->iManagerId == TUid::Uid(0))
	    {
	    manager = &iData->iEnv->TextureManager();	        
	    }
	else
	    {
	    manager = iData->iEnv->SharedTextureManager(iData->iManagerId);	            
	    } 
	    
	if (iData->iId != 0)
	    {
    	manager->AppendTextureL(this);	        
	    }
	}

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfTexture::SetServerHandle(TInt aHandle)
    {
    iData->iServerSideHandle = aHandle;
    }
    
// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTexture::ServerHandle() const
    {
    return iData->iServerSideHandle;
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfTexture::SetBitmapProvider(MAlfBitmapProvider* aBitmapProvider)
    {
    iData->iBitmapProvider = aBitmapProvider;
    }
    
// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
MAlfBitmapProvider* CAlfTexture::BitmapProvider() const
    {
    return iData->iBitmapProvider;                
    }
    
// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfTexture::SetFileNameL(const TFileName & aFileName)
    {
    delete iData->iFileName;
    iData->iFileName = NULL;
    if (aFileName.Length()>0)
        {
        iData->iFileName = aFileName.AllocL();
        if (iData->iAnimated)
            {
            iData->iServerSideHandle = 
        	iData->iEnv->Client().TextureCreateAnimatedL( iData->iId, 
            iData->iFlags, iData->iManagerId.iUid,*iData->iFileName );    	            
            }
        }       
    }
    
// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C HBufC* CAlfTexture::FileName() const
    {
    return iData->iFileName;                        
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTexture::HasContent() const
    {
    TBool retVal = EFalse;
    iData->iEnv->Client().TextureHasContent(retVal, 
        iData->iId, 
        iData->iManagerId.iUid);
    return retVal;                                
    }
    
// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfTexture::SetMaxTextureSize(TSize aTextureMaxSize)
    {
    iData->iTextureMaxSize = aTextureMaxSize;    
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C TSize CAlfTexture::MaxTextureSize()
    {
    return iData->iTextureMaxSize;    
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTexture::Id() const
    {
    return iData->iId;        
    }
    
// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
TAlfTextureFlags CAlfTexture::Flags() const
    {
    return iData->iFlags;                
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfTexture::SetFlags(TAlfTextureFlags aFlags)
    {
    iData->iFlags = aFlags;    
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfTexture::SetSize(TSize aSize)
    {
    iData->iSize = aSize;
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTexture::IsAnimated() const
    {
    return iData->iAnimated;
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTexture::StartAnimation()
    {
    if (iData->iAnimated)
        {
        iData->iEnv->Client().TextureStartAnimation( iData->iId );
        }
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTexture::StopAnimation()
    {
    if (iData->iAnimated)
        {
        iData->iEnv->Client().TextureStopAnimation( iData->iId );
        }
    }
 
// ---------------------------------------------------------------------------
// Sets the texture release priority
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfTexture::SetPriority( TInt aPriority )
    {
    iData->iPriority = aPriority;
    }

// ---------------------------------------------------------------------------
// Returns the texture release priority
// ---------------------------------------------------------------------------
//    
EXPORT_C TInt CAlfTexture::Priority() const
    {
    return iData->iPriority;
    }


// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C TSize CAlfTexture::Size() const
    {
    return iData->iSize;
    }
    
// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
TInt CAlfTexture::ReleaseFlags() const
    {
    return iData->iReleaseFlags;
    }

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfTexture::SetReleaseFlags(TInt aReleaseFlags) 
    {
    iData->iReleaseFlags = aReleaseFlags;
    }

// ---------------------------------------------------------------------------
// Sets autosize related parameters.
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTexture::SetAutoSizeParams
    (const TAlfTextureAutoSizeParams& aParams)
    {
    TInt err = iData->iEnv->Client().TextureSetAutoSizeParams( iData->iId, 
        iData->iManagerId.iUid, aParams );
    if (err == KErrNone)
        {
        iData->iAutoSizeParams = aParams;            
        }    
    }
    
// ---------------------------------------------------------------------------
// Gets autosize related parameters.
// ---------------------------------------------------------------------------
//
EXPORT_C TAlfTextureAutoSizeParams CAlfTexture::AutoSizeParams() const
    {
    return iData->iAutoSizeParams;        
    }





// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTexture::EnableRefCounting(TBool aEnable)
    {
    iData->iRefCounted = aEnable;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TBool CAlfTexture::RefCountingEnabled()
    {
    return iData->iRefCounted;    
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTexture::SetRefCountingAction(TInt aAction)
    {
    iData->iRefCountingAction = aAction;                    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTexture::RefCountingAction() const
    {
    return iData->iRefCountingAction;            
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
CAlfTextureManager* CAlfTexture::TextureManager()
    {
	CAlfTextureManager* manager = NULL;
	if (iData->iManagerId == TUid::Uid(0))
	    {
	    manager = &iData->iEnv->TextureManager();	        
	    }
	else
	    {
	    manager = iData->iEnv->SharedTextureManager(iData->iManagerId);	            
	    }
	return manager;     	               
    }
    

