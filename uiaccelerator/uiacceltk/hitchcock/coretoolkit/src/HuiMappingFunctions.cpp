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



#include "uiacceltk/HuiMappingFunctions.h"
#include "uiacceltk/huifixmath.h"


EXPORT_C
THuiConstantMappingFunction::THuiConstantMappingFunction(TReal32 aValue) __SOFTFP
        : iValue(aValue)
    {
    }
    
    
EXPORT_C TReal32 THuiConstantMappingFunction::MapValue(TReal32 /*aValue*/, 
                                                     TInt /*aComponent*/) const __SOFTFP
    {
    return iValue;
    }



EXPORT_C THuiLinearMappingFunction::THuiLinearMappingFunction(TReal32 aFactor, TReal32 aOffset) __SOFTFP
        : iFactor(aFactor), iOffset(aOffset)
    {
    }
    

EXPORT_C TReal32 THuiLinearMappingFunction::MapValue(TReal32 aValue, TInt /*aComponent*/) const __SOFTFP
    {
    return aValue * iFactor + iOffset;
    }


EXPORT_C 
THuiSineMappingFunction::THuiSineMappingFunction(TReal32 aFactor, TReal32 aOffset) __SOFTFP
        : iFactor(aFactor), iOffset(aOffset)
    {
    }


EXPORT_C TReal32 THuiSineMappingFunction::MapValue(TReal32 aValue, TInt /*aComponent*/) const __SOFTFP
    {
    TReal32 mapped = HuiFixMath::FixToFloat(HuiFixMath::FixSin(HuiFixMath::FloatToFix(aValue)));
    return mapped * iFactor + iOffset;
    }


EXPORT_C 
THuiCosineMappingFunction::THuiCosineMappingFunction(TReal32 aFactor, TReal32 aOffset) __SOFTFP
        : iFactor(aFactor), iOffset(aOffset)
    {
    }


EXPORT_C TReal32 THuiCosineMappingFunction::MapValue(TReal32 aValue, TInt /*aComponent*/) const __SOFTFP
    { 
    TReal32 mapped = HuiFixMath::FixToFloat(HuiFixMath::FixCos(HuiFixMath::FloatToFix(aValue)));
    return mapped * iFactor + iOffset;
    }


EXPORT_C
THuiAverageMappingFunction::THuiAverageMappingFunction(MHuiMappingFunction* aFunc1, 
                                                       MHuiMappingFunction* aFunc2)
        : iFunc1(aFunc1), iFunc2(aFunc2), iFactor(0.5f),
          iWeight( 0.5f ) // Balance between the functions.
    {
    }
    

EXPORT_C TReal32 THuiAverageMappingFunction::MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP
    {
    TReal32 value = 0;
    TReal32 weight = iWeight.Now();
    
    if(iFunc1)
        {
        value += 2 * (1 - weight) * iFunc1->MapValue(aValue, aComponent);
        }
    if(iFunc2)
        {
        value += 2 * weight * iFunc2->MapValue(aValue, aComponent);
        }

    return value * iFactor.Now();
    }


EXPORT_C TBool THuiAverageMappingFunction::MappingFunctionChanged() const
    {
    return (iFunc1 && iFunc1->MappingFunctionChanged() ||
            iFunc2 && iFunc2->MappingFunctionChanged() ||
            iFactor.Changed() || iWeight.Changed());
    }


EXPORT_C void THuiAverageMappingFunction::MappingFunctionClearChanged()
    {
    if(iFunc1)
        {
        iFunc1->MappingFunctionClearChanged();
        }
    if(iFunc2)
        {
        iFunc2->MappingFunctionClearChanged();
        }
    iFactor.ClearChanged();
    iWeight.ClearChanged();  
    }
