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
* Description:  Classes for executing Graphics OOM actions (e.g. closing applications and running plugins).
*
*/


#ifndef GOOMACTIONLISTINL_H_
#define GOOMACTIONLISTINL_H_

template <class T>
inline TInt CGOomPluginList<T>::Count()
    {
    return iPlugins.Count();
    }
    
template <class T>
inline T& CGOomPluginList<T>::Implementation(TInt aIndex)
    {
    return *(iPlugins[aIndex].iImpl);
    }

template <class T>
inline TInt32 CGOomPluginList<T>::Uid(TInt aIndex)
    {
    return iPlugins[aIndex].iUid;
    }
    
inline void CGOomActionList::SetCurrentTarget(TUint aCurrentTarget)
    {
    iCurrentTarget = aCurrentTarget;
    }


#endif /*GOOMACTIONLISTINL_H_*/
