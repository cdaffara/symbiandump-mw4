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
* ICTS Wlan Login Interface stubbing for WLAN Qt Utilities.
*/

#ifndef ICTSWLANLOGININTERFACE_H
#define ICTSWLANLOGININTERFACE_H

// System includes
#include <QObject>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration
class IctsWlanLoginInterface: public QObject
{
    Q_OBJECT

public:
    
    // Data types
    
    /*!
        Internet connectivity test result type
    */  
    enum ictsResultType {
        IctsPassed = 0,     //!< Internet connectivity test passed
        IctsHotspotPassed,  //!< Internet connectivity test passed against hotspot access point
        IctsCanceled,       //!< Internet connectivity test was cancelled due to user action
        IctsFailed          //!< internet connectivity test failed
    };
    
    IctsWlanLoginInterface(QObject *parent);

    ~IctsWlanLoginInterface();

public:
     
    void start(int iapId, int netId);

    void stop();

signals:

    void ictsResult(int ictsResult);
    
    void hotspotCase();
};

#endif // ICTSWLANLOGININTERFACE_H
