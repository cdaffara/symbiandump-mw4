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
* Description:   Mapping function handlers
*
*/



#include "alfmappingfunctionhandlers.h"
#include "alf/alfconstants.h"
#include "alf/alfserverutils.h"

#include <uiacceltk/HuiCurvePath.h>

// CONSTANT MAPPING FUNCTION

TAlfConstantMappingFunctionHandler::TAlfConstantMappingFunctionHandler( 
        MAlfInterfaceProvider& aResolver,
        TReal32 aValue )
        : iConstantMappingFunction( aValue ) , iResolver(aResolver)
    {
    }

void TAlfConstantMappingFunctionHandler::Release()
    {
    delete this;
    }
    
TAny* TAlfConstantMappingFunctionHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    switch ( aInterface )
        {
        case EHuiInterfaceMappingFunction:
            return &iConstantMappingFunction;
        default:
            return NULL;
        }
    }
    
void TAlfConstantMappingFunctionHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& /*aResponse*/ )
    {
    switch ( aCommandId )
        {
        case EAlfConstantMappingFunctionSetValue:
            {
            TReal32* mapValues = (TReal32*) aInputBuffer.Ptr();
            iConstantMappingFunction.iValue = *mapValues;
            break;
            }
 
        default:
            User::Leave( KErrNotSupported );
        }
    }
    
// LINEAR MAPPING FUNCTION

TAlfLinearMappingFunctionHandler::TAlfLinearMappingFunctionHandler( 
        MAlfInterfaceProvider& aResolver,
        TReal32 aFactor,
        TReal32 aOffset )
        : iLinearMappingFunction( aFactor,aOffset ) , iResolver(aResolver)
    {
    }

void TAlfLinearMappingFunctionHandler::Release()
    {
    delete this;
    }
    
TAny* TAlfLinearMappingFunctionHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    switch ( aInterface )
        {
        case EHuiInterfaceMappingFunction:
            return &iLinearMappingFunction;
        default:
            return NULL;
        }
    }
    
void TAlfLinearMappingFunctionHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& /*aResponse*/ )
    {
    switch ( aCommandId )
        {
        case EAlfLinearMappingFunctionSetFactor:
            {
            TReal32* mapValues = (TReal32*) aInputBuffer.Ptr();
            iLinearMappingFunction.iFactor = *mapValues;
            break;
            }
            
        case EAlfLinearMappingFunctionSetOffset:
            {
            TReal32* mapValues = (TReal32*) aInputBuffer.Ptr();
            iLinearMappingFunction.iOffset = *mapValues;
            break;
            }
 
        default:
            User::Leave( KErrNotSupported );
        }
    }

// SINE MAPPING FUNCTION

TAlfSineMappingFunctionHandler::TAlfSineMappingFunctionHandler( 
        MAlfInterfaceProvider& aResolver,
        TReal32 aFactor,
        TReal32 aOffset )
        : iSineMappingFunction( aFactor,aOffset ) , iResolver(aResolver)
    {
    }

void TAlfSineMappingFunctionHandler::Release()
    {
    delete this;
    }
    
TAny* TAlfSineMappingFunctionHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    switch ( aInterface )
        {
        case EHuiInterfaceMappingFunction:
            return &iSineMappingFunction;
        default:
            return NULL;
        }
    }
    
void TAlfSineMappingFunctionHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& /*aResponse*/ )
    {
    switch ( aCommandId )
        {
        case EAlfSineMappingFunctionSetFactor:
            {
            TReal32* mapValues = (TReal32*) aInputBuffer.Ptr();
            iSineMappingFunction.iFactor = *mapValues;
            break;
            }
            
        case EAlfSineMappingFunctionSetOffset:
            {
            TReal32* mapValues = (TReal32*) aInputBuffer.Ptr();
            iSineMappingFunction.iOffset = *mapValues;
            break;
            }
 
        default:
            User::Leave( KErrNotSupported );
        }
    }

// COSINE MAPPING FUNCTION

TAlfCosineMappingFunctionHandler::TAlfCosineMappingFunctionHandler( 
        MAlfInterfaceProvider& aResolver,
        TReal32 aFactor,
        TReal32 aOffset )
        : iCosineMappingFunction( aFactor,aOffset ) , iResolver(aResolver)
    {
    }

void TAlfCosineMappingFunctionHandler::Release()
    {
    delete this;
    }
    
TAny* TAlfCosineMappingFunctionHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    switch ( aInterface )
        {
        case EHuiInterfaceMappingFunction:
            return &iCosineMappingFunction;
        default:
            return NULL;
        }
    }
    
void TAlfCosineMappingFunctionHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& /*aResponse*/ )
    {
    switch ( aCommandId )
        {
        case EAlfCosineMappingFunctionSetFactor:
            {
            TReal32* mapValues = (TReal32*) aInputBuffer.Ptr();
            iCosineMappingFunction.iFactor = *mapValues;
            break;
            }
            
        case EAlfCosineMappingFunctionSetOffset:
            {
            TReal32* mapValues = (TReal32*) aInputBuffer.Ptr();
            iCosineMappingFunction.iOffset = *mapValues;
            break;
            }
 
        default:
            User::Leave( KErrNotSupported );
        }
    }

// AVERAGE MAPPING FUNCTION

TAlfAverageMappingFunctionHandler::TAlfAverageMappingFunctionHandler( 
        MAlfInterfaceProvider& aResolver,
        MHuiMappingFunction* aFunc1, 
        MHuiMappingFunction* aFunc2 )
        : iAverageMappingFunction( aFunc1, aFunc2 ) , iResolver(aResolver)
    {
    }

void TAlfAverageMappingFunctionHandler::Release()
    {
    delete this;
    }
    
TAny* TAlfAverageMappingFunctionHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    switch ( aInterface )
        {
        case EHuiInterfaceMappingFunction:
            return &iAverageMappingFunction;
        default:
            return NULL;
        }
    }
    
void TAlfAverageMappingFunctionHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& aResponse )
    {
    switch ( aCommandId )
        {
        case EAlfMappingFunctionMapValue:
            {
            TIntTReal* mapValues = (TIntTReal*) aInputBuffer.Ptr();
            
            const TReal32 returnValue = iAverageMappingFunction.MapValue( mapValues->iReal, mapValues->iInt );
            
            TPckg<TReal32> returnValuePckg(returnValue);
            aResponse = returnValuePckg; 
            break;
            }
        case EAlfAverageMappingFunctionSetFunction1:
            {
            TInt* params = (TInt*)aInputBuffer.Ptr();
            
            MHuiMappingFunction* func1 = NULL;
            
            if ( *params )
                {
                func1 = (MHuiMappingFunction*) iResolver.GetInterfaceL(EHuiInterfaceMappingFunction, *params );
                }
            
            iAverageMappingFunction.iFunc1 = func1;
            break;
            }
        case EAlfAverageMappingFunctionSetFunction2:
            {
            TInt* params = (TInt*)aInputBuffer.Ptr();
            
            MHuiMappingFunction* func2 = NULL;
            
            if ( *params )
                {
                func2 = (MHuiMappingFunction*) iResolver.GetInterfaceL(EHuiInterfaceMappingFunction, *params );
                }
            
            iAverageMappingFunction.iFunc2 = func2;
            break;
            }
        case EAlfAverageMappingFunctionSetFunctions:
            {
            TInt2* params = (TInt2*) aInputBuffer.Ptr();
            
            MHuiMappingFunction* func1 = NULL;
            MHuiMappingFunction* func2 = NULL;
            
            if ( params->iInt1 )
                {
                func1 = (MHuiMappingFunction*) iResolver.GetInterfaceL(EHuiInterfaceMappingFunction, params->iInt1 );
                }
                
            if ( params->iInt2 )
                {
                func2 = (MHuiMappingFunction*) iResolver.GetInterfaceL(EHuiInterfaceMappingFunction, params->iInt2 );
                }
                
                
            iAverageMappingFunction.iFunc1 = func1;
            iAverageMappingFunction.iFunc2 = func2; 
            break;
            }
            
        case EAlfAverageMappingFunctionSetWeight:
            {
            TAlfTimedValue* opacity = (TAlfTimedValue*) aInputBuffer.Ptr() ;
            AlfTimedValueUtility::CopyTimedValue(*opacity, iAverageMappingFunction.iWeight, iResolver);    
            break;
            }
            
        default:
            User::Leave( KErrNotSupported );
        }
    }
    
// CURVE PATH

