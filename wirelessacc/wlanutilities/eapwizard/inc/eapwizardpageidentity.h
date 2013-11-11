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
 *   EAP Wizard Page: EAP Identity.
 *
 */

#ifndef EAPWIZARDPAGEIDENTITY_H
#define EAPWIZARDPAGEIDENTITY_H

// System includes
#include <QObject>

// User includes
#include "eapwizardpage.h"

// Forward declarations
class HbLineEdit;
class HbCheckBox;
class HbLabel;
class HbDocumentLoader;
class EapQtValidator;
class EapWizardPrivate;

// External data types

// Constants

/*!
   @addtogroup group_eap_wizard
   @{
 */

// Class declaration

class EapWizardPageIdentity : public EapWizardPage
{
    Q_OBJECT
public:
    explicit EapWizardPageIdentity(EapWizardPrivate* parent);
    ~EapWizardPageIdentity();

public: // From WlanWizardPage
    HbWidget* initializePage();
    int nextId() const;
    bool showPage();

signals:
        
public slots:

protected:

protected slots:

private:
    Q_DISABLE_COPY(EapWizardPageIdentity)
    bool validateGroup(
        HbLineEdit *edit, 
        HbCheckBox *checkBox, 
        EapQtValidator* validator) const;

private slots:
    void loadDocmlSection(Qt::Orientation orientation);
    void textChanged(const QString &text);
    void stateChanged(int state);

private: // data
    // OWNED
    //! docml document loader 
    QScopedPointer<HbDocumentLoader> mDocumentLoader;
    //! Content validator and configurator for line edit of realm
    QScopedPointer<EapQtValidator> mValidatorRealm;
    //! Content validator and configurator for line edit of username
    QScopedPointer<EapQtValidator> mValidatorUsername;
    
    // NOT OWNED
    //! Visualization of the page
    HbWidget *mWidget;
    //! Line editor for username
    HbLineEdit *mEditUsername;
    //! Line editor for realm
    HbLineEdit *mEditRealm;
    //! Checkbox for username
    HbCheckBox *mCheckUsername;
    //! Checkbox for realm
    HbCheckBox *mCheckRealm;
    //! Title text for realm 
    HbLabel *mLabelRealm;
    //! Title text for username 
    HbLabel *mLabelUsername;
};

/*! @} */

#endif
