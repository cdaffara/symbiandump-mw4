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
* Description:   Transformation sub session
*
*/



#include "alfsrvtransformationsubsession.h"
#include "alf/alfconstants.h"
#include "alf/alfappsrvsessionbase.h"
#include "alfappsrvsession.h"
#include "alf/alfserverutils.h"
#include <uiacceltk/HuiEnv.h>
#include <uiacceltk/HuiTransformation.h>
#include <uiacceltk/HuiUtil.h>
#include <alf/alftransformation.h>


// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Constructor
// ---------------------------------------------------------------------------
//
CAlfSrvTransformationSubSession::CAlfSrvTransformationSubSession( 
    CAlfAppSrvSessionBase& aSession )
    : CAlfSrvSubSessionBase( aSession )
    {
    }


// ---------------------------------------------------------------------------
// 2nd phasse constructor
// ---------------------------------------------------------------------------
//
void CAlfSrvTransformationSubSession::ConstructL()
    {
    CAlfSrvSubSessionBase::ConstructL();
    }


// ---------------------------------------------------------------------------
// NewL
// ---------------------------------------------------------------------------
//
CAlfSrvTransformationSubSession* CAlfSrvTransformationSubSession::NewL( 
    CAlfAppSrvSessionBase& aSession )
    {
    CAlfSrvTransformationSubSession* self = 
        CAlfSrvTransformationSubSession::NewLC( aSession );
    CleanupStack::Pop( self );
    return self;
    }


// ---------------------------------------------------------------------------
// NewLC
// ---------------------------------------------------------------------------
//
CAlfSrvTransformationSubSession* CAlfSrvTransformationSubSession::NewLC( 
    CAlfAppSrvSessionBase& aSession )
    {
    CAlfSrvTransformationSubSession* self = 
        new( ELeave ) CAlfSrvTransformationSubSession( aSession );
    CleanupClosePushL( *self );
    self->ConstructL();
    return self;
    }


// ---------------------------------------------------------------------------
// Destructor
// ---------------------------------------------------------------------------
//
CAlfSrvTransformationSubSession::~CAlfSrvTransformationSubSession()
    {
    iTransformation= NULL;
    }

// ---------------------------------------------------------------------------
// Gets transformation
// ---------------------------------------------------------------------------
//
CHuiTransformation& CAlfSrvTransformationSubSession::Transformation()
    {
    __ASSERT_ALWAYS( iTransformation, USER_INVARIANT() );
    return *iTransformation;
    }
    
// ---------------------------------------------------------------------------
// Sets transformation
// ---------------------------------------------------------------------------
//
void CAlfSrvTransformationSubSession::SetTransformation( 
    CHuiTransformation* aTransformation )
    {
    iTransformation = aTransformation;
    }
    
// ---------------------------------------------------------------------------
// Reset matrix
// ---------------------------------------------------------------------------
// 
void CAlfSrvTransformationSubSession::LoadIdentityL(const RMessage2& aMessage)
    {
    // Load identity
    Transformation().LoadIdentity();
    
    // Complete
    aMessage.Complete( KErrNone );
    }
    
// ---------------------------------------------------------------------------
// Translate
// ---------------------------------------------------------------------------
//
void CAlfSrvTransformationSubSession::TranslateL(const RMessage2& aMessage)
    {
    // Parser parameters

    // 1: x (in)
    TReal32 x(0.0);
    TPckg<TReal32> xRealPckg(x);
    aMessage.Read(1,xRealPckg);
    // 2: y (in)
    TReal32 y(0.0);
    TPckg<TReal32> yRealPckg(y);
    aMessage.Read(2,yRealPckg);
    
    // Translate
    Transformation().Translate( x, y );
    
    // Complete
    aMessage.Complete( KErrNone );
    }
  
// ---------------------------------------------------------------------------
// Rotate
// ---------------------------------------------------------------------------
//  
void CAlfSrvTransformationSubSession::RotateL(const RMessage2& aMessage)
    {
    // Parser parameters

    // 1: angle (in)
    TAlfTimedValue alf_angle;
    TPckg<TAlfTimedValue> anglePckg(alf_angle);
    aMessage.Read(1,anglePckg);
    
    THuiTimedValue angle;
    AlfTimedValueUtility::CopyTimedValue(alf_angle, angle, Session());    
    
    // Rotate
    Transformation().Rotate( angle );
    
    // Complete
    aMessage.Complete( KErrNone );
    }
    
