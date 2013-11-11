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
* Description:   
*
*/


#ifndef __ALFEXSTICKERS_PAN__
#define __ALFEXSTICKERS_PAN__

/** AlfExStickers application panic codes */
enum TAlfExStickersPanics 
    {
    EAlfExStickersBasicUi = 1
    // add further panics here
    };

inline void Panic(TAlfExStickersPanics aReason)
    {
	_LIT(applicationName,"AlfExStickers");
    User::Panic(applicationName, aReason);
    }

#endif // __ALFEXSTICKERS_PAN__
