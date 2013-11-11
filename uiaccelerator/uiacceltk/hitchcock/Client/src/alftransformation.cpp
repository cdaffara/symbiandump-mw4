/*
* Copyright (c) 2006 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Transformation class
*
*/



#include "alf/alftransformation.h"
#include "alftransformationsubsession.h"
#include "alf/alfenv.h"
#include "alfclient.h"

// Provate data
struct CAlfTransformation::TPrivateData
    {
    CAlfEnv* iEnv;              // Not owned.
    RAlfTransformationSubSession iSubSession;     // Owned
    };

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfTransformation::CAlfTransformation()
    {
    }


// ---------------------------------------------------------------------------
// ConstructL
// ---------------------------------------------------------------------------
//
void CAlfTransformation::ConstructL(CAlfEnv& aEnv)
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iEnv = &aEnv;
    User::LeaveIfError( iData->iSubSession.Open( aEnv.Client() ) );
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTransformation* CAlfTransformation::NewL(CAlfEnv& aEnv)
    {
    CAlfTransformation* self = CAlfTransformation::NewLC(aEnv);
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTransformation* CAlfTransformation::NewLC(CAlfEnv& aEnv)
    {
    CAlfTransformation* self = new( ELeave ) CAlfTransformation;
    CleanupStack::PushL( self );
    self->ConstructL(aEnv);
    return self;
    }


// ---------------------------------------------------------------------------
// Destrucotr
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTransformation::~CAlfTransformation()
    {
    if ( iData )
        {
        iData->iSubSession.Close();
        }
    delete iData;
    iData = NULL;
    }

// ---------------------------------------------------------------------------
// Return server handle
// ---------------------------------------------------------------------------
//  
TInt CAlfTransformation::ServerHandle() const
    {
    return iData->iSubSession.SubSessionHandle();
    }

// ---------------------------------------------------------------------------
// Load identity
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::LoadIdentity()
    {
    iData->iSubSession.LoadIdentity();
    }
 

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTransformation::Count() const
    {
    return iData->iSubSession.Count();                            
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C TInt CAlfTransformation::NonIdentityCount() const
    {
    return iData->iSubSession.NonIdentityCount();                    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C CAlfTransformation::TTransform CAlfTransformation::Step(TInt aIndex)
    {
    return iData->iSubSession.Step(aIndex);            
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::ReplaceStep(TInt aIndex, 
    CAlfTransformation::TTransform aTransformationStep)
    {
    iData->iSubSession.ReplaceStep(aIndex,aTransformationStep);    
    }

    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::Rotate(TReal32 aAngle) __SOFTFP
    {
    TAlfTimedValue timedAngle;
    timedAngle.SetTarget(aAngle,0);
    Rotate(timedAngle);        
    }

// ---------------------------------------------------------------------------
// Rotate
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::Rotate(const TAlfTimedValue& aAngle)
    {
    Rotate(aAngle, 0.f, 0.f, -1.f);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::Rotate(TReal32 aAngle, TReal32 aAxisX, 
    TReal32 aAxisY, TReal32 aAxisZ) __SOFTFP
    {
    TAlfTimedValue timedAngle;
    timedAngle.SetTarget(aAngle,0);
    Rotate(timedAngle, aAxisX, aAxisY, aAxisZ);        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::Rotate(const TAlfTimedValue& aAngle, 
                         TReal32 aAxisX, TReal32 aAxisY, TReal32 aAxisZ) __SOFTFP
    {
    iData->iSubSession.Rotate(aAngle,aAxisX,aAxisY,aAxisZ);            
    }

    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::Translate(const TAlfTimedValue& aX, 
    const TAlfTimedValue& aY)
    {
    iData->iSubSession.Translate(aX, aY);      
    }

// ---------------------------------------------------------------------------
// Tramslate
// ---------------------------------------------------------------------------
//   
EXPORT_C void CAlfTransformation::Translate(TReal32 aX, TReal32 aY) __SOFTFP
    {
    iData->iSubSession.Translate( aX, aY );
    }
    

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::Scale(TReal32 aX, TReal32 aY) __SOFTFP
    {
    Scale(aX, aY, 1.f);
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::Scale(const TAlfTimedValue& aX, 
    const TAlfTimedValue& aY)
    {
    TAlfTimedValue timedZ;
    timedZ.SetTarget(1.f,0);
    Scale(aX, aY, timedZ);    
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::Scale(TReal32 aX, TReal32 aY, TReal32 aZ) __SOFTFP
    {
    TAlfTimedValue timedX;
    TAlfTimedValue timedY;
    TAlfTimedValue timedZ;
    timedX.SetTarget(aX,0);
    timedY.SetTarget(aY,0);
    timedZ.SetTarget(aZ,0);
    Scale(timedX, timedY, timedZ);                
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
//
EXPORT_C void CAlfTransformation::Scale(const TAlfTimedValue& aX, 
    const TAlfTimedValue& aY, const TAlfTimedValue& aZ)
    {
    iData->iSubSession.Scale(aX,aY,aZ);
    }
    
    

