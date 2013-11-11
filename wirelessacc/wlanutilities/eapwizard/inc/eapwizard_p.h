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
 *   EAP Wizard: Private implementation.
 *
 */

#ifndef EAPWIZARD_P_H
#define EAPWIZARD_P_H

// System includes
#include <QObject>
#include <QHash>

// User includes
#include "wlanwizardplugin.h"

// Forward declarations
class HbTranslator;
class HbStackedWidget;
class HbWidget;
class HbLabel;
class HbView;
class HbAction;
class HbDocumentLoader;
class WlanWizardHelper;
class EapWizardPage;
class EapQtConfigInterface;
class EapQtPluginHandle;
class EapQtConfig;
class EapWizard;
class TestWlanWizardContext;
// External data types

// Constants

/*!
   @addtogroup group_eap_wizard
   @{
 */

// Class declaration

class EapWizardPrivate : public QObject
{
    Q_OBJECT
public:
    /*!
       Enumerator for internal EAP configuration cache. This cache is used to
       store user input and selections. see configurations() and 
       setConfigurations() for howto access the configurations.
       
       Below is descripted which configuration can be used with a specific EAP
       type.
       EAP-TTLS & EAP-PEAP & EAP-TLS
       - CertificateCa
       - TunnelUsernameAutomatic
       - TunnelUsername
       - TunnelRealmAutomatic
       - TunnelRealm
       
       EAP-TTLS & EAP-PEAP & EAP-FAST
       - InnerType
       
       EAP-TLS
       - CertificateUser
       
       EAP-FAST
       - PacStorePassword
       - PacStoreState
       
       LEAP, PAP, EAP-GTC, EAP/Plain-MSChapv2
       - Username
       - Password
       
       PAP, EAP-GTC, EAP/Plain-MSChapv2
       - OuterType
       
       Remember to update trace.properties upon changes.
     */
    enum ConfigurationId
    {
        //! EapQtPluginHandle::Plugin (int), EAP outer type
        OuterType = 0,
        //! EapQtPluginHandle::Plugin (int), EAP inner type
        InnerType = 1,
        //! CertificateInformation, NULL: automatic, CA Certificate
        CertificateCa = 2,
        //! CertificateInformation, User Certificate
        CertificateUser = 3,
        //! bool, automatic username for EAP identify (outer type)
        TunnelUsernameAutomatic = 4,
        //! QString, username for EAP identify (outer type)
        TunnelUsername = 5,
        //! bool, automatic realm for EAP identify (outer type)
        TunnelRealmAutomatic = 6,
        //! QString, realm for EAP identify (outer type)
        TunnelRealm = 7,
        //! QString, username for inner methods or for LEAP
        Username = 8,
        //! QString, password for inner methods or for LEAP
        Password = 9,
        //! QString, PAC store password
        PacStorePassword = 10,
        //! EapQtPacStoreConfig::PacStoreState (int), PAC store state
        PacStoreState = 11,
    };

public:
    explicit EapWizardPrivate(WlanWizardHelper *wizardHelper);
    ~EapWizardPrivate();
   
public: // From WlanWizardPlugin
    bool summary(WlanWizardPlugin::Summary sum, QString &item, QString &value);
    bool storeSettings();
    QString errorString(int errorCode);
    
public:
    QVariant configurations(ConfigurationId confId) const;
    void setConfigurations(ConfigurationId confId, const QVariant &value);
    EapQtConfigInterface* eapConfigurationInterface() const;
    QString eapTypeToString(int id) const;
    WlanWizardHelper *wizardHelper() const;

signals:
        
public slots:

protected:

protected slots:

private:
    Q_DISABLE_COPY(EapWizardPrivate)
    void createPages();
    bool hasInnerMethod() const;
    void handleTlsMethodsSettings(
        EapQtConfig &eapConf, 
        EapQtPluginHandle &outerType);
    
    bool handleEapFastSettings(EapQtConfig &eapConf);
    
    void handleEapAkaSimSettings(EapQtConfig &eapConf);
    
    void handleLeapSettings(EapQtConfig &eapConf);
    
    bool storeOuterTypeSettings(EapQtPluginHandle &outerType);
    
    bool storeInnerTypeSettings(EapQtPluginHandle &outerType);
    
private slots:
    
private: // data
    // NOT owned pointers
    //! Wizard framework services (WLAN Wizard Plugin API)
    WlanWizardHelper *mWizardHelper;
    
    // Owned pointers
    //! Translator
    QScopedPointer<HbTranslator> mTranslator;
    //! EAP configuration and services (EAP Qt Configuration Interface API)
    QScopedPointer<EapQtConfigInterface> mEapConfIf;
    //! Internal container for EAP specific settings
    QHash<ConfigurationId, QVariant> mConfigurations;
    
    // Friend classes
    friend class TestWlanWizardContext;
};

/*! @} */

#endif
