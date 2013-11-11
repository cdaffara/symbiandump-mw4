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
* Description:   Mesh visual
*
*/



#include "alf/alfmeshvisual.h"
#include "alf/alfproceduralmesh.h"
#include "alf/alfm3gmesh.h"
#include "alf/alfcontrol.h"
#include "alflogger.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alf/alfimage.h"
#include "alf/alftexture.h"
#include "alfskinnableimage.h"
#include "alf/alfenv.h"

#include <uiacceltk/HuiUtil.h>

// ======== MEMBER FUNCTIONS ========

struct CAlfMeshVisual::TMeshVisualPrivateData
    {
    CAlfMesh* iMesh; // own
    CAlfSkinnableImage* iImage;
    CAlfSkinnableImage* iSecondaryImage;    
    CAlfSkinnableImage* iSpecularImage;
    };

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//

EXPORT_C CAlfMeshVisual* CAlfMeshVisual::AddNewL(
    CAlfControl& aOwnerControl,
    CAlfLayout* aParentLayout )
    {
    CAlfMeshVisual* text = STATIC_CAST(CAlfMeshVisual*,
        aOwnerControl.AppendVisualL(EAlfVisualTypeMesh, aParentLayout));
    return text;
    }

EXPORT_C CAlfMeshVisual::CAlfMeshVisual()
    {
    }

EXPORT_C void CAlfMeshVisual::ConstructL(CAlfControl& aOwner)
    {
    CAlfVisual::ConstructL(aOwner);
    
    iMeshVisualData = new (ELeave) TMeshVisualPrivateData;
    iMeshVisualData->iMesh = NULL;
    iMeshVisualData->iImage = NULL;
    iMeshVisualData->iSecondaryImage = NULL;
    iMeshVisualData->iSpecularImage = NULL;
    
    iMeshVisualData->iImage = new (ELeave) CAlfSkinnableImage(&aOwner.Env());
    iMeshVisualData->iSecondaryImage = new (ELeave) CAlfSkinnableImage(&aOwner.Env());
    iMeshVisualData->iSpecularImage = new (ELeave) CAlfSkinnableImage(&aOwner.Env());
    }

EXPORT_C CAlfMeshVisual::~CAlfMeshVisual()
    {
    if ( iMeshVisualData )
        {
        delete iMeshVisualData->iMesh;
        iMeshVisualData->iMesh = NULL;
        
        delete iMeshVisualData->iImage;
        iMeshVisualData->iImage = NULL;

        delete iMeshVisualData->iSecondaryImage;
        iMeshVisualData->iSecondaryImage = NULL;

        delete iMeshVisualData->iSpecularImage;
        iMeshVisualData->iSpecularImage = NULL;
        }
        
    delete iMeshVisualData;
    iMeshVisualData = NULL;
    }
    
EXPORT_C void CAlfMeshVisual::CreateMeshL(TInt aMeshType)
    {
    // Destroy the old mesh object.
    if( iMeshVisualData->iMesh )
        {
        delete iMeshVisualData->iMesh;
        iMeshVisualData->iMesh = NULL;
        }
    
    // Create a new mesh object.
    if ( aMeshType == EAlfMeshTypeProcedural )
        {
        iMeshVisualData->iMesh = CAlfProceduralMesh::NewL( *Comms() );
        }
    if ( aMeshType == EAlfMeshTypeM3G )
        {
        iMeshVisualData->iMesh = CAlfM3GMesh::NewL( *Comms(), Env() );
        }        
    }
    
EXPORT_C void CAlfMeshVisual::SetImage(const TAlfImage& aImage)
    {    
    iMeshVisualData->iImage->SetImage(aImage);// This texturizes skin graphics if needed
    
    TAlfImageParams params(iMeshVisualData->iImage->Image());

    TPckgC<TAlfImageParams> buf(params);
    TInt err = Comms()->DoCmdNoReply(EAlfMeshVisualSetImage, buf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfMeshVisual::SetImage ignore error %d", err )
        }
    }
    
