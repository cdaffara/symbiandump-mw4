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
* Description:  IKE socket definitions
*
*/


#ifndef IKESOCKETDEFS_H
#define IKESOCKETDEFS_H

namespace IkeSocket
    {
    const TInt KIkePort500( 500 );
    const TInt KIkePort4500( 4500 );
    
    enum TIkeMajorVersion
        {
        EIkeMajorV1 = 1,
        EIkeMajorV2 = 2
        };
    
    enum TIpVersion
        {
        EIPv4 = 4,
        EIPv6 = 6
        };
    }

#endif // IKESOCKETDEFS_H
