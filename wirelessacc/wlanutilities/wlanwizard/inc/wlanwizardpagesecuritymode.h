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
 *   WLAN Wizard Page: Security mode selection
 *
 */

#ifndef WLANWIZARDPAGESECURITYMODE_H
#define WLANWIZARDPAGESECURITYMODE_H

// System includes

// User includes
#include "wlanwizardpageinternal.h"

// Forward declarations
class HbRadioButtonList;
class HbLabel;
class WlanWizardPrivate;
class HbDocumentLoader;

// External data types

// Constants

/*!
 * @addtogroup group_wlan_wizard
 * @{
 */

class WlanWizardPageSecurityMode: public WlanWizardPageInternal
{
    Q_OBJECT
public:
    explicit WlanWizardPageSecurityMode(WlanWizardPrivate* parent);
    virtual ~WlanWizardPageSecurityMode();
    
public:
    HbWidget* initializePage();
    int nextId(bool &removeFromStack) const;
    bool showPage();
    
signals:

public slots:

protected:

protected slots:
    
private:
    Q_DISABLE_COPY(WlanWizardPageSecurityMode)
    void populateSecModeList(QStringList &list);
    void addToList(
        QStringList &list,
        const QString &item,
        int mode,
        int page,
        bool psk);
  
private slots: 
    void itemSelected();
    void loadDocmlSection(Qt::Orientation orientation);
    
private:
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
     * List of sequrity modes. This list is tied to the radio buttons during
     * the initialization of the page.
     */
    QList<int> mSecModes;
    
    /*!
     * List of page identifiers. This list is tied to the radio buttons during
     * the initialization of the page.
     */
    QList<int> mPageIds;
    
    /*!
     * List of password usage. This list is tied to the radio buttons during
     * the initialization of the page. True, if password is to be queried,
     * false otherwise
     */
    QList<bool> mUsePsk;
};

/*! @} */

#endif
