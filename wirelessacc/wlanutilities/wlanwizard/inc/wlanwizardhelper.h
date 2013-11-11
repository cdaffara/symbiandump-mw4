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
*   WLAN Wizard Plugin API: Interface for plugins to wizard services.
*/

#ifndef WLANWIZARDHELPER_H
#define WLANWIZARDHELPER_H

// System includes
#include <cmmanagerdefines_shim.h>

// User includes

// Forward declarations
class HbMainWindow;
class HbWidget;
class WlanWizardPage;

// External data types

// Constants

/*!
 * @addtogroup group_wlan_wizard_api_internal
 * @{
 */

/*!
   This class specifies the interface class that wizard plugins (EAP and WPS) 
   can use to access wizard framework services.
 */
class WlanWizardHelper
{
public:
    /*!
        WLAN Wizard configuration, which are accessible by wlan wizard and it's 
        plugins. WLAN wizard pages uses this information to store configurations
        from ui and state events.
        
        Please remember to update trace.properties upon changes.
     */
    enum ConfigurationId {
        //! QString: WLAN Network name
        ConfName = 0,
        //! QByteArray: WLAN Network SSID
        ConfSsid = 1,
        //! int: CMManagerShim::WlanConnectionMode
        ConfNetworkMode = 2,
        //! int: CMManagerShim::WlanSecMode
        ConfSecurityMode = 3,
        //! QString: WPA PSK
        ConfKeyWpa = 4,
        //! QString: WEP Key 1
        ConfKeyWep1 = 5,
        //! QString: WEP Key 2
        ConfKeyWep2 = 6,
        //! QString: WEP Key 3
        ConfKeyWep3 = 7,
        //! QString: WEP Key 4
        ConfKeyWep4 = 8,
        //! int: CMManagerShim:::WlanWepKeyIndex, Default WEP Key index
        ConfKeyWepDefault = 9,
        //! bool: Internet Connectivity Test result, true passed.
        ConfIctStatus = 10,
        //! int, -1 (WlanQtUtils::IapIdNone): not defined: IAP ID
        ConfIapId = 11,
        //! bool: true if connection was established successfully
        ConfConnected = 12,
        //! bool: WLAN AP explicit scanning need
        ConfWlanScanSSID = 13,
        //! bool: if true use psk, if false use EAP 
        ConfUsePsk = 14,
        //! bool: if true processing connection page is required.
        ConfProcessSettings = 15,
        //! QString: Localized error string for WlanWizardPageGenericError
        ConfGenericErrorString = 16,
        //! WlanWizardScanList: Available network options
        ConfAvailableNetworkOptions = 17,
        //! bool: Wps supported
        ConfWpsSupported = 18,
        //! int: Identifies how many steps should be gone backwards from 
        // GenericErrorPage
        ConfGenericErrorPageStepsBackwards = 19,
    };
    
public:
    
    /*!
       Reader method for wlan configurations.
       
       See ConfigurationId for further details about the data types in QVariant.
       
       @param [in] confId Defines what configuration is read.
       
       @return configuration value.
     */
    virtual QVariant configuration(ConfigurationId confId) const = 0;
    
    /*!
       Sets wlan configuration value for given configuration identifier.
       See ConfigurationId for further details about the data types in QVariant.
       
       @param [in] confId Configuration Identifier do to be set
       @param [in] value  Value for configuration.
     */
    virtual void setConfiguration(
        ConfigurationId confId, 
        const QVariant &value) = 0;
    
    /*!
     * Clears wlan configuration at the given configuration identifier and sets
     * the configuration value to type Invalid.
     * 
     * @param [in] confId Configuration Identifier do to be cleared
     */
    virtual void clearConfiguration(ConfigurationId confId) = 0;
    
    /*!
     * Tests whether a configuration has been set.
     * 
     * @param [in] confId Configuration Identifier do to be tested
     * @return true if configuration value != Invalid, else false.
     */
    virtual bool configurationExists(ConfigurationId confId) = 0;
    
    /*!
       Enables the next button in wizard.
       
       @param [in] enable to enable button set to true.
     */
    virtual void enableNextButton(bool enable) = 0;

    /*!
       Adds a new wizard page into the wizard framework.
       
       @param [in] pageId The identifier of Page. See WlanWizardPage::PageIds.
       @param [in] page New wizard page.
     */
    virtual void addPage(int pageId, WlanWizardPage *page) = 0;

    /*!
       Triggers movement to the next page.
       
       @note This operation has been protected with 1.5 second timer to prevent
       too short lasting pages.
       
       @note Must be used with WlanWizardPage::startOperation().
     */
    virtual void nextPage() = 0;
    
    /*!
       @return the main window of wizard.
     */
    virtual HbMainWindow *mainWindow() const = 0;
    
    /*!
       Checks if provided page visualization is the current visible page.
       
       @param [in] page Wizard Page visualization to be checked
       
       @return true if the page is currently visible.
     */
    virtual bool isCurrentPage(const HbWidget *page) const = 0;

    /*!
       Can be used to get correct page id based on the existing configuration.
       
       This method can be used in so called entry points to wizards
       - WLAN Wizard has been lauched and setParameters() has been called
       - WLAN Wizard makes own internal scanning
       - WPS Wizard uses manual configuration mode.
       
       @param [in] useWps true: if WPS Wizard needs to be opened, false: otherwise
       
       @return Page Id based on configurations:
       - WEP/WPA(2) with PSK: WlanWizardPageInternal::PageKeyQuery
       - Open: WlanWizardPage::PageProcessSettings
       - 802.1x or WPA (2) with EAP: WlanWizardPage::PageEapStart
       - UseWps: WlanWizardPage::PageWpsStart
     */
    virtual int nextPageId(bool useWps) = 0;
    
signals:
    
public slots:
    
protected:

protected slots:

private:

private slots:

private: // data
};

/*! @} */

#endif // WLANWIZARDHELPER_H
