/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
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


#ifndef __WebCoreKeyValuePair_h__
#define __WebCoreKeyValuePair_h__

// INCLUDES
#include <e32base.h>

// CONSTANTS

// MACROS

// DATA TYPES

// FUNCTION PROTOTYPES

// FORWARD DECLARATIONS

// CLASS DECLARATION

/**
*
*  @lib webcore.lib
*  @since 3.0
*/
class CWebCoreKeyValuePair
{
public:
    CWebCoreKeyValuePair() {}
    virtual ~CWebCoreKeyValuePair() {delete iKey; delete iValue;}
    HBufC* iKey;
    HBufC* iValue;
};

#endif

// End of File