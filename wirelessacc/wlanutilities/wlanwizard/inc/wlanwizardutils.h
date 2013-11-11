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
 *   WLAN Wizard Utilities
 *
 */

#ifndef WLANWIZARDUTILS_H
#define WLANWIZARDUTILS_H

// System includes

// User includes

// Forward declarations

// External data types

// Constants

/*!
   @addtogroup group_wlan_wizard
   @{
 */

class WlanWizardUtils
{
public:
    enum KeyStatus {
        KeyStatusOk = 0,
        KeyStatusIllegalCharacters,
        KeyStatusWpaTooShort,
        KeyStatusWpaTooLong,
        KeyStatusWepInvalidLength,
    };

    enum SsidStatus {
        SsidStatusOk = 0,
        SsidStatusIllegalCharacters,
        SsidStatusInvalidLength
    };

    static const int SsidMinLength = 1;
    static const int SsidMaxLength = 32;
    static const int WpaMinLength = 8;
    static const int WpaMaxLength = 64;
    static const int WepHex64BitMaxLength = 10;
    static const int WepHex128BitMaxLength = 26;
    static const int WepAscii64BitMaxLength = 5;
    static const int WepAscii128BitMaxLength = 13;

public:
    static KeyStatus validateWepKey(const QString &key);
    static KeyStatus validateWpaKey(const QString &key);
    static KeyStatus isAscii(const QString &key);
    static KeyStatus isHex(const QString &key);
    static SsidStatus validateSsid(const QString &ssid);
    
signals:
    
public slots:
    
protected:

protected slots:

private:
    Q_DISABLE_COPY(WlanWizardUtils)

private slots:

private: // data
};

/*! @} */

#endif /* WLANWIZARDUTILS_H */