// ---------------------------------------------------------------------------
// Count
// ---------------------------------------------------------------------------
//  
void CAlfSrvTransformationSubSession::CountL(const RMessage2& aMessage)
    {
    TInt count = 0;
    TPckg<TInt> outputPckg(count);    
        
    // Count
    count = Transformation().Count();
    
    // Write output params
    aMessage.Write(1,outputPckg);    
    
    // Complete
    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------------------------
// NonIdentityCount
// ---------------------------------------------------------------------------
//  
void CAlfSrvTransformationSubSession::NonIdentityCountL(const RMessage2& aMessage)
    {
    TInt count = 0;
    TPckg<TInt> outputPckg(count);    
        
    // Count
    count = Transformation().NonIdentityCount();
    
    // Write output params
    aMessage.Write(1,outputPckg);    
    
    // Complete
    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------------------------
// Step
// ---------------------------------------------------------------------------
//  
void CAlfSrvTransformationSubSession::StepL(const RMessage2& aMessage)
    {
    // 1: Index (in) 
    TInt index = aMessage.Int1();
    
    // 2: Transform step (out)
    CAlfTransformation::TTransform step;
    TPckg<CAlfTransformation::TTransform> outputPckg(step);    
               
    // Step
    CHuiTransformation::TTransform huiStep = Transformation().Step(index);
    step.iType = (CAlfTransformation::TType)huiStep.iType;
    for (TInt i=0; i<KHuiTransformParamCount; i++)
        {
        AlfTimedValueUtility::CopyTimedValue(huiStep.iParams[i], step.iParams[i]);    
        step.iParams[i].Flags() = EAlfValueFlagsNone; // clear flags so client will send only changed values back         
        }
    
    // Write output params
    aMessage.Write(2,outputPckg);    
    
    // Complete
    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------------------------
// ReplaceStep
// ---------------------------------------------------------------------------
//  
void CAlfSrvTransformationSubSession::ReplaceStepL(const RMessage2& aMessage)
    {
    // 1: Index (in) 
    TInt index = aMessage.Int1();
    
    // 2: Transform step (in)
    CAlfTransformation::TTransform step;
    TPckg<CAlfTransformation::TTransform> stepPckg(step);    
    aMessage.Read(2,stepPckg);               
    
    // ReplaceStep
    CHuiTransformation::TTransform& huiStep = Transformation().Step(index); 
    huiStep.iType = (CHuiTransformation::TType)step.iType;
    for (TInt i=0; i<KHuiTransformParamCount; i++)
        {
        AlfTimedValueUtility::CopyTimedValue(step.iParams[i], huiStep.iParams[i], Session());    
        }    
    
    // Complete
    aMessage.Complete( KErrNone );
    }

// ---------------------------------------------------------------------------
// Rotate (3D version of the function)
// ---------------------------------------------------------------------------
//  
void CAlfSrvTransformationSubSession::Rotate3DL(const RMessage2& aMessage)
    {
    // Parse parameters

    // 1: params (in)
    TAlfTransformationRotateParams params;
    TPckg<TAlfTransformationRotateParams> paramPckg(params);
    aMessage.Read(1,paramPckg);
    
    THuiTimedValue angle;
    AlfTimedValueUtility::CopyTimedValue(params.iAngle, angle, Session());    

    // Rotate
    Transformation().Rotate( angle, params.iAxisX, params.iAxisY, params.iAxisZ );
    
    // Complete
    aMessage.Complete( KErrNone );
    }


// ---------------------------------------------------------------------------
// Scale (3D version of the function)
// ---------------------------------------------------------------------------
//  
void CAlfSrvTransformationSubSession::Scale3DL(const RMessage2& aMessage)
    {
    // Parse parameters

    // 1: params (in)
    TAlfTransformationTimedParams params;
    TPckg<TAlfTransformationTimedParams> paramPckg(params);
    aMessage.Read(1,paramPckg);
    
    THuiTimedValue x;
    AlfTimedValueUtility::CopyTimedValue(params.iX, x, Session());    

    THuiTimedValue y;
    AlfTimedValueUtility::CopyTimedValue(params.iY, y, Session());    

    THuiTimedValue z;
    AlfTimedValueUtility::CopyTimedValue(params.iZ, z, Session());    

    // Rotate
    Transformation().Scale( x,y,z );
    
    // Complete
    aMessage.Complete( KErrNone );
    
    }

// ---------------------------------------------------------------------------
// Translate  (2D version of the function with timedvalues)
// ---------------------------------------------------------------------------
//  
void CAlfSrvTransformationSubSession::Translate2DL(const RMessage2& aMessage)
    {
    // Parse parameters

    // 1: params (in)
    TAlfTransformationTimedParams params;
    TPckg<TAlfTransformationTimedParams> paramPckg(params);
    aMessage.Read(1,paramPckg);
    
    THuiTimedValue x;
    AlfTimedValueUtility::CopyTimedValue(params.iX, x, Session());    

    THuiTimedValue y;
    AlfTimedValueUtility::CopyTimedValue(params.iY, y, Session());    

    // Rotate
    Transformation().Translate( x,y );
    
    // Complete
    aMessage.Complete( KErrNone );
    }


// ---------------------------------------------------------------------------
// From CAlfSrvSubSessionBase
// Process the message.
// ---------------------------------------------------------------------------
// 
void CAlfSrvTransformationSubSession::DoProcessMessageL(const RMessage2& aMessage)
    {
    switch( aMessage.Int0() )
        {
        case EAlfTransformationLoadIdentity:
            {
            LoadIdentityL( aMessage );
            break;
            }
        case EAlfTransformationTranslate:
            {
            TranslateL( aMessage );
            break;
            }
        case EAlfTransformationRotate:
            {
            RotateL( aMessage );
            break;
            }

        case EAlfTransformationCount:
            {
            CountL( aMessage );                
            break;
            }
        case EAlfTransformationNonIdentityCount:
            {
            NonIdentityCountL( aMessage );     
            break;
            }
        case EAlfTransformationStep:
            {
            StepL( aMessage );                
            break;
            }
        case EAlfTransformationReplaceStep:
            {
            ReplaceStepL( aMessage );                                
            break;
            }
        case EAlfTransformationRotate3D:
            {
            Rotate3DL( aMessage );                                                
            break;
            }
        case EAlfTransformationTranslate2D:
            {
            Translate2DL( aMessage );                                                                
            break;
            }
        case EAlfTransformationScale:
            {
            Scale3DL( aMessage );                                                                                
            break;
            }
            
        case EAlfTransformationSubSessionClose: // obsolete
            {
            Session().CloseSubSession( aMessage );
            break;
            }            
        default:
            CAlfSrvSubSessionBase::DoProcessMessageL( aMessage );
            break;
        }
    }
    
 

