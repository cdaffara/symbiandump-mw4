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
 *   EAP Wizard API
 *   
 */

#ifndef EAPWIZARD_H
#define EAPWIZARD_H

// System includes
#include <QObject>

// User includes
#include "wlanwizardplugin.h"

// Forward declarations
class WlanWizardHelper;
class EapWizardPrivate;
class TestWlanWizardContext;

// External data types

// Constants
#ifdef BUILD_EAP_WIZARD_DLL
#define EAP_WIZARD_EXPORT Q_DECL_EXPORT
#else
#define EAP_WIZARD_EXPORT Q_DECL_IMPORT
#endif

/*!
 * @addtogroup group_eap_wizard
 * @{
 */

// Class declaration

class EAP_WIZARD_EXPORT EapWizard : public QObject, public WlanWizardPlugin
{
    Q_OBJECT
    
public:
    explicit EapWizard(WlanWizardHelper *wizardHelper); 
    ~EapWizard();
    
public: // From WlanWizardPlugin
    virtual bool summary(WlanWizardPlugin::Summary sum, QString &item, QString &value);
    virtual bool storeSettings();
    virtual QString errorString(int errorCode);
    
signals:
        
public slots:

protected:

protected slots:

private:
    Q_DISABLE_COPY(EapWizard)

private slots:
    
private: // data
    QScopedPointer<EapWizardPrivate> d_ptr;
    
    // Friend classes
    friend class EapWizardPrivate;
    friend class TestWlanWizardContext;
};

/*! @} */

#endif /* EAPWIZARD_H */
