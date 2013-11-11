/*
* Copyright (c) 2008-2008 Nokia Corporation and/or its subsidiary(-ies). 
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
* Description:   Panic codes of Analog Dialer
*
*/



#ifndef ANALOGDIALER_APPLICATION_PAN
#define ANALOGDIALER_APPLICATION_PAN

/** AlfExAnalogDialer application panic codes */
enum TAlfExAnalogDialerPanics 
    {
    EAlfExAnalogDialerBasicUi = 1
    // add further panics here
    };

/**
 * Panics the current thread with the "AlfExAnalogDialer" panic category 
 * and the panic number specified by TAlfExAnalogDialerPanics
 *
 * @since S60 5.0
 * @param aReason  The panic number.
 */

inline void Panic(TAlfExAnalogDialerPanics aReason)
    {
	_LIT(applicationName,"AlfExAnalogDialer");
    User::Panic(applicationName, aReason);
    }

#endif // ANALOGDIALER_APPLICATION_PAN
