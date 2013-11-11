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
 *   EAP Wizard Page: New PAC store password
 *
 */

#ifndef EAPWIZARDPAGENEWPACSTORE_H
#define EAPWIZARDPAGENEWPACSTORE_H

// System includes
#include <QObject>

// User includes
#include "eapwizardpage.h"

// Forward declarations
class HbDocumentLoader;
class HbLineEdit;
class HbLabel;
class EapQtValidator;

// External data types

// Constants

/*!
   @addtogroup group_eap_wizard
   @{
 */

// Class declaration

class EapWizardPageNewPacStore : public EapWizardPage
{
    Q_OBJECT
public:
    explicit EapWizardPageNewPacStore(EapWizardPrivate* parent);
    ~EapWizardPageNewPacStore();

public: // From WlanWizardPage
    HbWidget* initializePage();
    int nextId() const;
    bool showPage();
    
signals:
        
public slots:

protected:

protected slots:

private:
    Q_DISABLE_COPY(EapWizardPageNewPacStore)

private slots:
    void textChanged(const QString &text);
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
    //! Line editor for new password
    HbLineEdit *mEditPasswordNew;
    //! Line editor for new password confirmation
    HbLineEdit *mEditPasswordConfirm;
    //! Title for new password
    HbLabel *mTitlePasswordNew;
    //! Title for new password confirmation
    HbLabel *mTitlePasswordConfirm;
};

/*! @} */

#endif
