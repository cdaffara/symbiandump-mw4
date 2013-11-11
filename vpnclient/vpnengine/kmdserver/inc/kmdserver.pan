/*
* Copyright (c) 2008-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  KMD server panic definitions
*
*/


#ifndef KMDSERVER_PAN
#define KMDSERVER_PAN

_LIT( KKmdPanicCategory, "KMDServerPanic" );

enum TKmdServerPanics
    {
    EKmdPanicIllegalCommand = 0,
    EKmdPanicRequestAlreadyPending    
    };

#endif // KMDSERVER_PAN
