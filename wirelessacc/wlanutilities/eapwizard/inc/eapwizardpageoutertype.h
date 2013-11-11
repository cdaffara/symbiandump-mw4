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
 *   EAP Wizard Page: Outer type selection
 *
 */

#ifndef EAPWIZARDPAGEOUTERTYPE_H
#define EAPWIZARDPAGEOUTERTYPE_H

// System includes
#include <QObject>

// User includes
#include "eapwizardpage.h"

// Forward declarations
class HbRadioButtonList;
class HbWidget;
class HbDocumentLoader;
class HbLabel;
class EapWizardPrivate;

// External data types

// Constants

/*!
   @addtogroup group_eap_wizard
   @{
 */

// Class declaration

class EapWizardPageOuterType : public EapWizardPage
{
    Q_OBJECT
public:
     explicit EapWizardPageOuterType(EapWizardPrivate* parent);
    ~EapWizardPageOuterType();

public: // From WizardPage
    HbWidget* initializePage();
    int nextId() const;
    bool showPage();
    
signals:
        
public slots:

protected:

protected slots:

private:
    Q_DISABLE_COPY(EapWizardPageOuterType)
    void appendToList(
        QStringList &items, 
        int pluginHandle );
    
    int nextIdForEapFast() const;
    
private slots:
    void itemSelected(int index);    
    void loadDocmlSection(Qt::Orientation orientation);

private: // data
    // NOT OWNED
    //! Visualization of the page
    HbWidget *mWidget;
    //! List of outer types
    HbRadioButtonList *mRadio;
    //! Title for radio button list
    HbLabel *mTitle;
    
    // OWNED 
    //! Document loader for docml
    QScopedPointer<HbDocumentLoader> mDocumentLoader;
    //! Is page content valid?
    bool mValid;
    //! Mapped from index (0..n) in radiobuttonlist to eap types
    QList<int> mEapTypes;
};

/*! @} */

#endif
