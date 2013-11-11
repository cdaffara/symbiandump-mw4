/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of "Eclipse Public License v1.0""
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description:  
* Entry library for launching Wlan Login application.
*/

#ifndef ICTSWLANLOGININTERFACE_H
#define ICTSWLANLOGININTERFACE_H

// System includes
#include <QObject>
#include <QUrl>
#include <xqappmgr.h>


// User includes
#include "ictsqtwrapper.h"

// Forward declarations

// External data types

// Constants

// Library's export macro definitions
#ifdef BUILD_ICTSWLANLOGININTERFACE_DLL
#define ICTSWLANLOGININTERFACEDLL_EXPORT Q_DECL_EXPORT
#else
#define ICTSWLANLOGININTERFACEDLL_EXPORT Q_DECL_IMPORT
#endif

// Class declaration
class ICTSWLANLOGININTERFACEDLL_EXPORT IctsWlanLoginInterface: public QObject
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

public slots:

    void handleConnectivityTestResult(IctsWrapper::ConnectivityTestResult result, QUrl redirectUrl);

signals:

    /*!
        Signal that informs about ICTS result.
        
        @param [in] ictsResult ICTS result (IctsWlanLoginInterface::ictsResultType).
    */
    void ictsResult(int ictsResult);
    
    void hotspotCase();
    
private slots:

    void handleOk(const QVariant &result);

    void handleError(int errorCode);
     
private: // functions

    void wlanLoginAppStart(int iapId, int netId, QUrl url);
    
private: // data
    
    IctsWrapper* mIctsWrapper;       //!< Icts wrapper instance
    XQApplicationManager* mAiwMgr;   //!< Application interworking manager    
    XQAiwRequest* mAsyncRequest;     //!< Pointer to async AIW Request
    int mIapId;                      //!< IAP identifier
    int mNetId;                      //!< Network identifier
        
    // Friend classes
};

#endif // ICTSWLANLOGININTERFACE_H
