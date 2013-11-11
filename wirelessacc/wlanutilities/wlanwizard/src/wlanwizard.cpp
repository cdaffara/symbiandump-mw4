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
* WLAN Wizard: API.
*/

// System includes

// User includes
#include "wlanwizard.h"
#include "wlanwizard_p.h"

/*!
   \class WlanWizard
   \brief Interface of WLAN Wizard.
   
   Example usage:
   \code
   MyClass::createWizard() {
       mWizard = new WlanWizard(mainWindow());
       connect(
           mWizard,
           SIGNAL(finished(int, bool)), 
           this,
           SLOT(finished(int, bool)));
       connect(mWizard, SIGNAL(cancelled()), this, SLOT(cancelled()));
   
       // If client know the parameters for WLAN Access Point call following
       mWizard->setParameters(
           "MySSid", 
           CmManagerShim::Infra, 
           CmManagerShim::WlanSecModeWpa,
           true,    // WPA-PSK
           false);  // Non-Wifi Protected Setup  
    
       // and execute wizard
       mWizard->show();
   }
   
   void MyClass::finished(int iapId, bool connected) {
       // User has successfully created WLAN IAP with the wizard. 
       
       // if connected equals to true, wizard has established connection to it
       // now the client needs to connect also to given IAP Id to keep the
       // connection open. 
       // see WlanQtUtils or RConnection.
       
       // Delete wizard. Do not delete in this call stack since this call has 
       // been done from the context of the wizards call stack.
       mWizard->deleteLater();
       mWizard = NULL;
   }  
   
   void MyClass::cancelled() {
       // wizard operation was cancelled by user, iap has not been created
       // and WLAN connection is not established
 
       // Delete wizard. Do not delete in this call stack since this call has 
       // been done from the context of the wizards call stack.      
       mWizard->deleteLater();
       mWizard = NULL;
   }
   \endcode
     
   Implements wizard based on wizard pattern.
 */

/*!
   \fn void WlanWizard::cancelled()
   This signal is emitted when the execution of wizard has been cancelled.
 */

/*!
   \fn void WlanWizard::finished(int iapId, bool connected)
   This signal is emitted when the execution of wizard has been finished
   succesfully.
   
   @param iapId IAP ID that has been created.
   @param connected true if the connection to wlan has been established.
 */

// External function prototypes

// Local constants


// ======== LOCAL FUNCTIONS ========

// ======== MEMBER FUNCTIONS ========

/*!
   Constructor of WLAN Wizard.
   
   @param [in] mainWindow HbMainWindow to where the wizard is going to be executed.
 */
WlanWizard::WlanWizard(HbMainWindow *mainWindow) :
    d_ptr(new WlanWizardPrivate(this, mainWindow))
{
}

/*!
   Destructor.
 */
WlanWizard::~WlanWizard()
{
    delete d_ptr;
}

/*!
   Client can set the known WLAN Access Point configurations to speed up wizard
   processing and make it easier for end user.
   
   Values for network mode (CmManagerShim::WlanConnectionMode) and security mode 
   (CmManagerShim::WlanSecMode). 
   
   Supported configuration sets: 
   - Open: \a ssid \a networkMode \a securityMode 
   - WEP: \a ssid \a networkMode \a securityMode
   - WPA (2) with EAP: \a ssid \a networkMode \a securityMode \a usePsk (false)
   - WPA (2) with PSK: \a ssid \a networkMode \a securityMode \a usePsk (true)
   - 802.1x: \a ssid \a networkMode \a securityMode
   
   Hidden WLAN:
   \a hidden can be used with \a networkMode CmManagerShim::Infra 
   
   Wifi Protected Setup
   \a wps can be used with Open, WEP and WPA (2) with PSK.
   
   @param [in] name The name of WLAN network, max length 32 characters.
   @param [in] ssid The SSID of WLAN network, max length 32 characters.
   @param [in] networkMode Network mode of known access point
   @param [in] securityMode Security mode of known access point
   @param [in] usePsk used only with WPA or WPA2 \a securityMode
   @param [in] wps is Wifi Protected Setup supported?
 */
void WlanWizard::setParameters(
    const QString &name, 
    const QByteArray &ssid, 
    int networkMode, 
    int securityMode, 
    bool usePsk,
    bool wps)
{
    d_ptr->setParameters(name, ssid, networkMode, securityMode, usePsk, wps);
}

/*!
   Executes the wizard and shows the first page.
   
   @note it is not allowed to call this method twice with same instance.
   
   Possible signals:
   - finished(int, bool): called after wizard has successfully completed
   - cancelled(): user has cancelled the wizard operations.
   
   First page for the wizard is SSID (WLAN network name) query or if 
   setParameters() has been called the first page is determined based on the
   given configuration combination. 
 */
void WlanWizard::show()
{
    d_ptr->show();
}
