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



#include "uiacceltk/huiproceduralmesh.h"
#include "uiacceltk/HuiStatic.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiProbe.h"
#include "uiacceltk/HuiUtil.h"


EXPORT_C CHuiProceduralMesh::CHuiProceduralMesh()
    {
    iMaterial.iPreset = EHuiMaterialChrome;
    }

EXPORT_C CHuiProceduralMesh::~CHuiProceduralMesh()
    {
    }
    
    
EXPORT_C void CHuiProceduralMesh::Reset()
    {
    iMaterial = THuiMaterial();
    iMaterial.iPreset = EHuiMaterialChrome;
    }


EXPORT_C void CHuiProceduralMesh::MakeCubeL(TReal32 /*aSizeX*/, TReal32 /*aSizeY*/, TReal32 /*aSizeZ*/, 
                                  TReal32 /*aEdgeRadius*/) __SOFTFP
    {
    }


EXPORT_C void CHuiProceduralMesh::MakeSphereL(TReal32 /*aRadius*/, TInt /*aColumns*/, TInt /*aRows*/) __SOFTFP
    {
    }


EXPORT_C void CHuiProceduralMesh::MakeTorusL(TReal32 /*aMainRadius*/, TReal32 /*aOuterRadius*/, 
                                   TInt /*aMainSegments*/, TInt /*aOuterSegments*/,
                                   TReal32 /*aSegmentAngleOffset*/) __SOFTFP
    {
    }


EXPORT_C void CHuiProceduralMesh::StretchUniformly(TReal32 /*aXLimit*/, TReal32 /*aXOffset*/, 
                                         TReal32 /*aYLimit*/, TReal32 /*aYOffset*/,
                                         TReal32 /*aZLimit*/, TReal32 /*aZOffset*/) __SOFTFP
    {
    }


EXPORT_C void CHuiProceduralMesh::ScaleUniformly(TReal32 /*aX*/, TReal32 /*aY*/, TReal32 /*aZ*/) __SOFTFP
    {
    }


EXPORT_C void CHuiProceduralMesh::UpdateSurface(TReal32 /*aYaw*/, TReal32 /*aPitch*/) __SOFTFP
    {
    }


EXPORT_C void CHuiProceduralMesh::Draw(CHuiGc& /*aGc*/, const THuiImage* /*aImage*/, 
                    const THuiImage* /*aSecondaryImage*/,
                    TReal32 /*aSecondaryAlpha*/) const __SOFTFP
    {
    }

EXPORT_C void CHuiProceduralMesh::ProceduralMeshExtension(const TUid& /*aExtensionUid*/, TAny** aExtensionParams)
    {
    // If no extension with given UID was found, indicate it by returning null
    *aExtensionParams = NULL;
    }

