/*
* Copyright (c) 2007 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Inline methods related to client server communication
*
*/



#ifndef ALFCLIENTSERVER_H
#define ALFCLIENTSERVER_H


#include <e32std.h>
#include "alfpanicreasons.h"

/**
 * Calculates padded length of descriptor.
 *
 * @since S60 5.0
 * @param aLength length
 * @return padded length
 */
inline TInt AlfPaddedLength( TInt aLength );

/**
 * Defines to which boundary lengths should be padded.
 */
const TInt KAlfClientServerRoundToBoundary = 4;

/**
 * Size of command batch buffer used in client & server side.
 */
const TInt KAlfClientServerDefaultCommandBatchBufferSize = 512;

/**
 * Panics client with specified reason.
 * @param aMessage message to panic.
 * @param aReason reason to use.
 */
inline void AlfPanicClient( const RMessage2& aMessage, TAlfPanicSrv aReason );

#include "alfclientserver.inl"

#endif // ALFCLIENTSERVER_H