EXPORT_C const TAlfImage& CAlfMeshVisual::Image() const
    {
    return iMeshVisualData->iImage->Image();
    }

EXPORT_C void CAlfMeshVisual::SetSecondaryImage(const TAlfImage& aImage)
    {
    iMeshVisualData->iSecondaryImage->SetImage(aImage);// This texturizes skin graphics if needed
    
    TAlfImageParams params(iMeshVisualData->iSecondaryImage->Image());

    TPckgC<TAlfImageParams> buf(params);
    TInt err = Comms()->DoCmdNoReply(EAlfMeshVisualSetSecondaryImage, buf );
    
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfMeshVisual::SetSecondaryImage ignore error %d", err )
        }
    }
    
EXPORT_C const TAlfImage& CAlfMeshVisual::SecondaryImage() const
    {
    return iMeshVisualData->iSecondaryImage->Image();
    }

EXPORT_C void CAlfMeshVisual::SetSpecularImage(const TAlfImage& aImage)
    {
    // This functionality applies to procedural mesh only.
    if(Mesh().MeshType() == EAlfMeshTypeProcedural)
        {    
        CAlfProceduralMesh& mesh = static_cast<CAlfProceduralMesh&>(Mesh());
        iMeshVisualData->iSpecularImage->SetImage(aImage);// This texturizes skin graphics if needed
        
        TAlfImageParams params(iMeshVisualData->iSpecularImage->Image());

        TPckgC<TAlfImageParams> buf(params);
        TInt err = Comms()->DoCmdNoReply(EAlfMeshVisualSetSpecularImage, buf );
        
        if ( err )
            {
            __ALFLOGSTRING1( "CAlfMeshVisual::SetSpecularImage ignore error %d", err )
            }
            
        mesh.SetSpecularImage( iMeshVisualData->iSpecularImage->Image() );        
        }
    }
    
EXPORT_C const TAlfImage& CAlfMeshVisual::SpecularImage() const
    {
    return iMeshVisualData->iSpecularImage->Image();
    }

EXPORT_C CAlfMesh& CAlfMeshVisual::Mesh()
    {
    // Create a default mesh
    if ( !iMeshVisualData->iMesh )
        {
        TRAPD( err, CreateMeshL(EAlfMeshTypeProcedural) );
        if ( err )
            {
            __ALFLOGSTRING1( "CAlfMeshVisual::Mesh panic error %d", err )
            USER_INVARIANT();
            }
        }
    return *iMeshVisualData->iMesh;
    }
    
EXPORT_C CAlfProceduralMesh* CAlfMeshVisual::ProceduralMesh() const
    {
    if(iMeshVisualData->iMesh && (iMeshVisualData->iMesh->MeshType() == EAlfMeshTypeProcedural))
        {
        return static_cast<CAlfProceduralMesh*>(iMeshVisualData->iMesh);
        }    
    return NULL;
    }

EXPORT_C CAlfM3GMesh* CAlfMeshVisual::M3GMesh() const
    {
    if(iMeshVisualData->iMesh && (iMeshVisualData->iMesh->MeshType() == EAlfMeshTypeM3G))
        {
        return static_cast<CAlfM3GMesh*>(iMeshVisualData->iMesh);
        }    
    return NULL;    
    }
    
EXPORT_C TAlfTimedValue CAlfMeshVisual::SecondaryAlpha() const
    {
    TAlfTimedValue result;
    TPckg<TAlfTimedValue> buf(result);
    TInt err = Comms()->DoSynchronousCmd( EAlfMeshVisualSecondaryAlpha, KNullDesC8, buf);
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMeshVisual::SecondaryAlpha panic error %d", err )
        USER_INVARIANT();
        }
    
    return result;  
    }
    
