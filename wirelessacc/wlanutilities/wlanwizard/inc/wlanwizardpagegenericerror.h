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
 *   WLAN Wizard Page: Generic error page
 *
 */

#ifndef WLANWIZARDPAGEGENERICERROR_H
#define WLANWIZARDPAGEGENERICERROR_H

// System includes

// User includes
#include "wlanwizardpageinternal.h"

// Forward declarations
class HbLabel;
class WlanWizardPrivate;

// External data types

// Constants

/*!
   @addtogroup group_wlan_wizard
   @{
 */

class WlanWizardPageGenericError : public WlanWizardPageInternal
{
public:
    explicit WlanWizardPageGenericError(WlanWizardPrivate* parent);
    virtual ~WlanWizardPageGenericError();

public: // From WlanWizardPage
    HbWidget* initializePage();
    bool showPage();
    int previousTriggered();
    
signals:
    
public slots:
    
protected:

protected slots:

private:
    Q_DISABLE_COPY(WlanWizardPageGenericError)

private slots:

private: // data
    //! Pointer to visialization of this page
    HbWidget *mWidget;
    //! Error label. Gets string from WlanWizardHelper::ConfGenericErrorString
    HbLabel *mLabel;
};

/*! @} */

#endif // WLANWIZARDPAGEGENERICERROR_H
