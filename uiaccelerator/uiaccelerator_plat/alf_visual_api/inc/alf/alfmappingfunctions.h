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



#ifndef M_ALFMAPPINGFUNCTIONS_H
#define M_ALFMAPPINGFUNCTIONS_H

#include <e32base.h>
#include <alf/alftimedvalue.h>

class CAlfEnv;

/**
 * Mapping function interface. 
 * Timed values use this interface for mapping
 * the return value of a timed value evaluation. Can be used to define
 * custom interpolation functions for example timed values.
 *
 *  @lib alfclient.lib
 *  @since S60 v3.2
 */
class MAlfMappingFunction
    {

public:

    /**
     * Perform the mapping. Defines a function y = f(x, mode).
     *
     * Implement this method that calculates a output value for
     * a timed value based on the input value and a component/mode
     * parameter.
     *
     * For example the aValue input could be an angle, and the aMode
     * could be a X or Y component enumeration, then the MapValue could
     * calculate return a x or y position on a curve path.
     *
     * @param aValue      Unmapped value.
     * @param aMode       Mapping mode used for mapping this value.
     *
     * @return  Mapped value.
     */
    virtual TReal32 MapValue(TReal32 aValue, TInt aMode) const __SOFTFP = 0;
    
    /**
     * Returns the mapping function identifier on the server side.
     */ 
    virtual TInt MappingFunctionIdentifier() const = 0;
    
    };
    
    
/**
 * Constant value mapping function. 
 *
 * Does not implement change flags, which means that if iValue is changed 
 * while the mapping function is in use, timed values will not notify the 
 * change.
 * Usage:
 * @code
 * //Create constant mapping function instant
 * CAlfConstantMappingFunction* imageXposMappingFun = CAlfConstantMappingFunction::NewL(*iEnv);  
 * 
 * TAlfTimedPoint timedPoint;
 * //Set mapping function on x timed value
 * timedPoint.iX.SetValueNow(0); 
 * timedPoint.iX.SetTarget( 2000 );
 * timedPoint.iX.SetMappingFunctionIdentifier( imageXposMappingFun->MappingFunctionIdentifier() );
 * 
 * //Set mapping function on Y timed value
 * timedPoint.iY.SetValueNow(0); 
 * timedPoint.iY.SetTarget(2000);
 * timedPoint.iY.SetMappingFunctionIdentifier( imageYposMappingFunction->MappingFunctionIdentifier() );
 * //Set position of image visual
 * iImageVisual->SetPos( timedPoint );   
 * @endcode
 */
class CAlfConstantMappingFunction : public CBase, public MAlfMappingFunction
    {
public:

    /**
     * Constructor.
     */
    IMPORT_C static CAlfConstantMappingFunction* NewL( 
        CAlfEnv& aEnv, 
        TReal32 aValue = 0.0 ) __SOFTFP;
    IMPORT_C ~CAlfConstantMappingFunction();
    
    /* implements MAlfMappingFunction */
    TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
    
    /* implements MAlfMappingFunction */
    TInt MappingFunctionIdentifier() const;
    
    IMPORT_C void SetValue( TReal32 aValue ) __SOFTFP;
    IMPORT_C TReal32 Value() const __SOFTFP;
    
private:

    CAlfConstantMappingFunction();
    void ConstructL( CAlfEnv& aEnv, TReal32 aValue );

private:

    struct TPrivateData;
    TPrivateData* iData;
    
    };
    
/**
 * Linear mapping function.
 *
 * Does not implement change flags, which means that if iFactor or iOffset 
 * is changed while the mapping function is in use, timed values will 
 * not notify the change.
 */
class CAlfLinearMappingFunction : public CBase, public MAlfMappingFunction
    {
public:

    /**
     * Constructor.
     */
    IMPORT_C static CAlfLinearMappingFunction* NewL( 
        CAlfEnv& aEnv, 
        TReal32 aFactor = 1.0,
        TReal32 aOffset = 0.0 ) __SOFTFP;
    IMPORT_C ~CAlfLinearMappingFunction();
    
    /* implements MAlfMappingFunction */
    TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
    
    /* implements MAlfMappingFunction */
    TInt MappingFunctionIdentifier() const;
    
    IMPORT_C void SetFactor( TReal32 aFactor ) __SOFTFP;
    IMPORT_C TReal32 Factor() const __SOFTFP;
    
    IMPORT_C void SetOffset( TReal32 aOffset ) __SOFTFP;
    IMPORT_C TReal32 Offset() const __SOFTFP;
        
private:

    CAlfLinearMappingFunction();
    void ConstructL( CAlfEnv& aEnv,TReal32 aFactor, TReal32 aOffset );
    
private:

    struct TPrivateData;
    TPrivateData* iData;
    
    };

/**
 * Sine mapping function.
 *
 * Does not implement change flags, which means that if iFactor or iOffset 
 * is changed while the mapping function is in use, timed values will 
 * not notify the change.
 */
class CAlfSineMappingFunction : public CBase, public MAlfMappingFunction
    {
public:    

    /**
     * Constructor.
     */
    IMPORT_C static CAlfSineMappingFunction* NewL( 
        CAlfEnv& aEnv, 
        TReal32 aFactor = 1.0,
        TReal32 aOffset = 0.0 ) __SOFTFP;
    IMPORT_C ~CAlfSineMappingFunction();
    
    /* implements MAlfMappingFunction */
    TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
    
    /* implements MAlfMappingFunction */
    TInt MappingFunctionIdentifier() const;
    
    IMPORT_C void SetFactor( TReal32 aFactor ) __SOFTFP;
    IMPORT_C TReal32 Factor() const __SOFTFP;
    
    IMPORT_C void SetOffset( TReal32 aOffset ) __SOFTFP;
    IMPORT_C TReal32 Offset() const __SOFTFP;

private:
    CAlfSineMappingFunction();
    void ConstructL( CAlfEnv& aEnv, TReal32 aFactor, TReal32 aOffset );

private:

    struct TPrivateData;
    TPrivateData* iData;        
    
    };
    
    
 
    