CAlfCurvePathHandler* CAlfCurvePathHandler::NewL( 
    MAlfInterfaceProvider& aResolver,
    CHuiCurvePath* aCurvePath )
    {
    CAlfCurvePathHandler* self = new (ELeave) CAlfCurvePathHandler( aResolver );
    CleanupStack::PushL( self );
    self->ConstructL( aCurvePath );
    CleanupStack::Pop( self );
    return self;
    }
    
CAlfCurvePathHandler::CAlfCurvePathHandler( MAlfInterfaceProvider& aResolver )
 : iResolver( aResolver )
    {
    }
    
void CAlfCurvePathHandler::ConstructL(CHuiCurvePath* aCurvePath)
    {
    if ( aCurvePath )
        {
        iCurvePath = aCurvePath;
        iOwnCurvePath = EFalse;
        }
    else
        {
        iCurvePath = CHuiCurvePath::NewL();
        iOwnCurvePath = ETrue;
        }
    }
    
CAlfCurvePathHandler::~CAlfCurvePathHandler()
    {
    if ( iOwnCurvePath )
        {
        delete iCurvePath;
        }
    iCurvePath = NULL;
    }
    

void CAlfCurvePathHandler::Release()
    {
    delete this;
    }
    
TAny* CAlfCurvePathHandler::GetInterface(const THuiInterfaceSupport& aInterface)
    {
    switch ( aInterface )
        {
        case EHuiInterfaceMappingFunction:
            return static_cast<MHuiMappingFunction*>(iCurvePath);
        default:
            return NULL;
        }
    }
    
void CAlfCurvePathHandler::HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse)
    {
    switch( aCommandId )
        {
        case EAlfCurvePathReset:
            {
            iCurvePath->Reset();
            break;
            }
            
        case EAlfCurvePathEnableLoop:
            {
            TBool* params = (TBool*) aInputBuffer.Ptr();
            iCurvePath->EnableLoop( *params );
            break;
            }
            
        case EAlfCurvePathLoop:
            {
            const TBool returnValue = iCurvePath->Loop();
            
            TPckg<TBool> returnValuePckg(returnValue);
            aResponse = returnValuePckg; 
            break;
            }
            
        case EAlfCurvePathSetOrigin:
            {
            TReal32* params = (TReal32*) aInputBuffer.Ptr();
            iCurvePath->SetOrigin( *params );
            break;
            }
            
        case EAlfCurvePathAppendLine:
            {
            TAlfCurvePathLineParams* params = (TAlfCurvePathLineParams*) aInputBuffer.Ptr();
            iCurvePath->AppendLineL( 
                params->iStart,
                params->iEnd,
                params->iLength );
            break;
            }
            
        case EAlfCurvePathAppendLine2:
            {
            TAlfCurvePathLineRealParams* params = (TAlfCurvePathLineRealParams*) aInputBuffer.Ptr();
            THuiRealPoint start(params->iStart.iX, params->iStart.iY);
            THuiRealPoint end(params->iEnd.iX, params->iEnd.iY);
            iCurvePath->AppendLineL( 
                start,
                end,
                params->iLength );
            break;
            }
            
        case EAlfCurvePathAppendArc:
            {
            TAlfCurvePathArcParams* params = (TAlfCurvePathArcParams*) aInputBuffer.Ptr();
            iCurvePath->AppendArcL( 
                params->iOrigin,
                params->iSize,
                params->iStartAngle,
                params->iEndAngle,
                params->iLength );
            break;
            }
            
        case EAlfCurvePathAppendArc2:
            {
            TAlfCurvePathArcRealParams* params = (TAlfCurvePathArcRealParams*) aInputBuffer.Ptr();
            THuiRealPoint origin(params->iOrigin.iX, params->iOrigin.iY);
            THuiRealSize size2(params->iSize.iWidth, params->iSize.iHeight);
            iCurvePath->AppendArcL( 
                origin,
                size2,
                params->iStartAngle,
                params->iEndAngle,
                params->iLength );
            break;
            }
            
        case EAlfCurvePathLength:
            {
            const TReal32 returnValue = iCurvePath->Length();
            
            TPckg<TReal32> returnValuePckg(returnValue);
            aResponse = returnValuePckg; 
            break;
            }
            
            
        case EAlfCurvePathEvaluate:
            {
            TReal32* params = (TReal32*) aInputBuffer.Ptr();
            
            
            THuiRealPoint result;
            iCurvePath->Evaluate( *params, result );
            
            TAlfRealPoint returnValue(result.iX, result.iY);
            TPckg<TAlfRealPoint> returnValuePckg(returnValue);
            aResponse = returnValuePckg; 
            break;
            }
            
        case EAlfCurvePathOffset:
            {
            TAlfTimedPoint returnValue;
            AlfTimedPointUtility::CopyTimedPoint(iCurvePath->iOffset, returnValue);

            TPckg<TAlfTimedPoint> returnValuePckg(returnValue);
            aResponse = returnValuePckg;
            break;
            }
        
        case EAlfCurvePathSetOffset:
            {
            TAlfTimedPoint* params = (TAlfTimedPoint*) aInputBuffer.Ptr();
            AlfTimedPointUtility::CopyTimedPoint(*params, iCurvePath->iOffset, iResolver);    
            break;
            }
            
        case EAlfCurvePathMapValue:
            {
            TIntTReal* mapValues = (TIntTReal*) aInputBuffer.Ptr();
            
            const TReal32 returnValue = iCurvePath->MapValue( mapValues->iReal, mapValues->iInt );
            
            TPckg<TReal32> returnValuePckg(returnValue);
            aResponse = returnValuePckg; 
            break;
            }
            
        default:
            User::Leave( KErrNotSupported );
        }
    }

    
    
