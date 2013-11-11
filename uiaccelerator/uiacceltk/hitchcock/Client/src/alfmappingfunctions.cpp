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
* Description:   Alf mapping functions
*
*/




#include "alf/alfmappingfunctions.h"
#include "alf/alfgencomponent.h"
#include "alf/alfconstants.h"
#include "alflogger.h"

#include <uiacceltk/HuiUtil.h>

#include <e32math.h>
#include <e32std.h>


// CONSTANT MAPPING FUNCTION

struct CAlfConstantMappingFunction::TPrivateData
    {
    CAlfGenComponent* iComms;
    TReal32 iValue;
    };

EXPORT_C CAlfConstantMappingFunction* CAlfConstantMappingFunction::NewL( 
        CAlfEnv& aEnv, 
        TReal32 aValue  ) __SOFTFP
    {
    CAlfConstantMappingFunction* self = new (ELeave) CAlfConstantMappingFunction;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aValue );
    CleanupStack::Pop( self );
    return self;
    }
    
CAlfConstantMappingFunction::CAlfConstantMappingFunction()
    {
    }
    
void CAlfConstantMappingFunction::ConstructL( CAlfEnv& aEnv, TReal32 aValue )
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iComms = NULL;
    
    TPckgC<TReal32> paramsPckg( aValue );    
    
    iData->iComms = CAlfGenComponent::NewL(aEnv,
            EAlfMappingFunctionConstantCreate, 
            0, 
            paramsPckg);
            
    iData->iValue = aValue;
    }
    
    
EXPORT_C CAlfConstantMappingFunction::~CAlfConstantMappingFunction()
    {
    if ( iData )
        {
        delete iData->iComms;
        iData->iComms = NULL;
        }
    delete iData;
    iData = NULL;
    }
    
TReal32 CAlfConstantMappingFunction::MapValue(TReal32 /*aValue*/, TInt /*aComponent*/) const __SOFTFP
    {
    return iData->iValue;
    }
    
TInt CAlfConstantMappingFunction::MappingFunctionIdentifier() const
    {
    return iData->iComms->Identifier();
    }
    
EXPORT_C void CAlfConstantMappingFunction::SetValue( TReal32 aValue ) __SOFTFP
    {
    TPckgC<TReal32> paramsPckg( aValue );
    
    TInt err = iData->iComms->DoCmdNoReply(
        EAlfConstantMappingFunctionSetValue, 
        paramsPckg );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfConstantMappingFunction::SetValue panic error %d", err )
        USER_INVARIANT();
        }
    
    iData->iValue = aValue;
    }
    
EXPORT_C TReal32 CAlfConstantMappingFunction::Value() const __SOFTFP
    {
    return iData->iValue;
    }
    
// LINEAR MAPPING FUNCTION

struct CAlfLinearMappingFunction::TPrivateData
    {
    CAlfGenComponent* iComms;
    TReal32 iFactor; 
    TReal32 iOffset;
    };

EXPORT_C CAlfLinearMappingFunction* CAlfLinearMappingFunction::NewL( 
        CAlfEnv& aEnv, 
        TReal32 aFactor,
        TReal32 aOffset) __SOFTFP
    {
    CAlfLinearMappingFunction* self = new (ELeave) CAlfLinearMappingFunction;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aFactor, aOffset );
    CleanupStack::Pop( self );
    return self;
    }
    
CAlfLinearMappingFunction::CAlfLinearMappingFunction()
    {
    }
    
void CAlfLinearMappingFunction::ConstructL( CAlfEnv& aEnv, TReal32 aFactor, TReal32 aOffset )
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iComms = NULL;
    
    TReal2 params( aFactor, aOffset );
    TPckgC<TReal2> paramsPckg( params );    
    
    iData->iComms = CAlfGenComponent::NewL(aEnv,
            EAlfMappingFunctionLinearCreate, 
            0, 
            paramsPckg);
            
    iData->iFactor = aFactor;
    iData->iOffset = aOffset;
    }
    
EXPORT_C CAlfLinearMappingFunction::~CAlfLinearMappingFunction()
    {
    if ( iData )
        {
        delete iData->iComms;
        iData->iComms = NULL;
        }
    delete iData;
    iData = NULL;
    }
    
TReal32 CAlfLinearMappingFunction::MapValue(TReal32 aValue, TInt /*aComponent*/) const __SOFTFP
    {
    return aValue * iData->iFactor + iData->iOffset;
    }
    
