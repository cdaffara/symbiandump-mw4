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
* Description:  Configuration representation classes for Graphics Out of Memory Monitor
*
*/


#ifndef GOOMCLOSEAPPCONFIGINL_H_
#define GOOMCLOSEAPPCONFIGINL_H_

inline TUint CGOomCloseAppConfig::CalculateCloseAppPriority(const CGOomWindowGroupList& aWindowGroupList, TUint aAppIndexInWindowGroup)
    {
    // Calculating the priority for application closures is simple, we just get the base class to apply the rules
    return Priority(aWindowGroupList, aAppIndexInWindowGroup);
    }

#endif /*GOOMCLOSEAPPCONFIGINL_H_*/
