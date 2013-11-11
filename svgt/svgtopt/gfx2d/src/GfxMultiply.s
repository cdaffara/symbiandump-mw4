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
* Description:  Graphics Extension Library source file
 *
*/



@ 
@   GfxMultiplyAsm - Multiply a 16:16 fixed point factor into iValue.
@
@       This function multiplies a 16:16 fixed point factor into iValue using
@   an ARM 64 bit multiply instruction.  This is much faster than using the
@   Symbian TInt64 class.  Note that iValue MUST be the first data member of
@   this class.
@
@       r0 = Pointer to iValue (First data item in TFixPt class).
@       r1 = Second factor to multiply.

        .align  2
        .globl  GfxMultiplyAsm__6TFixPti
        .thumb_func
        .code   16

GfxMultiplyAsm__6TFixPti:

        add     r2, pc, #4            @ Switch from Thumb to ARM execution.
        bx      r2
        nop

        .align  2
        .code   32

        ldr     r2, [r0]              @ r2 = *this, r1 = aVal.iValue.
        smull   r3, r2, r1, r2        @ r2:r3 = iValue * aVal.iValue.
        mov     r2, r2, lsl #16
        orr     r2, r2, r3, lsr #16   @ Extract middle 32 bits from 64 bit product.
        str     r2, [r0]              @ Store into iValue.
        bx      lr