TInt CAlfLinearMappingFunction::MappingFunctionIdentifier() const
    {
    return iData->iComms->Identifier();
    }
    
EXPORT_C void CAlfLinearMappingFunction::SetFactor( TReal32 aFactor ) __SOFTFP
    {
    TPckgC<TReal32> paramsPckg( aFactor );

    TInt err = iData->iComms->DoCmdNoReply(
        EAlfLinearMappingFunctionSetFactor, 
        paramsPckg );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLinearMappingFunction::SetFactor panic error %d", err )
        USER_INVARIANT();
        }
    
    iData->iFactor = aFactor;
    }
    
EXPORT_C TReal32 CAlfLinearMappingFunction::Factor() const __SOFTFP
    {
    return iData->iFactor;
    }
    
EXPORT_C void CAlfLinearMappingFunction::SetOffset( TReal32 aOffset ) __SOFTFP
    {
    TPckgC<TReal32> paramsPckg( aOffset );
    
    TInt err = iData->iComms->DoCmdNoReply(
        EAlfLinearMappingFunctionSetOffset, 
        paramsPckg );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfLinearMappingFunction::SetOffset panic error %d", err )
        USER_INVARIANT();
        }
    
    iData->iOffset = aOffset;
    }
    
EXPORT_C TReal32 CAlfLinearMappingFunction::Offset() const __SOFTFP
    {
    return iData->iOffset;
    }
    
// SINE MAPPING FUNCTION

struct CAlfSineMappingFunction::TPrivateData
    {
    CAlfGenComponent* iComms;
    TReal32 iFactor; 
    TReal32 iOffset;
    };

EXPORT_C CAlfSineMappingFunction* CAlfSineMappingFunction::NewL( 
        CAlfEnv& aEnv, 
        TReal32 aFactor,
        TReal32 aOffset) __SOFTFP
    {
    CAlfSineMappingFunction* self = new (ELeave) CAlfSineMappingFunction;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aFactor,aOffset );
    CleanupStack::Pop( self );
    return self;
    }
    
CAlfSineMappingFunction::CAlfSineMappingFunction()
    {
    }
    
void CAlfSineMappingFunction::ConstructL( CAlfEnv& aEnv, TReal32 aFactor, TReal32 aOffset )
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iComms = NULL;
    
    TReal2 params( aFactor, aOffset );
    TPckgC<TReal2> paramsPckg( params );    
    
    iData->iComms = CAlfGenComponent::NewL(aEnv,
            EAlfMappingFunctionSineCreate, 
            0, 
            paramsPckg);
            
    iData->iFactor = aFactor;
    iData->iOffset = aOffset;
    }
    
EXPORT_C CAlfSineMappingFunction::~CAlfSineMappingFunction()
    {
    if ( iData )
        {
        delete iData->iComms;
        iData->iComms = NULL;
        }
    delete iData;
    iData = NULL;
    }
    
TReal32 CAlfSineMappingFunction::MapValue(TReal32 aValue, TInt /*aComponent*/) const __SOFTFP
    {
    TReal mapped = 0;
    Math::Sin(mapped, aValue);
    return mapped * iData->iFactor + iData->iOffset;
    }
    
TInt CAlfSineMappingFunction::MappingFunctionIdentifier() const
    {
    return iData->iComms->Identifier();
    }
    
EXPORT_C void CAlfSineMappingFunction::SetFactor( TReal32 aFactor ) __SOFTFP
    {
    TPckgC<TReal32> paramsPckg( aFactor );

    TInt err = iData->iComms->DoCmdNoReply(
        EAlfSineMappingFunctionSetFactor, 
        paramsPckg );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfSineMappingFunction::SetFactor panic error %d", err )
        USER_INVARIANT();
        }
    
    iData->iFactor = aFactor;
    }
    
EXPORT_C TReal32 CAlfSineMappingFunction::Factor() const __SOFTFP
    {
    return iData->iFactor;
    }
    
EXPORT_C void CAlfSineMappingFunction::SetOffset( TReal32 aOffset ) __SOFTFP
    {
    TPckgC<TReal32> paramsPckg( aOffset );
    
    TInt err = iData->iComms->DoCmdNoReply(
        EAlfSineMappingFunctionSetOffset, 
        paramsPckg );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfSineMappingFunction::SetOffset panic error %d", err )
        USER_INVARIANT();
        }
    
    iData->iOffset = aOffset;
    }
    
