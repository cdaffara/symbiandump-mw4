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
* Description:   General-purpose mapping functions.
*
*/


 
#ifndef __HUIMAPPINGFUNCTIONS_H__
#define __HUIMAPPINGFUNCTIONS_H__


#include <uiacceltk/HuiTimedValue.h>

/**
* The parameters of each mapping function are declared and 
* accessed as public for convenience.  To preserve binary 
* compatiblity, avoid making changes to these classes.
*/

/**
 * Constant value mapping function. 
 *
 * Does not implement change flags, which means that if iValue is changed 
 * while the mapping function is in use, timed values will not notify the 
 * change.
 */
class THuiConstantMappingFunction : public MHuiMappingFunction
    {
public:

    /**
     * Constructor.
     */
    IMPORT_C THuiConstantMappingFunction(TReal32 aValue = 0.0) __SOFTFP;
    
    /* implements MHuiMappingFunction */
    IMPORT_C TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
    

public:

    /* Parameters. */
    
    TReal32 iValue;
    
    };


/**
 * Linear mapping function.
 *
 * Does not implement change flags, which means that if iFactor or iOffset 
 * is changed while the mapping function is in use, timed values will 
 * not notify the change.
 */
class THuiLinearMappingFunction : public MHuiMappingFunction
    {
public:

    /**
     * Constructor.
     */
    IMPORT_C THuiLinearMappingFunction(TReal32 aFactor = 1.0, TReal32 aOffset = 0.0) __SOFTFP;
    
    /* implements MHuiMappingFunction */
    IMPORT_C TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
        
    
public:

    /* Parameters. */
    
    /** Multiply by a factor. */
    TReal32 iFactor;
    
    /** Apply offset after multiplication. */
    TReal32 iOffset;    
    
    };


/**
 * Sine mapping function.
 *
 * Does not implement change flags, which means that if iFactor or iOffset 
 * is changed while the mapping function is in use, timed values will 
 * not notify the change.
 */
class THuiSineMappingFunction : public MHuiMappingFunction
    {
public:    

    /**
     * Constructor.
     */
    IMPORT_C THuiSineMappingFunction(TReal32 aFactor = 1.0, TReal32 aOffset = 0.0) __SOFTFP;

    /* implements MHuiMappingFunction */
    IMPORT_C TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;


public:

    /* Parameters. */
    
    TReal32 iFactor;
    
    TReal32 iOffset;            
    
    };
    
    
/**
 * Cosine mapping function.
 * 
 * Does not implement change flags, which means that if iFactor or iOffset 
 * is changed while the mapping function is in use, timed values will 
 * not notify the change.
 */    
class THuiCosineMappingFunction : public MHuiMappingFunction
    {
public:
    
    /**
     * Constructor.
     */
    IMPORT_C THuiCosineMappingFunction(TReal32 aFactor = 1.0, TReal32 aOffset = 0.0) __SOFTFP;

    /* implements MHuiMappingFunction */
    IMPORT_C TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
    

public:    
        
    /* Parameters. */
    
    TReal32 iFactor;
    
    TReal32 iOffset;            
        
    };
    
    
/**
 * Weighted average function between two other mapping functions.
 * By default calculates the average of the two functions.
 */    
class THuiAverageMappingFunction : public MHuiMappingFunction
    {
public:
    
    /**
     * Constructor. 
     *
     * @param aFunc1  Function 1.
     * @param aFunc2  Function 2.
     */
    IMPORT_C THuiAverageMappingFunction(MHuiMappingFunction* aFunc1 = 0, MHuiMappingFunction* aFunc2 = 0);


    /* Implements MHuiMappingFunction. */
    
    IMPORT_C TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
    
    IMPORT_C TBool MappingFunctionChanged() const;
    
    IMPORT_C void MappingFunctionClearChanged();


public:

    /* Parameters. */
    
    /** First mapping function to average with. */
    MHuiMappingFunction* iFunc1;
    
    /** Second mapping function to average with. */    
    MHuiMappingFunction* iFunc2;
            
    /** Defaults to 0.5. Applied to the sum of the functions 1 and 2. */            
    THuiTimedValue iFactor;                
            
    /** Weight. 0.0 means function 2 does not contribute to the result, 1.0 
        means that function 1 does not contribute to the result. The default
        is 0.5, which means both functions contribute equally. */
    THuiTimedValue iWeight;        
            
    };


#endif
