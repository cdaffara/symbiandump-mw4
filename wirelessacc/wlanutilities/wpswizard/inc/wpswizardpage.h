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
* Description: API for wps wizard pages
*
*/

#ifndef WPSWIZARDPAGE_H_
#define WPSWIZARDPAGE_H_


// System includes
#include <QObject>

// User includes
#include "wlanwizardpage.h"

// Forward declarations
class WpsWizardPrivate;
class HbWidget;


// External data types
// Constants

/*!
 * @addtogroup group_wps_wizard_plugin
 * @{
 */

// Class declaration
class WpsWizardPage : public QObject, public WlanWizardPage
{
    Q_OBJECT
    
public:
    enum WpsPageIds {
        PageWpsWizardStep2 = WlanWizardPage::PageWpsStart,
        PageWpsWizardStep3_Button,
        PageWpsWizardStep3_Number,
        PageWpsWizardStep4,
        PageWpsWizardStep5,
        PageWpsWizardStep6
    };
    
    enum WpsMode {
        WpsPushButtonMode = 0,
        WpsPinCodeMode,
        WpsManualMode
    };  
    
public:
    explicit WpsWizardPage(WpsWizardPrivate* parent);
    virtual ~WpsWizardPage();

    /*!
     * Creates a visualization of the page.
     */
    virtual HbWidget* initializePage() = 0;
    /*!
     * Returns id of next page. updates settings EapWizard.
     */
    virtual int nextId(bool &removeFromStack) const = 0;
    /*!
     * This method is called when "previous" button has been pressed.
     */
    virtual int previousTriggered() { return OneStepBackwards;};
    /*!
     * This method is called when Cancel button has been pressed.
     */
    virtual void cancelTriggered() {};
    
signals:

public slots:

protected:

    //data
    //! Pointer to private implementation
    WpsWizardPrivate* mWizard;

protected slots:

private:
    Q_DISABLE_COPY(WpsWizardPage)
    
private slots:
    
private:
	
};

#endif /* WPSWIZARDPAGE_H_ */