EXPORT_C void CAlfMeshVisual::SetSecondaryAlpha( const TAlfTimedValue& aSecondaryAlpha )
    {
    TPckgC<TAlfTimedValue> buf(aSecondaryAlpha);

    TInt err = Comms()->DoCmdNoReply( EAlfMeshVisualSetSecondaryAlpha, buf );
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMeshVisual::SetSecondaryAlpha panic error %d", err )
        USER_INVARIANT();
        }
    }
    
    
EXPORT_C TAlfTimedValue CAlfMeshVisual::YawAngle() const
    {
    TAlfTimedValue result;
    TPckg<TAlfTimedValue> buf(result);
    TInt err = Comms()->DoSynchronousCmd( EAlfMeshVisualYawAngle, KNullDesC8, buf);
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMeshVisual::YawAngle panic error %d", err )
        USER_INVARIANT();
        }
    
    return result;  
    }
    
EXPORT_C void CAlfMeshVisual::SetYawAngle( const TAlfTimedValue& aYawAngle )
    {
    TPckgC<TAlfTimedValue> buf(aYawAngle);
    
    TInt err = Comms()->DoCmdNoReply( EAlfMeshVisualSetYawAngle, buf );
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMeshVisual::SetYawAngle panic error %d", err )
        USER_INVARIANT();
        }
    }
    
    
EXPORT_C TAlfTimedValue CAlfMeshVisual::PitchAngle() const
    {
    TAlfTimedValue result;
    TPckg<TAlfTimedValue> buf(result);
    TInt err = Comms()->DoSynchronousCmd( EAlfMeshVisualPitchAngle, KNullDesC8, buf);
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMeshVisual::PitchAngle panic error %d", err )
        USER_INVARIANT();
        }
    
    return result; 
    }
    
EXPORT_C void CAlfMeshVisual::SetPitchAngle( const TAlfTimedValue& aPitchAngle )
    {
    TPckgC<TAlfTimedValue> buf(aPitchAngle);

    TInt err = Comms()->DoCmdNoReply( EAlfMeshVisualSetPitchAngle, buf );
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMeshVisual::SetPitchAngle panic error %d", err )
        USER_INVARIANT();
        }
    }
    
    
EXPORT_C TAlfTimedValue CAlfMeshVisual::Scale() const
    {
    TAlfTimedValue result;
    TPckg<TAlfTimedValue> buf(result);
    TInt err = Comms()->DoSynchronousCmd( EAlfMeshVisualScale, KNullDesC8, buf);
    
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMeshVisual::Scale panic error %d", err )
        USER_INVARIANT();
        }
    
    return result; 
    }
    
EXPORT_C void CAlfMeshVisual::SetScale( const TAlfTimedValue& aScale )
    {
    TPckgC<TAlfTimedValue> buf(aScale);

    TInt err = Comms()->DoCmdNoReply( EAlfMeshVisualSetScale, buf );
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMeshVisual::SetScale panic error %d", err )
        USER_INVARIANT();
        }
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//     
EXPORT_C void CAlfMeshVisual::RemoveAndDestroyAllD()
    {
    CAlfVisual::RemoveAndDestroyAllD();
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfMeshVisual::UpdateChildrenLayout(TInt aTransitionTime )
    {
    CAlfVisual::UpdateChildrenLayout( aTransitionTime );
    }
  
// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C CAlfVisual* CAlfMeshVisual::FindTag(const TDesC8& aTag)
    {
    return CAlfVisual::FindTag( aTag );
    }

// ---------------------------------------------------------------------------
// Place holder from CAlfVisual
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfMeshVisual::DoRemoveAndDestroyAllD()
    {
    CAlfVisual::DoRemoveAndDestroyAllD();
    }
    
// ---------------------------------------------------------------------------
//  future proofing  
// ---------------------------------------------------------------------------
//  
EXPORT_C void CAlfMeshVisual::PropertyOwnerExtension(const TUid& aExtensionUid, TAny** aExtensionParams)
    {
    CAlfVisual::PropertyOwnerExtension(aExtensionUid,aExtensionParams);
    }
    

    
