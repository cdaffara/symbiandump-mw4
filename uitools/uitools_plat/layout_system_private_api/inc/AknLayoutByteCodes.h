/*
* Copyright (c) 2002-2004 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  
*
*/


#ifndef AKNLAYOUTBYTECODES_H
#define AKNLAYOUTBYTECODES_H

// the macro AKNLAYOUT_DEFINE_BYTECODE must come from any component that uses this header
AKNLAYOUT_DEFINE_BYTECODE(KByteWord, 255)	// Treat the following 2 bytes as a signed 16-bit value
AKNLAYOUT_DEFINE_BYTECODE(KByteEmpty, 254)	// The empty value.
AKNLAYOUT_DEFINE_BYTECODE(KByteP1, 253)		// Use the next byte as an 8-bit signed offset from the parent value
AKNLAYOUT_DEFINE_BYTECODE(KByteP2, 252)		// Use the next 2 bytes as a 16-bit signed offset from the parent value
AKNLAYOUT_DEFINE_BYTECODE(KByteLong, 251)	// Treat the following 4 bytes as a signed 32-bit value

#define KMaxSingleByteValue 250             // This value must be lower than the lowest ByteCode.
#define KMaxDoubleByteValue 65535             // This value must be lower than the lowest ByteCode.
#define KMinSingleByteParentRelativeValue -128
#define KMaxSingleByteParentRelativeValue 127


#endif
