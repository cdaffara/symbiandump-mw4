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
* This is a source file for esock library stub functions for emulator.
*/

#include <es_sock.h>

// ----------------------------------------------------
// Stub functions for esock library, used in emulator
// ----------------------------------------------------

/**
 * Return always success in emulator.
 */
void RConnection::Start(class TConnPref &, class TRequestStatus &aStatus)
{
    User::RequestComplete(&aStatus, KErrNone);
}
