/*
* Copyright (c) 2003 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  SVG Implementation header file
 *
*/


#ifndef __MEMORYMANAGER_H__
#define __MEMORYMANAGER_H__

#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif

#include "GfxFloatFixPt.h"

//forward declerations
class CCssValue;

class CPaintCssValueImpl;
class CFloatCssValueImpl;
class CIntCssValueImpl;
class CStrCssValueImpl;
class CVectorCssValueImpl;
class CClrCssValueImpl;

class CSvgElementImpl;

class CSvgMemoryManager : public CBase
    {
    public:

   static CSvgMemoryManager* NewL();

   static CSvgMemoryManager* NewLC();

   CSvgMemoryManager();

   ~CSvgMemoryManager();

    void    ConstructL();

    void    Print();

    //this really shouldnt require the CSvgElementImpl to be passed to it
        //but our paint class is so messed up
        CCssValue* CloneCssValueL(CCssValue* aValue);

//----------------------------------------------------------
//              CPaintCssValueImpl
//----------------------------------------------------------
    public:
        CPaintCssValueImpl* GetCssPaintObjectL( CSvgElementImpl* aElementImpl = NULL );
        CPaintCssValueImpl* GetCssPaintObjectL( const TDesC& aValue, CSvgElementImpl* aElementImpl );

    private:
        void    AllocateCssPaintBlockL( );

        //each of the pointers in this contains a block of #granularity Paints
        RPointerArray<CPaintCssValueImpl> iCPaintCssValueImplArrays;

        CPaintCssValueImpl* iCPaintCssValueImplArray;
        TInt iCurrentPaintObjectIndex;
        static const TUint KCssPaintBlockSize;

//----------------------------------------------------------
//              CFloatCssValueImpl
//----------------------------------------------------------
    public:
        CFloatCssValueImpl* GetCssFloatObjectL( float aFloat = 0.0 );
        CFloatCssValueImpl* GetCssFloatObjectL( CFloatCssValueImpl* aFloatValue );
        CFloatCssValueImpl* GetCssFloatObjectL( const TDesC& aValueString );

    private:
        void    AllocateCssFloatBlockL( );

        //each of the pointers in this contains a block of #granularity Floats
        RPointerArray<CFloatCssValueImpl> iCFloatCssValueImplArrays;

        CFloatCssValueImpl* iCFloatCssValueImplArray;
        TInt iCurrentFloatObjectIndex;
        static const TUint KCssFloatBlockSize;

//----------------------------------------------------------
//              CClrCssValueImpl
//----------------------------------------------------------
    public:
    CClrCssValueImpl* GetCssClrObjectL(  );
    CClrCssValueImpl* GetCssClrObjectL( const TDesC& aValueString );

    private:
    void    AllocateCssClrBlockL( );

    //each of the pointers in this contains a block of #granularity Clrs
    RPointerArray<CClrCssValueImpl> iCClrCssValueImplArrays;

        CClrCssValueImpl* iCClrCssValueImplArray;
        TInt iCurrentClrObjectIndex;
        static const TUint KCssClrBlockSize;

//----------------------------------------------------------
//              CIntCssValueImpl
//----------------------------------------------------------
    public:
    CIntCssValueImpl* GetCssIntObjectL( TInt aInitValue = 0 );
    CIntCssValueImpl* GetCssIntObjectL( const TDesC& aValueString );
    CIntCssValueImpl* GetCssIntObjectL( CIntCssValueImpl* aIntValue );

    private:
    void    AllocateCssIntBlockL( );

        //each of the pointers in this contains a block of #granularity Ints
        RPointerArray<CIntCssValueImpl> iCIntCssValueImplArrays;

        CIntCssValueImpl* iCIntCssValueImplArray;
        TInt iCurrentIntObjectIndex;
        static const TUint KCssIntBlockSize;

//----------------------------------------------------------
//              CStrCssValueImpl
//----------------------------------------------------------
    public:
    CStrCssValueImpl* GetCssStrObjectL( const TDesC& aValueString );

    private:
    void    AllocateCssStrBlockL( );

    //each of the pointers in this contains a block of #granularity Strs
    RPointerArray<CStrCssValueImpl> iCStrCssValueImplArrays;

    CStrCssValueImpl* iCStrCssValueImplArray;
        TInt iCurrentStrObjectIndex;
        static const TUint KCssStrBlockSize;

//----------------------------------------------------------
//              CVectorCssValueImpl
//----------------------------------------------------------
    public:
    CVectorCssValueImpl* GetCssVectorObjectL( const TDesC& aValueString );
    CVectorCssValueImpl* GetCssVectorObjectL( CArrayFix<TFloatFixPt>* aValue );

    private:
        void    AllocateCssVectorBlockL( );

        //each of the pointers in this contains a block of #granularity Vector
        RPointerArray<CVectorCssValueImpl> iCVectorCssValueImplArrays;

        CVectorCssValueImpl* iCVectorCssValueImplArray;
        TInt iCurrentVectorObjectIndex;
        static const TUint KCssVectorBlockSize;


};

#endif