/**
 * Cosine mapping function.
 * 
 * Does not implement change flags, which means that if iFactor or iOffset 
 * is changed while the mapping function is in use, timed values will 
 * not notify the change.
 */    
class CAlfCosineMappingFunction : public CBase, public MAlfMappingFunction
    {
public:    

    /**
     * Constructor.
     */
    IMPORT_C static CAlfCosineMappingFunction* NewL( 
        CAlfEnv& aEnv, 
        TReal32 aFactor = 1.0,
        TReal32 aOffset = 0.0 ) __SOFTFP;
    IMPORT_C ~CAlfCosineMappingFunction();
    
    /* implements MAlfMappingFunction */
    TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
    
    /* implements MAlfMappingFunction */
    TInt MappingFunctionIdentifier() const;
    
    IMPORT_C void SetFactor( TReal32 aFactor ) __SOFTFP;
    IMPORT_C TReal32 Factor() const __SOFTFP;
    
    IMPORT_C void SetOffset( TReal32 aOffset ) __SOFTFP;
    IMPORT_C TReal32 Offset() const __SOFTFP;

private:
    CAlfCosineMappingFunction();
    void ConstructL( CAlfEnv& aEnv, TReal32 aFactor, TReal32 aOffset );

private:

    struct TPrivateData;
    TPrivateData* iData;        
    
    };
    
    
/**
 * Weighted average function between two other mapping functions.
 * By default calculates the average of the two functions.
 */       
class CAlfAverageMappingFunction : public CBase, public MAlfMappingFunction
    {
public:
    
    /**
     * Constructor. 
     *
     * @param aFunc1  Function 1.
     * @param aFunc2  Function 2.
     */
    IMPORT_C static CAlfAverageMappingFunction* NewL( 
        CAlfEnv& aEnv,
        MAlfMappingFunction* aFunc1 = 0, 
        MAlfMappingFunction* aFunc2 = 0 );
    
    IMPORT_C ~CAlfAverageMappingFunction();

    /* Implements MAlfMappingFunction. */
    
    TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
    
    TInt MappingFunctionIdentifier() const;
    
    IMPORT_C void SetMappingFunction1( MAlfMappingFunction* aFunction1 );
    IMPORT_C void SetMappingFunction2( MAlfMappingFunction* aFunction2 );
    IMPORT_C void SetMappingFunctions( 
        MAlfMappingFunction* aFunction1,
        MAlfMappingFunction* aFunction2 );
        
    IMPORT_C void SetWeight( const TAlfTimedValue& aWeight );
    
private:

    CAlfAverageMappingFunction();
    
    void ConstructL( 
        CAlfEnv& aEnv,
        MAlfMappingFunction* aFunc1, 
        MAlfMappingFunction* aFunc2
        );

private:
    
    struct TPrivateData;
    TPrivateData* iData;
    };


/**
 *  Mapping table function data provider, to be used with CAlfTableMappingFunction.
 *
 */
class MAlfTableMappingFunctionDataProvider
    {

public:

    /**
     * Perform the mapping. Defines a function y = f(x, mode).
     *
     * Implement this method that calculates a output value for
     * a timed value based on the input value and a component/mode
     * parameter.
     *
     * For example the aValue input could be an angle, and the aMode
     * could be a X or Y component enumeration, then the MapValue could
     * calculate return a x or y position on a curve path.
     *
     * @param aValue      Unmapped value.
     * @param aMode       Mapping mode used for mapping this value.
     *
     * @return  Mapped value.
     */
    virtual TReal32 MapValue(TReal32 aValue, TInt aMode) const __SOFTFP = 0;    
    };

/**
 *
 * Mapping table based mapping function. This can be used in some situations
 * to implement new mapping function functionality, without writing a whole new 
 * mapping function (as a server extension).
 * 
 */       
class CAlfTableMappingFunction : public CBase, public MAlfMappingFunction
    {
public:
    
    /**
     * Constructor. 
     */
    IMPORT_C static CAlfTableMappingFunction* NewL(CAlfEnv& aEnv);
    
    /**
     * Destructor. 
     */
    IMPORT_C ~CAlfTableMappingFunction();

    /**
     * Calculates mapping table for given range using given mapping data provider function. This fucntion
     * should be called at least once to initialize the mapping table values.
     *
     * @param aStart Start value for the mapping table
     * @param aEnd End value for the mapping table
     * @param aFunction Function that is used to calculate mapping table values.
     *
     */
    IMPORT_C void SetMappingTableValues(TReal32 aStart, TReal32 aEnd, MAlfTableMappingFunctionDataProvider* aFunction) __SOFTFP;

    /* Implements MAlfMappingFunction. */
    
    TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;
    
    TInt MappingFunctionIdentifier() const;
        
private:

    CAlfTableMappingFunction();
    
    void ConstructL(CAlfEnv& aEnv);

private:
    
    struct TPrivateData;
    TPrivateData* iData;
    };


#endif // M_ALFMAPPINGFUNCTIONS_H
