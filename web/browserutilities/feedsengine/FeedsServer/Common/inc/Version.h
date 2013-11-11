/*
* Copyright (c) 2005 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the License "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  Feeds version definition
*
*/


#ifndef VERSION_H
#define VERSION_H

#define KName (_L("Feeds Client"))

#if defined (__SERIES60_30__)
    #define KVersion (_L("3.0"))
#elif defined(__SERIES60_28__)
    #define KVersion (_L("2.8"))
#elif defined (__SERIES60_27__)
    #define KVersion (_L("2.7"))
#elif defined (__SERIES60_26__)
    #define KVersion (_L("2.6"))
#else
    #define KVersion (_L("2.x"))
#endif

#define KBuild (_L("0529"))


#endif VERSION_H
//end of file
