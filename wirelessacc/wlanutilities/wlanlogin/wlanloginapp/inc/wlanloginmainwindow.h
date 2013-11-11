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
 * 
 */

#ifndef WLANLOGINMAINWINDOW_H
#define WLANLOGINMAINWINDOW_H

// System includes
#include <HbMainWindow>

// User includes

// Forward declarations

// External data types

// Constants

// Forward declarations
class WlanLoginView;
class WlanLoginApplication;

// Class declaration
class WlanLoginMainWindow : public HbMainWindow
    {
    Q_OBJECT

public:
    explicit WlanLoginMainWindow(WlanLoginApplication* appRef);    
    ~WlanLoginMainWindow();

    WlanLoginView* loginView() const;
    WlanLoginApplication* application() const;

private:
    void addLoginView();
    
    
private: //Data

    // Not owned data

    // Owned data
    WlanLoginApplication* mAppRef; //!< pointer to application instance
    WlanLoginView* mLoginView; //!< pointer to login view

    };

#endif // WLANLOGINMAINWINDOW_H
