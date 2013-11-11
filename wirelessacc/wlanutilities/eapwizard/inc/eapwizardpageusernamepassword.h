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
 *   EAP Wizard Page: Common Username & password query.
 *
 */

#ifndef EAPWIZARDPAGEUSERNAMEPASSWORD_H
#define EAPWIZARDPAGEUSERNAMEPASSWORD_H

// System includes
#include <QObject>

// User includes
#include "eapwizardpage.h"

// Forward declarations
class HbDocumentLoader;
class HbLineEdit;
class HbLabel;
class EapWizardPrivate;
class EapQtValidator;

// External data types

// Constants

/*!
   @addtogroup group_eap_wizard
   @{
 */

// Class declaration

class EapWizardPageUsernamePassword : public EapWizardPage
{
    Q_OBJECT
public:
    explicit EapWizardPageUsernamePassword(EapWizardPrivate* parent);
    ~EapWizardPageUsernamePassword();

public: // From WlanWizardPage
    HbWidget* initializePage();
    int nextId() const;
    bool showPage();

signals:
        
public slots:

protected:

protected slots:

private:
    Q_DISABLE_COPY(EapWizardPageUsernamePassword)
    void createPages();
    bool hasInnerMethod() const;

private slots:
    void textChanged(const QString &text);
    void loadDocmlSection(Qt::Orientation orientation);
    
private: // data
    // OWNED 
    //! Document loader for docml
    QScopedPointer<HbDocumentLoader> mDocumentLoader;
    //! Validator for username 
    QScopedPointer<EapQtValidator> mValidatorUsername;
    //! Validator for password
    QScopedPointer<EapQtValidator> mValidatorPassword;
    
    // NOT OWNED
    //! Visualization of the page
    HbWidget *mWidget;
    //! Line editor for username
    HbLineEdit *mEditUsername;
    //! Line editor for password
    HbLineEdit *mEditPassword;
    //! Title/label for username
    HbLabel *mTitleUsername;
    //! Title/label for password
    HbLabel *mTitlePassword;
};

/*! @} */

#endif
