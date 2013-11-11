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
 *   WLAN Wizard: Interface for wizard pages (wlan internal).
 *
 */

#ifndef WLANWIZARDPAGEINTERNAL_H
#define WLANWIZARDPAGEINTERNAL_H

// System includes
#include <QObject>

// User includes
#include "wlanwizardpage.h"

// Forward declarations
class HbWidget;
class WlanWizardPrivate;
class HbDocumentLoader;

// External data types

// Constants

/*!
   @addtogroup group_wlan_wizard
   @{
 */

class WlanWizardPageInternal : public QObject, public WlanWizardPage
{
public:
    /*! 
       WLAN Wizard internal pages.
       
       Remember to update trace.properties file upon changes.
     */
    enum WlanPageIds {
        //! SSID Query
        PageSsid = WlanWizardPage::PageWlanStart,
        //! Scanning page
        PageScanning,
        //! Manual network security selection page
        PageNetworkSecurity,
        //! WEP/WPA(2) key query page
        PageKeyQuery,
        //! WLAN Network mode page
        PageNetworkMode,
        //! Wizard summary page
        PageSummary,
    };

public: // from WlanWizardPage
    explicit WlanWizardPageInternal(WlanWizardPrivate *wizard);
    virtual ~WlanWizardPageInternal();

public:
    //! Pointer to private object. Provides services to pages
    WlanWizardPrivate* mWizard;
    
signals:
    
public slots:
    
protected:
    void loadDocmlSection(
        HbDocumentLoader *loader,
        Qt::Orientation orientation,
        const QString &filename,
        const QString &portraitSection,
        const QString &landscapeSection) const;
    
protected slots:

private:
    Q_DISABLE_COPY(WlanWizardPageInternal)
    
private slots:

private: // data
};

/*! @} */

#endif  // WLANWIZARDPAGEINTERNAL_H
