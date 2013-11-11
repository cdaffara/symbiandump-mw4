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
 *   WLAN Wizard Page: Processing settings, connection and running ict.
 *
 */

#ifndef WLANWIZARDPAGEPROCESSINGSETTINGS_H
#define WLANWIZARDPAGEPROCESSINGSETTINGS_H

// System includes

// User includes
#include "wlanwizardpageinternal.h"
#include "wlanqtutils.h"

// Forward declarations
class HbDocumentLoader;
class HbLabel;
class WlanWizardPrivate;

// External data types

// Constants

/*!
   @addtogroup group_wlan_wizard
   @{
 */

class WlanWizardPageProcessingSettings: public WlanWizardPageInternal
{
    Q_OBJECT
public:
    explicit WlanWizardPageProcessingSettings(WlanWizardPrivate* parent);
    virtual ~WlanWizardPageProcessingSettings();

public: // From WlanWizardPage
    HbWidget* initializePage();
    int nextId(bool &removeFromStack) const;
    bool showPage();
    bool requiresStartOperation();
    void startOperation();
    
signals:
    
public slots:
    void loadDocmlSection(Qt::Orientation orientation);
    void wlanNetworkClosed(int iapId, int reason);
    void ictResult(int iapId, int result);
    void wlanNetworkOpened(int iapId);
    
protected:

protected slots:

private:
    Q_DISABLE_COPY(WlanWizardPageProcessingSettings)

private slots:

private: // data
    // OWNED
    //! Docml document loader
    HbDocumentLoader *mDocLoader;
    
    // NOT OWNED
    //! Visualization of the page
    HbWidget *mWidget;
    //! Title for text in the page
    HbLabel *mLabel;
    //! PageId for next wizard page
    int mNextPageId;
};

/*! @} */

#endif // WLANWIZARDPAGEPROCESSINGSETTINGS_H
