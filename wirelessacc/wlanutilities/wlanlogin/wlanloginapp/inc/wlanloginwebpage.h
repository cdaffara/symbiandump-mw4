/*
 * Copyright (c) 2009 Nokia Corporation and/or its subsidiary(-ies).
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
 * Description: Wlan Login web page class definition
 * 
 */

#ifndef WLANLOGINWEBPAGE_H
#define WLANLOGINWEBPAGE_H

// System includes
#include <QWebPage>

// User includes

// Forward declarations

// External data types

// Constants

// Class declaration
class WlanLoginWebPage : public QWebPage
    {
    Q_OBJECT
    
public:
    explicit WlanLoginWebPage(QObject* parent=0);
    ~WlanLoginWebPage();
      
protected:
    bool acceptNavigationRequest(QWebFrame *frame, const QNetworkRequest &request, NavigationType type);
        
signals:
    void formSubmitted();
    };

#endif // WLANLOGINWEBPAGE_H
