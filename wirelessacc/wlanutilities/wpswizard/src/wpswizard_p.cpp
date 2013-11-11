/*
* Copyright (c) 2010 Nokia Corporation and/or its subsidiary(-ies).
* All rights reserved.
* This component and the accompanying materials are made available
* under the terms of the "Eclipse Public License v1.0"
* which accompanies this distribution, and is available
* at the URL "http://www.eclipse.org/legal/epl-v10.html".
*
* Initial Contributors:
* Nokia Corporation - initial contribution.
*
* Contributors:
*
* Description: 
* WPS Wizard: Private implementation.
*/

// System includes

#include <QGraphicsWidget>
#include <QApplication>

#include <HbTranslator>
#include <HbDocumentLoader>
#include <HbMainWindow>
#include <HbView>
#include <HbStackedWidget>
#include <HbLabel>
#include <HbAction>

#include <cmmanagerdefines_shim.h>

// User includes

#include "wlanwizardhelper.h"
// WPS Wizard 
#include "wpswizard.h"
#include "wpswizard_p.h"
// Wizard Pages
#include "wpswizardsteptwo.h"
#include "wpswizardstepthreebutton.h"
#include "wpswizardstepthreenumber.h"
#include "wpswizardstepfour.h"
#include "wpswizardstepfive.h"

// Trace includes

#include "OstTraceDefinitions.h"
#ifdef OST_TRACE_COMPILER_IN_USE
#include "wpswizard_pTraces.h"
#endif

//Forward Decalrations

// External function prototypes

// Local constants


/*!
 \class WpsWizardPrivate
 \brief This class providesthe private class implementation for WPS wizard 
 */

// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Constructor for the private implementation of wps wizard
   
   @param [in] wizard WpsWizard* pointer to the framework object
   @param [in] wizardHelper WlanWizardHelper * pointer to the wizard helpper class 
   
   @return WpsWizardPrivate 
 */
WpsWizardPrivate::WpsWizardPrivate(
    WpsWizard* wizard,
    WlanWizardHelper *wizardHelper) :
    q_ptr(wizard), 
    mWizardHelper(wizardHelper),
    mTranslator(new HbTranslator("wpswizardplugin")), 
    mRandomNumber(0)
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_WPSWIZARDPRIVATE_ENTRY, this);
    createPages();
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_WPSWIZARDPRIVATE_EXIT, this);
}

/*!
   Destructor
 */
WpsWizardPrivate::~WpsWizardPrivate()
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_WPSWIZARDPRIVATE_DESTRUCTOR_ENTRY, this);
    // have to delete the memory that was allocated in this object
    mIapList.clear();
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_WPSWIZARDPRIVATE_DESTRUCTOR_EXIT, this);
}

/*!
   Provides the caller with the functionality to enable/disable the 'Next' button
   
   @param [in] enable bool to indicate whether to enable or disable the 'next button'
 */
void WpsWizardPrivate::enableNextButton(bool enable)
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_ENABLENEXTBUTTON_ENTRY, this);
    mWizardHelper->enableNextButton(enable);
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_ENABLENEXTBUTTON_EXIT, this);
}

/*!
   Provides the functionality to automatically move to the next page.
 */
void WpsWizardPrivate::nextPage()
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_NEXTPAGE_ENTRY, this);
    mWizardHelper->nextPage();
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_NEXTPAGE_EXIT, this);
}

/*!
   Provides access to the main window
   
   @return HbMainWindow* An instance of HbMainWindow.
 */
HbMainWindow* WpsWizardPrivate::mainWindow()
{
    return mWizardHelper->mainWindow();
}

/*!
   Provides the functionality to store the received settings
   
   @param [in] index int indicates the index in the array of received 
            settings to be stored
 */
