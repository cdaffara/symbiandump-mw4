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
* Description:   M3G 3D Mesh
*
*/



#include "alf/alfm3gmesh.h"
#include "alf/alfenv.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alflogger.h"

struct CAlfM3GMesh::TM3GMeshPrivateData
    {
    CAlfEnv* iEnv; // Not owned
    CAlfGenComponent* iComms; // Not owned
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Exposed constructor
// ---------------------------------------------------------------------------
//
CAlfM3GMesh* CAlfM3GMesh::NewL( CAlfGenComponent& aComms, CAlfEnv& aEnv )
    {
    CAlfM3GMesh* self = new( ELeave ) CAlfM3GMesh;
    CleanupStack::PushL( self );
    self->ConstructL( aComms, aEnv );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// First phase constructor
// ---------------------------------------------------------------------------
//
CAlfM3GMesh::CAlfM3GMesh()
    {
    }

// ---------------------------------------------------------------------------
// Second phase constructor
// ---------------------------------------------------------------------------
//
void CAlfM3GMesh::ConstructL( CAlfGenComponent& aComms, CAlfEnv& aEnv )
    {
    // Construct the base class
    CAlfMesh::ConstructL(aComms, EAlfMeshTypeM3G);
    
    // Create object data.
    iData = new (ELeave) TM3GMeshPrivateData;
    
    // Zero all object data
    iData->iEnv = NULL;
    iData->iComms = NULL;
    
    // Fill data
    iData->iEnv = &aEnv;
    iData->iComms = &aComms;
    
    // Create input and output buffers for inter-process communication
    TBuf8<1> outDummy;
    TInt param = EAlfMeshTypeM3G;
    TPckgC<TInt> buf(param);
        
    // Call a synchronous command to create an associated server-side M3G mesh.
    TInt err = iData->iComms->DoSynchronousCmd(EAlfMeshVisualCreateMesh, buf, outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfM3GMesh::ConstructL leave error %d", err )
        User::Leave( err );
        }
    }

// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfM3GMesh::~CAlfM3GMesh()
    {
    delete iData;
    iData = NULL;
    }


// ---------------------------------------------------------------------------
// Loads a new M3G Mesh scene into this M3G mesh
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfM3GMesh::LoadSceneL(const TDesC& aFileName)
    {
    ASSERT(iData != NULL);
    
    TFileName filename = aFileName;
    
    // Prepend the image file path to the filename.
    iData->iEnv->TextureManager().PrependImagePath(filename);
    
    // Create input and output buffers for inter-process communication
    TBuf8<1> outDummy;
    TAlfMeshLoadM3GSceneParams params;
    params.iFilename = filename;
    
    TPckg<TAlfMeshLoadM3GSceneParams> paramsPckg(params);
    
    // Call a synchronous command to load the M3G scene graph
    TInt err = iData->iComms->DoSynchronousCmd(EAlfMeshLoadM3GScene, paramsPckg, outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfM3GMesh::LoadSceneL leave error %d", err )
        User::Leave( err );
        }    
    }
