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
 *   EAP Wizard Page: Inner Type selection
 *
 */

#ifndef EAPWIZARDPAGEINNERTYPE_H
#define EAPWIZARDPAGEINNERTYPE_H

// System includes
#include <QObject>
#include <eapqtpluginhandle.h>

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

class EapWizardPageInnerType : public EapWizardPage
{
    Q_OBJECT
public:
    EapWizardPageInnerType(EapWizardPrivate* parent, int outerType);
    ~EapWizardPageInnerType();

public: // From WlanWizardPage
    HbWidget* initializePage();
    int nextId() const;
    bool showPage();
    
signals:
        
public slots:
    void itemSelected(int index);    
    void loadDocmlSection(Qt::Orientation orientation);

protected:

protected slots:

private:
    Q_DISABLE_COPY(EapWizardPageInnerType)

    void appendToList(
        QStringList &items, 
        EapQtPluginHandle::Plugin innerType);
    
private slots:
    
private: // data
    // NOT OWNED
    //! Visualization of the page
    HbWidget *mWidget;
    //! List of innertypes
    HbRadioButtonList *mRadio;
    //! Title for inner types
    HbLabel *mTitle;
    
    // OWNED
    //! Document loader for docml
    QScopedPointer<HbDocumentLoader> mDocumentLoader;
    //! Page ID for this page
    int mOuterType;
    //! Is content of the page valid?
    bool mValid;
    //! Mapped from index (0..n) in radiobuttonlist to eap types
    QList<int> mEapTypes;
};

/*! @} */

#endif
