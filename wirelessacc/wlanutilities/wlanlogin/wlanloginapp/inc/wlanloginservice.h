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
* Wlan login QtHighway Service.
*/

#ifndef WLANLOGINSERVICE_H
#define WLANLOGINSERVICE_H

// System includes
#include <xqserviceprovider.h>
#include <QUrl>
// User includes

// Forward declarations

// External data types

// Constants

// Class declaration
class WlanLoginService: public XQServiceProvider
{
    Q_OBJECT

public:
    // Data types
    
    /*!
        Enumeration for WLAN Login application service request return values
    */ 
    enum WlanLoginStatus {
        WlanLoginStatusNone = 0, //!< No status   
        WlanLoginStatusCancel, //!< User cancel
        WlanLoginStatusNext, //!< Proceed to next phase
        WlanLoginStatusConnectionDropped, //!< WLAN connection dropped during login
        WlanLoginStatusFailed //!< Login process failed
    };
    
    explicit WlanLoginService(QObject *parent = 0);
    ~WlanLoginService();
    
signals:

     void startLogin(int iapId, int netId, QUrl url);
     void stopLogin();
     
public slots:

    void start(int iapId, int netId, QUrl url);
    void stop();
    void completeServiceRequest(int status);
    
private:

    QVariant mReturnValue; //!< Return value deliverd to back to entry
    int mAsyncRequestIndex; //!< Service request ID    
};

#endif //WLANLOGINSERVICE_H