EXPORT_C TReal32 CAlfSineMappingFunction::Offset() const __SOFTFP
    {
    return iData->iOffset;
    }
    
// COSINE MAPPING FUNCTION

struct CAlfCosineMappingFunction::TPrivateData
    {
    CAlfGenComponent* iComms;
    TReal32 iFactor; 
    TReal32 iOffset;
    };

EXPORT_C CAlfCosineMappingFunction* CAlfCosineMappingFunction::NewL( 
        CAlfEnv& aEnv, 
        TReal32 aFactor,
        TReal32 aOffset) __SOFTFP
    {
    CAlfCosineMappingFunction* self = new (ELeave) CAlfCosineMappingFunction;
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aFactor,aOffset );
    CleanupStack::Pop( self );
    return self;
    }
    
CAlfCosineMappingFunction::CAlfCosineMappingFunction()
    {
    }
    
void CAlfCosineMappingFunction::ConstructL( CAlfEnv& aEnv, TReal32 aFactor, TReal32 aOffset )
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iComms = NULL;
    
    TReal2 params( aFactor, aOffset );
    TPckgC<TReal2> paramsPckg( params );    
    
    iData->iComms = CAlfGenComponent::NewL(aEnv,
            EAlfMappingFunctionCosineCreate, 
            0, 
            paramsPckg);
            
    iData->iFactor = aFactor;
    iData->iOffset = aOffset;
    }
    
EXPORT_C CAlfCosineMappingFunction::~CAlfCosineMappingFunction()
    {
    if ( iData )
        {
        delete iData->iComms;
        iData->iComms = NULL;
        }
    delete iData;
    iData = NULL;
    }
    
TReal32 CAlfCosineMappingFunction::MapValue(TReal32 aValue, TInt /*aComponent*/) const __SOFTFP
    {
    TReal mapped = 0;
    Math::Cos(mapped, aValue);
    return mapped * iData->iFactor + iData->iOffset;
    }
    
TInt CAlfCosineMappingFunction::MappingFunctionIdentifier() const
    {
    return iData->iComms->Identifier();
    }
    
EXPORT_C void CAlfCosineMappingFunction::SetFactor( TReal32 aFactor ) __SOFTFP
    {
    TPckgC<TReal32> paramsPckg( aFactor );
    
    TInt err = iData->iComms->DoCmdNoReply(
        EAlfCosineMappingFunctionSetFactor, 
        paramsPckg );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCosineMappingFunction::SetFactor panic error %d", err )
        USER_INVARIANT();
        }
    
    iData->iFactor = aFactor;
    }
    
EXPORT_C TReal32 CAlfCosineMappingFunction::Factor() const __SOFTFP
    {
    return iData->iFactor;
    }
    
EXPORT_C void CAlfCosineMappingFunction::SetOffset( TReal32 aOffset ) __SOFTFP
    {
    TPckgC<TReal32> paramsPckg( aOffset );

    TInt err = iData->iComms->DoCmdNoReply(
        EAlfCosineMappingFunctionSetOffset, 
        paramsPckg );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfCosineMappingFunction::SetOffset panic error %d", err )
        USER_INVARIANT();
        }
    
    iData->iOffset = aOffset;
    }
    
EXPORT_C TReal32 CAlfCosineMappingFunction::Offset() const __SOFTFP
    {
    return iData->iOffset;
    }
        
// AVERAGE MAPPING FUNCTION

struct CAlfAverageMappingFunction::TPrivateData
    {
    CAlfGenComponent* iComms;
    };
    

EXPORT_C CAlfAverageMappingFunction* CAlfAverageMappingFunction::NewL( 
        CAlfEnv& aEnv,
        MAlfMappingFunction* aFunc1, 
        MAlfMappingFunction* aFunc2 )
    {
    CAlfAverageMappingFunction* self = new (ELeave)CAlfAverageMappingFunction();
    CleanupStack::PushL( self );
    self->ConstructL( aEnv, aFunc1, aFunc2 );
    CleanupStack::Pop( self );
    return self;
    }
    
CAlfAverageMappingFunction::CAlfAverageMappingFunction()
    {
    }
    
