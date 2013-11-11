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
* Description:   Texture
*
*/



#include "alf/alftextureprocessor.h"
#include "alf/alftexture.h"
#include "alf/alfenv.h"
#include "alf/alftexturemanager.h"
#include "alfclient.h"

struct CAlfTextureProcessor::TPrivateData
    {
    CAlfEnv* iEnv;
    };



// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfTextureProcessor::CAlfTextureProcessor()
	{
		
	}


// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
CAlfTextureProcessor::~CAlfTextureProcessor()
	{
	if ( iData )
	    {
	    }
	delete iData;
	iData = NULL;
	}

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
CAlfTextureProcessor* CAlfTextureProcessor::NewL(CAlfEnv& aEnv)
	{
	CAlfTextureProcessor* self = 
		CAlfTextureProcessor::NewLC(aEnv);        
    CleanupStack::Pop( self );
    return self;		
	}
	

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
CAlfTextureProcessor* CAlfTextureProcessor::NewLC(CAlfEnv& aEnv)
	{
    CAlfTextureProcessor* self = new( ELeave ) CAlfTextureProcessor;
    CleanupStack::PushL( self );
    self->ConstructL(aEnv);
    return self;	
	}

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
void CAlfTextureProcessor::ConstructL(CAlfEnv& aEnv)
	{
    iData = new (ELeave) TPrivateData;
    iData->iEnv = &aEnv;
	}

// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextureProcessor::BlurL(const CAlfTexture& aSrc, CAlfTexture& aDest,
    TInt aFilterSize, TInt aFlags)
    {
    // By default, use a quarter of the original size.
    TSize originalSize = aSrc.Size();
    return BlurL(aSrc, aDest,
                 TSize(originalSize.iWidth/2, originalSize.iHeight/2),
                 aFilterSize, aFlags);        
    }   
                        
// ---------------------------------------------------------------------------
// ?description
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTextureProcessor::BlurL(const CAlfTexture& aSrc, CAlfTexture& aDest,   
    const TSize& aPreferredSize, TInt aFilterSize, TInt aFlag)
    {
    iData->iEnv->Client().TextureProcessorBlurL( aSrc.ServerHandle(), 
	    aDest.ServerHandle(), 
	    0, 
	    aPreferredSize, 
	    aFilterSize, 
	    aFlag );        
    }


