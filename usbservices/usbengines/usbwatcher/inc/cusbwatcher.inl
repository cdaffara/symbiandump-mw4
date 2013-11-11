/*
* Copyright (c) 2006-2009 Nokia Corporation and/or its subsidiary(-ies).
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
* Description:  Inline methods for CUsbWatcher class
*
*/


#include <UsbWatcherInternalCRKeys.h>

// ----------------------------------------------------------------------------
// Return personality array.
// ----------------------------------------------------------------------------
//
inline const RArray<TUsbSupportedPersonalityInf>& CUsbWatcher::Personalities() 
        const
    {
    return iSupportedPersonalities;
    }

// End of file
