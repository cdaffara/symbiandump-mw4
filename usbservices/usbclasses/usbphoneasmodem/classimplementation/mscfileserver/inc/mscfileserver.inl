// Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
// All rights reserved.
// This component and the accompanying materials are made available
// under the terms of "Eclipse Public License v1.0"
// which accompanies this distribution, and is available
// at the URL "http://www.eclipse.org/legal/epl-v10.html".
//
// Initial Contributors:
// Nokia Corporation - initial contribution.
//
// Contributors:
//
// Description: Implements a Symbian OS server that exposes the RUsbMassStorge API
// 
// 

#ifndef MSCFILESERVER_INL
#define MSCFILESERVER_INL


/**
 Returns a reference to the CMscFileController

 @internalTechnology
 @return	the reference to the CMscFileController
 */
inline CMscFileController& CMscFileServer::Controller() const
    {
    return *iController;
    }

/**
 Gets session count
 
 @internalTechnology
 @return iSessionCount
 */
inline TInt CMscFileServer::SessionCount() const
    {
    return iSessionCount;
    }

#endif // MSCFILESERVER_INL

