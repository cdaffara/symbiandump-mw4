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
@       ClearAsm - Clear a contiguous block of memory Fast!
@
@   This function writes a contiguous block of memory very fast with a single
@ color.  The size of the block must be a multiple of 16 pixels (32 bytes).
@
@       r0 = Pointer to this class (not used).
@       r1 = Address of memory block to clear.
@       r2 = Color.
@       r3 = Size of block to clear (in 16 pixel increments).

        .align  2
        .globl  ClearAsm__8CGfx2dGcPiii
@       .thumb_func
@       .code   16

ClearAsm__8CGfx2dGcPiii:
        stmfd   sp!, {r4-r9}
        mov     r0,r1                   @ Save address and size.
        mov     r9,r3
        orr     r2,r2,r2,lsl #16        @ Replicate color pattern.
        mov     r1,r2           
        mov     r3,r2          
        mov     r4,r2
        mov     r5,r2
        mov     r6,r2
        mov     r7,r2
        mov     r8,r2
loop:   stmia   r0!,{r1-r8}
        subs    r9,r9,#1
        bne     loop

        ldmfd   sp!, {r4-r9}
        bx      lr

