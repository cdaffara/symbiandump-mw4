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
* WLAN Sniffer main window. 
*/

#ifndef WLANSNIFFERMAINWINDOW_H
#define WLANSNIFFERMAINWINDOW_H

// System includes

#include <QObject>
#include <QString>
#include <HbMainWindow>
#include <QSharedPointer>

// User includes

#include "wlansniffer.h"

// Forward declarations

class WlanWizard;
class WlanSnifferListView;
class WlanQtUtilsAp;
class WlanSnifferEngine;

// External data types

// Constants

// Class declaration

class WlanSnifferMainWindow : public HbMainWindow
{
    Q_OBJECT

public:

    // Data types

    explicit WlanSnifferMainWindow(WlanSnifferEngine *engine);
    
    ~WlanSnifferMainWindow();

signals:

public slots:

    void toListView(const QString &title);

protected:

protected slots:

private:

    Q_DISABLE_COPY(WlanSnifferMainWindow)

    void addListView();
    
private slots:

    void startWlanWizard(const WlanQtUtilsAp *ap);
    
    void handleWlanWizardComplete(int iapId, bool connected);
    
    void handleWlanWizardCancelled();

private: // data

    // Owned data

    QSharedPointer<WlanWizard> mWizard; //!< WLAN Wizard object
    WlanSnifferListView *mListView;     //!< List view object

    // Not owned data

    WlanSnifferEngine *mEngine;         //!< WLAN Sniffer engine

    // Friend classes
};

#endif // WLANSNIFFERMAINWINDOW_H
