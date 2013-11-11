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
 *   EAP Wizard Page: PAC store password confirmation
 *
 */

#ifndef EAPWIZARDPAGENPACSTOREPASSWORDCONFIRM_H
#define EAPWIZARDPAGENPACSTOREPASSWORDCONFIRM_H

// System includes
#include <QObject>

// User includes
#include "eapwizardpage.h"

// Forward declarations
class HbDocumentLoader;
class HbLineEdit;
class HbLabel;
class EapQtValidator;
class EapWizardPrivate;

// External data types

// Constants


/*!
 * @addtogroup group_eap_wizard
 * @{
 */

// Class declaration

class EapWizardPagePacStorePasswordConfirm: public EapWizardPage
{
    Q_OBJECT
public:
    explicit EapWizardPagePacStorePasswordConfirm(EapWizardPrivate* parent);
    ~EapWizardPagePacStorePasswordConfirm();
    
public: // From WlanWizardPage    
    HbWidget* initializePage();
    int nextId() const;
    bool showPage();
    
signals:
        
public slots:

protected:
    bool eventFilter(QObject *obj, QEvent *event);

protected slots:

private:
    Q_DISABLE_COPY(EapWizardPagePacStorePasswordConfirm)
    void createPages();
    bool hasInnerMethod() const;

private slots:
    void loadDocmlSection(Qt::Orientation orientation);
    
private: // data
    // OWNED 
    //! Document loader for docml
    QScopedPointer<HbDocumentLoader> mDocumentLoader;
    //! Validator for PAC store password 
    QScopedPointer<EapQtValidator> mValidator;
    
    // NOT OWNED
    //! Visualization of the page
    HbWidget *mWidget;
    //! line editor for PAC store password confirmation 
    HbLineEdit *mEditPasswordConfirm;
    //! Label for embadded error string
    HbLabel *mLabelError;
};

/*! @} */

#endif