// TABLE MAPPING FUNCTION

TAlfTableMappingFunctionHandler::TAlfTableMappingFunctionHandler( 
        MAlfInterfaceProvider& aResolver) 
        : iResolver(aResolver)
    {
    }

void TAlfTableMappingFunctionHandler::Release()
    {
    delete this;
    }
    
TAny* TAlfTableMappingFunctionHandler::GetInterface(
    const THuiInterfaceSupport& aInterface)
    {
    switch ( aInterface )
        {
        case EHuiInterfaceMappingFunction:
            return &iMappingFunction;
        default:
            return NULL;
        }
    }
    
void TAlfTableMappingFunctionHandler::HandleCmdL(
    TInt aCommandId, 
    const TDesC8& aInputBuffer, 
    TDes8& aResponse )
    {
    switch ( aCommandId )
        {   
        case EAlfMappingFunctionMapValue:
            {
            TIntTReal* mapValues = (TIntTReal*) aInputBuffer.Ptr();
            
            const TReal32 returnValue = iMappingFunction.MapValue( mapValues->iReal, mapValues->iInt );
            
            TPckg<TReal32> returnValuePckg(returnValue);
            aResponse = returnValuePckg; 
            break;
            }            
        case EAlfMappingFunctionTableSetValues:
            {
            TAlfTableMappingFunctionParams* mapValues = (TAlfTableMappingFunctionParams*) aInputBuffer.Ptr();
            iMappingFunction.iParams = *mapValues;                        
            break;
            }            
        default:
            User::Leave( KErrNotSupported );
        }
    }

TAlfTableMappingFunction::TAlfTableMappingFunction()
    {
        
    }
    
TReal32 TAlfTableMappingFunction::MapValue(TReal32 aValue, TInt /*aComponent*/) const __SOFTFP
    {
    // For performance resons, this same function exist both client and serverside
    TReal32 retVal = aValue;
    
    TBool inverted = EFalse;
    
    if (iParams.iValues[KAlfTableMappingNumberOfMappedValues - 1] < iParams.iValues[0])
        {
        inverted = ETrue;    
        }
    
    TInt i = 0;
    
    if (!inverted)
        {
        // Find closest pre-calculated value...
        for (i=0; i<KAlfTableMappingNumberOfMappedValues-1;i++)
            {
            if ((!inverted && aValue < iParams.iValues[i]) ||
                inverted && (aValue > iParams.iValues[i]))
                {
                retVal = iParams.iMappedValues[i];
                break;                    
                }
            retVal = iParams.iMappedValues[i];                   
            }

        // ...do approximation, real value is between pre-calculated values
        if (i != 0) 
            {
            TReal32 valueDelta = iParams.iValues[i] - aValue;
            if (inverted)
                {
                valueDelta = -valueDelta;    
                }
            
            TReal32 valueStep = iParams.iValues[i] - iParams.iValues[i-1];
            TReal32 mappedValueStep = iParams.iMappedValues[i] - iParams.iMappedValues[i-1];
            TReal32 fix = (valueDelta/valueStep) * mappedValueStep;
            retVal -= fix;                               
            }     
        }
     
    return retVal;   
    }
            
