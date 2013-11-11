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
* Private implementation of wrapper for Symbian Esock library.
*/

#ifndef WLANQTUTILSESOCKWRAPPER_SYMBIAN_H
#define WLANQTUTILSESOCKWRAPPER_SYMBIAN_H

// System includes

#include <es_sock.h>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration

class WlanQtUtilsEsockWrapperPrivate: public CActive
{

public:
    
    // Data types

    WlanQtUtilsEsockWrapperPrivate(WlanQtUtilsEsockWrapper *wrapper);
    
    ~WlanQtUtilsEsockWrapperPrivate();

    void ConnectIap(int iapId);
    
    void DisconnectIap();

protected:
    
private:
    
    void RunL();
    
    void DoCancel();
        
private: // data

    // Owned data
    
    RSocketServ iSocketServer; //!< Socket server handle
    RConnection iConnection;   //!< Connection handle
    TBool iConnectionActive;   //!< Do we have an active connection handle?

    // Not owned data
    
    //! Public implementation to report progress to
    WlanQtUtilsEsockWrapper *q_ptr;

    // Friend classes
};

#endif // WLANQTUTILSESOCKWRAPPER_SYMBIAN_H
