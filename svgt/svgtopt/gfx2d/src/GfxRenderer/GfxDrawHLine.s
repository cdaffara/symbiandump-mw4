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
@   DrawHLine - Draw a horizontal line Fast!
@
@       Draws a horizontal line using a very fast inner loop which writes pixels
@   in pairs.  If either the first and/or last pixels are not aligned to a
@   4 byte boundary, they are masked into the frame buffer appropriately.
@
@       Entry Parameters:
@
@       r0 = pBuffer.
@       r1 = Color (High 16 bits must be zero).
@       r2 = LeftX.
@       r3 = Width (Can be zero).

        .align  2
        .globl  DrawHLine__20TGfxPolygonRendererPPUsUlii
        .thumb_func
        .code   16

DrawHLine__20TGfxPolygonRendererPPUsUlii:

        push    {r4}
        mov     r4, #3          @ Round pointer to 4 byte boundary.
        bic     r0, r0, r4
        lsl     r4, r1, #16
        lsr     r2, r2, #1      @ First pixel aligned to 4 byte boundary?
        bcc     skip                    @ Yes, skip to loop.

        sub     r3, r3, #1              @ No, decrement count.
        bcc     exit            @ Width was 0!

        ldrh    r2, [r0]        @ Draw first pixel with masking.
        orr     r2, r2, r4
        stmia   r0!, {r2}
skip:   sub     r3, r3, #1      @ Two or more pixels remaining?
        bls     last                    @ No, skip to last pixel.

        orr     r4, r4, r1              @ Yes, combine two pixel patterns.
loop:   stmia   r0!, {r4}       @ Loop to draw aligned pixel pairs.
        sub     r3, r3, #2
        bhi     loop                    @ Two or more pixels remain.

last:   bcc     exit            @ Last pixel remaining?

        ldrh    r4, [r0, #2]            @ Yes, draw last pixel with masking.
        lsl     r4, r4, #16
        orr     r4, r4, r1
        str     r4, [r0]
exit:   pop     {r4}
        bx      lr

