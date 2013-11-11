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
* Description:   a special assert macros used in pkiservice.
*
*/



#ifndef PKISERVICEASSERT_H
#define PKISERVICEASSERT_H

#include "log_r6.h"

#if defined(_DEBUG)

#define PKISERVICE_ASSERT(cond) if(!(cond)){ LOG_("ASSERTION FAILED"); LOG8_1("%s, ", __FILE__); LOG8_1("%d", __LINE__); User::Invariant();}
#define PKISERVICE_INVARIANT() LOG_("ASSERTION FAILED"); LOG8_1("%s, ", __FILE__); LOG8_1("%d", __LINE__); User::Invariant()

#else

#define PKISERVICE_ASSERT(cond) 
#define PKISERVICE_INVARIANT() 

#endif


#endif //PKISERVICEASSERT_H