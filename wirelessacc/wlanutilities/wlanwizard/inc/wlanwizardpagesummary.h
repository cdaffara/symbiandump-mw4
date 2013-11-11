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
 *   WLAN Wizard Page: Summary
 *
 */

#ifndef WLANWIZARDPAGESUMMARY_H
#define WLANWIZARDPAGESUMMARY_H

// System includes

// User includes
#include "wlanwizardpageinternal.h"

// Forward declarations
class HbLabel;
class WlanWizardPrivate;
class HbListWidget;

// External data types

// Constants

/*!
   @addtogroup group_wlan_wizard
   @{
 */

class WlanWizardPageSummary : public WlanWizardPageInternal
{
public:
    explicit WlanWizardPageSummary(WlanWizardPrivate* parent);
    virtual ~WlanWizardPageSummary();

public: // From WlanWizardPage
    HbWidget* initializePage();

signals:
    
public slots:
    
protected:

protected slots:

private:
    Q_DISABLE_COPY(WlanWizardPageSummary)
    void loadDocml();
    QString networkModeText() const;
    QString securityModeText() const;
    void addDynamicItems(int &row);
    void appendToList(int &row, const QString title, const QString value);
    void appendToListPluginInfo(WlanWizardPlugin::Summary id, int &row);

private slots:

private: // data
    //! Visualization of the page
    HbListWidget *mListWidget;
};

/*! @} */

#endif // WLANWIZARDPAGESUMMARY_H
