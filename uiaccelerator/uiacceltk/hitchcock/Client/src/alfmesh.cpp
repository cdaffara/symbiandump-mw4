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
* Description:   Mesh base class
*
*/



#include "alf/alfmesh.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alflogger.h"
#include "alf/alftexture.h"

#include <uiacceltk/HuiUtil.h>

struct CAlfMesh::TMeshPrivateData
    {
    CAlfGenComponent* iComms; // not own
    TInt iMeshType;
    };

// ======== MEMBER FUNCTIONS ========


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlfMesh::CAlfMesh()
    {
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
void CAlfMesh::ConstructL( CAlfGenComponent& aComms, TInt aMeshType )
    {
    iData = new (ELeave) TMeshPrivateData;
    iData->iComms = &aComms;
    
    iData->iMeshType = aMeshType;
    }


// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlfMesh* CAlfMesh::NewL( CAlfGenComponent& aComms )
    {
    CAlfMesh* self = new( ELeave ) CAlfMesh;
    CleanupStack::PushL( self );
    self->ConstructL( aComms );
    CleanupStack::Pop( self );
    return self;
    }

// ---------------------------------------------------------------------------
// ?description_if_needed
// ---------------------------------------------------------------------------
//
CAlfMesh::~CAlfMesh()
    {
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// Returns the mesh type definition of this mesh.
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfMesh::MeshType() const
    {
    return iData->iMeshType;
    }

// ---------------------------------------------------------------------------
// ?implementation_description
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfMesh::Reset()
    {
    TBuf8<1> outDummy;
    TInt err = iData->iComms->DoSynchronousCmd( EAlfMeshReset, KNullDesC8(), outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMesh::Reset panic error %d", err )
        USER_INVARIANT();
        }
    }
   
EXPORT_C void CAlfMesh::StretchUniformly(
        TReal32 aXLimit, 
        TReal32 aXOffset, 
        TReal32 aYLimit, 
        TReal32 aYOffset,
        TReal32 aZLimit, 
        TReal32 aZOffset) __SOFTFP
    {
    TRAP_IGNORE(StretchUniformlyL(aXLimit, aXOffset,aYLimit, aYOffset, aZLimit, aZOffset))
    }

void CAlfMesh::StretchUniformlyL(
        TReal32 aXLimit, 
        TReal32 aXOffset, 
        TReal32 aYLimit, 
        TReal32 aYOffset,
        TReal32 aZLimit, 
        TReal32 aZOffset)
    {
    RArray<TReal32> array;
    CleanupClosePushL( array );
    
    array.AppendL( aXLimit );
    array.AppendL( aXOffset );
    array.AppendL( aYLimit );
    array.AppendL( aYOffset );
    array.AppendL( aZLimit );
    array.AppendL( aZOffset );
    
    HBufC8* buffer = ExternalizeLC(array);
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd( EAlfMeshStretchUniformly, *buffer, outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMesh::StretchUniformly panic error %d", err )
        USER_INVARIANT();
        }
    
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &array );
    }
                                
EXPORT_C void CAlfMesh::ScaleUniformly(TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP
    {
    TRAP_IGNORE(ScaleUniformlyL(aX, aY, aZ))
    }

void CAlfMesh::ScaleUniformlyL(TReal32 aX, TReal32 aY, TReal32 aZ)
    {
    RArray<TReal32> array;
    CleanupClosePushL( array );
    
    array.AppendL( aX );
    array.AppendL( aY );
    array.AppendL( aZ );
    
    HBufC8* buffer = ExternalizeLC(array);
    TBuf8<1> outDummy;
    
    TInt err = iData->iComms->DoSynchronousCmd( EAlfMeshScaleUniformly, *buffer, outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMesh::ScaleUniformly panic error %d", err )
        USER_INVARIANT();
        }
    
    CleanupStack::PopAndDestroy( buffer );
    CleanupStack::PopAndDestroy( &array );
    }


EXPORT_C TInt CAlfMesh::AnimationControllerCount() const
    {
    return DoAnimationControllerCount();
    }

TInt CAlfMesh::DoAnimationControllerCount() const
    {
    TInt result = 0;
    TPckg<TBool> outBuf( result );
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfMeshAnimationControllerCount, 
        KNullDesC8(), 
        outBuf );
        
    if (err)
        {
        __ALFLOGSTRING1( "CAlfMesh::AnimationControllerCount panic error %d", err )
        USER_INVARIANT();
        }
        
    return result;
    }

EXPORT_C void CAlfMesh::SetAnimationPosition(TInt aControllerId, TReal32 aTarget, TInt aTime) __SOFTFP
    {
    TRAP_IGNORE(SetAnimationPositionL(aControllerId, aTarget, aTime))
    }

void CAlfMesh::SetAnimationPositionL(TInt aControllerId, TReal32 aTarget, TInt aTime)
    {
    ASSERT(iData != NULL);
    
    // Create input and output buffers for inter-process communication
    TAlfMeshAnimationPositionParams params;
    params.iControllerId = aControllerId;    
	params.iTarget = aTarget;
	params.iTime = aTime;
    
    TPckg<TAlfMeshAnimationPositionParams > paramsPckg(params);
    
    // Call a synchronous command
    TInt err = iData->iComms->DoCmdNoReply(EAlfMeshSetAnimationPosition, paramsPckg );
    if (err)
        {
        __ALFLOGSTRING1( "CAlfM3GMesh::SetAnimationPosition leave error %d", err )
        User::Leave( err );
        }    
    }
    
EXPORT_C void CAlfMesh::StartAnimationController(TInt aControllerId)
    {
    TRAP_IGNORE(StartAnimationControllerL(aControllerId))
    }

void CAlfMesh::StartAnimationControllerL(TInt aControllerId)
    {
    ASSERT(iData != NULL);
        
    TBuf8<1> outDummy;
    TInt params = aControllerId;
    
    TPckg<TInt> paramsPckg(params);
    
    // Call a synchronous command
    TInt err = iData->iComms->DoSynchronousCmd(EAlfMeshStartAnimationController, paramsPckg, outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfM3GMesh::StartAnimationController leave error %d", err )
        User::Leave( err );
        }    
    }    
    
EXPORT_C void CAlfMesh::StopAnimationController(TInt aControllerId)
    {
    TRAP_IGNORE(StopAnimationControllerL(aControllerId))
    }

void CAlfMesh::StopAnimationControllerL(TInt aControllerId)
    {
    ASSERT(iData != NULL);
        
    TBuf8<1> outDummy;
    TInt params = aControllerId;
    
    TPckg<TInt> paramsPckg(params);
    
    // Call a synchronous command
    TInt err = iData->iComms->DoSynchronousCmd(EAlfMeshStopAnimationController, paramsPckg, outDummy);
    if (err)
        {
        __ALFLOGSTRING1( "CAlfM3GMesh::StopAnimationController leave error %d", err )
        User::Leave( err );
        }    
    }

