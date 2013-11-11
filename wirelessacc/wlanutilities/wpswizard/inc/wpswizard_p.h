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
 * Description: WPS wizard private implementation
 *
 */

#ifndef WPSWIZARD_P_H_
#define WPSWIZARD_P_H_

// System includes
#include <QObject>
#include <wlanmgmtcommon.h>


// User includes
#include "wlanwizardhelper.h"
#include "wpswizard.h"

// Forward declarations
class HbStackedWidget;
class HbTranslator;
class HbWidget;
class HbLabel;
class HbView;
class HbAction;
class HbDocumentLoader;
class WlanWizardHelper;

// External data types
// Constants

/*!
 * @addtogroup group_wps_wizard_plugin
 * @{
 */

// Class declaration
class WpsWizardPrivate : public QObject
    {
    Q_OBJECT
      
public:
    WpsWizardPrivate(WpsWizard* wizard, WlanWizardHelper *wizardHelper);
    ~WpsWizardPrivate();
public:

    void enableNextButton(bool enable);

    void nextPage();

    QString getSsid();

    void storeSettings(int index);
    
    HbMainWindow* mainWindow();

    void storeSettingsArray(
            QList<TWlanProtectedSetupCredentialAttribute>& credentials);
    QList<TWlanProtectedSetupCredentialAttribute> getSettingsArray();

    void setPin(int random);

    int getPin();

    int nextPageId(bool useWps);

    void setConfiguration(WlanWizardHelper::ConfigurationId confId,
            const QVariant &value);
    
    
signals:

public slots:

protected:

protected slots:

private:

    void createPages();
    
    int convertSecurityModes(TWlanIapSecurityMode securityMode);
    
    int convertNetworkModes(TWlanOperatingMode networkMode);
    
    int convertDefaultWepKey(TWlanDefaultWepKey defaultWepKey);

    Q_DISABLE_COPY(WpsWizardPrivate)

private slots:

private:
        
    //data

    //! Pointer to the WPS wizard
    WpsWizard *q_ptr;
    //! pointer to the wizard helper object
    WlanWizardHelper *mWizardHelper;
    //! Translation object
    QScopedPointer<HbTranslator> mTranslator;
    //! Object containing WLAN credentials
    TWlanProtectedSetupCredentialAttribute mWpsAttr;
    //! Array of objects containing WLAN credentials.
    QList<TWlanProtectedSetupCredentialAttribute> mIapList;
    /*! Random number generated for Number entry mode. When the value is zero (0)
     *  Push-Button mode is used.
     */
    int mRandomNumber;
    
    friend class WpsWizard;
    friend class TestWlanWizardContext;
    };

#endif /* WPSWIZARD_P_H_ */
