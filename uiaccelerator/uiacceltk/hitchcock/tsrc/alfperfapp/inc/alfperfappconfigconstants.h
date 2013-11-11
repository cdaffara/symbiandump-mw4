/*
* Copyright (c) 2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:  alfperfapp Configurable constants.
*
*/


#ifndef AlFPERFAPPCONFIGCONSTANTS_H_
#define AlFPERFAPPCONFIGCONSTANTS_H_

//Image and Text test cases
//Number of Text and Image Visuals in ManyVisuals Test cases of text and Image Visuals
const TInt KVisualCout = 500;
//Portion of screen to be animated in ImageAnimateFull test case.
// Value 1 - Full area is animated
// Value 0.5 - Half area is animated
// Value 0.25 - One fourth area is animated ... so on
const TReal KRelativeAnimatedArea = 1;
//Image and text visual test cases position, size and opacity are timed values.
//That animation speed could be controled with this value.
const TInt KVisualAnimSpeed = 500;


//Scroll Test cases
//Cycle Length for Incremental scroll test case
// ( this is basically transition time for setting viewport position )
const TInt KCycleLenghtIncremental = 400;
//Cycle Length for ScrollContinuous test case
// ( this is basically transition time for setting viewport position )
const TInt KCycleLenghtContinuous = 5000;
//Number times scrolling cycle is executed
const TInt KNumberOfRepeats = 5;


#endif /* AlFPERFAPPCONFIGCONSTANTS_H_ */
