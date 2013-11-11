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
 *   WLAN Wizard Plugin API: Service required from the wizard plugin.
 *
 */

#ifndef WLANWIZARDPLUGIN_H
#define WLANWIZARDPLUGIN_H

// System includes

// User includes

// Forward declarations

// External data types

// Constants

/*!
   @addtogroup group_wlan_wizard_api_internal
   @{
 */

/*!
   Interface that wizard plugin must implement or at least the plugin MUST 
   inherit this interface and use the default implementations.
 */
class WlanWizardPlugin
{
public:
    /*!
       Enumerators for WLAN Wizard summary page.
     */
    enum Summary {
        //! EAP Outer Type
        SummaryEapOuterType = 0,
        //! EAP Inner Type
        SummaryEapInnerType, 
        //! EAP Fast: Provisioning mode
        SummaryEapFastProvisioningMode,
    };
    
public:
    /*!
       This method is used to query summary items from the wizard plugins
       that are eap and wps wizard.
       
       Both item and value are localized strings.
       
       @param [in] sum Summary id to be queried
       @param [out] item The item string is returned here 
       @param [out] value the value of item is returned here.
       
       @return true if summary item is found, false otherwise.
     */
    virtual bool summary(
        WlanWizardPlugin::Summary sum, 
        QString &item, 
        QString &value) 
    {
        Q_UNUSED(sum);
        Q_UNUSED(item);
        Q_UNUSED(value);
        return false;
    };
    
    /*!
       Stores wizard specific settings if any.
     */
    virtual bool storeSettings() 
    {
        return true;
    };
    
    /*!
       Maps plugin specific error codes into a localized string.
       
       @param [in] errorCode Symbian error code.
       
       @return If mapping can be done a localized error string is returned
       otherwise an empty string is returned. In case of empty string wlanwizard
       will use default error code.
     */
    virtual QString errorString(int errorCode) 
    {
        Q_UNUSED(errorCode);
        return QString();
    }
signals:
    
public slots:
    
protected:

protected slots:

private:

private slots:

private: // data    
};
/*! @} */

#endif /* WLANWIZARDPLUGIN_H */
