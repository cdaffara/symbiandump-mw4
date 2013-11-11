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
 *   WLAN Wizard Page: Manual Network mode query page.  
 *
 */

#ifndef WLANWIZARDPAGENETWORKMODE_H
#define WLANWIZARDPAGENETWORKMODE_H

// System includes

// User includes
#include "wlanwizardpageinternal.h"

// Forward declarations
class HbRadioButtonList;
class HbLabel;
class WlanWizardPrivate;
class HbDocumentLoader;
class WlanNetworkSetting;

// External data types

// Constants

/*!
 * @addtogroup group_wlan_wizard
 * @{
 */

class WlanWizardPageNetworkMode : public WlanWizardPageInternal
{
    Q_OBJECT
public:
    
    explicit WlanWizardPageNetworkMode(WlanWizardPrivate* parent);
    virtual ~WlanWizardPageNetworkMode();
    
public: // From WlanWizardPageInternal
    HbWidget* initializePage();
    int nextId(bool &removeFromStack) const;   
    bool showPage();

signals:
    
public slots:
    void itemSelected();
    void loadDocmlSection(Qt::Orientation orientation);
    
protected:
    
protected slots:
    
private:
    Q_DISABLE_COPY(WlanWizardPageNetworkMode)
    void addToList(
        QStringList &list,
        const QString &item,
        int mode,
        bool isHidden,
        bool wpsSupported);
    void populateRadioButtonList(QStringList &list);
    int selectNextPage(const WlanNetworkSetting &setting) const;

private slots:

private:

    /*!
     * Internal network mode value constant. Used when wps used.
     */
    static const int NetworkModeNone = -1;
    
    /*!
     * Pointer to the view.
     */
    HbWidget *mWidget;
    
    /*!
     * Pointer to the radio button list object.
     */
    HbRadioButtonList *mList;
    
    /*!
     * Pointer to the label object.
     */
    HbLabel *mLabel;
    
    /*!
     * Pointer to document loader object.
     */
    HbDocumentLoader *mLoader;
    
    /*!
     * List of network modes. This list is tied to the radio buttons during
     * the initialization of the page.
     */
    QList<int> mNetworkModes;
    
    /*!
     * List of publicity modes. This list is tied to the radio buttons during
     * the initialization of the page.
     */
    QList<bool> mIsHidden;
    
    /*!
     * List of wps support. This list is tied to the radio buttons during
     * the initialization of the page.
     */
    QList<bool> mWpsSupported;
};

/*! @} */

#endif