void CAlfAverageMappingFunction::ConstructL( 
        CAlfEnv& aEnv,
        MAlfMappingFunction* aFunc1, 
        MAlfMappingFunction* aFunc2
        )
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iComms = NULL;
    
    TInt2 functions( 
        aFunc1 ? aFunc1->MappingFunctionIdentifier() : 0,
        aFunc2 ? aFunc2->MappingFunctionIdentifier() : 0 );
    TPckgC<TInt2> functionsPckg( functions );    
    
    iData->iComms = CAlfGenComponent::NewL(aEnv,
            EAlfMappingFunctionAverageCreate, 
            0, 
            functionsPckg);
    }
    
EXPORT_C CAlfAverageMappingFunction::~CAlfAverageMappingFunction()
    {
    if ( iData )
        {
        delete iData->iComms;
        iData->iComms = NULL;
        }
    delete iData;
    iData = NULL;
    }

TReal32 CAlfAverageMappingFunction::MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP
    {
    TIntTReal mapValues( aComponent, aValue );
    TPckgC<TIntTReal> mapValuesPckg( mapValues );
    
    TReal32 returnValue = aValue;
    TPckg<TReal32> returnBuf(returnValue);
    
    TInt err = iData->iComms->DoSynchronousCmd(
        EAlfMappingFunctionMapValue, 
        mapValuesPckg, 
        returnBuf );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfAverageMappingFunction::MapValue panic error %d", err )
        USER_INVARIANT();
        }
    
    return returnValue;
    }
    
TInt CAlfAverageMappingFunction::MappingFunctionIdentifier() const
    {
    return iData->iComms->Identifier();
    }
    
EXPORT_C void CAlfAverageMappingFunction::SetMappingFunction1( MAlfMappingFunction* aFunction1 )
    {
    TInt function = aFunction1 ? aFunction1->MappingFunctionIdentifier() : 0;
    TPckgC<TInt> functionPckg( function );

    TInt err = iData->iComms->DoCmdNoReply(
        EAlfAverageMappingFunctionSetFunction1, 
        functionPckg );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfAverageMappingFunction::SetMappingFunction1 panic error %d", err )
        USER_INVARIANT();
        }
    }
    
EXPORT_C void CAlfAverageMappingFunction::SetMappingFunction2( MAlfMappingFunction* aFunction2 )
    {
    TInt function = aFunction2 ? aFunction2->MappingFunctionIdentifier() : 0;
    TPckgC<TInt> functionPckg( function );
    TBuf8<1> outDum;
    
    TInt err = iData->iComms->DoCmdNoReply(
        EAlfAverageMappingFunctionSetFunction2, 
        functionPckg );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfAverageMappingFunction::SetMappingFunction2 panic error %d", err )
        USER_INVARIANT();
        }
    }
    
EXPORT_C void CAlfAverageMappingFunction::SetMappingFunctions( 
        MAlfMappingFunction* aFunction1,
        MAlfMappingFunction* aFunction2 )
    {
    TInt2 functions( 
        aFunction1 ? aFunction1->MappingFunctionIdentifier() : 0,
        aFunction2 ? aFunction2->MappingFunctionIdentifier() : 0 );
    TPckgC<TInt2> functionsPckg( functions ); 
    TBuf8<1> outDum;
    
    TInt err = iData->iComms->DoCmdNoReply(
        EAlfAverageMappingFunctionSetFunctions, 
        functionsPckg );
        
    if ( err )
        {
        __ALFLOGSTRING1( "CAlfAverageMappingFunction::SetMappingFunctions panic error %d", err )
        USER_INVARIANT();
        }
    }

EXPORT_C void CAlfAverageMappingFunction::SetWeight( 
    const TAlfTimedValue& aWeight )
    {
    TPckgC<TAlfTimedValue> buf(aWeight);

    TInt err = iData->iComms->DoCmdNoReply( 
        EAlfAverageMappingFunctionSetWeight, 
        buf);

    if ( err )
        {
        __ALFLOGSTRING1( "CAlfAverageMappingFunction::SetWeight panic error %d", err )
        USER_INVARIANT();
        }
    }    
struct CAlfTableMappingFunction::TPrivateData
    {
    CAlfGenComponent* iComms;
    TAlfTableMappingFunctionParams iParams;
    MAlfTableMappingFunctionDataProvider* iLastFunction;
    TReal32 iLastStart;
    TReal32 iLastEnd;
    };

    

EXPORT_C CAlfTableMappingFunction* CAlfTableMappingFunction::NewL( 
        CAlfEnv& aEnv)
    {
    CAlfTableMappingFunction* self = new (ELeave)CAlfTableMappingFunction();
    CleanupStack::PushL( self );
    self->ConstructL( aEnv );
    CleanupStack::Pop( self );
    return self;
    }
    
