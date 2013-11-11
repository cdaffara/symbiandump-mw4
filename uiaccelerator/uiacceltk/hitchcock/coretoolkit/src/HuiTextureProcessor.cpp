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
* Description:   Implementation of CHuiTextureProcessor. CHuiTextureProcessor 
*                is the abstract base class for texture processors.
*
*/



#include "uiacceltk/HuiTextureProcessor.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiUtil.h"


EXPORT_C CHuiTextureProcessor::CHuiTextureProcessor(CHuiEnv& aEnv)
        : iEnv(aEnv)
    {
    }

EXPORT_C CHuiTextureProcessor::~CHuiTextureProcessor()
    {
    }
    

EXPORT_C CHuiEnv& CHuiTextureProcessor::Env()
    {
    return iEnv;
    }


EXPORT_C void CHuiTextureProcessor::Release()
    {
    HUI_DEBUG(_L("CHuiTextureProcessor::Release() - Called. -=- NOT IMPLEMENTED -=-"));
    }
    
    
EXPORT_C void CHuiTextureProcessor::RestoreL()
    {
    HUI_DEBUG(_L("CHuiTextureProcessor::RestoreL() - Called. -=- NOT IMPLEMENTED -=-"));
    }

EXPORT_C void CHuiTextureProcessor::TextureProcessorExtension(const TUid& /*aExtensionUid*/, TAny** aExtensionParams)
    {
    // If no extension with given UID was found, indicate it by returning null
    *aExtensionParams = NULL;
    }

