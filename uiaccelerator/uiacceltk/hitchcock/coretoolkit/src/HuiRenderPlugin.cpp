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



#include "HuiRenderPlugin.h"
#include "uiacceltk/huifontmanager.h"

EXPORT_C CHuiRenderPlugin::CHuiRenderPlugin(THuiRenderPluginId aId)
        : iId(aId)
    {
    }

EXPORT_C CHuiRenderPlugin::~CHuiRenderPlugin()
    {
    }


EXPORT_C THuiRenderPluginId CHuiRenderPlugin::Id() const
    {
    return iId;
    }


EXPORT_C TBool CHuiRenderPlugin::Allows(THuiRenderPluginAllow /*aAllow*/) const
    {
    // Allow full functionality by default.
    return ETrue;
    }


EXPORT_C MHuiRenderSurface* CHuiRenderPlugin::CreateRenderSurfaceL(CHuiDisplay& /*aDisplay*/)
    {
    return NULL;
    }


EXPORT_C CHuiGc* CHuiRenderPlugin::CreateGcL()
    {
    return NULL;
    }


EXPORT_C CHuiTexture* CHuiRenderPlugin::CreateTextureL(const THuiTextureHandle* /*aExistingTexture*/)
    {
    return NULL;
    }


EXPORT_C CHuiTexture* CHuiRenderPlugin::CreateTextureLC(const THuiTextureHandle* aExistingTexture)
    {
    CHuiTexture* texture = CreateTextureL(aExistingTexture);
    CleanupStack::PushL(texture);
    return texture;
    }


EXPORT_C CHuiTextureManager* CHuiRenderPlugin::CreateTextureManagerL(CHuiEnv& /*aEnv*/)
    {
    return NULL;
    }

EXPORT_C CHuiFxEngine* CHuiRenderPlugin::CreateEffectsEngineL()
    {
    return NULL;
    }

EXPORT_C THuiFontManager* CHuiRenderPlugin::CreateFontManagerL() const
    {
    // Construct the default font manager instance. This can
    // be overridden in CHuiRenderPlugin - derived classes if 
    // specialized font manager is to be used.
    THuiFontManager* fontManager = new (ELeave) THuiFontManager();
    return fontManager;
    }


EXPORT_C CHuiMesh* CHuiRenderPlugin::CreateMeshL(THuiMeshType /*aMeshType*/)
    {
    return NULL;
    }


EXPORT_C CHuiCurvePath* CHuiRenderPlugin::CreateCurvePathL()
    {
    return NULL;
    }


EXPORT_C void CHuiRenderPlugin::DeleteNamedTexture(TUint /*aName*/)
    {
    }


EXPORT_C void CHuiRenderPlugin::NotifyDisplayCountL(TUint /*aCount*/)
    {
    }

    
EXPORT_C void CHuiRenderPlugin::Release()
	{
	}


EXPORT_C void CHuiRenderPlugin::RestoreL()
	{
	}

EXPORT_C void CHuiRenderPlugin::EnableOffScreenBitmapL(TInt /*aFlags*/)
	{	
	}

EXPORT_C void CHuiRenderPlugin::DisableOffScreenBitmap()
	{	
	}

EXPORT_C CFbsBitmap* CHuiRenderPlugin::OffScreenBitmap() const
	{
	return NULL;
	}

EXPORT_C void CHuiRenderPlugin::UpdateOffScreenBitmapL(CHuiDisplay& /*aDisplay*/)
	{	
	}

EXPORT_C void CHuiRenderPlugin::DrawOffScreenBitmapToWindowL(CHuiDisplay& /*aDisplay*/)
	{	
	}

EXPORT_C CHuiCanvasGc* CHuiRenderPlugin::CreateCanvasGcL()
    {
    return NULL;        
    }

EXPORT_C void CHuiRenderPlugin::AddRestoreStateFlags(TInt /*aFlags*/)
    {
    }

EXPORT_C TInt CHuiRenderPlugin::GetRestoreStateFlags()
    {
    return 0;
    }

EXPORT_C void CHuiRenderPlugin::ClearRestoreStateFlags()
    {
    }
	
EXPORT_C void CHuiRenderPlugin::RenderPluginExtension(const TUid& /*aExtensionUid*/, TAny** aExtensionParams)
    {
    // If no extension with given UID was found, indicate it by returning null
    *aExtensionParams = NULL;
    }