CAlfTableMappingFunction::CAlfTableMappingFunction()
    {
    }
    
void CAlfTableMappingFunction::ConstructL(CAlfEnv& aEnv)
    {
    iData = new (ELeave) TPrivateData;
    
    iData->iComms = NULL;
    Mem::FillZ(&iData->iParams, sizeof(iData->iParams));
    iData->iLastStart = 0;
    iData->iLastEnd = 0;
    iData->iLastFunction = NULL;
    
    TReal2 param(0.f, 0.f);    
    TPckgC<TReal2> paramPckg( param );    
    iData->iComms = CAlfGenComponent::NewL(aEnv,
            EAlfMappingFunctionTableCreate, 
            0, 
            paramPckg);
    }
    
EXPORT_C CAlfTableMappingFunction::~CAlfTableMappingFunction()
    {
    if ( iData )
        {
        delete iData->iComms;
        iData->iComms = NULL;
        }
    delete iData;
    iData = NULL;
    }

TReal32 CAlfTableMappingFunction::MapValue(TReal32 aValue, TInt /*aComponent*/) const __SOFTFP
    {
    // For performance resons, this same function exist both client and serverside
    TReal32 retVal = aValue;
    
    TBool inverted = EFalse;
    
    if (iData->iParams.iValues[KAlfTableMappingNumberOfMappedValues - 1] < iData->iParams.iValues[0])
        {
        inverted = ETrue;    
        }
    
    TInt i = 0;
    
    if (!inverted)
        {
        // Find closest pre-calculated value...
        for (i=0; i<KAlfTableMappingNumberOfMappedValues-1;i++)
            {
            if ((!inverted && aValue < iData->iParams.iValues[i]) ||
                inverted && (aValue > iData->iParams.iValues[i]))
                {
                retVal = iData->iParams.iMappedValues[i];
                break;                    
                }
            retVal = iData->iParams.iMappedValues[i];                   
            }

        // ...do approximation, real value is between pre-calculated values
        if (i != 0) 
            {
            TReal32 valueDelta = iData->iParams.iValues[i] - aValue;
            if (inverted)
                {
                valueDelta = -valueDelta;    
                }
            
            TReal32 valueStep = iData->iParams.iValues[i] - iData->iParams.iValues[i-1];
            TReal32 mappedValueStep = iData->iParams.iMappedValues[i] - iData->iParams.iMappedValues[i-1];
            TReal32 fix = (valueDelta/valueStep) * mappedValueStep;
            retVal -= fix;                               
            }     
        }

    return retVal;   
    }
    
TInt CAlfTableMappingFunction::MappingFunctionIdentifier() const
    {
    return iData->iComms->Identifier();
    }
    
EXPORT_C void CAlfTableMappingFunction::SetMappingTableValues(TReal32 aStart, TReal32 aEnd, MAlfTableMappingFunctionDataProvider* aFunction) __SOFTFP
    {        
    TBool valuesChanged = (iData->iLastStart != aStart || iData->iLastEnd != aEnd);
    TBool functionChanged = (aFunction != iData->iLastFunction);
                    
    if (aFunction && (valuesChanged || functionChanged))
        {                           
        TReal32 step = (aEnd - aStart)/(KAlfTableMappingNumberOfMappedValues - 1);
        TReal32 value = aStart;
        for (TInt i=0; i<KAlfTableMappingNumberOfMappedValues;i++)
            {
            iData->iParams.iValues[i] = value;
            iData->iParams.iMappedValues[i] = aFunction->MapValue(value, 0);                
            value += step;                        
            }
        
        iData->iParams.iValues[KAlfTableMappingNumberOfMappedValues -1] = aEnd;
        iData->iParams.iMappedValues[KAlfTableMappingNumberOfMappedValues -1] = 
            aFunction->MapValue(aEnd, 0);                

        TPckgC<TAlfTableMappingFunctionParams> mapValuesPckg( iData->iParams );

        TInt err = iData->iComms->DoCmdNoReply(
            EAlfMappingFunctionTableSetValues, 
            mapValuesPckg );
        __ASSERT_ALWAYS( err == KErrNone, USER_INVARIANT() );

        iData->iLastStart = aStart;
        iData->iLastEnd = aEnd;
        iData->iLastFunction = aFunction;
        }        
    }
    
