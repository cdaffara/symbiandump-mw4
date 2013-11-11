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
 *   WLAN Wizard Page: Ssid query
 *
 */

#ifndef WLANWIZARDPAGESSID_H
#define WLANWIZARDPAGESSID_H

// System includes

// User includes
#include "wlanwizardpageinternal.h"
#include "wlanwizardutils.h"

// Forward declarations
class WlanWizardPrivate;
class HbLabel;
class HbLineEdit;
class HbDocumentLoader;
class TestWlanWizardUi;

// External data types

// Constants

/*!
 * @addtogroup group_wlan_wizard
 * @{
 */

class WlanWizardPageSsid: public WlanWizardPageInternal
{
    Q_OBJECT
public:
    explicit WlanWizardPageSsid(WlanWizardPrivate* parent);
    virtual ~WlanWizardPageSsid();
    
public:
    HbWidget* initializePage();
    virtual bool showPage();
    int nextId(bool &removeFromStack) const;

signals:
    
public slots: 
    void loadDocmlSection(Qt::Orientation orientation);
    void textChanged(const QString &text);
    
protected:
    
protected slots:

private:
    Q_DISABLE_COPY(WlanWizardPageSsid)
    
private slots:
    
private:
    /*!
     * Pointer to the view.
     */
    HbWidget *mWidget;
    
    /*!
     * Pointer to the header label object.
     */
    HbLabel *mLabel;
    
    /*!
     * Pointer to the line edit object.
     */
    HbLineEdit *mSsid;
    
    /*!
     * Pointer to the document loader object.
     */
    HbDocumentLoader *mLoader;
    
    // Friend classes
    friend class TestWlanWizardUi;
};

/*! @} */

#endif
