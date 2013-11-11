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
* Description:   Implementation of CHuiMeshVisual. CHuiMeshVisual is a visual
*                that is able to display a 3D triangle mesh.
*
*/



#include "uiacceltk/HuiMeshVisual.h"
#include "HuiRenderPlugin.h"
#include "uiacceltk/HuiMesh.h" // only OpenGL ES supported
#include "uiacceltk/huim3gmesh.h"
#include "uiacceltk/huiproceduralmesh.h"
#include "uiacceltk/HuiGc.h"
#include "uiacceltk/HuiPanic.h"
#include "uiacceltk/HuiUtil.h"
#include "uiacceltk/HuiStatic.h"
#include "uiacceltk/HuiEnv.h"
#include "uiacceltk/HuiTexture.h"
#include "uiacceltk/HuiControl.h"
#include "uiacceltk/HuiVisual.h"

EXPORT_C CHuiMeshVisual* CHuiMeshVisual::AddNewL(CHuiControl& aOwnerControl,
												 THuiMeshType aMeshType,
                                                 CHuiLayout* aParentLayout)
    {
    CHuiMeshVisual* mesh = STATIC_CAST(CHuiMeshVisual*,
        aOwnerControl.AppendVisualL(EHuiVisualTypeMesh, aParentLayout));
        
	mesh->CreateMeshL(aMeshType);
                
    return mesh;
    }


CHuiMeshVisual::CHuiMeshVisual(MHuiVisualOwner& aOwner)
        : CHuiVisual(aOwner)
    {
    }


void CHuiMeshVisual::ConstructL()
    {
    CHuiVisual::ConstructL();
    
    CreateMeshL(EHuiMeshTypeProcedural);
    
    iSecondaryAlpha.SetStyle(EHuiTimedValueStyleLinear);

    iScale.Set(1.f);

    // Meshes use 3D projection by default.
    SetFlag(EHuiVisualFlag3DProjection);
    }


CHuiMeshVisual::~CHuiMeshVisual()
    {
    }


EXPORT_C void CHuiMeshVisual::CreateMeshL(THuiMeshType aMeshType)
	{
    // Create a new mesh.
    iMesh.Set(CHuiMesh::NewL(aMeshType), EHuiHasOwnership);
	}


EXPORT_C void CHuiMeshVisual::SetImage(const THuiImage& aImage)
    {
    iImage = aImage;
    }


EXPORT_C void CHuiMeshVisual::SetSecondaryImage(const THuiImage& aImage)
    {
    iSecondaryImage = aImage;
    SetChanged();
    }


EXPORT_C void CHuiMeshVisual::SetSpecularImage(const THuiImage& aImage)
    {
    if(iMesh->MeshType() == EHuiMeshTypeProcedural) 
        {
        CHuiProceduralMesh *mesh = static_cast<CHuiProceduralMesh*>(&iMesh.NonConstRef());
        mesh->iMaterial.iSpecularImage = aImage;
        SetChanged();
        }
    }


void CHuiMeshVisual::SetPos( const THuiRealPoint& aPos, TInt aTransitionTime )
	{
	SetChanged(); // \todo optimize: do not set if the value is the same(?)
	CHuiVisual::SetPos( aPos, aTransitionTime);
	if ( iMesh->MeshType() == EHuiMeshTypeM3G )
		{
		// \todo Handle animation?
		THuiRealPoint realSize = LocalPointInPixels(THuiRealPoint(Size().Target().iX,Size().Target().iY),EHuiReferenceStateTarget);
		THuiRealRect rect( LocalPointInPixels(Pos().Target(),EHuiReferenceStateTarget), 
							THuiRealSize(realSize.iX, realSize.iY) );
		M3GMesh()->SetViewportRect( rect );
		}
	}


void CHuiMeshVisual::SetSize( const THuiRealSize& aSize, TInt aTransitionTime )
	{
	SetChanged(); // \todo optimize: do not set if the value is the same(?)
	CHuiVisual::SetSize( aSize, aTransitionTime);
	if ( iMesh->MeshType() == EHuiMeshTypeM3G )
		{
		// \todo Handle animation
		THuiRealPoint realSize = LocalPointInPixels(THuiRealPoint(Size().Target().iX,Size().Target().iY),EHuiReferenceStateTarget);
		THuiRealRect rect( LocalPointInPixels(Pos().Target(),EHuiReferenceStateTarget), 
							THuiRealSize(realSize.iX, realSize.iY) );
		M3GMesh()->SetViewportRect( rect );
		}
	}


void CHuiMeshVisual::SetRect( const THuiRealRect& aRect, TInt aTransitionTime )
	{
	SetChanged(); // \todo optimize: do not set if the value is the same(?)
	CHuiVisual::SetRect( aRect, aTransitionTime );
	if ( iMesh->MeshType() == EHuiMeshTypeM3G )
		{
		// \todo Handle animation
		THuiRealPoint realSize = LocalPointInPixels(THuiRealPoint(Size().Target().iX,Size().Target().iY),EHuiReferenceStateTarget);
		THuiRealRect rect( LocalPointInPixels(Pos().Target(),EHuiReferenceStateTarget), 
							THuiRealSize(realSize.iX, realSize.iY) );
		M3GMesh()->SetViewportRect( rect );
		}
	}


