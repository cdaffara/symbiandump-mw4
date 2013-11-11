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
 *   WLAN Wizard Page: Key Query page for WEP and WPA (2).
 *
 */

#ifndef WLANWIZARDPAGEKEYQUERY_H
#define WLANWIZARDPAGEKEYQUERY_H

// System includes

// User includes
#include "wlanwizardpageinternal.h"
#include "wlanwizardutils.h"

// Forward declarations
class QChar;
class HbLabel;
class HbLineEdit;
class HbDocumentLoader;
class WlanWizardPrivate;

// External data types

// Constants

/*!
   @addtogroup group_wlan_wizard
   @{
 */

class WlanWizardPageKeyQuery: public WlanWizardPageInternal
{
    Q_OBJECT
public:
    explicit WlanWizardPageKeyQuery(WlanWizardPrivate* parent);
    virtual ~WlanWizardPageKeyQuery();

public: // From WlanWizardPageInternal
    HbWidget* initializePage();
    int nextId(bool &removeFromStack) const;
    bool showPage();

signals:    
    
public slots:
    void loadDocmlSection(Qt::Orientation orientation);
    
protected:
    bool eventFilter(QObject *obj, QEvent *event);
    
protected slots:

private:
    Q_DISABLE_COPY(WlanWizardPageKeyQuery)
    QString keyStatusToErrorString(WlanWizardUtils::KeyStatus status) const;

private slots:
    
private:
    // NOT OWNED  
    //! Visualization of the page
    HbWidget *mWidget;
    //! Title text for line editor
    HbLabel *mLabelTitle;
    //! Line editor for WLAN WEP / WPA PSK key
    HbLineEdit *mLineEdit;
    //! Label for key specific error texts
    HbLabel *mLabelError;
    
    // OWNED 
    //! docml document loader
    HbDocumentLoader *mDocLoader;
};

/*! @} */

#endif // WLANWIZARDPAGEKEYQUERY_H
