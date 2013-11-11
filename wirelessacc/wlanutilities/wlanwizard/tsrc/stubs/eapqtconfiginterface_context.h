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
* Description:
*/

#ifndef EAPQTCONFIGINTERFACE_CONTEXT_H
#define EAPQTCONFIGINTERFACE_CONTEXT_H

#include "eapqtconfiginterface.h"
#include "eapqtpacstoreconfig.h"

class EapQtConfigInterfaceContext
{
public:
    EapQtConfigInterfaceContext();
    ~EapQtConfigInterfaceContext();

    void setObject(EapQtConfigInterface *eapQtConfig);
    
public: // Helpper methods to create simple valid configurations
    void createEapAkaSim(int pluginHandleEnum);
    
    void createLeap(const QString& username, const QString& passwd);

    void createEapIdentifyValidators(EapQtPluginHandle::Plugin outerType);
    
    void createEapTtlsPeap(
        EapQtPluginHandle::Plugin type,
        bool automaticCert,
        bool usernameAutomatic,
        const QString &username,
        bool realmAutomatic,
        const QString &realm,
        EapQtPluginHandle::Plugin innerType);

    void createEapTls(
        QList<EapQtCertificateInfo> &caCerts,
        int indexForCaCert,
        QList<EapQtCertificateInfo> &userCerts,
        int indexForUserCert);
    
    void createInner(
        EapQtPluginHandle::Plugin outerType,
        EapQtPluginHandle::Plugin innerType,
        const QString &username,
        const QString &password);
    
    void createInnerTypeValidators(EapQtPluginHandle::Plugin innerType);
    
    void createEapFast(int pacState, const QString &password = QString());
    
public: // Methods to set the exact behavior of the interface
    QStringList calledMethods();
    void setConfigurationReference(int iapId, bool retValue);
    void setConfigurationReferenceReturn(bool retValue);
    void setCertsCa(QList<EapQtCertificateInfo> &certs);
    void setCertsUser(QList<EapQtCertificateInfo> &certs);
    void setSupportedOuterTypes(QList<EapQtPluginHandle> &outerTypes);
    void setSupportedInnerTypes(EapQtPluginHandle outerHandle, QList<EapQtPluginHandle> &innerTypes);
    void setSelectectedOuterTypes(QList<EapQtPluginHandle> &types, bool retValue);
    void setSelectectedOuterTypesReturn(bool retValue);
    void setOuterConfig(EapQtPluginHandle::Plugin handle, EapQtConfig &config, bool retValue);
    void setOuterConfigReturn(bool retValue);
    void setInnerConfig(EapQtPluginHandle::Plugin handle, EapQtConfig &config, bool retValue);
    void setInnerConfigReturn(bool retValue);
    // Moved ownership to context
    void setValidator(
        EapQtExpandedEapType::Type eapType,
        EapQtConfig::SettingsId, 
        EapQtValidator *validator);
    
    EapQtValidator *validator(
        EapQtExpandedEapType::Type eapType,
        EapQtConfig::SettingsId id);

    void setPacStoreConfigRead(EapQtPacStoreConfig &config, bool retValue);
    void setPacStoreConfigReadReturn(bool retValue);
    void setPacStoreConfigSave(EapQtPacStoreConfig &config, bool retValue);
    void setPacStoreConfigSaveReturn(bool retValue);
    void setValidatorPacStore(int id, EapQtValidator * validator);
    
    EapQtValidator *validatorPacStore(int id);

    
private:
    EapQtConfigInterface* mEapQtConfig;
};

#endif /* EAPQTCONFIGINTERFACE_CONTEXT_H */

// End of File
