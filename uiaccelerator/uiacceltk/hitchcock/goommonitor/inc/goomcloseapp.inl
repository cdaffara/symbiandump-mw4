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
* Description:  Classes for executing GOOM actions (e.g. closing applications and running plugins).
*
*/


#ifndef GOOMCLOSEAPPINL_H_
#define GOOMCLOSEAPPINL_H_

inline TBool CGOomCloseApp::IsRunning()
    {
    return iAppCloserRunning;
    }

inline TUint CGOomCloseApp::WgId() const
    {
    return iWgId;
    }

#endif /*GOOMCLOSEAPPINL_H_*/
