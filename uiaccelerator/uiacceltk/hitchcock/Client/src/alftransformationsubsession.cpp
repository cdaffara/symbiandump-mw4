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
* Description:   Sub-session for transformation
*
*/



#include "alftransformationsubsession.h"
#include "alf/alfconstants.h"
#include "alflogger.h"

// ======== MEMBER FUNCTIONS ========

// ---------------------------------------------------------------------------
// Opens subsession
// ---------------------------------------------------------------------------
//
TInt RAlfTransformationSubSession::Open( RAlfClient& aSession )
    {
    TAlfImplementationInformation info(EAlfTransformationSubSessionOpen);
    TPckgC<TAlfImplementationInformation> infoBuf(info);


    return CreateSubSession( 
        aSession,
        EAlfCreateSubSession,
        TIpcArgs(&infoBuf) );
    }
 
// ---------------------------------------------------------------------------
// Closes subsession
// ---------------------------------------------------------------------------
//   
void RAlfTransformationSubSession::Close()
    {
    CloseSubSession( EAlfCloseSubSession );
    }
    
// ---------------------------------------------------------------------------
// Loads identity matrix i.e. resets.
// ---------------------------------------------------------------------------
// 
void RAlfTransformationSubSession::LoadIdentity()
    {
    Send/*Receive*/(
        EAlfDoSubSessionCmd, 
        TIpcArgs( EAlfTransformationLoadIdentity ) );
    __ALFLOGSTRING( "RAlfTransformationSubSession::LoadIdentity sent" )
    }
    
// ---------------------------------------------------------------------------
// 2D translate.
// ---------------------------------------------------------------------------
// 
void RAlfTransformationSubSession::Translate( 
    TReal32 aX, 
    TReal32 aY )
    {
    TPckg<TReal32> xRealPckg(aX);
    TPckg<TReal32> yRealPckg(aY);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs( EAlfTransformationTranslate, &xRealPckg, &yRealPckg ) );
        
    if ( err )
        {
        __ALFLOGSTRING1( "RAlfTransformationSubSession::Translate ignore error %d", err )
        }
    }
    
// ---------------------------------------------------------------------------
// 2D rotation.
// ---------------------------------------------------------------------------
// 
void RAlfTransformationSubSession::Rotate( const TAlfTimedValue& aAngle )
    {
    TPckgC<TAlfTimedValue> anglePckg(aAngle);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfTransformationRotate, &anglePckg ) );
    if ( err )
        {
        __ALFLOGSTRING1( "RAlfTransformationSubSession::Rotate ignore error %d", err )
        }
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt RAlfTransformationSubSession::Count() const
    {
    TInt retVal = 0;
    TPckg<TInt> retValPckg(retVal);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfTransformationCount, &retValPckg ) );
    if ( err )
        {
        __ALFLOGSTRING1( "RAlfTransformationSubSession::Count ignore error %d", err )
        }       
    return retVal;
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
TInt RAlfTransformationSubSession::NonIdentityCount() const
    {
    TInt retVal = 0;
    TPckg<TInt> retValPckg(retVal);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfTransformationNonIdentityCount, &retValPckg ) );
    if ( err )
        {
        __ALFLOGSTRING1( "RAlfTransformationSubSession::NonIdentityCount ignore error %d", err )
        }                   
    return retVal;
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
CAlfTransformation::TTransform RAlfTransformationSubSession::Step(TInt aIndex)
    {
    CAlfTransformation::TTransform retVal;
    TPckg<CAlfTransformation::TTransform> retValPckg(retVal);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfTransformationStep, aIndex, &retValPckg) );
    if ( err )
        {
        __ALFLOGSTRING1( "RAlfTransformationSubSession::Step ignore error %d", err )
        }                  
    return retVal;        
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void RAlfTransformationSubSession::ReplaceStep(TInt aIndex, 
    CAlfTransformation::TTransform aTransformationStep)
    {
    TPckg<CAlfTransformation::TTransform> inputValPckg(aTransformationStep);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfTransformationReplaceStep, aIndex, &inputValPckg ) );
    if ( err )
        {
        __ALFLOGSTRING1( "RAlfTransformationSubSession::ReplaceStep ignore error %d", err )
        }               
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void RAlfTransformationSubSession::Rotate(const TAlfTimedValue& aAngle, 
                         TReal32 aAxisX, TReal32 aAxisY, TReal32 aAxisZ)
    {
    TAlfTransformationRotateParams params;
    params.iAngle = aAngle;
    params.iAxisX = aAxisX;
    params.iAxisY = aAxisY;
    params.iAxisZ = aAxisZ;
    
    TPckg<TAlfTransformationRotateParams> inputValPckg(params);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfTransformationRotate3D, &inputValPckg ) );
    if ( err )
        {
        __ALFLOGSTRING1( "RAlfTransformationSubSession::Rotate ignore error %d", err )
        }                   
        
    }
    
// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void RAlfTransformationSubSession::Translate(const TAlfTimedValue& aX, 
    const TAlfTimedValue& aY)
    {
    TAlfTransformationTimedParams params;
    params.iX = aX;
    params.iY = aY;
    
    TPckg<TAlfTransformationTimedParams> inputValPckg(params);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfTransformationTranslate2D, &inputValPckg ) );
    if ( err )
        {
        __ALFLOGSTRING1( "RAlfTransformationSubSession::Translate ignore error %d", err )
        }                               
    }

// ---------------------------------------------------------------------------
// 
// ---------------------------------------------------------------------------
// 
void RAlfTransformationSubSession::Scale(const TAlfTimedValue& aX, 
    const TAlfTimedValue& aY, const TAlfTimedValue& aZ)
    {
    TAlfTransformationTimedParams params;
    params.iX = aX;
    params.iY = aY;
    params.iZ = aZ;
    
    TPckg<TAlfTransformationTimedParams> inputValPckg(params);
    TInt err = SendReceive(
        EAlfDoSubSessionCmd, 
        TIpcArgs(EAlfTransformationScale, &inputValPckg ) );
    if ( err )
        {
        __ALFLOGSTRING1( "RAlfTransformationSubSession::Scale ignore error %d", err )
        }                   
        
    }
