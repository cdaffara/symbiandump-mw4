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

#ifndef WLANWIZARDSCANLIST_H_
#define WLANWIZARDSCANLIST_H_

// System includes
#include <QList>
#include <QHash>
#include <QMetaType>
#include <QString>
#include <QSharedPointer>

/*!
   @addtogroup group_wlan_wizard
   @{
 */

// User includes

// Forward declarations
class WlanQtUtilsAp;

// External data types
/*!
 * Data structure with scan results and access point visibility information.
 */
class WlanScanResult
{
public:
    QSharedPointer<WlanQtUtilsAp> scanResult;
    bool networkHidden;
};

/*!
 * Data structure containing security setting information and wizard page
 * navigation information.
 */
class WlanSecuritySetting
{
public:
    //! Enumerated value defined by CMManagerShim::WlanSecMode
    int mode;
    
    //! true, if passkey is required, false otherwise
    bool usePsk;
    
    /*! 
     * Identifier of the next page associated with this security mode.
     * Page ids are defined in WlanWizardPageInternal::WlanPageIds
     */
    int nextPageId;
    
    bool operator==(const WlanSecuritySetting& setting) const {
        return (mode == setting.mode && 
            usePsk == setting.usePsk &&
            nextPageId == setting.nextPageId);
    }
};

/*!
 * Data structure containing network mode, visibility and wireless protection
 * setup suppport information.
 */
class WlanNetworkSetting 
{
public:
    //! Enumerated value defined by CMManagerShim::WlanConnMode
    int mode;
    
    //! true, if hidden, false otherwise
    bool hidden;
    
    //! true, if supported, false otherwise
    bool wpsSupported;
    
    bool operator==(const WlanNetworkSetting& setting) const {
        return (mode == setting.mode && 
            hidden == setting.hidden &&
            wpsSupported == setting.wpsSupported);
    }
};

/*!
 * Necessary public function for an argument of type WlanNetworkSetting to
 * function as a QHash key.
 */
inline uint qHash(const WlanNetworkSetting &key)
 {
    uint hash = key.mode;
    hash <<= 1;
    hash |= key.hidden ? 1 : 0;
    hash <<= 1;
    hash |= key.wpsSupported ? 1 : 0;
    return qHash(hash);
 }

// Constants

class WlanWizardScanList
{
public:
    WlanWizardScanList();
    WlanWizardScanList(const WlanWizardScanList &scanList);
    ~WlanWizardScanList();
    void build(const QList<WlanScanResult> &results);
    const WlanSecuritySetting &getSecMode(WlanNetworkSetting netMode, int index = 0) const;
    int netModes() const;
    int secModes(WlanNetworkSetting netMode) const;
    QList<WlanNetworkSetting> getNetModes() const;
    bool wpsSelected() const;
    
private:
    // Disabling implicit assignment operator
    WlanWizardScanList &operator=(const WlanWizardScanList &);
    
private:
    QHash<WlanNetworkSetting, QList<WlanSecuritySetting> > mOpenOptions;
    bool mWpsSelected;
};

/*!
 * This macro makes WlanWizardScanList QVariant-compatible.
 */
Q_DECLARE_METATYPE(WlanWizardScanList)

/*! @} */

#endif /* WLANWIZARDSCANLIST_H_ */
