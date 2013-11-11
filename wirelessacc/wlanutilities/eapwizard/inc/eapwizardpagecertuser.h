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
 *   EAP Wizard Page: User Certificate Selection.
 *
 */

#ifndef EAPWIZARDPAGECERTUSER_H
#define EAPWIZARDPAGECERTUSER_H

// System includes
#include <QObject>

// User includes
#include "eapwizardpage.h"

// Forward declarations
class HbDocumentLoader;
class HbRadioButtonList;
class HbLabel;
class EapQtCertificateInfo;
class EapWizardPrivate;

// External data types

// Constants

/*!
   @addtogroup group_eap_wizard
   @{
 */

// Class declaration

class EapWizardPageCertUser : public EapWizardPage
{
    Q_OBJECT
public:
    explicit EapWizardPageCertUser(EapWizardPrivate* parent);
    ~EapWizardPageCertUser();

public: // From WlanWizardPage
    HbWidget* initializePage();
    int nextId() const;
    bool showPage();
    
signals:
        
public slots:

protected:

protected slots:

private:
    Q_DISABLE_COPY(EapWizardPageCertUser)
    void createPages();
    bool hasInnerMethod() const;

private slots:
    void loadDocmlSection(Qt::Orientation orientation);
    void itemSelected(int index);
    
private: // data
    // NOT OWNED
    //! Visualization of the page
    HbWidget *mWidget;
    //! Certificate list
    HbRadioButtonList *mCertList;
    //! Error text used when there are not any certificates
    HbLabel *mErrorLabel;
    
    // OWNED
    //! docml document loader 
    QScopedPointer<HbDocumentLoader> mDocumentLoader;
    //! Is page valid? it is when a certificate has been selected
    bool mValid;
    //! List of system's user certificates
    QList<EapQtCertificateInfo> mCerts;
};

/*! @} */

#endif