void WpsWizardPrivate::storeSettings(int index)
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_STORESETTINGS_ENTRY, this);
    
    if (mIapList.count() > index)
        {
        mWpsAttr = mIapList[index];
    
        // SSID
        QString ssid = QString::fromUtf8(reinterpret_cast<const char*> (mWpsAttr.iSsid.Ptr()),
                mWpsAttr.iSsid.Length());    
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfName, ssid);
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfSsid, ssid.toUtf8());
        // WEP Key1
        QString str = QString::fromUtf8(reinterpret_cast<const char*>(mWpsAttr.iWepKey1.Ptr()),
                mWpsAttr.iWepKey1.Length());
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfKeyWep1, str);
        // WEP Key2
        str = QString::fromUtf8(reinterpret_cast<const char*> (mWpsAttr.iWepKey2.Ptr()),
                mWpsAttr.iWepKey2.Length());
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfKeyWep2, str);
        // WEP Key3
        str = QString::fromUtf8(reinterpret_cast<const char*> (mWpsAttr.iWepKey3.Ptr()),
                mWpsAttr.iWepKey3.Length());
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfKeyWep3, str);
        // WEP Key4
        str = QString::fromUtf8(reinterpret_cast<const char*> (mWpsAttr.iWepKey4.Ptr()),
                mWpsAttr.iWepKey4.Length());
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfKeyWep4, str);
        //Default WEP key in use
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfKeyWepDefault,
                convertDefaultWepKey(mWpsAttr.iWepDefaultKey));
        //Key WPA
        str = QString::fromUtf8(reinterpret_cast<const char*> (mWpsAttr.iWpaPreSharedKey.Ptr()),
                mWpsAttr.iWpaPreSharedKey.Length());
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfKeyWpa, str);
        // security mode
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfSecurityMode,
                convertSecurityModes(mWpsAttr.iSecurityMode));
        // Network Mode
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfNetworkMode,
                convertNetworkModes(mWpsAttr.iOperatingMode));
        
        mWizardHelper->setConfiguration(WlanWizardHelper::ConfUsePsk, true);
        }

    OstTraceFunctionExit1(WPSWIZARDPRIVATE_STORESETTINGS_EXIT, this);
}

/*!
   Provides the functionality to retrieve the ssid of the network to connect
   
   @return returns the SSID as a QString
 */
QString WpsWizardPrivate::getSsid()
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_GETSSID_ENTRY, this);
    QString ssid;
    ssid = mWizardHelper->configuration(WlanWizardHelper::ConfSsid).toString();
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_GETSSID_EXIT, this);
    return ssid;
}

/*!
   Provides functionality for the caller to store the array of 
   recieved settings from AP
   
   @param [in] credentials QList<TWlanProtectedSetupCredentialAttribute>& 
        received settings from wps enabled AP
 */
void WpsWizardPrivate::storeSettingsArray(QList<
        TWlanProtectedSetupCredentialAttribute>& credentials)
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_STORESETTINGSARRAY_ENTRY, this);
    mIapList = credentials;
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_STORESETTINGSARRAY_EXIT, this);
}

/*!
   Returns the array of settings received from the AP
   
   @return Array containing the settings
 */
QList<TWlanProtectedSetupCredentialAttribute> WpsWizardPrivate::getSettingsArray()
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_GETSETTINGSARRAY_ENTRY, this);
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_GETSETTINGSARRAY_EXIT, this);
    return mIapList;
}

/*!
   Creates the required pages for WPS setup
 */
void WpsWizardPrivate::createPages()
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_CREATEPAGES_ENTRY, this);
    
    mWizardHelper->addPage(WpsWizardPage::PageWpsWizardStep2,
            new WpsPageStepTwo(this));

    mWizardHelper->addPage(WpsWizardPage::PageWpsWizardStep3_Button,
            new WpsPageStepThreeButton(this));
    
    mWizardHelper->addPage(WpsWizardPage::PageWpsWizardStep3_Number,
            new WpsPageStepThreeNumber(this));
    
    mWizardHelper->addPage(WpsWizardPage::PageWpsWizardStep4,
            new WpsPageStepFour(this));
    
    mWizardHelper->addPage(WpsWizardPage::PageWpsWizardStep5,
            new WpsPageStepFive(this));
    
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_CREATEPAGES_EXIT, this);
}

/*!
   Converts enums between wlan security mode to cm manager enums.
   
   @param [in] securityMode Security operating mode from wlanmgmt api
   
   @return int converted CMManagerShim::WlanSecMode codes
 */
