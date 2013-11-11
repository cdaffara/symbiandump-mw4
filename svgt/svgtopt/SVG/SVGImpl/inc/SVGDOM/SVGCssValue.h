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


#ifndef __INC_MCSSVALUE__
#define __INC_MCSSVALUE__


#if !defined(__E32BASE_H__)
#include <e32base.h>
#endif



/**
 * Class description goes here.
 *
 *  @lib SVGEngine.lib
 *  @since 1.0
 */

class CCssValue:public CBase

    {
    public:

	
       virtual void	 Print() = 0;
       
       //AJD would be nice if this would have been an overloaded == operator
       virtual TBool IsEqual(CCssValue* aValue) = 0;
    };

#endif /* __INC_MCSSVALUE__ */
