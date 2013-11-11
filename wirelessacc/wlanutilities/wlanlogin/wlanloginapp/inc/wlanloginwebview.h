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
* Description: Wlan Login web view class definition
* 
*/

#ifndef WLANLOGINWEBVIEW_H
#define WLANLOGINWEBVIEW_H

// System includes
#include <QGraphicsWebView>

// User includes

// Forward declarations
class WlanLoginWebPage;

// External data types

// Constants

// Class declaration
class WlanLoginWebView: public QGraphicsWebView 
{
    Q_OBJECT

public:
    WlanLoginWebView();
    ~WlanLoginWebView();

protected:
    bool eventFilter(QObject *, QEvent *event);
    
private: // Data
    
    // Not owned data

    // Owned data
    WlanLoginWebPage* mWebPage; //!< Pointer to subclassed QWebPage
    bool mMousePressed; //!< Variable to hold whether mouse(finger) is pressed down
};

#endif //WLANLOGINWEBVIEW_H