int WpsWizardPrivate::convertSecurityModes(TWlanIapSecurityMode securityMode)
{
    CMManagerShim::WlanSecMode cmManagerSecurityMode;
    switch(securityMode) {
    case EWlanIapSecurityModeWep:
        cmManagerSecurityMode = CMManagerShim::WlanSecModeWep;
        break;
        
    case EWlanIapSecurityModeWpa:
        cmManagerSecurityMode = CMManagerShim::WlanSecModeWpa;
        break;
        
    case EWlanIapSecurityModeWpa2Only:
        cmManagerSecurityMode = CMManagerShim::WlanSecModeWpa2;
        break;
        
    default:
        // invalid securityMode.
        // Not possible to get EWlanIapSecurityMode802d1x here. Since not 
        // supported with wps
        Q_ASSERT(EWlanIapSecurityModeAllowUnsecure == securityMode);
        cmManagerSecurityMode = CMManagerShim::WlanSecModeOpen;
        break;
    }
    
    return cmManagerSecurityMode;
}

/*!
   Converts enums between wlan wep key index to cm manager enums.
   
   @param [in] defaultWepKey Default wep key used in wlanmgmt api
   
   @return int converted CMManagerShim::WlanWepKeyIndex codes
 */
int WpsWizardPrivate::convertDefaultWepKey(TWlanDefaultWepKey defaultWepKey)
{
    int cmManagerWepKeyIndex;
    
    switch (defaultWepKey) {
    case EWlanDefaultWepKey2:
        cmManagerWepKeyIndex = CMManagerShim::WepKeyIndex2;
        break;
        
    case EWlanDefaultWepKey3:
        cmManagerWepKeyIndex = CMManagerShim::WepKeyIndex3;
        break;
        
    case EWlanDefaultWepKey4:
        cmManagerWepKeyIndex = CMManagerShim::WepKeyIndex4;
        break;
        
    case EWlanDefaultWepKey1:
    default:
        // It is possible that the value of defaultWepKey is not initialized
        // when security mode is Open. That's why we cannot assert here.
        cmManagerWepKeyIndex = CMManagerShim::WepKeyIndex1;
        break;
    }
    return cmManagerWepKeyIndex;
}

/*!
   Converts enums between wlan network mode to cm manager enums.
   
   @param [in] networkMode Networking operating mode from wlanmgmt api
   
   @return int converted CMManagerShim::WlanConnMode codes
 */
int WpsWizardPrivate::convertNetworkModes(TWlanOperatingMode networkMode)
{
    CMManagerShim::WlanConnMode wlanConnMode;
    switch (networkMode) {
    case EWlanOperatingModeAdhoc:
        wlanConnMode = CMManagerShim::Adhoc;
        break;
        
    default: 
        Q_ASSERT(EWlanOperatingModeInfrastructure == networkMode);
        wlanConnMode = CMManagerShim::Infra;
        break;
    }
    return wlanConnMode;
}

/*!
   Provides with the functionality to store the PIN generated
   
   @param [in] random The generated PIN code to be used in succesive steps
   
 */
void WpsWizardPrivate::setPin(int random)
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_SETPIN_ENTRY, this);
    mRandomNumber = random;
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_SETPIN_EXIT, this);
}

/*!
   Returns the PIN code stored in one of the WPS Pages
   
   @return PIN code to be used in the WPS operation.
 */
int WpsWizardPrivate::getPin()
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_GETPIN_ENTRY, this); 
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_GETPIN_EXIT, this);
    return mRandomNumber;
}

/*!
   Returns page id when manual configuration is chosen
   
   @param [in] useWps is WPS used or not?
   
   @return new page id
 */
int WpsWizardPrivate::nextPageId(bool useWps)
{
    OstTraceFunctionEntry1(WPSWIZARDPRIVATE_NEXTPAGEID_ENTRY, this);
    OstTraceFunctionExit1(WPSWIZARDPRIVATE_NEXTPAGEID_EXIT, this);
    return mWizardHelper->nextPageId(useWps);
}

/*!
    Sets wlan configuration value for given configuration identifier.
    See ConfigurationId for further details about the data types in QVariant.
    
    @param [in] confId Configuration Identifier do to be set
    @param [in] value  Value for configuration.
  */
void WpsWizardPrivate::setConfiguration(
    WlanWizardHelper::ConfigurationId confId,
    const QVariant &value)
{
    mWizardHelper->setConfiguration(confId, value);
}
