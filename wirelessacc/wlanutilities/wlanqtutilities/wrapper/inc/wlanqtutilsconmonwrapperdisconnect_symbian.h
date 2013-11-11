/*
* Copyright (c) 2009-2010 Nokia Corporation and/or its subsidiary(-ies).
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
* Private implementation of wrapper for Symbian Connection Monitor
* library's connection disconnect interface.
*/

#ifndef WLANQTUTILSCONMONWRAPPERDISCONNECT_SYMBIAN_H
#define WLANQTUTILSCONMONWRAPPERDISCONNECT_SYMBIAN_H

// System includes

#include <rconnmon.h>

// User includes

// Forward declarations

class WlanQtUtilsConMonWrapper;

// External data types

// Constants

// Class declaration

class WlanQtUtilsConMonWrapperDisconnect
{
public:

    // Data types

    WlanQtUtilsConMonWrapperDisconnect(WlanQtUtilsConMonWrapper *wrapper);

    ~WlanQtUtilsConMonWrapperDisconnect();

    void DisconnectConnection(int iapId);

protected:

private:

private: // data
    
    // Owned data

    RConnectionMonitor iMonitor; //!< Handle to Connection Monitor Server
    
    // Not owned data

    //! Public implementation to report progress to
    WlanQtUtilsConMonWrapper *q_ptr;

    // Friend classes
};

#endif // WLANQTUTILSCONMONWRAPPERDISCONNECT_SYMBIAN_H
