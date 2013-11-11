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
* WLAN Wizard API.
*/

#ifndef WLANWIZARD_H
#define WLANWIZARD_H

// System includes

#include <QObject>
#include <QString>
#include <QByteArray>

// User includes

// Forward declarations

class HbMainWindow;
class WlanWizardPrivate;
class TestWlanWizardContext;

// External data types

// Constants

#ifdef BUILD_WLAN_WIZARD_DLL
#define WLAN_WIZARD_EXPORT Q_DECL_EXPORT
#else
#define WLAN_WIZARD_EXPORT Q_DECL_IMPORT
#endif

/*!
   @addtogroup group_wlan_wizard_api
   @{
 */
class WLAN_WIZARD_EXPORT WlanWizard: public QObject
{
    Q_OBJECT

public:
    explicit WlanWizard(HbMainWindow *mainWindow);
    ~WlanWizard();

    void setParameters(
        const QString &name, 
        const QByteArray &ssid, 
        int networkMode,
        int securityMode,
        bool usePsk,
        bool wps);

signals:
    void cancelled();
    void finished(int iapId, bool connected);
        
public slots:
    void show();

protected:

protected slots:

private:
    Q_DISABLE_COPY(WlanWizard)

private slots:
    
private:
    //! Pointer to private implementation
    WlanWizardPrivate *d_ptr;
    
    // Friend classes
    friend class WlanWizardPrivate;
    friend class TestWlanWizardContext;    
};

/*! @} */

#endif // WLANWIZARD_H
