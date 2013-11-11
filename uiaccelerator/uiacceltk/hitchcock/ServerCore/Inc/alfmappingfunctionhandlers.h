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



#ifndef T_ALFMAPPINGFUNCTIONHANDLERS_H
#define T_ALFMAPPINGFUNCTIONHANDLERS_H

#include "alf/alfextensionfactory.h"
#include <alf/alfconstants.h>
#include <uiacceltk/HuiMappingFunctions.h>

class CHuiCurvePath;

/**
 *  Average mapping function
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(TAlfAverageMappingFunctionHandler): public MAlfExtension
    {
public:  

    TAlfAverageMappingFunctionHandler(
        MAlfInterfaceProvider& aResolver, 
        MHuiMappingFunction* aFunc1 = 0, 
        MHuiMappingFunction* aFunc2 = 0 );

    void Release();
    TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

private:
    THuiAverageMappingFunction iAverageMappingFunction;
    MAlfInterfaceProvider& iResolver;
    };
    
/**
 *  Constant mapping function
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(TAlfConstantMappingFunctionHandler): public MAlfExtension
    {
public:  

    TAlfConstantMappingFunctionHandler(
        MAlfInterfaceProvider& aResolver, 
        TReal32 aValue );

    void Release();
    TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

private:
    THuiConstantMappingFunction iConstantMappingFunction;
    MAlfInterfaceProvider& iResolver;
    };
    
/**
 *  Linear mapping function
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(TAlfLinearMappingFunctionHandler): public MAlfExtension
    {
public:  

    TAlfLinearMappingFunctionHandler(
        MAlfInterfaceProvider& aResolver, 
        TReal32 aFactor = 1.0,
        TReal32 aOffset = 0.0  );

    void Release();
    TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

private:
    THuiLinearMappingFunction iLinearMappingFunction;
    MAlfInterfaceProvider& iResolver;
    };
    
/**
 *  Sine mapping function
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(TAlfSineMappingFunctionHandler): public MAlfExtension
    {
public:  

    TAlfSineMappingFunctionHandler(
        MAlfInterfaceProvider& aResolver, 
        TReal32 aFactor = 1.0,
        TReal32 aOffset = 0.0 );

    void Release();
    TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

private:
    THuiSineMappingFunction iSineMappingFunction;
    MAlfInterfaceProvider& iResolver;
    };
    
/**
 *  Cosine mapping function
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(TAlfCosineMappingFunctionHandler): public MAlfExtension
    {
public:  

    TAlfCosineMappingFunctionHandler(
        MAlfInterfaceProvider& aResolver, 
        TReal32 aFactor = 1.0,
        TReal32 aOffset = 0.0 );

    void Release();
    TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

private:
    THuiCosineMappingFunction iCosineMappingFunction;
    MAlfInterfaceProvider& iResolver;
    };
  
/**
 *  Curve path
 *
 *  @since S60 v3.2
 */  
NONSHARABLE_CLASS(CAlfCurvePathHandler): public CBase, public MAlfExtension
    {
public:  
    
    static CAlfCurvePathHandler* NewL(  
        MAlfInterfaceProvider& aResolver,
        CHuiCurvePath* aCurvePath = 0 );
    ~CAlfCurvePathHandler();

    void Release();
    TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

private:
    CAlfCurvePathHandler( MAlfInterfaceProvider& aResolver );
    void ConstructL(CHuiCurvePath* aCurvePath);

private:
    MAlfInterfaceProvider& iResolver;
    CHuiCurvePath* iCurvePath;
    TBool iOwnCurvePath;
    };

/**
 *  Table mapping function
 *
 *  @since S60 v3.2
 */
NONSHARABLE_CLASS(TAlfTableMappingFunction): public MHuiMappingFunction
    {
public:
    
    /**
     * Constructor.
     */
    TAlfTableMappingFunction();

    /* implements MHuiMappingFunction */
    TReal32 MapValue(TReal32 aValue, TInt aComponent) const __SOFTFP;

public:

    TAlfTableMappingFunctionParams iParams;
    };


NONSHARABLE_CLASS(TAlfTableMappingFunctionHandler): public MAlfExtension
    {
public:  

    TAlfTableMappingFunctionHandler(MAlfInterfaceProvider& aResolver);

    void Release();
    TAny* GetInterface(const THuiInterfaceSupport& aInterface);
    void HandleCmdL(TInt aCommandId, const TDesC8& aInputBuffer, TDes8& aResponse);

private:
    TAlfTableMappingFunction iMappingFunction;
    MAlfInterfaceProvider& iResolver;
    };



#endif // T_ALFMAPPINGFUNCTIONHANDLERS_H
