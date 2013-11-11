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
* Description:   Implementation of CHuiMatrixStack. CHuiMatrixStack is a stack 
*                of 2D transformation matrices.
*
*/



#include "HuiMatrixStack.h"
#include "uiacceltk/HuiPanic.h"


EXPORT_C CHuiMatrixStack::TMatrix::TMatrix()
        : iIsIdentity(ETrue)
    {
    // First column.
    iMatrix[0] = 1;
    iMatrix[1] = 0;
    iMatrix[2] = 0;

    // Second column.
    iMatrix[3] = 0;
    iMatrix[4] = 1;
    iMatrix[5] = 0;

    // Third column.
    iMatrix[6] = 0;
    iMatrix[7] = 0;
    iMatrix[8] = 1;
    }

#define HUI_MATRIX_INDEX(aRow, aColumn) (aColumn*3 + aRow)

void CHuiMatrixStack::TMatrix::Multiply(const TMatrix& aOther)
    {
    TMatrix result;

    // Clear the result.
    Mem::FillZ(result.iMatrix, sizeof(result.iMatrix));

    // It could still be an identity matrix, but probably not.
    result.iIsIdentity = EFalse;

    for(TInt i = 0; i < 3; ++i) // Row.
        {
        for(TInt j = 0; j < 3; ++j) // Column.
            {
            for(TInt k = 0; k < 3; ++k)
                {
                result.iMatrix[ HUI_MATRIX_INDEX(i, j) ] +=
                    iMatrix[ HUI_MATRIX_INDEX(i, k) ] * aOther.iMatrix[ HUI_MATRIX_INDEX(k, j) ];

                }
            }
        }

    *this = result;
    }


EXPORT_C void CHuiMatrixStack::TMatrix::Multiply(THuiRealPoint& aPoint) const
    {
    if(iIsIdentity)
        {
        // No need to do anything.
        return;
        }

    TReal32 wPoint[3] =
        {
        aPoint.iX,
        aPoint.iY,
        1.0f
        };
    TReal32 result[3] =
        {
        0, 0, 0
        };

    for(TInt i = 0; i < 3; ++i)
        {
        for(TInt j = 0; j < 3; ++j)
            {
            result[i] += wPoint[j] * iMatrix[ HUI_MATRIX_INDEX(i, j) ];
            }
        }

    if(result[2] != 0)
        {
        // Normalize.
        result[0] /= result[2];
        result[1] /= result[2];
        }
    else
        {
        // Completely zero.
        result[0] = 0;
        result[1] = 0;
        }

    aPoint.iX = result[0];
    aPoint.iY = result[1];
    }


EXPORT_C CHuiMatrixStack* CHuiMatrixStack::NewL()
    {
    CHuiMatrixStack* self = CHuiMatrixStack::NewLC();
    CleanupStack::Pop(self);
    return self;
    }

    
CHuiMatrixStack* CHuiMatrixStack::NewLC()
    {
    CHuiMatrixStack* self = new (ELeave) CHuiMatrixStack();
    CleanupStack::PushL(self);
    self->ConstructL();
    return self;
    }
    
    
CHuiMatrixStack::~CHuiMatrixStack()
    {
    iStack.Close();    
    }
    

CHuiMatrixStack::CHuiMatrixStack()
    {
    }
    
    
void CHuiMatrixStack::ConstructL()
    {
    // Append one identity matrix as the default contents of the stack.
    User::LeaveIfError( iStack.Append(TMatrix()) );
    }


EXPORT_C TInt CHuiMatrixStack::Count() const
    {
    return iStack.Count();
    }


EXPORT_C CHuiMatrixStack::TMatrix& CHuiMatrixStack::Current()
    {
    // There is always at least one matrix in the stack.
    return iStack[iStack.Count() - 1];
    }


EXPORT_C TInt CHuiMatrixStack::Push()
    {
    // Make a copy of the topmost item.
    return iStack.Append(Current());
    }


EXPORT_C void CHuiMatrixStack::Pop()
    {
    if(iStack.Count() <= 1)
        {
        // Invalid operation.
        THuiPanic::Panic(THuiPanic::EMatrixStackPopFromEmpty);
        }

    iStack.Remove(iStack.Count() - 1);
    }
      

EXPORT_C void CHuiMatrixStack::LoadIdentity()
    {
    Current() = TMatrix();
    }    


EXPORT_C void CHuiMatrixStack::Multiply(const TMatrix& aMatrix)
    {
    Current().Multiply(aMatrix);
    }
