/*
* Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies). 
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



#ifndef SENUSER_INL
#define SENUSER_INL

// INCLUDES
#include "senuser.h"

// CLASS DECLARATION

// -----------------------------------------------------------------------------
// CSenUser::UserName()
// -----------------------------------------------------------------------------
//
EXPORT_C inline TDesC8& CSenUser::UserName() const
{
        return (TDesC8&)iUserName;
}

// -----------------------------------------------------------------------------
// CSenUser::PIN()
// -----------------------------------------------------------------------------
//
EXPORT_C inline TDesC8& CSenUser::PIN() const
{
        return (TDesC8&)iPIN;
}

// -----------------------------------------------------------------------------
// CSenUser::Active()
// -----------------------------------------------------------------------------
//
EXPORT_C inline TBool CSenUser::Active() const
{
        return iActive;
}


// -----------------------------------------------------------------------------
// CSenUser::Identites()
// -----------------------------------------------------------------------------
//
EXPORT_C inline RPointerArray<CSenBaseIdentity>& CSenUser::Identites()
    {
    return iBaseIdentities;
    }


	
#endif // SENUSER_INL

