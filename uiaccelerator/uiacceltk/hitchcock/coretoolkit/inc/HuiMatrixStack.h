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
* Description:   Definition of CHuiMatrixStack. CHuiMatrixStack is a stack of 
*                2D transformation matrices.
*
*/



#ifndef __HUIMATRIXSTACK_H__
#define __HUIMATRIXSTACK_H__


#include <e32base.h>
#include "uiacceltk/HuiRealPoint.h"


/**
 * Stack of 2D transformation matrices.
 */
NONSHARABLE_CLASS(CHuiMatrixStack) : public CBase
    {
public:

    /** 2D transformation matrix. */
    struct TMatrix
        {   
    public:        
        TReal32 iMatrix[9];       

        /** Matrix is an identity matrix. Multiplication can be skipped. */
        TBool iIsIdentity;

    public:
        /** Constructor that initializes the matrix as an identity matrix. */
        IMPORT_C TMatrix();

        /**
         * Multiplies this matrix with another one. Result is stored in this
         * matrix.
         *
         * @param aOther  Matrix to multiply with.
         */
        void Multiply(const TMatrix& aOther);

        /**
         * Multiplies a point with the matrix.
         *
         * @param aPoint  2D point. Modified.
         */
        IMPORT_C void Multiply(THuiRealPoint& aPoint) const;
        };
    
    
public:

    /* Constructors and destructor. */
    
    IMPORT_C static CHuiMatrixStack* NewL();
    
    static CHuiMatrixStack* NewLC();
    
    
    /**
     * Destructor.
     */
    virtual ~CHuiMatrixStack();
    
    
    /* Methods. */
    
    IMPORT_C TInt Count() const;

    IMPORT_C TMatrix& Current();

    /**
     * @return  Error code.
     */
    IMPORT_C TInt Push();

    IMPORT_C void Pop();
    
    IMPORT_C void LoadIdentity();
    
    IMPORT_C void Multiply(const TMatrix& aMatrix);    
    
    
protected:

    /* Constructors. */
    
    CHuiMatrixStack();
    
    void ConstructL();    
        
    
private:

    // Typedef for convenience.
    typedef RArray<TMatrix> RMatrixStack;
            
    RMatrixStack iStack;            
    };


#endif // __HUIMATRIXSTACK_H__
