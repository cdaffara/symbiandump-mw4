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



#include "uiacceltk/HuiMesh.h"
#include "uiacceltk/HuiStatic.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiProbe.h"
#include "uiacceltk/HuiUtil.h"


EXPORT_C CHuiMesh* CHuiMesh::NewL(THuiMeshType aMeshType)
    {
    CHuiMesh* self = CHuiMesh::NewLC(aMeshType);
    CleanupStack::Pop(self);
    return self;
    }
    
EXPORT_C CHuiMesh* CHuiMesh::NewLC(THuiMeshType aMeshType)
    {
    CHuiMesh* self = CHuiStatic::Renderer().CreateMeshL(aMeshType);
    CleanupStack::PushL(self);
    return self;
    }


CHuiMesh::CHuiMesh(THuiMeshType aMeshType)
    : iMeshType(aMeshType)
    {
    HUI_PROBE_ASSOCIATE_WITH_CURRENT_SESSION
    HUI_PROBE_REPORT_CONSTRUCTED
    }

EXPORT_C CHuiMesh::~CHuiMesh()
    {
    HUI_PROBE_REPORT_DESTRUCTED
    }
    
    
EXPORT_C THuiMeshType CHuiMesh::MeshType() const
	{
	return iMeshType;
	}


EXPORT_C void CHuiMesh::Reset()
    {
    }


EXPORT_C void CHuiMesh::StretchUniformly(TReal32 /*aXLimit*/, TReal32 /*aXOffset*/, 
                                         TReal32 /*aYLimit*/, TReal32 /*aYOffset*/,
                                         TReal32 /*aZLimit*/, TReal32 /*aZOffset*/) __SOFTFP
    {
    }


EXPORT_C void CHuiMesh::ScaleUniformly(TReal32 /*aX*/, TReal32 /*aY*/, TReal32 /*aZ*/) __SOFTFP
    {
    }


void CHuiMesh::Draw(CHuiGc& /*aGc*/, const THuiImage* /*aImage*/, 
                    const THuiImage* /*aSecondaryImage*/,
                    TReal32 /*aSecondaryAlpha*/) const __SOFTFP
    {
    }

EXPORT_C TInt CHuiMesh::AnimationControllerCount() const
	{
	return 0;
	}
		
EXPORT_C void CHuiMesh::SetAnimationPosition(TInt /*aControllerId*/, TReal32 /*aTraget*/, TInt /*aTime*/) __SOFTFP
	{		
	}
	
// ---------------------------------------------------------------------------
// Start animation of the controller
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiMesh::StartAnimationController(TInt /*aControllerId*/)
	{	
	}

// ---------------------------------------------------------------------------
// Stop animation of the controller
// ---------------------------------------------------------------------------
//    
EXPORT_C void CHuiMesh::StopAnimationController(TInt /*aControllerId*/)
	{	
	}
	
	

HUI_SESSION_OBJECT_IMPL_EXPORT(CHuiMesh, ETypeMesh)


