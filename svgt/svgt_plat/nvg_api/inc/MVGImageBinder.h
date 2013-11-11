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
* Description:  NVG Decoder source file
*
*/



#ifndef MVGIMAGEBINDER_H_
#define MVGIMAGEBINDER_H_

#include <e32base.h>

class MVGImageBinder
{
public:
	virtual ~MVGImageBinder(){}
	
	virtual TInt BindClientBuffer(TUint aBuffer) = 0;
	virtual TInt UnBindClientBuffer() = 0;
};

#endif
