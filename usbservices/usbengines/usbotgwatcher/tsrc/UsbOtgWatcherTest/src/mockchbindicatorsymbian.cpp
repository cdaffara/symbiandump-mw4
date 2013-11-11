/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
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


#include <hb/hbcore/hbindicatorsymbian.h>
#include "debug.h"

EXPORT_C CHbIndicatorSymbian* CHbIndicatorSymbian::NewL()
    {
    CHbIndicatorSymbian *me = new CHbIndicatorSymbian();
    me->ConstructL();
    return me;
    }

EXPORT_C CHbIndicatorSymbian::~CHbIndicatorSymbian()
{
    delete d;
}

/*!
    Activates an indicator of type \a aIndicatorType.
    An extra parameter can be passed along to the indicator plugin.
    Returns true, if the indicator is activated, false, if an error occurred.  

    \sa Deactivate
 */
EXPORT_C TBool CHbIndicatorSymbian::Activate(const TDesC& aIndicatorType, const CHbSymbianVariant* aParameter)
{
    LOG( (" MockCHbIndicatorSymbian::Activate"));
    return true;
}

/*!
    Deactivates an indicator of type \a indicatorType.
    An extra parameter can be passed along to the indicator plugin.
    If indicator is not currently active, does nothing and returns true.
    Returns false, if error occurred.

    \sa Activate
 */
EXPORT_C TBool CHbIndicatorSymbian::Deactivate(const TDesC& aIndicatorType, const CHbSymbianVariant* aParameter)
{
    return true;
}

/*!
    Returns the last error code. The code is cleared when any other API function than error() is called.
*/
EXPORT_C TInt CHbIndicatorSymbian::Error() const
{
    return 0;
}

EXPORT_C void CHbIndicatorSymbian::SetObserver(MHbIndicatorSymbianObserver* aObserver)
{
	
}

CHbIndicatorSymbian::CHbIndicatorSymbian()
{
}

void CHbIndicatorSymbian::ConstructL()
{
   
}