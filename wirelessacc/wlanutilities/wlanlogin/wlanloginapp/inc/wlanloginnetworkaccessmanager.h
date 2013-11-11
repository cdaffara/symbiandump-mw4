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
* Engine of Wlan Login application.
*/

#ifndef WLANLOGINNETWORKACCESSMANAGER_H
#define WLANLOGINNETWORKACCESSMANAGER_H

// System includes
#include <QNetworkAccessManager>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration
class WlanLoginNetworkAccessManager : public QNetworkAccessManager
{
    Q_OBJECT

public:
    explicit WlanLoginNetworkAccessManager(QObject *parent = 0);
    ~WlanLoginNetworkAccessManager();
    
private slots:
    void handleSslErrors(QNetworkReply *reply, const QList<QSslError> &errors);
    void handleNetworkRequestFinished(QNetworkReply* reply);
};

#endif // WLANLOGINNETWORKACCESSMANAGER_H
