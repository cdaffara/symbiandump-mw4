/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description: Header declaration
*
*/








#ifndef M_SEN_FILTER_ACTION_H
#define M_SEN_FILTER_ACTION_H

// INCLUDES

#include <e32base.h>

#include "SenBaseFragment.h"
#include <SenXmlElement.h> // check if this include is needed(?)

// CONSTANTS

// FORWARD DECLARATIONS

// CLASS DEFINITIONS

/**
 * @author lewontin
 *
 */
class MSenFilterAction 
{

public:

    virtual void PerformL(const TDesC8& aEvent, 
                          const RAttributeArray& aAttributes) = 0;
};

#endif // M_SEN_FILTER_ACTION_H