void CHuiMeshVisual::DrawSelf(CHuiGc& aGc, const TRect& aDisplayRect) const
    {
    
    THuiRealRect content = DisplayRect();
    content.Shrink(PaddingInPixels(EHuiReferenceStateNow));
    TReal32 opacity = EffectiveOpacity();

    /*
    aGc.SetProjection(CHuiGc::EProjectionPerspective);
    if(Flags() & EHuiVisualFlagLocalHorizon)
        {
        aGc.SetFrustumOffset(content.Center());
        }
    */

    aGc.SetPenColor(TRgb(255, 255, 255));
    aGc.SetPenAlpha(TInt(255 * opacity));

    aGc.Push(EHuiGcMatrixModel);

    TReal32 scaling = 1.0f;

    TReal32 yaw = iYawAngle.Now();
    TReal32 pitch = iPitchAngle.Now();

	CHuiProceduralMesh *proceduralMesh = ProceduralMesh();
	CHuiM3GMesh *m3gMesh = M3GMesh();
	if(proceduralMesh)
		{
		//translate
	    THuiRealPoint mid = content.Center();
	    aGc.Translate(EHuiGcMatrixModel, mid.iX, mid.iY, 0.f);
	    
	    // Scale the model based on the size (width) of the visual.
	    /** @todo  Where does this factor really come from? () */
    	scaling = .000113f * .2f * Min(content.Width(), content.Height()) * iScale.Now();
    	aGc.Scale(EHuiGcMatrixModel, scaling, scaling, scaling);
    	
    	// Rotate
		aGc.Rotate(EHuiGcMatrixModel, pitch, 1.f, 0.f, 0.f);		
	    aGc.Rotate(EHuiGcMatrixModel, yaw, 0.f, 1.f, 0.f);  
		}
	if(m3gMesh)
		{
    	m3gMesh->SetViewportRect(aDisplayRect);
    	
		// translate
	    THuiRealPoint tl = content.iTl;
	    TRect dispArea = aGc.DisplayArea();
	    // don't divide by 0
	    if ( dispArea.Width() != 0 && dispArea.Height() != 0 )
	        {
	        aGc.Translate(EHuiGcMatrixModel, 2.f * (tl.iX / dispArea.Width()), -2.f * (tl.iY / dispArea.Height()), 0.f);
	        }
	    
	    // scale
	    scaling = iScale.Now();
	    aGc.Scale(EHuiGcMatrixModel, scaling, scaling, scaling);
	    
	    // animate	    
	    m3gMesh->AnimateControllers();
	    
    	m3gMesh->StoreCamera();
    	
    	// rotate    
    	m3gMesh->RotateYaw(yaw);
    	m3gMesh->RotatePitch(pitch);
    	
		}
				
    // Generate texture coordinates. Use NonConstRef() to get a modifiable
    // reference to the mesh instance even though we are currently in
    // a const method.
    
    if(proceduralMesh) 
    	{
    	proceduralMesh->UpdateSurface(yaw, pitch);


        if(proceduralMesh->iMaterial.iPreset == EHuiMaterialShadow)
            {
            // Set up a shadow projection.
            /** @todo  Should determine max Z of model and project there. */
            TReal32 mat[16];
            TReal32 point[3] =
                {
                0, 0, -18000
                };
            TReal32 normal[3] =
                {
                0, 0, 1
                };
            TReal32 light[4] =
                {
                -750, -750, 1000, 0
                };
            HuiUtil::ShadowMatrix(point, normal, light, mat);
            aGc.Multiply(EHuiGcMatrixModel, mat);
            }

    	}
	
    // Depth testing is needed when drawing in 3D.
    /** @todo  Push attrib needed for depth test. */
    aGc.Enable(CHuiGc::EFeatureDepthTest);
    aGc.DrawMesh(iMesh.Ref(), &iImage, &iSecondaryImage, iSecondaryAlpha.Now());
    aGc.Disable(CHuiGc::EFeatureDepthTest);

	if (m3gMesh) 
		{
		m3gMesh->RestoreCamera();
		}

    // Restore original transformation.
    aGc.Pop(EHuiGcMatrixModel);
    }

TBool CHuiMeshVisual::Changed() const
    {
    if(CHuiVisual::Changed())
        {
        return ETrue;
        }
    return iYawAngle.Changed() || iPitchAngle.Changed() || iScale.Changed() || iImage.Changed() || iSecondaryImage.Changed();
    }


void CHuiMeshVisual::ClearChanged()
    {
    CHuiVisual::ClearChanged();
    iYawAngle.ClearChanged();
    iPitchAngle.ClearChanged();
    iImage.ClearChanged();
    iSecondaryImage.ClearChanged();
    }


EXPORT_C CHuiMesh& CHuiMeshVisual::Mesh()
    {
    SetChanged(); // \todo Why this is always set?
    return iMesh.Ref();
    }

// ---------------------------------------------------------------------------
// Return procedural mesh
// ---------------------------------------------------------------------------
//
EXPORT_C CHuiProceduralMesh* CHuiMeshVisual::ProceduralMesh() const
	{
	if(iMesh->MeshType() != EHuiMeshTypeProcedural) 
		{
		return NULL;
		}    
	return (CHuiProceduralMesh*)(&iMesh.Ref());
	}

// ---------------------------------------------------------------------------
// Return M3G mesh
// ---------------------------------------------------------------------------
//
EXPORT_C CHuiM3GMesh* CHuiMeshVisual::M3GMesh() const 
	{
	if(iMesh->MeshType() != EHuiMeshTypeM3G) 
		{
		return NULL;
		}    
	return (CHuiM3GMesh*)(&iMesh.Ref());
	}

